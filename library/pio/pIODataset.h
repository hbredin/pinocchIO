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

#ifndef _PINOCCHIO_DATASET_H
#define _PINOCCHIO_DATASET_H

#include "pIOTypes.h"

#define PIODatasetIsValid(d)   (((d).identifier > 0) && ((d).link_identifier > 0))
#define PIODatasetIsInvalid(d) (!PIODatasetIsValid(d))

PIODataset pioNewDataset(PIOFile pioFile, 
						 const char* path, const char* description,
						 PIOTimeline pioTimeline,
						 PIODatatype pioDatatype); 

int pioRemoveDataset(PIOObject pioObject, const char* path);

PIODataset pioOpenDataset(PIOObject pioObject, const char* path);

int pioCloseDataset(PIODataset* pioDataset);


/// \returns int
///		- number of timelines in file when successfull
///		- negative value otherwise
int pioGetListOfDatasets(PIOFile pioFile, char*** pathsToDatasets);

#endif
