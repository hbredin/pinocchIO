/*
 *  gptServer.h
 *  pinocchIO / gepetto
 *
 *  Created by Hervé BREDIN on 09/11/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

#ifndef _GEPETTO_SERVER_H
#define _GEPETTO_SERVER_H

#include <libconfig.h>

#include "gptTypes.h"

#define GPTServerIsInvalid(o)   ((o).nfiles < 1)
#define GPTServerIsValid(o)     (!GPTServerIsInvalid(o))


GPTServer gptNewServer(int numberOfDataFiles, char** pathToDataFile, const char* pathToDataDataset,
                       GPTLabelFilterType labelFilterType, int labelFilterReference,
                       int numberOfLabelFiles, char** pathToLabelFile, const char* pathToLabelDataset);


//GPTServer gptNewServerFromConfigurationFile(const char* path);

int gptCloseServer(GPTServer* gptServer);

int gptGetServerDimension(GPTServer gptServer);

//int gptReadNextData(GPTServer* server,
//                    PIODatatype pioDatatype,
//                    void** buffer);

// 2 steps usage:
//   - already_allocated_buffer = malloc(pioDumpServer(pioServer, pioDatatype, NULL))
//   - number = pioDumpServer(pioServer, pioDatatype, already_allocated_buffer)
//int gptDumpServer(GPTServer* server, 
//                  PIODatatype pioDatatype, 
//                  void* already_allocated_buffer);

#endif
