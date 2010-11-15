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

PIOServer pioNewServerFromConfigurationFile(const char* filename)
{
    config_t config;
    const config_setting_t *config_setting = NULL;
    
    char** files = NULL;
    int nfiles = 0;
    int f, ff;
    const char* cur_file = NULL;
    const char* cur_dataset = NULL;
    char* dataset = NULL;
    
    PIOServer server = PIOServerInvalid;
    config_init(&config);
    config_read_file(&config, filename);
    if (config_lookup_string(&config, "dataset", &cur_dataset) == CONFIG_FALSE)
    {
        fprintf(stderr, "Could not find path to dataset in configuration file %s.\n",
                filename);
        config_destroy(&config);
        return PIOServerInvalid;        
    }
    
    dataset = (char*)malloc((strlen(cur_dataset)+1)*sizeof(char));
    sprintf(dataset, "%s", cur_dataset);
    
    config_setting = config_lookup(&config, "files");
    if (config_setting)
    {
        if (config_setting_is_list(config_setting) != CONFIG_TRUE)
        {
            fprintf(stderr, "Files must be provided as a list of files.\n");
            fprintf(stderr, "Example: files = ( \"file1\", \"file2\" );");
            config_destroy(&config);
            return PIOServerInvalid;            
        }
        
        nfiles = config_setting_length(config_setting);
        files = (char**) malloc(nfiles * sizeof(char*));
        for (f=0; f<nfiles; f++)
        {
            cur_file = config_setting_get_string_elem(config_setting, f);
            
            if (!cur_file)
            {
                fprintf(stderr, 
                        "There must be something wrong with %dth file in list.\n", 
                        f+1);
                for (ff=0; ff<f; ff++) free(files[ff]); free(files);
                config_destroy(&config);
                return PIOServerInvalid;                            
            }

            files[f] = (char*) malloc((strlen(cur_file)+1)*sizeof(char));
            sprintf(files[f], "%s", cur_file);
        }
    }
    else
    {
        fprintf(stderr, "Could not find list of files in configuration file %s.\n",
                filename);
        config_destroy(&config);
        return PIOServerInvalid;
    }

    config_destroy(&config);
    
    server = pioNewServer(files, nfiles, dataset);

    for (f=0; f<nfiles; f++) free(files[f]); free(files);
    free(dataset);
    
    return server;
}

PIOServer pioNewServer(char** files, int nfiles, 
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

int pioGetServerDimension(PIOServer server)
{
    return server.datatype.dimension;
}

int pioReadNextData(PIOServer* server, PIODatatype pioDatatype, void** buffer)
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

int pioDumpServer(PIOServer* pioServer, 
                  PIODatatype pioDatatype, 
                  void* already_allocated_buffer)
{
    int ds = 0;
    
    int tmp_number = -1;
    int number = 0;
        
    size_t tmp_size = -1;
    size_t size = 0;
    
    if (already_allocated_buffer)
    {
        for (ds=0; ds<pioServer->nfiles; ds++) 
        {
            pioServer->current_file = pioOpenFile(pioServer->files[ds], 
                                                  PINOCCHIO_READONLY);
            pioServer->current_dataset = pioOpenDataset(PIOMakeObject(pioServer->current_file), 
                                                        pioServer->dataset);
            
            // dump dataset at correct position
            tmp_number = pioDumpDataset(&(pioServer->current_dataset), 
                                        pioDatatype, 
                                        already_allocated_buffer+size);
            
            // close what needs to be closed
            pioCloseDataset(&(pioServer->current_dataset));
            pioCloseFile(&(pioServer->current_file));
            
            if (tmp_number < 0) return -1; // stop if something bad happened
            
            tmp_size = tmp_number * H5Tget_size(pioDatatype.identifier);
            size = size + tmp_size;
            
            number = number + tmp_number;
        }    
    }
    else 
    {
        for (ds=0; ds<pioServer->nfiles; ds++) 
        {
            pioServer->current_file = pioOpenFile(pioServer->files[ds], 
                                                  PINOCCHIO_READONLY);
            pioServer->current_dataset = pioOpenDataset(PIOMakeObject(pioServer->current_file), 
                                                        pioServer->dataset);
            
            // dump dataset at correct position
            tmp_size = pioDumpDataset(&(pioServer->current_dataset), 
                                        pioDatatype, 
                                        NULL);
            
            // close what needs to be closed
            pioCloseDataset(&(pioServer->current_dataset));
            pioCloseFile(&(pioServer->current_file));
            
            if (tmp_size < 0) return -1; // stop if something bad happened
            
            size = size + tmp_size;            
        }        
        
        return size;
    }
    
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

