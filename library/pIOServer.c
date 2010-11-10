/*
 *  pIOServer.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 09/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdlib.h>
#include <string.h>

#include "pIOServer.h"
#include "pIOFile.h"
#include "pIODataset.h"
#include "pIODatatype.h"
#include "pIOTimeline.h"
#include "pIORead.h"

PIOServer pioNewServer(const char** files, int nfiles, 
                       const char* path)
{
    int f = 0;
    PIODatatype pioCommonDatatype = PIODatatypeInvalid;
    PIOServer pioServer = PIOServerInvalid;
    
    // test file/dataset accessibility & similarity
    for (f=0; f<nfiles; f++)
    {
        pioServer.current_file = pioOpenFile(files[f], PINOCCHIO_READONLY);
        if (PIOFileIsInvalid(pioServer.current_file)) 
        {
            fprintf(stderr, "PIOServer > Cannot open file %s.\n", files[f]);
            fflush(stderr);
            pioCloseServer(&pioServer);
            return PIOServerInvalid;
        }
        
        pioServer.current_dataset = pioOpenDataset(PIOMakeObject(pioServer.current_file),
                                                   path);
        if (PIODatasetIsInvalid(pioServer.current_dataset))
        {
            fprintf(stderr, "PIOServer > Cannot open dataset %s in file %s.\n",
                    path, files[f]);
            fflush(stderr);
            pioCloseServer(&pioServer);
            return PIOServerInvalid;
        }
        
        pioServer.datatype = pioGetDatatype(pioServer.current_dataset);
        if (PIODatatypeIsInvalid(pioServer.datatype))
        {
            fprintf(stderr, "PIOServer > Cannot get datatype of dataset %s in file %s.\n",
                    path, files[f]);
            fflush(stderr);
            pioCloseServer(&pioServer);
            return PIOServerInvalid;            
        }
        
        if (f == 0)
        {
            pioCommonDatatype = pioNewDatatype(pioServer.datatype.type, 
                                               pioServer.datatype.dimension);
            if (PIODatatypeIsInvalid(pioCommonDatatype))
            {
                fprintf(stderr, "PIOServer > Invalid datatype of dataset %s in file %s.\n",
                        path, files[f]);
                fflush(stderr);
             
                pioCloseServer(&pioServer);
                return PIOServerInvalid;            
            }
        }
        else
        {
            if ((pioServer.datatype.dimension != pioCommonDatatype.dimension) ||
                (pioServer.datatype.type      != pioCommonDatatype.type))
            {
                fprintf(stderr, 
                        "PIOServer > Datatype of dataset %s in file %s do not match the one in file %s.\n",
                        path, files[f], files[0]);
                fflush(stderr);
                
                pioCloseDatatype(&pioCommonDatatype);
                pioCloseServer(&pioServer);
                return PIOServerInvalid;            
            }
        }
        pioCloseServer(&pioServer);
    }
    
    // initialize
    pioServer.nfiles = nfiles;
    pioServer.files = (char**) malloc(nfiles*sizeof(char*));
    for (f=0; f<nfiles; f++) {
        pioServer.files[f] = (char*) malloc((strlen(files[f])+1)*sizeof(char));
        sprintf(pioServer.files[f], "%s", files[f]);
    }
    
    pioServer.dataset = (char*) malloc((strlen(path)+1)*sizeof(char));
    sprintf(pioServer.dataset, "%s", path);
    
    pioServer.ntimeranges = (int*) malloc(nfiles*sizeof(int));
    pioServer.stored = (int*) malloc(nfiles*sizeof(int));
    for (f=0; f<nfiles; f++)
    {
        pioServer.current_file = pioOpenFile(files[f], PINOCCHIO_READONLY);
        if (PIOFileIsInvalid(pioServer.current_file)) 
        {
            fprintf(stderr, "PIOServer > Cannot open file %s.\n", files[f]);
            fflush(stderr);
            pioCloseServer(&pioServer);
            return PIOServerInvalid;
        }
        
        pioServer.current_dataset = pioOpenDataset(PIOMakeObject(pioServer.current_file), path);
        if (PIODatasetIsInvalid(pioServer.current_dataset))
        {
            fprintf(stderr, "PIOServer > Cannot open dataset %s in file %s.\n",
                    path, files[f]);
            fflush(stderr);
            pioCloseServer(&pioServer);
            return PIOServerInvalid;
        }
        
        pioServer.ntimeranges[f] = pioServer.current_dataset.ntimeranges;
        pioServer.stored[f] = pioServer.current_dataset.stored;
        
        pioCloseDataset(&(pioServer.current_dataset));
        pioCloseFile(&(pioServer.current_file));
    }
    
    pioServer.datatype = pioCommonDatatype;
    
    pioServer.current_file_index = 0;
    pioServer.current_file = PIOFileInvalid;
    pioServer.current_timerange_index = 0;
    pioServer.current_dataset = PIODatasetInvalid;
    pioServer.current_timeline = PIOTimelineInvalid;
    
    return pioServer;
}

int pioReadNext(PIOServer* server, PIODatatype pioDatatype, void** buffer)
{
    int number = -1;
    
    if (server->current_timerange_index == server->ntimeranges[server->current_file_index])
    {
        if (PIODatasetIsValid(server->current_dataset))
            pioCloseDataset(&(server->current_dataset));
        if (PIOFileIsValid(server->current_file))
            pioCloseFile(&(server->current_file));
        server->current_timerange_index = 0;
        server->current_file_index++;
    }
    
    if (server->current_file_index == server->nfiles)
    {
        server->current_file_index = 0;
        server->current_timerange_index = 0;
        return -1;
    }
    
    if (server->current_timerange_index == 0)
    {
        server->current_file = pioOpenFile(server->files[server->current_file_index],
                                           PINOCCHIO_READONLY);
        server->current_dataset = pioOpenDataset(PIOMakeObject(server->current_file),
                                                 server->dataset);
    }
        
    number = pioRead(&(server->current_dataset), server->current_timerange_index,
                     pioDatatype, buffer);
    
    server->current_timerange_index++;
    
    return number;
}

int pioCloseServer(PIOServer* pioServer)
{
    int f = 0;
    
    if (pioServer->files)
    {
        for (f=0; f<pioServer->nfiles; f++) {
            if (pioServer->files[f]) free(pioServer->files[f]); 
            pioServer->files[f] = NULL;
        }
        free(pioServer->files);
    }
    pioServer->files = NULL;
    
    if (pioServer->dataset) free(pioServer->dataset);
    pioServer->dataset = NULL;

    if (pioServer->ntimeranges) free(pioServer->ntimeranges);
    pioServer->ntimeranges = NULL;

    if (pioServer->stored) free(pioServer->stored);
    pioServer->stored = NULL;
    
    if (PIOTimelineIsValid(pioServer->current_timeline)) 
        pioCloseTimeline(&(pioServer->current_timeline));
    pioServer->current_timeline = PIOTimelineInvalid;
        
    if (PIODatasetIsValid(pioServer->current_dataset))
        pioCloseDataset(&(pioServer->current_dataset));
    pioServer->current_dataset = PIODatasetInvalid;
    
    if (PIOFileIsValid(pioServer->current_file)) 
        pioCloseFile(&(pioServer->current_file));
    pioServer->current_file = PIOFileInvalid;
    
    if (PIODatatypeIsValid(pioServer->datatype))
        pioCloseDatatype(&(pioServer->datatype));
    pioServer->datatype = PIODatatypeInvalid;
        
    return 1;
}

