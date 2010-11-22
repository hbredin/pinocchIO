/*
 *  pIOWrite.h
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 22/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

#ifndef _PINOCCHIO_WRITE_H
#define _PINOCCHIO_WRITE_H

#include "pIOTypes.h"

int pioWrite(PIODataset* pioDataset, int timerangeIndex, 
			 void* dataBuffer, int dataNumber, PIODatatype dataType);

#endif
