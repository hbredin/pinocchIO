/*
 *  pIODataset.h
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 19/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

#ifndef _PINOCCHIO_DATASET_H
#define _PINOCCHIO_DATASET_H

#include "pIOTypes.h"

#define PIODatasetIsValid(d)   (((d).identifier > 0) && ((d).count_identifier > 0))
#define PIODatasetIsInvalid(d) (!PIODatasetIsValid(d))

PIODataset pioNewDataset(PIOFile pioFile, 
						 const char* path, const char* description,
						 PIOTimeline pioTimeline,
						 PIODatatype pioDatatype); 


PIODataset pioOpenDataset(PIOObject pioObject, const char* path);

int pioCloseDataset(PIODataset pioDataset);


/// \returns int
///		- number of timelines in file when successfull
///		- negative value otherwise
int pioGetListOfDatasets(PIOFile pioFile, char*** pathsToDatasets);


#endif
