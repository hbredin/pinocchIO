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
 \defgroup datatype Datatype API
 \ingroup api
 
 @brief Functions dealing with pinocchIO datatypes

 @{
 */

#include "pIOTypes.h"

/**
 @brief Check datatype invalidity
 
 Check whether the pinocchIO datatype handle is invalid.
 
 @param[in] pioDatatype pinocchIO datatype handle
 @returns
 - TRUE if the pinocchIO datatype handle is invalid
 - FALSE otherwise
 */
#define PIODatatypeIsInvalid PIOObjectIsInvalid 

/**
 @brief Check datatype validity
 
 Check whether the pinocchIO datatype handle is valid.
 
 @param[in] pioDatatype pinocchIO datatype handle
 @returns
 - TRUE if the pinocchIO datatype handle is valid
 - FALSE otherwise
 */
#define PIODatatypeIsValid   PIOObjectIsValid 

/**
 @brief Create new pinocchIO datatype
 
 Create a new pinocchIO array datatype of dimension \a dimension and type \a type.
  
 @param[in] type Type of array elements (see \ref PIOBaseType for more information)
 @param[in] dimension Number of elements in array.
 @returns 
 - a pinocchIO datatype handle when successful
 - \ref PIODatatypeInvalid otherwise
 
 @note
 Use pioCloseDatatype() to close the datatype when no longer needed. 
 */
PIODatatype pioNewDatatype( const PIOBaseType type, int dimension );

/**
 @brief Close pinocchIO datatype
 
 Close a previously opened pinocchIO datatype.
 Upon success, the pinocchIO datatype handle will be set to \ref PIODatatypeInvalid.
 
 @param[in,out] pioDatatype pinocchIO datatype handle
 @returns
 - 1 when file is successfully closed
 - 0 otherwise
 */
int pioCloseDatatype( PIODatatype* pioDatatype );

/**
 @brief Get size of datatype
    
 Get the size in bytes of a pinocchIO datatype.
 This is basically the size in bytes of one element with base type, multiplied
 by the dimension of the array.
 
 For instance, the size of pioNewDatatype( PINOCCHIO_TYPE_FLOAT, 3) equals
 3*sizeof(float).
 
 @param[in] pioDatatype pinocchIO datatype
 @returns 
    - datatype size in bytes, when successful
    - 0 otherwise
 */
size_t pioGetSize(PIODatatype pioDatatype);


/**
 @}
 */

/**
 @brief Get pinocchIO datatype of pinocchIO dataset
 
 Get the pinocchIO datatype of the pinocchIO dataset.
 
 @param[in] pioDataset  pinocchIO dataset handle
 @returns
 - a pinocchIO datatype handle when successful
 - \ref PIODatatypeInvalid otherwise
 
 @note
 Use pioCloseDatatype() to close the datatype when no longer needed. 
 
 @ingroup dataset
 */
PIODatatype pioGetDatatype(PIODataset pioDataset);

#endif


