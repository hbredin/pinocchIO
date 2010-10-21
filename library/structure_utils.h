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


int getTimesUsed(PIOTimeline pioTimeline);
int incrementTimesUsed(PIOTimeline pioTimeline);
int decrementTimesUsed(PIOTimeline pioTimeline);

/// \returns int
///		- number of datasets when successfull
///		- negative value otherwise
int allDatasetsInGroup(hid_t file,
					   const char* path2group,
					   char*** path);

/// \returns int
///		- number of matching datasets when successfull
///		- negative value otherwise
int allMatchingDatasetsInGroup(hid_t file,
							   const char* path2group,
							   const char* filter,
							   char*** path);

#endif