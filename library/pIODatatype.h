/*
 *  pIODatatype.h
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 20/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

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
int pioCloseDatatype( PIODatatype datatype );

PIODatatype pioGetDatatype(PIODataset pioDataset);

#endif