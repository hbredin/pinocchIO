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

// Returns /dataset/path/count
int internalPathToDatasetCount( const char* path, char** internalPath )
{
	int length = 3*strlen("/")+ \
	strlen(PIOFile_Structure_Group_Datasets)+ \
	strlen(PIOFile_Structure_Datasets_Count)+ \
	strlen(path);
	*internalPath = (char*) malloc((length+1)*sizeof(char));
	sprintf(*internalPath, "/%s/%s/%s", 
			PIOFile_Structure_Group_Datasets, path, PIOFile_Structure_Datasets_Count);	
	return 1;
}

hid_t countDatatype()
{
	hid_t tCount;	
	tCount = H5Tcopy(H5T_NATIVE_INT32);
	return tCount;
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
	hid_t dataspace;
	hsize_t dataspaceMinSize[1] = { 0 };
	hsize_t dataspaceMaxSize[1] = { H5S_UNLIMITED };
	hsize_t dataspaceChunkSize[1] = { 1 };
	hid_t count_datatype;
		
	ERROR_SWITCH_INIT
	
	// make sure a dataset doesn't already exist at path
	pioDataset = pioOpenDataset(pioFile, path);
	if (PIODatasetIsValid(pioDataset))
	{
		pioCloseDataset(pioDataset);
		return PIODatasetInvalid;
	}
	
	// check if description is not NULL
	if (!description) return PIODatasetInvalid;
		
	// get internal paths
	internalPathToDatasetData(path, &internalPathToData);
	internalPathToDatasetCount(path, &internalPathToCount);
	
	// create dataset/link creation propery
	datasetCreationProperty = H5Pcreate(H5P_DATASET_CREATE);
	H5Pset_chunk(datasetCreationProperty, 1, dataspaceChunkSize);

	linkCreationProperty    = H5Pcreate(H5P_LINK_CREATE);
	H5Pset_create_intermediate_group(linkCreationProperty, 1);	
	
	// create dataspace
	dataspace = H5Screate_simple(1, dataspaceMinSize, dataspaceMaxSize);
	
	// create count datatype
	count_datatype = countDatatype();
	
	// create datasets
	ERROR_SWITCH_OFF
	
	pioDataset.identifier = H5Dcreate2(pioFile.identifier, internalPathToData, 
									   pioDatatype.identifier, dataspace, 
									   linkCreationProperty, datasetCreationProperty, H5P_DEFAULT);
	
	pioDataset.count_identifier = H5Dcreate2(pioFile.identifier, internalPathToCount, 
											 count_datatype, dataspace, 
											 linkCreationProperty, datasetCreationProperty, H5P_DEFAULT);
	ERROR_SWITCH_ON
	
	free(internalPathToData);
	free(internalPathToCount);
	// close creation properties
	H5Pclose(datasetCreationProperty);
	H5Pclose(linkCreationProperty);
	// close dataspace
	H5Sclose(dataspace);
	// close datatype
	H5Tclose(count_datatype);
	
	// check if dataset was created successfully
	if (PIODatasetIsInvalid(pioDataset)) return PIODatasetInvalid;
		
	// add description as attribute of data dataset
	if (H5LTset_attribute_string(pioDataset.identifier, ".", PIOAttribute_Description, description) < 0)
	{
		pioCloseDataset(pioDataset);
		return PIODatasetInvalid;
	}
	
	// add pinocchIO version as attribute of dataset
	if (H5LTset_attribute_string(pioDataset.identifier, ".", PIOAttribute_Version, PINOCCHIO_VERSION) < 0)
	{
		pioCloseDataset(pioDataset);
		return PIODatasetInvalid;
	}
	
	// add path to timeline as attribute of dataset
	if (H5LTset_attribute_string(pioDataset.identifier, ".", PIOAttribute_Timeline, pioTimeline.description) < 0)
	{
		pioCloseDataset(pioDataset);
		return PIODatasetInvalid;
	}
	
	// increment timeline 'times_used' attribute
	if (incrementTimesUsed(pioTimeline) < 0)
	{
		pioCloseDataset(pioDataset);
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
	pioDataset.allocated = 0;
	
	return pioDataset;
}

PIODataset pioOpenDataset(PIOFile pioFile, const char* path)
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
	internalPathToDatasetCount(path, &internalPathToCount);
	
	// open dataset
	ERROR_SWITCH_OFF	
	pioDataset.identifier = H5Dopen2(pioFile.identifier, internalPathToData, H5P_DEFAULT);
	pioDataset.count_identifier = H5Dopen2(pioFile.identifier, internalPathToCount, H5P_DEFAULT);
	ERROR_SWITCH_ON
	free(internalPathToData);
	free(internalPathToCount);
	
	if (PIODatasetIsInvalid(pioDataset)) return PIODatasetInvalid;
	
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
	
	pioDataset.stored = 0;
	pioDataset.allocated = 0;
		
	return pioDataset;
}

int pioCloseDataset(PIODataset pioDataset)
{
	if (pioDataset.path) free(pioDataset.path);
	pioDataset.path = NULL;
	
	if (pioDataset.description) free(pioDataset.description);
	pioDataset.description = NULL;
	
	pioDataset.stored = -1;
	pioDataset.allocated = -1;
	
	if (H5Dclose(pioDataset.identifier) < 0) return 0;
	pioDataset.identifier = -1;
	
	if (H5Dclose(pioDataset.count_identifier) < 0) return 0;
	pioDataset.count_identifier = -1;
	
	return 1;
}