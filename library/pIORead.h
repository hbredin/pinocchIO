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

int pioRead(PIODataset* pioDataset, int timerangeIndex, 
            PIODatatype dataType, void** buffer);
#define pioReadData pioRead

int pioReadNumber(PIODataset pioDataset, int timerangeIndex);

#endif
