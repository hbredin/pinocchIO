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

#include "pIOTypes.h"


PIOServer pioNewServer(const char** files, int nfiles, 
                       const char* path);

int pioReadNext(PIOServer* server, PIODatatype pioDatatype, void** buffer);

int pioCloseServer( PIOServer* server );

#endif
