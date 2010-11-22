/*
 *  structure_utils.h
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 20/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */


#ifndef _STRUCTURE_UTILS_H
#define _STRUCTURE_UTILS_H

#include <hdf5.h>
#include "pIOTypes.h"

typedef struct {
	int32_t position;
	int32_t number;
} link_t;

hid_t linkDatatype();

typedef struct listOfPaths_s {
    char* path;
    struct listOfPaths_s *next;
} listOfPaths_t;

int lengthOfList( listOfPaths_t* list);
listOfPaths_t* addCopyToList( listOfPaths_t* list, char* path );
int destroyList( listOfPaths_t* list);

int getTimesUsed(PIOTimeline pioTimeline);
int incrementTimesUsed(PIOTimeline pioTimeline);
int decrementTimesUsed(PIOTimeline pioTimeline);

/// \returns int
///		- number of datasets when successfull
///		- negative value otherwise
listOfPaths_t* allDatasetsInGroup(hid_t file, const char* path2group);

//
///// \returns int
/////		- number of matching datasets when successfull
/////		- negative value otherwise
//int allMatchingDatasetsInGroup(hid_t file,
//							   const char* path2group,
//							   const char* filter,
//							   char*** path);
//
#endif
