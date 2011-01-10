// 
// Copyright 2010 Herve BREDIN (bredin@limsi.fr)
// Contact: http://pinocchio.niderb.fr/
// 
// This file is part of pinocchIO.
//  
//      pinocchIO is free software: you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation, either version 3 of the License, or
//      (at your option) any later version.
//  
//      pinocchIO is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//  
//      You should have received a copy of the GNU General Public License
//      along with pinocchIO. If not, see <http://www.gnu.org/licenses/>.
// 

#include "pIOFile.h"
#include "pIOVersion.h"
#include "pIOAttributes.h"

#include <hdf5_hl.h>
#include <string.h>
#include <stdlib.h>

PIOFile pioNewFile( const char* path, const char* medium )
{
	PIOFile pioFile = PIOFileInvalid;
	hid_t group; 

	ERROR_SWITCH_INIT
	
	if (!medium) return PIOFileInvalid;

	// --- create file
	ERROR_SWITCH_OFF
	pioFile.identifier = H5Fcreate(path, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
	ERROR_SWITCH_ON
	
	// add path to media as attribute of root group
	if (H5LTset_attribute_string(pioFile.identifier, "/", PIOAttribute_File_Medium, medium) < 0)
	{
		pioCloseFile(&pioFile);
		return PIOFileInvalid;
	}
	
	// add pinocchIO version as attribute of root group
	if (H5LTset_attribute_string(pioFile.identifier, "/", PIOAttribute_Version, PINOCCHIO_VERSION) < 0)
	{
		pioCloseFile(&pioFile);
		return PIOFileInvalid;
	}
	
	// create group used to store timelines
	ERROR_SWITCH_OFF
	group = H5Gcreate2(pioFile.identifier, PIOFile_Structure_Group_Timelines, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ERROR_SWITCH_ON
	if (group < 0)
	{
		pioCloseFile(&pioFile);
		return PIOFileInvalid;		
	}
	H5Gclose(group);
		
	// create group used to store datasets
	ERROR_SWITCH_OFF
	group = H5Gcreate2(pioFile.identifier, PIOFile_Structure_Group_Datasets, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	ERROR_SWITCH_ON
	if (group < 0)
	{
		pioCloseFile(&pioFile);
		return PIOFileInvalid;		
	}
	H5Gclose(group);
	
	// save path to medium in pioFile structure
	pioFile.medium = (char*) malloc((strlen(medium)+1)*sizeof(char));
	strncpy( pioFile.medium, medium, strlen(medium));
	pioFile.medium[strlen(medium)] = '\0';
	
	//read-n-write access
	pioFile.rights = PINOCCHIO_READNWRITE;
	
	return pioFile;
}

PIOFile pioOpenFile( const char* path, PIOFileRights rights )
{
	PIOFile pioFile = PIOFileInvalid;
	hid_t attr;
	hsize_t storage;
	char *version;
	
	ERROR_SWITCH_INIT
	
	// --- open file
	ERROR_SWITCH_OFF
	switch (rights) {
		case PINOCCHIO_READONLY:
			pioFile.identifier = H5Fopen(path, H5F_ACC_RDONLY, H5P_DEFAULT);
			break;
		case PINOCCHIO_READNWRITE:
			pioFile.identifier = H5Fopen(path, H5F_ACC_RDWR, H5P_DEFAULT);
			break;
		default:
			break;
	}
	ERROR_SWITCH_ON
	
	if (PIOObjectIsInvalid(pioFile)) return PIOFileInvalid;
	
	// --- read path to media 
	// get length of medium path
	attr = H5Aopen_name(pioFile.identifier, PIOAttribute_File_Medium);
	storage = H5Aget_storage_size(attr); H5Aclose(attr);
	
	pioFile.medium = (char*)malloc( (storage+1) * sizeof(char));
	H5LTget_attribute_string(pioFile.identifier, "/", PIOAttribute_File_Medium, pioFile.medium);
	pioFile.medium[storage] = '\0';
	
	// --- read stored pinocchIO version and compare
	attr = H5Aopen_name(pioFile.identifier, PIOAttribute_Version);
	storage = H5Aget_storage_size(attr); H5Aclose(attr);
	version = (char*)malloc( storage + sizeof(char));
	H5LTget_attribute_string(pioFile.identifier, "/", PIOAttribute_Version, version);
	if (strcmp(version, PINOCCHIO_VERSION)>0)
		fprintf(stdout, 
				"WARNING: pinocchIO versions do not match (you: %s, file: %s)\n",
				PINOCCHIO_VERSION, version);
	free(version);	
	
	return pioFile;
}

int pioCloseFile( PIOFile* pioFile )
{
	if (pioFile->medium) free(pioFile->medium);
	pioFile->medium = NULL;

    if (pioFile->identifier > -1)
        if (H5Fclose(pioFile->identifier) < 0) 
            return 0;
	pioFile->identifier = -1;
	
	return 1;	
}

