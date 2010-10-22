/*
 *  pioRead.h
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 22/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */


#ifndef _PINOCCHIO_READ_H
#define _PINOCCHIO_READ_H

#include "pIOTypes.h"

int pioReallocBuffer(PIODataset pioDataset, int timerangeIndex,
					 void** buffer, PIODatatype datatype);

int pioRead(PIODataset pioDataset, int timerangeIndex, 
			 void* buffer, PIODatatype dataType);

#endif
