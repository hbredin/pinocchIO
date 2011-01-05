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


/**
 \defgroup server Server API
 \ingroup gepetto
 
 @{
 */


#ifndef _GEPETTO_SERVER_H
#define _GEPETTO_SERVER_H

#include <libconfig.h>
#include "gptTypes.h"

#define GPTServerIsInvalid(o)   ((o).numberOfDataFiles < 1)
#define GPTServerIsValid(o)     (!GPTServerIsInvalid(o))


GPTServer gptNewServer(int numberOfDataFiles, char** pathToDataFile, const char* pathToDataDataset,
                       GPTLabelFilterType labelFilterType, int labelFilterReference,
                       int numberOfLabelFiles, char** pathToLabelFile, const char* pathToLabelDataset);

int gptCloseServer(GPTServer* gptServer);

int gptGetServerDimension(GPTServer gptServer);

int gptReadNextData(GPTServer* gptServer,
                    PIODatatype pioDatatype,
                    void** buffer,
                    int* label);

// 2 steps usage:
//   - already_allocated_buffer = malloc(pioDumpServer(pioServer, pioDatatype, NULL))
//   - number = pioDumpServer(pioServer, pioDatatype, already_allocated_buffer)
//int gptDumpServer(GPTServer* server, 
//                  PIODatatype pioDatatype, 
//                  void* already_allocated_buffer);

#endif

/**
	@}
 */

