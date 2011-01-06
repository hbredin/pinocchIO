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
	@brief Writes data into dataset for a given timerange.
 
	@param[in, out] pioDataset Dataset
	@param[in] timerangeIndex Index of timerange
	@param[in] dataBuffer Data buffer
	@param[in] dataNumber Number of data
	@param[in] dataType Type of data buffer
	@returns 
        - dataNumber when successful
        - -1 otherwise
 @ingroup dataset
 */
int pioWrite(PIODataset* pioDataset, int timerangeIndex,
			 void* dataBuffer, int dataNumber, PIODatatype dataType);


#endif


