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
