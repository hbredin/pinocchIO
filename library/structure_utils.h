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

/// \returns int
///		- number of datasets when successfull
///		- negative value otherwise
int allDatasetsInGroup(hid_t file,
					   const char* path2group,
					   char*** path);
//
///// \returns int
/////		- number of matching datasets when successfull
/////		- negative value otherwise
//int allMatchingDatasetsInGroup(char* path2group,
//							   char* match,
//							   char** path);
//
#endif