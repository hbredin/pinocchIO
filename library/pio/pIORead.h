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

#ifndef _PINOCCHIO_READ_H
#define _PINOCCHIO_READ_H

#include "pIOTypes.h"

/**
	Reads data from dataset for a given timerange
	@param[in] pioDataset Dataset
	@param[in] timerangeIndex Index of timerange
	@param[in] pioDatatype Type of data 
	@param[out] buffer Data buffer
	@returns 
        - number of data when successful
        - -1 otherwise
 
    @ingroup dataset
 */
int pioReadData(PIODataset* pioDataset,
                int timerangeIndex,
                PIODatatype pioDatatype,
                void** buffer);

#define pioRead pioReadData


/**
	Reads number of data stored in dataset for a given timerange
	@param[in] pioDataset Dataset
	@param[in] timerangeIndex Index of timerange
	@returns 
        - number of data when successful
        - -1 otherwise
 
 @ingroup dataset
 */
int pioReadNumber(PIODataset pioDataset,
                  int timerangeIndex);

/**
	Dump whole dataset into buffer
 
    Dump whole dataset into buffer.
    Buffer has to be previously allocated with sufficient memory to contain whole dataset.
    Buffer size can be obtained by calling this function with NULL buffer first.
 
	@param[in] pioDataset Dataset
	@param[in] pioDatatype Buffer datatype
	@param[in, out] already_allocated_buffer Data buffer
	@returns 
        - -1 when something went bad
        - when succesful,
            -# if already_allocated_buffer == NULL, buffer size in bytes
            -# otherwise, total number of data
 
 @ingroup dataset
 */
int pioDumpDataset(PIODataset* pioDataset,
                   PIODatatype pioDatatype,
                   void* already_allocated_buffer);

#endif

