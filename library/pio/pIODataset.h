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


/**
 \defgroup dataset Dataset API
 \ingroup api
 @{
 */

#ifndef _PINOCCHIO_DATASET_H
#define _PINOCCHIO_DATASET_H

#include "pIOTypes.h"

#define PIODatasetIsValid(d)   (((d).identifier > 0) && ((d).link_identifier > 0))
#define PIODatasetIsInvalid(d) (!PIODatasetIsValid(d))

/**
	Creates new dataset in file
	@param pioFile pinocchIO file in which dataset is created
	@param path internal path to dataset in pinocchIO file
	@param description dataset textual description
	@param pioTimeline dataset timeline
	@param pioDatatype dataset datatype
	@returns
        - valid dataset if successful
        - invalid dataset otherwise
 */
PIODataset pioNewDataset(PIOFile pioFile,
						 const char* path, const char* description,
						 PIOTimeline pioTimeline,
						 PIODatatype pioDatatype);

/**
	Remove dataset from file
	@param pioObject any object included in affected file
	@param path internal path to removed dataset
	@returns 
        - 1 if dataset is successfully removed
        - 0 otherwise
 */
int pioRemoveDataset(PIOObject pioObject, const char* path);

/**
	Open dataset 
	@param pioObject any object included in file containing requested dataset
	@param path internal path to dataset
	@returns 
        - valid dataset if successful
        - invalid dataset otherwise
 */
PIODataset pioOpenDataset(PIOObject pioObject, const char* path);

/**
	Close dataset
	@param pioDataset pointer to dataset
	@returns
        - 1 when successful
        - 0 otherwise
 */
int pioCloseDataset(PIODataset* pioDataset);

/**
	Get list of datasets
	@param pioFile pinocchIO file
	@param pathsToDatasets pointer to list of dataset paths
	@returns number of datasets in file
 */
int pioGetListOfDatasets(PIOFile pioFile, char*** pathsToDatasets);

#endif


/**
	@}
 */

