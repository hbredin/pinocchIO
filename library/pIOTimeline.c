/*
 *  pIOTimeline.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 15/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

#include "pIOTimeline.h"
#include "pIOAttributes.h"
#include "pIOFile.h"
#include "pIOTimeComparison.h"
#include "pIOVersion.h"
#include "structure_utils.h"

#include <string.h>
#include <stdlib.h>
#include <hdf5_hl.h>

int internalPathToTimeline( const char* path, char** internalPath )
{
	// /timeline/path
	int length = 2*strlen("/")+strlen(PIOFile_Structure_Group_Timelines)+strlen(path);
	*internalPath = (char*) malloc((length+1)*sizeof(char));
	sprintf(*internalPath, "/%s/%s", PIOFile_Structure_Group_Timelines, path);	
	return 1;
}

hid_t timelineDatatype()
{
	hid_t timetype;
	hid_t tInt64, tInt32;
	
	timetype = H5Tcreate(H5T_COMPOUND, sizeof(PIOTimeRange));
	tInt64 = H5Tcopy(H5T_NATIVE_INT64);
	tInt32 = H5Tcopy(H5T_NATIVE_INT32);
	H5Tinsert(timetype, "time",     HOFFSET(PIOTimeRange, time) ,    tInt64);
	H5Tinsert(timetype, "duration", HOFFSET(PIOTimeRange, duration), tInt64);
	H5Tinsert(timetype, "scale",    HOFFSET(PIOTimeRange, scale),    tInt32);
	H5Tclose(tInt64); H5Tclose(tInt32);	
	
	return timetype;
}

PIOTimeline pioNewTimeline(PIOFile pioFile, const char* path, const char* description,
						   int numberOfTimeRanges, PIOTimeRange* timeranges)
{
	PIOTimeline pioTimeline;

	int t; // timerange loop index
	char* internalPath = NULL; // name says it all
	
	hid_t datasetCreationProperty, linkCreationProperty; 
	hid_t dataspace;
	hsize_t dataspaceMinSize[1] = { numberOfTimeRanges };
	hsize_t dataspaceMaxSize[1] = { numberOfTimeRanges };
	hid_t datatype;
	herr_t write_err;
		
	ERROR_SWITCH_INIT
	
	// make sure a timeline doesn't already exist at path
	pioTimeline = pioOpenTimeline(PIOMakeObject(pioFile), path);
	if (PIOTimelineIsValid(pioTimeline))
	{
		pioCloseTimeline(&pioTimeline);
		return PIOTimelineInvalid;
	}
	
	// check if description is not NULL
	if (!description) return PIOTimelineInvalid;
	
	// check if timeranges are sorted in chronological order
	for (t=1; t<numberOfTimeRanges; t++)
		if (pioCompareTimeRanges(timeranges[t-1], timeranges[t]) == PINOCCHIO_TIMERANGE_COMPARISON_DESCENDING)
        {
            fprintf(stderr, "Timeranges should be sorted in chronological order (see timerange %d).\n", t+1);
			fflush(stderr);
            return PIOTimelineInvalid;
        }
	// get internal path
	internalPathToTimeline(path, &internalPath);
	
	// create dataset/link creation propery
	datasetCreationProperty = H5Pcreate(H5P_DATASET_CREATE);
	linkCreationProperty    = H5Pcreate(H5P_LINK_CREATE);
	H5Pset_create_intermediate_group(linkCreationProperty, 1);	

	// create dataspace
	dataspace = H5Screate_simple(1, dataspaceMinSize, dataspaceMaxSize);
	
	// create datatype
	datatype = timelineDatatype();
	
	// create dataset
	ERROR_SWITCH_OFF
	pioTimeline.identifier = H5Dcreate2(pioFile.identifier, internalPath, 
										datatype, dataspace, 
										linkCreationProperty, datasetCreationProperty, H5P_DEFAULT);
	ERROR_SWITCH_ON
	
	free(internalPath);
	// close creation properties
	H5Pclose(datasetCreationProperty);
	H5Pclose(linkCreationProperty);
	// close dataspace
	H5Sclose(dataspace);
	// close datatype
	H5Tclose(datatype);
	
	// check if dataset was created successfully
	if ( PIOTimelineIsInvalid(pioTimeline)) return PIOTimelineInvalid;
	
	// create datatype
	datatype = timelineDatatype();
		
	// dump timeline into dataset
	ERROR_SWITCH_OFF
	write_err = H5Dwrite(pioTimeline.identifier, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, timeranges);	
	ERROR_SWITCH_ON
	
	// close datatype
	H5Tclose(datatype);
	
	// check if write was successfull
	if (write_err < 0) 
	{
		pioCloseTimeline(&pioTimeline);
		return PIOTimelineInvalid;
	}
	
	// add description as attribute of timeline
	if (H5LTset_attribute_string(pioTimeline.identifier, ".", PIOAttribute_Description, description) < 0)
	{
		pioCloseTimeline(&pioTimeline);
		return PIOTimelineInvalid;
	}
	
	// add pinocchIO version as attribute of timeline
	if (H5LTset_attribute_string(pioTimeline.identifier, ".", PIOAttribute_Version, PINOCCHIO_VERSION) < 0)
	{
		pioCloseTimeline(&pioTimeline);
		return PIOTimelineInvalid;
	}
	
	// add number of datasets referencing this timeline as attribute
	int times_used = 0;
	if (H5LTset_attribute_int(pioTimeline.identifier, ".", PIOAttribute_TimesUsed, &times_used, 1))
	{
		pioCloseTimeline(&pioTimeline);
		return PIOTimelineInvalid;
	}
	
	// store everything in PIOTimeline structure
	pioTimeline.ntimeranges = numberOfTimeRanges;
	pioTimeline.timeranges = malloc(numberOfTimeRanges*sizeof(PIOTimeRange));
	for (t=0; t<numberOfTimeRanges; t++) pioTimeline.timeranges[t] = timeranges[t];
	
	pioTimeline.path = (char*) malloc((strlen(path)+1)*sizeof(char));
	strncpy( pioTimeline.path, path, strlen(path));
	pioTimeline.path[strlen(path)] = '\0';

	pioTimeline.description = (char*) malloc((strlen(description)+1)*sizeof(char));
	strncpy( pioTimeline.description, description, strlen(description));
	pioTimeline.description[strlen(description)] = '\0';
	
	return pioTimeline;
}

PIOTimeline pioOpenTimeline(PIOObject pioObjectInFile, const char* path)
{
	PIOTimeline pioTimeline = PIOTimelineInvalid;
	
	char* internalPath;
	hid_t dataspace;
	hsize_t dimensions[1];
	hid_t datatype;
	herr_t read_err;
	
	hid_t attr;
	hsize_t storage;
	char* version;
	
	ERROR_SWITCH_INIT
	
	// get internal path
	internalPathToTimeline(path, &internalPath);
	
	// open dataset
	ERROR_SWITCH_OFF	
	pioTimeline.identifier = H5Dopen2(pioObjectInFile.identifier, internalPath, H5P_DEFAULT);
	ERROR_SWITCH_ON
	free(internalPath);
	
	if (PIOTimelineIsInvalid(pioTimeline)) return PIOTimelineInvalid;
	
	// get dataspace extent
	dataspace = H5Dget_space(pioTimeline.identifier);
	H5Sget_simple_extent_dims(dataspace, dimensions, NULL);
	
	// allocate memory to store timeline data
	pioTimeline.ntimeranges = dimensions[0];
	pioTimeline.timeranges = (PIOTimeRange*) malloc(pioTimeline.ntimeranges*sizeof(PIOTimeRange));
	
	// get timeline type
	datatype = timelineDatatype();
	
	// load whole timeline
	ERROR_SWITCH_OFF
	read_err = H5Dread(pioTimeline.identifier, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, pioTimeline.timeranges);
	ERROR_SWITCH_ON
	
	// close dataspace
	H5Sclose(dataspace);
	// close datatype
	H5Tclose(datatype);
	
	if (read_err < 0)
	{
		pioCloseTimeline(&pioTimeline);
		return PIOTimelineInvalid;
	}
	
	// store path to timeline
	pioTimeline.path = (char*) malloc((strlen(path)+1)*sizeof(char));
	strncpy( pioTimeline.path, path, strlen(path));
	pioTimeline.path[strlen(path)] = '\0';
	
	// store timeline description
	attr = H5Aopen_name(pioTimeline.identifier, PIOAttribute_Description);
	storage = H5Aget_storage_size(attr); H5Aclose(attr);
	pioTimeline.description = (char*)malloc( (storage+1) * sizeof(char));
	H5LTget_attribute_string(pioTimeline.identifier, ".", PIOAttribute_Description, pioTimeline.description);
	pioTimeline.description[storage] = '\0';
	
	// read stored pinocchIO version and compare
	attr = H5Aopen_name(pioTimeline.identifier, PIOAttribute_Version);
	storage = H5Aget_storage_size(attr); H5Aclose(attr);
	version = (char*)malloc( storage + sizeof(char));
	H5LTget_attribute_string(pioTimeline.identifier, ".", PIOAttribute_Version, version);
	if (strcmp(version, PINOCCHIO_VERSION)>0)
		fprintf(stdout, 
				"WARNING: pinocchIO versions do not match (you: %s, file: %s)\n",
				PINOCCHIO_VERSION, version);
	free(version);	
	
	return pioTimeline;
}

int pioCloseTimeline( PIOTimeline* pioTimeline )
{
	if (pioTimeline->path) free(pioTimeline->path); 
	pioTimeline->path = NULL;
	
	if (pioTimeline->description) free(pioTimeline->description);
	pioTimeline->description = NULL;
	
	if (pioTimeline->timeranges) free(pioTimeline->timeranges);
	pioTimeline->timeranges = NULL;
	
	pioTimeline->ntimeranges = -1;

	if (H5Dclose(pioTimeline->identifier) < 0) return 0;
	pioTimeline->identifier = -1;
	
	return 1;
}

int pioGetListOfTimelines(PIOFile pioFile, char*** pathsToTimelines)
{
	int tl = -1;
	int numberOfTimelines = -1;
	listOfPaths_t* paths = NULL;
	listOfPaths_t* path = NULL;
	paths = allDatasetsInGroup(pioFile.identifier, PIOFile_Structure_Group_Timelines);
	numberOfTimelines = lengthOfList(paths);
	
	*pathsToTimelines = (char**) malloc(sizeof(char*)*numberOfTimelines);
	path = paths;
	tl = 0;
	while (path != NULL) 
	{
		(*pathsToTimelines)[tl] = (char*) malloc(sizeof(char)*(strlen(path->path)+1));
		sprintf((*pathsToTimelines)[tl], "%s", path->path);
		tl++;
		path = path->next;
	}
	destroyList(paths); paths = NULL;
	return numberOfTimelines;
}

PIOTimeline pioGetTimeline(PIODataset pioDataset)
{
	hid_t attr;
	hsize_t storage;
	char* path2timeline;
	
	PIOTimeline pioTimeline = PIOTimelineInvalid;
	
	// read stored path to timeline
	attr = H5Aopen_name(pioDataset.identifier, PIOAttribute_Timeline);
	storage = H5Aget_storage_size(attr); H5Aclose(attr);
	path2timeline = (char*)malloc( storage + sizeof(char));
	H5LTget_attribute_string(pioDataset.identifier, ".", PIOAttribute_Timeline, path2timeline);
	
	// open timeline
	pioTimeline = pioOpenTimeline(PIOMakeObject(pioDataset), path2timeline);
	
	free(path2timeline);
	
	return pioTimeline;
}

