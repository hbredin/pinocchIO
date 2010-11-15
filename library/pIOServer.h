/*
 *  pioServer.h
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 09/11/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

#ifndef _PINOCCHIO_SERVER_H
#define _PINOCCHIO_SERVER_H

#include <libconfig.h>

#include "pIOTypes.h"

#define PIOServerIsInvalid(o)   ((o).nfiles < 1)
#define PIOServerIsValid(o)     (!PIOServerIsInvalid(o))

PIOServer pioNewServer(char** files, int nfiles, 
                       const char* path);
PIOServer pioNewServerFromConfigurationFile(const char* path);

int pioCloseServer( PIOServer* server );

int pioGetServerDimension(PIOServer server);

int pioReadNextData(PIOServer* server,
                    PIODatatype pioDatatype,
                    void** buffer);

// 2 steps usage:
//   - already_allocated_buffer = malloc(pioDumpServer(pioServer, pioDatatype, NULL))
//   - number = pioDumpServer(pioServer, pioDatatype, already_allocated_buffer)
int pioDumpServer(PIOServer* server, 
                  PIODatatype pioDatatype, 
                  void* already_allocated_buffer);


#endif
