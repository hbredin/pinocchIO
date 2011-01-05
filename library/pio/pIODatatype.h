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

#ifndef _PINOCCHIO_DATATYPE_H
#define _PINOCCHIO_DATATYPE_H

/**
 \defgroup datatype Datatype
 \ingroup dataset
 @{
 */


#include "pIOTypes.h"

#define PIODatatypeIsInvalid PIOObjectIsInvalid 
#define PIODatatypeIsValid   PIOObjectIsValid 

/**
	Creates a new array datatype
	@param type type of array elements
	@param dimension number of elements in array
	@returns pinocchIO array datatype
 */
PIODatatype pioNewDatatype( const PIOBaseType type, int dimension );

/**
	Closes a previously created datatype
	@param[in] datatype datatype 
	@returns 
        - 1 when successful
        - 0 otherwise
 */
int pioCloseDatatype( PIODatatype* datatype );


/**
	Get dataset datatype
	@param[in] pioDataset Dataset
	@returns 
        - datatype when successful
        - invalid datatype otherwise
 */
PIODatatype pioGetDatatype(PIODataset pioDataset);


/**
	Get size of datatype
	@param pioDatatype Datatype
	@returns 
        - size in bytes, when successful
        - 0 otherwise
 */
size_t pioGetSize(PIODatatype pioDatatype);


#endif

/**
	@}
 */


