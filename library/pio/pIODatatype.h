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

#include "pIOTypes.h"

#define PIODatatypeIsInvalid PIOObjectIsInvalid 
#define PIODatatypeIsValid   PIOObjectIsValid 

/// Creates new pinocchIO datatype. 
///
/// Creates a new datatype as an dimension-D array of type type
///
/// \param[in] type	Datatype of array elements.
/// \param[in] dimension	Dimension of array.
///
/// \returns	PIODatatype
///		- new datatype handle when successful
///		- negative value otherwise.
///
PIODatatype pioNewDatatype( const PIOBaseType type, int dimension );

/// Closes a previously created datatype. 
///
/// \param[in] datatype	Datatype handle
///
/// \returns	int
///		- positive value when successful
///		- negative value otherwise.
///
int pioCloseDatatype( PIODatatype* datatype );

PIODatatype pioGetDatatype(PIODataset pioDataset);

size_t pioGetSize(PIODatatype pioDatatype);

#endif
