/*
 *  pIODataset.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 19/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

#include "pIODataset.h"
#include "pIOFile.h"
#include "pIOTypes.h"
#include "pIOAttributes.h"
#include "pIOVersion.h"
#include "structure_utils.h"

#include <stdlib.h>
#include <string.h>
#include <hdf5_hl.h>

int monoDimensionalDatasetExtent(hid_t monoDimensionalDataset)
{
	hid_t dataDataspace;
	hsize_t extent[1] = {-1};
		
	dataDataspace = H5Dget_space(monoDimensionalDataset);
	H5Sget_simple_extent_dims(dataDataspace, extent, NULL);
	H5Sclose(dataDataspace);
	
	return (int)extent[0];
}

// Returns /dataset/path/data
int internalPathToDatasetData( const char* path, char** internalPath )
{
	int length = 3*strlen("/")+ \
	             strlen(PIOFile_Structure_Group_Datasets)+ \
				 strlen(PIOFile_Structure_Datasets_Data)+ \
				 strlen(path);
	*internalPath = (char*) malloc((length+1)*sizeof(char));
	sprintf(*internalPath, "/%s/%s/%s", 
			PIOFile_Structure_Group_Datasets, path, PIOFile_Structure_Datasets_Data);	
	return 1;
}

// Returns /dataset/path/link
int internalPathToDatasetLink( const char* path, char** internalPath )
{
	int length = 3*strlen("/")+ \
	strlen(PIOFile_Structure_Group_Datasets)+ \
	strlen(PIOFile_Structure_Datasets_Link)+ \
	strlen(path);
	*internalPath = (char*) malloc((length+1)*sizeof(char));
	sprintf(*internalPath, "/%s/%s/%s", 
			PIOFile_Structure_Group_Datasets, path, PIOFile_Structure_Datasets_Link);	
	return 1;
}

PIODataset pioNewDataset(PIOFile pioFile, 
						 const char* path, const char* description,
						 PIOTimeline pioTimeline,
						 PIODatatype pioDatatype)
{
	PIODataset pioDataset;
	
	char* internalPathToData  = NULL; // name says it all
	char* internalPathToCount = NULL; // name says it all
	
	hid_t datasetCreationProperty, linkCreationProperty; 
	hid_t dataspaceForData;
	hsize_t dataspaceForDataMinSize[1] = { 0 };
	hsize_t dataspaceForDataMaxSize[1] = { H5S_UNLIMITED };
	hsize_t dataspaceForDataChunkSize[1] = { 1 };

	hid_t link_datatype;
	hid_t dataspaceForLink;
	hsize_t dataspaceForLinkFixedSize[1] = { pioTimeline.ntimeranges };
	
	ERROR_SWITCH_INIT
	
	// make sure a dataset doesn't already exist at path
	pioDataset = pioOpenDataset(PIOMakeObject(pioFile), path);
	if (PIODatasetIsValid(pioDataset))
	{
		pioCloseDataset(&pioDataset);
		return PIODatasetInvalid;
	}
	
	// check if description is not NULL
	if (!description) return PIODatasetInvalid;
		
	// get internal paths
	internalPathToDatasetData(path, &internalPathToData);
	internalPathToDatasetLink(path, &internalPathToCount);
	
	// create dataset/link creation propery
	datasetCreationProperty = H5Pcreate(H5P_DATASET_CREATE);
	H5Pset_chunk(datasetCreationProperty, 1, dataspaceForDataChunkSize);

	linkCreationProperty    = H5Pcreate(H5P_LINK_CREATE);
	H5Pset_create_intermediate_group(linkCreationProperty, 1);	
	
	// create dataspace
	dataspaceForData = H5Screate_simple(1,  dataspaceForDataMinSize,    dataspaceForDataMaxSize);
	dataspaceForLink = H5Screate_simple(1, dataspaceForLinkFixedSize, NULL);
	
	// create count datatype
	link_datatype = linkDatatype();
	
	// create datasets
	ERROR_SWITCH_OFF
	
	pioDataset.identifier = H5Dcreate2(pioFile.identifier, internalPathToData, 
									   pioDatatype.identifier, dataspaceForData, 
									   linkCreationProperty, datasetCreationProperty, H5P_DEFAULT);
	
	pioDataset.link_identifier = H5Dcreate2(pioFile.identifier, internalPathToCount, 
											 link_datatype, dataspaceForLink, 
											 linkCreationProperty, datasetCreationProperty, H5P_DEFAULT);
	ERROR_SWITCH_ON
	
	free(internalPathToData);
	free(internalPathToCount);
	// close creation properties
	H5Pclose(datasetCreationProperty);
	H5Pclose(linkCreationProperty);
	// close dataspace
	H5Sclose(dataspaceForData);
	// close datatype
	H5Tclose(link_datatype);
	
	// check if dataset was created successfully
	if (PIODatasetIsInvalid(pioDataset)) return PIODatasetInvalid;
		
	// add description as attribute of data dataset
	if (H5LTset_attribute_string(pioDataset.identifier, ".", PIOAttribute_Description, description) < 0)
	{
		pioCloseDataset(&pioDataset);
		return PIODatasetInvalid;
	}
	
	// add pinocchIO version as attribute of dataset
	if (H5LTset_attribute_string(pioDataset.identifier, ".", PIOAttribute_Version, PINOCCHIO_VERSION) < 0)
	{
		pioCloseDataset(&pioDataset);
		return PIODatasetInvalid;
	}
	
	// add path to timeline as attribute of dataset
	if (H5LTset_attribute_string(pioDataset.identifier, ".", PIOAttribute_Timeline, pioTimeline.path) < 0)
	{
		pioCloseDataset(&pioDataset);
		return PIODatasetInvalid;
	}
	
	// increment timeline 'times_used' attribute
	if (incrementTimesUsed(pioTimeline) < 0)
	{
		pioCloseDataset(&pioDataset);
		return PIODatasetInvalid;
	}
	
	// store everything in PIODataset structure
	pioDataset.path = (char*) malloc((strlen(path)+1)*sizeof(char));
	strncpy( pioDataset.path, path, strlen(path));
	pioDataset.path[strlen(path)] = '\0';
	
	pioDataset.description = (char*) malloc((strlen(description)+1)*sizeof(char));
	strncpy( pioDataset.description, description, strlen(description));
	pioDataset.description[strlen(description)] = '\0';
	
	pioDataset.stored = 0;
	pioDataset.ntimeranges = monoDimensionalDatasetExtent(pioDataset.link_identifier);
	
	return pioDataset;
}

PIODataset pioOpenDataset(PIOObject pioObject, const char* path)
{
	PIODataset pioDataset = PIODatasetInvalid;	
	
	char* internalPathToData;
	char* internalPathToCount;
		
	hid_t attr;
	hsize_t storage;
	char* version;
	
	ERROR_SWITCH_INIT
	
	// get internal path
	internalPathToDatasetData(path, &internalPathToData);
	internalPathToDatasetLink(path, &internalPathToCount);
	
	// open dataset
	ERROR_SWITCH_OFF	
	pioDataset.identifier = H5Dopen2(pioObject.identifier, internalPathToData, H5P_DEFAULT);
	pioDataset.link_identifier = H5Dopen2(pioObject.identifier, internalPathToCount, H5P_DEFAULT);
	ERROR_SWITCH_ON
	free(internalPathToData);
	free(internalPathToCount);
	
	if (PIODatasetIsInvalid(pioDataset)) return PIODatasetInvalid;
	
	// get dimension of dataset
	pioDataset.stored = monoDimensionalDatasetExtent(pioDataset.identifier);
	
	// get dimension of link dataset
	pioDataset.ntimeranges = monoDimensionalDatasetExtent(pioDataset.link_identifier);
	
	// store path to dataset
	pioDataset.path = (char*) malloc((strlen(path)+1)*sizeof(char));
	strncpy( pioDataset.path, path, strlen(path));
	pioDataset.path[strlen(path)] = '\0';
	
	// store dataset description
	attr = H5Aopen_name(pioDataset.identifier, PIOAttribute_Description);
	storage = H5Aget_storage_size(attr); H5Aclose(attr);
	pioDataset.description = (char*)malloc( (storage+1) * sizeof(char));
	H5LTget_attribute_string(pioDataset.identifier, ".", PIOAttribute_Description, pioDataset.description);
	pioDataset.description[storage] = '\0';
	
	// read stored pinocchIO version and compare
	attr = H5Aopen_name(pioDataset.identifier, PIOAttribute_Version);
	storage = H5Aget_storage_size(attr); H5Aclose(attr);
	version = (char*)malloc( storage + sizeof(char));
	H5LTget_attribute_string(pioDataset.identifier, ".", PIOAttribute_Version, version);
	if (strcmp(version, PINOCCHIO_VERSION)!=0)
		fprintf(stdout, 
				"WARNING: pinocchIO versions do not match (you: %s, dataset: %s)\n",
				PINOCCHIO_VERSION, version);
	free(version);	
			
	return pioDataset;
}

int pioCloseDataset(PIODataset* pioDataset)
{
	if (pioDataset->path) free(pioDataset->path);
	pioDataset->path = NULL;
	
	if (pioDataset->description) free(pioDataset->description);
	pioDataset->description = NULL;
	
	pioDataset->stored = -1;
	pioDataset->ntimeranges = -1;
	
	if (H5Dclose(pioDataset->identifier) < 0) return 0;
	pioDataset->identifier = -1;
	
	if (H5Dclose(pioDataset->link_identifier) < 0) return 0;
	pioDataset->link_identifier = -1;
	
	return 1;
}

int pioGetListOfDatasets(PIOFile pioFile, char*** pathsToDatasets)
{	
	int ds = -1;
	listOfPaths_t* rawPaths = NULL;
	listOfPaths_t* rawPath = NULL;
		
	int rawpath_length = -1;
	int data_length = -1;
	
	int numberOfDatasets = -1;
	
	// get all HDF5 datasets in /dataset/
	rawPaths = allDatasetsInGroup(pioFile.identifier, PIOFile_Structure_Group_Datasets);
	
	// count those matching /dataset/****/data
	data_length = strlen(PIOFile_Structure_Datasets_Data);
	rawPath = rawPaths;
	numberOfDatasets = 0;
	while (rawPath != NULL) 
	{
		rawpath_length = strlen(rawPath->path);
		if (strcmp(rawPath->path+rawpath_length-data_length, PIOFile_Structure_Datasets_Data) == 0)
			numberOfDatasets++;
		rawPath = rawPath->next;
	}
	
	// copy /dataset/**** path 
	*pathsToDatasets = (char**) malloc(sizeof(char*)*numberOfDatasets);
	rawPath = rawPaths;
	ds = 0;
	while (rawPath != NULL) 
	{
		rawpath_length = strlen(rawPath->path);
		if (strcmp(rawPath->path+rawpath_length-data_length, PIOFile_Structure_Datasets_Data) == 0)
		{
			(*pathsToDatasets)[ds] = (char*) malloc((rawpath_length-data_length)*sizeof(char));
			rawPath->path[rawpath_length-data_length-1] = '\0';
			sprintf((*pathsToDatasets)[ds], "%s", rawPath->path);
			ds++;
		}
		rawPath = rawPath->next;
	}
	destroyList(rawPaths); rawPaths = NULL;
	return numberOfDatasets;
}
