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

int pioReadData(PIODataset* pioDataset, 
                int timerangeIndex, 
                PIODatatype pioDatatype, 
                void** buffer);
#define pioRead pioReadData

int pioReadNumber(PIODataset pioDataset, 
                  int timerangeIndex);

// 2 steps usage:
//   - already_allocated_buffer = malloc(pioDumpDataset(pioDataset, pioDatatype, NULL))
//   - number = pioDumpDataset(pioDataset, pioDatatype, already_allocated_buffer)
int pioDumpDataset(PIODataset* pioDataset, 
                   PIODatatype pioDatatype, 
                   void* already_allocated_buffer);

#endif
