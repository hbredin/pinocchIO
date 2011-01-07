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

#ifndef _PINOCCHIO_WRITE_H
#define _PINOCCHIO_WRITE_H

#include "pIOTypes.h"

/**
 @brief Write data into dataset for a given time range
 
 Write data stored in @a buffer into pinocchIO @a dataset for the time range
 at position @a timerangeIndex in @a dataset timeline. 

 \image	html write.png 
 \image	latex write.eps 
 
 @param[in, out] dataset pinocchIO dataset
 @param[in] timerangeIndex Index of timerange
 @param[in] buffer Data buffer
 @param[in] number Number N of entries in buffer
 @param[in] datatype Buffer datatype

 The content of the buffer is described by parameters @a number and @a datatype:
 - the N=@a number entries are stored in the buffer the one after the other...
 - ... as multi-dimensional arrays with the base type and dimension of @a datatype.
 
 @note
 Buffer @a datatype does not have to match @a dataset datatype exactly.\n 
 While dimensions must be the same, @ref PIOBaseType "base types" can 
 be different: pinocchIO automatically performs the conversion when needed. 

 @returns 
 - number when successful
 - negative value otherwise
 
 \par Example
\verbatim
 // dataset is meant to store entries as 3-dimensional integer arrays
 PIODatatype datasetDatatype = pioNewDatatype( PINOCCHIO_TYPE_INT, 3);
 PIODataset dataset = pioNewDataset(..., datatype);
 
 // buffer contains entries as 3-dimensional float arrays
 PIODatatype bufferDatatype = pioNewDatatype( PINOCCHIO_TYPE_FLOAT, 3);
 
 // buffer contains 2 entries
 // 1st entry: [ 1.2 3.5  4.2]
 // 2nd entry: [32.0 3.14 6.2]
 int number = 2;
 float buffer[6] = {1.2, 3.5, 4.2, 32.0, 3.14, 6.2};
 
 // Buffer entries are written into dataset for the 17th time range
 pioWrite(&dataset, 17, buffer, number, bufferDatatype);
 // Conversion from float to integer is automatic.
 // 17th time range now contains 2 entries:
 // 1st entry: [ 1 3 4]
 // 2nd entry: [32 3 6]
 
 pioCloseDatatype(&bufferDatatype);
 pioCloseDatatype(&datasetDatatype);
 pioCloseDataset(&dataset);
\endverbatim
  
 @note
 In case the @a dataset already contains data for the selected time range, 
 pioWrite() will overwrite the existing data.
 
 @ingroup dataset
 */
int pioWrite(PIODataset* dataset, int timerangeIndex,
			 void* buffer, int number, PIODatatype datatype);


#endif


