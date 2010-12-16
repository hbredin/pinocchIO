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
