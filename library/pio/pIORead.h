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
 @brief Read data stored in dataset for a given time range
 
 Update @a buffer so that it points to data stored in @a dataset for the time range at 
 position @a timerangeIndex in @a dataset timeline.

 @note
 pinocchIO uses an internal buffer to store the requested data. <b>Do not free it!</b>\n
 This buffer is modified (and possibly moved) by each call to pioReadData().\n
 Typically, one would call pioReadData() and then copy the buffer content 
 into another variable before calling pioReadData() again.
 
 @param[in,out] dataset pinocchIO dataset
 @param[in] timerangeIndex Index of timerange
 @param[in] datatype Buffer datatype
 @param[out] buffer Data buffer
  
 @note
 Buffer @a datatype does not have to match @a dataset datatype exactly.\n 
 While dimensions must be the same, @ref PIOBaseType "base types" can 
 be different: pinocchIO automatically performs the conversion when needed. 

 The content of the buffer is described by parameter @a datatype and return value @a number:
 - the N=@a number entries are stored in the buffer the one after the other...
 - ... as multi-dimensional arrays with the base type and dimension of @a datatype. 
 
 \image	html write.png 
 \image	latex write.eps 
 
 @returns
 - @a number of entries when successful
 - negative value otherwise
 
 @ingroup dataset
 */
int pioReadData(PIODataset* dataset,
                int timerangeIndex,
                PIODatatype datatype,
                void** buffer);

/**
	@brief Read data stored in dataset for a given time range.
    
    See pioReadData()
    @ingroup dataset
 */
#define pioRead pioReadData

/**
 @brief Get number of entries stored in dataset for a given time range
 
 Get number of entries stored in @a datasetfor the time range at 
 position @a timerangeIndex in @a dataset timeline.
 
 @param[in] dataset pinocchIO dataset
 @param[in] timerangeIndex Index of timerange
  
 @returns
 - @a number of entries when successful
 - negative value otherwise
 
 @ingroup dataset
 */
int pioReadNumber(PIODataset dataset,
                  int timerangeIndex);

/**
 @brief Dump whole dataset into buffer
 
 Dump whole @a dataset into previously allocated @a buffer.

 @param[in] dataset Dataset
 @param[in] datatype Buffer datatype
 @param[in, out] buffer Data buffer
 @param[in, out] number Number of entries per timerange
 
 Entries are stored in the buffer the one after the other,
 sorted in time range chronological order.
 Their number per timerange are stored in array @a number if it is not NULL.

 @note
 Buffer @a datatype does not have to match @a dataset datatype exactly.\n 
 While dimensions must be the same, @ref PIOBaseType "base types" can 
 be different: pinocchIO automatically performs the conversion when needed. 

 @note
 @a buffer has to be allocated with enough memory space to contain the whole
 @a dataset. A first call with a NULL buffer will return the required buffer
 size in bytes.
 
 @note
 @a number has to be allocated first. See example below.
  
 @returns 
    - required buffer size in bytes if successful if buffer!=NULL
    - total number of entries if successful and buffer!=NULL
    - a negative value otherwise
  
 \par Example
\verbatim
 // Get amount of memory needed to store the whole dataset
 size_t size = pioDumpDataset(dataset, datatype, NULL, NULL);
 // Allocate buffer for data and array for number of entries per timerange
 buffer = malloc(size);
 number = (int*)malloc(dataset.ntimeranges*sizeof(int));
 // Dump the whole dataset
 totalNumber = pioDumpDataset(dataset, datatype, buffer, number)
\endverbatim
 
 
 @ingroup dataset
 */
int pioDumpDataset(PIODataset* dataset,
                   PIODatatype datatype,
                   void* buffer,
                   int* number);

#endif

