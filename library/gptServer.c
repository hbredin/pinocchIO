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

#include "gptServer.h"

//GPTServer gptNewServerFromConfigurationFile(const char* filename)
//{
//    config_t config;
//    const config_setting_t *config_setting = NULL;
//    
//    char** files = NULL;
//    int nfiles = 0;
//    int f, ff;
//    const char* cur_file = NULL;
//    const char* cur_dataset = NULL;
//    char* dataset = NULL;
//    
//    GPTServer server = GPTServerInvalid;
//    config_init(&config);
//    config_read_file(&config, filename);
//    if (config_lookup_string(&config, "dataset", &cur_dataset) == CONFIG_FALSE)
//    {
//        fprintf(stderr, "Could not find path to dataset in configuration file %s.\n",
//                filename);
//        config_destroy(&config);
//        return GPTServerInvalid;        
//    }
//    
//    dataset = (char*)malloc((strlen(cur_dataset)+1)*sizeof(char));
//    sprintf(dataset, "%s", cur_dataset);
//    
//    config_setting = config_lookup(&config, "files");
//    if (config_setting)
//    {
//        if (config_setting_is_list(config_setting) != CONFIG_TRUE)
//        {
//            fprintf(stderr, "Files must be provided as a list of files.\n");
//            fprintf(stderr, "Example: files = ( \"file1\", \"file2\" );");
//            config_destroy(&config);
//            return GPTServerInvalid;            
//        }
//        
//        nfiles = config_setting_length(config_setting);
//        files = (char**) malloc(nfiles * sizeof(char*));
//        for (f=0; f<nfiles; f++)
//        {
//            cur_file = config_setting_get_string_elem(config_setting, f);
//            
//            if (!cur_file)
//            {
//                fprintf(stderr, 
//                        "There must be something wrong with %dth file in list.\n", 
//                        f+1);
//                for (ff=0; ff<f; ff++) free(files[ff]); free(files);
//                config_destroy(&config);
//                return GPTServerInvalid;                            
//            }
//
//            files[f] = (char*) malloc((strlen(cur_file)+1)*sizeof(char));
//            sprintf(files[f], "%s", cur_file);
//        }
//    }
//    else
//    {
//        fprintf(stderr, "Could not find list of files in configuration file %s.\n",
//                filename);
//        config_destroy(&config);
//        return GPTServerInvalid;
//    }
//
//    config_destroy(&config);
//    
//    server = gptNewServer(files, nfiles, dataset);
//
//    for (f=0; f<nfiles; f++) free(files[f]); free(files);
//    free(dataset);
//    
//    return server;
//}


int isFiltered(int label, 
               GPTLabelFilterType labelFilterType,
               int labelFilterReference)
{
    switch (labelFilterType)
    {
        case GEPETTO_LABEL_FILTER_TYPE_NONE:
            return 1;
            break;
        case GEPETTO_LABEL_FILTER_TYPE_EQUALS_TO:
            return (label == labelFilterReference);
            break;
        case GEPETTO_LABEL_FILTER_TYPE_DIFFERS_FROM:
            return (label != labelFilterReference);
            break;
        case GEPETTO_LABEL_FILTER_TYPE_GREATER_THAN:
            return (label > labelFilterReference);
            break;
        case GEPETTO_LABEL_FILTER_TYPE_SMALLER_THAN:
            return (label < labelFilterReference);
            break;
        default:
            break;
    }
    return -1;
}

GPTServer gptNewServer(int numberOfDataFiles, char** pathToDataFile, const char* pathToDataDataset,
                       GPTLabelFilterType labelFilterType, int labelFilterReference,
                       int numberOfLabelFiles, char** pathToLabelFile, const char* pathToLabelDataset)
{
    int f = 0; // file counter
    int t = 0; // timerange counter
    int label_t = 0; // label timerange counter
    int data_t = 0;  // data timerange counter
    
    GPTServer gptServer = GPTServerInvalid;
    
    // if a label filter is applied, make sure there is as many data files than label files
    if ((labelFilterType != GEPETTO_LABEL_FILTER_TYPE_NONE) && 
        (numberOfDataFiles != numberOfLabelFiles))
        return GPTServerInvalid;
    
    // if a label filter is applied
    if (labelFilterType != GEPETTO_LABEL_FILTER_TYPE_NONE)
    {
        // initialize server label stuff
        
        gptServer.numberOfLabelFiles = numberOfLabelFiles;
        gptServer.pathToLabelFile = (char**) malloc(gptServer.numberOfLabelFiles*sizeof(char*));        
        for (f=0; f<gptServer.numberOfLabelFiles; f++) gptServer.pathToLabelFile[f] = NULL;
        
        gptServer.pathToLabelDataset = (char*) malloc((strlen(pathToLabelDataset)+1)*sizeof(char));
        sprintf(gptServer.pathToLabelDataset, "%s", pathToLabelDataset);
        
        gptServer.labelTimeline         = (PIOTimeRange**) malloc(gptServer.numberOfLabelFiles*sizeof(PIOTimeRange*));
        for (f=0; f<gptServer.numberOfLabelFiles; f++) gptServer.labelTimeline[f] = NULL;

        gptServer.lengthOfLabelTimeline = (int*)  malloc(gptServer.numberOfLabelFiles*sizeof(int));
        gptServer.label                 = (int**) malloc(gptServer.numberOfLabelFiles*sizeof(int*));   
        for (f=0; f<gptServer.numberOfLabelFiles; f++) gptServer.label[f] = NULL;
        
        gptServer.labelDatatype = pioNewDatatype(PINOCCHIO_TYPE_INT, 1);        
        
        for (f=0; f<gptServer.numberOfLabelFiles; f++)
        {
            gptServer.pathToLabelFile[f] = (char*) malloc((strlen(pathToLabelFile[f])+1)*sizeof(char));
            sprintf(gptServer.pathToLabelFile[f], "%s", pathToLabelFile[f]);
            
            // check if label file is readable
            gptServer.current_file = pioOpenFile(gptServer.pathToLabelFile[f], PINOCCHIO_READONLY);
            if (PIOFileIsInvalid(gptServer.current_file)) 
            {
                fprintf(stderr, "Geppeto cannot open label file %s.\n", gptServer.pathToLabelFile[f]);
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;
            }
            
            // check if label dataset is readable
            gptServer.current_dataset = pioOpenDataset(PIOMakeObject(gptServer.current_file),
                                                       gptServer.pathToLabelDataset);
            if (PIODatasetIsInvalid(gptServer.current_dataset))
            {
                fprintf(stderr, "Gepetto cannot open label dataset %s in file %s.\n",
                        gptServer.pathToLabelDataset, gptServer.pathToLabelFile[f]);
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;
            }
            
            // check if label datatype is mono-dimensional
            gptServer.current_datatype = pioGetDatatype(gptServer.current_dataset);
            if (PIODatatypeIsInvalid(gptServer.current_datatype))
            {
                fprintf(stderr, "Gepetto cannot get datatype of label dataset %s in file %s.\n",
                        gptServer.pathToLabelDataset, gptServer.pathToLabelFile[f]);
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;            
            }
            
            if (gptServer.current_datatype.dimension != 1)
            {
                fprintf(stderr, 
                        "Gepetto found that the datatype of label dataset %s in file %s is not mono-dimensional.\n",
                        gptServer.pathToLabelDataset, gptServer.pathToLabelFile[f]);
                fflush(stderr);                
                gptCloseServer(&gptServer);
                return GPTServerInvalid;            
            }
            
            // load label timeline
            gptServer.current_timeline = pioGetTimeline(gptServer.current_dataset);
            if (PIOTimelineIsInvalid(gptServer.current_timeline))
            {
                fprintf(stderr, "Geppeto cannot open timeline of label dataset %s in file %s.\n",
                        gptServer.pathToLabelDataset, gptServer.pathToLabelFile[f]);
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;
                
            }
            
            gptServer.lengthOfLabelTimeline[f] = gptServer.current_timeline.ntimeranges;
            gptServer.labelTimeline[f] = (PIOTimeRange*) malloc(gptServer.lengthOfLabelTimeline[f]*sizeof(PIOTimeRange));
            for (t=0; t<gptServer.lengthOfLabelTimeline[f]; t++) 
                gptServer.labelTimeline[f][t] = gptServer.current_timeline.timeranges[t];
            
            // load label data
            gptServer.label[f] = (int*) malloc(gptServer.lengthOfLabelTimeline[f]*sizeof(int));
            if (pioDumpDataset(&(gptServer.current_dataset),
                               gptServer.labelDatatype, gptServer.label[f]) != gptServer.lengthOfLabelTimeline[f])
            {
                fprintf(stderr, 
                        "Gepetto cannot load as many label as timeranges from label dataset %s in file %s.\n",
                        gptServer.pathToLabelDataset, gptServer.pathToLabelFile[f]);
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;
            }
            
            // prepare for next file
            pioCloseTimeline(&(gptServer.current_timeline));
            pioCloseDatatype(&(gptServer.current_datatype));
            pioCloseDataset(&(gptServer.current_dataset));
            pioCloseFile(&(gptServer.current_file));
        }
    }

    // initialize data-related stuff
    gptServer.numberOfDataFiles = numberOfDataFiles;
    gptServer.pathToDataFile = (char**) malloc(gptServer.numberOfDataFiles*sizeof(char*));
    for (f=0; f<gptServer.numberOfDataFiles; f++) gptServer.pathToDataFile[f] = NULL;

    gptServer.pathToDataDataset = (char*) malloc((strlen(pathToDataDataset)+1)*sizeof(char));
    sprintf(gptServer.pathToDataDataset, "%s", pathToDataDataset);
    
    gptServer.dataTimeline = (PIOTimeRange**) malloc(gptServer.numberOfDataFiles*sizeof(PIOTimeRange*));
    for (f=0; f<gptServer.numberOfDataFiles; f++) gptServer.dataTimeline[f] = NULL;

    gptServer.lengthOfDataTimeline = (int*) malloc(gptServer.numberOfDataFiles*sizeof(int));
    
    // test, initialize and (partially, timeline only) load data
    for (f=0; f<gptServer.numberOfDataFiles; f++)
    {
        gptServer.pathToDataFile[f] = (char*) malloc((strlen(pathToDataFile[f])+1)*sizeof(char));
        sprintf(gptServer.pathToDataFile[f], "%s", pathToDataFile[f]);
        
        // test fth data file
        gptServer.current_file = pioOpenFile(gptServer.pathToDataFile[f], PINOCCHIO_READONLY);
        if (PIOFileIsInvalid(gptServer.current_file)) 
        {
            fprintf(stderr, "Geppeto cannot open data file %s.\n", gptServer.pathToDataFile[f]);
            fflush(stderr);
            gptCloseServer(&gptServer);
            return GPTServerInvalid;
        }
        
        // test fth data dataset
        gptServer.current_dataset = pioOpenDataset(PIOMakeObject(gptServer.current_file),
                                                   gptServer.pathToDataDataset);
        if (PIODatasetIsInvalid(gptServer.current_dataset))
        {
            fprintf(stderr, "Gepetto cannot open data dataset %s in file %s.\n",
                    gptServer.pathToDataDataset, gptServer.pathToDataFile[f]);
            fflush(stderr);
            gptCloseServer(&gptServer);
            return GPTServerInvalid;
        }
        
        // test fth data datatype
        gptServer.current_datatype = pioGetDatatype(gptServer.current_dataset);
        if (PIODatatypeIsInvalid(gptServer.current_datatype))
        {
            fprintf(stderr, "Gepetto cannot get datatype of data dataset %s in file %s.\n",
                    gptServer.pathToDataDataset, gptServer.pathToDataFile[f]);
            fflush(stderr);
            gptCloseServer(&gptServer);
            return GPTServerInvalid;            
        }
        
        if (f == 0)
        {
            // initialize data datatype
            gptServer.datatype = pioNewDatatype(gptServer.current_datatype.type,
                                                gptServer.current_datatype.dimension);
            if (PIODatatypeIsInvalid(gptServer.datatype))
            {
                fprintf(stderr, "Gepetto found that datatype of dataset %s in first file %s is invalid.\n",
                        gptServer.pathToDataDataset, gptServer.pathToDataFile[f]);
                fflush(stderr);
                
                gptCloseServer(&gptServer);
                return GPTServerInvalid;            
            }
        }
        else
        {
            // test fth label datatype
            if ((gptServer.current_datatype.dimension != gptServer.datatype.dimension) ||
                (gptServer.current_datatype.type      != gptServer.datatype.type))
            {
                fprintf(stderr, 
                        "Gepetto found that the datatype of dataset %s in file %s does not match the one in first file %s.\n",
                        gptServer.pathToDataDataset, gptServer.pathToDataFile[f], gptServer.pathToDataFile[0]);
                fflush(stderr);                
                gptCloseServer(&gptServer);
                return GPTServerInvalid;            
            }
        }
        
        // load data timeline
        gptServer.current_timeline = pioGetTimeline(gptServer.current_dataset);
        if (PIOTimelineIsInvalid(gptServer.current_timeline))
        {
            fprintf(stderr, "Geppeto cannot open timeline of data dataset %s in file %s.\n",
                    gptServer.pathToDataDataset, gptServer.pathToDataFile[f]);
            fflush(stderr);
            gptCloseServer(&gptServer);
            return GPTServerInvalid;
            
        }
        gptServer.lengthOfDataTimeline[f] = gptServer.current_timeline.ntimeranges;
        gptServer.dataTimeline[f] = (PIOTimeRange*) malloc(gptServer.lengthOfDataTimeline[f]*sizeof(PIOTimeRange));
        for (t=0; t<gptServer.lengthOfDataTimeline[f]; t++) 
            gptServer.dataTimeline[f][t] = gptServer.current_timeline.timeranges[t];
        
        // prepare for next file
        pioCloseTimeline(&(gptServer.current_timeline));
        pioCloseDatatype(&(gptServer.current_datatype));
        pioCloseDataset(&(gptServer.current_dataset));
        pioCloseFile(&(gptServer.current_file));
    }
    
    // initialize filter mask 
    gptServer.filterMask = (int**) malloc(gptServer.numberOfDataFiles*sizeof(int*));
    gptServer.labelFilterType = labelFilterType;
    gptServer.labelFilterReference = labelFilterReference;
    
    for (f=0; f<gptServer.numberOfDataFiles; f++) 
    {
        gptServer.filterMask[f] = (int*) malloc(gptServer.lengthOfDataTimeline[f]*sizeof(int));
        if (labelFilterType == GEPETTO_LABEL_FILTER_TYPE_NONE)
        {
            for (data_t=0; data_t<gptServer.lengthOfDataTimeline[f]; data_t++) 
                gptServer.filterMask[f][data_t] = 1;
        }
        else 
        {
            label_t = 0;
            for (data_t=0; data_t<gptServer.lengthOfDataTimeline[f]; data_t++) 
            {
                // look for first label timerange intersecting data timerange
                while (
                       // not yet reached the end of label timeline
                       (label_t < gptServer.lengthOfLabelTimeline[f])
                       &&
                       // label timerange is before data timerange
                       (pioCompareTimeRanges(gptServer.labelTimeline[f][label_t], 
                                             gptServer.dataTimeline[f][data_t]) != PINOCCHIO_TIMERANGE_COMPARISON_DESCENDING) 
                       &&
                       // label timerange do not intersect data timerange
                       (!pioTimeRangeIntersectsTimeRange(gptServer.labelTimeline[f][label_t], 
                                                         gptServer.dataTimeline[f][data_t]))
                       )
                {
                    // try next label
                    label_t++;
                }
                
                // if found, update mask based on 
                if (pioTimeRangeIntersectsTimeRange(gptServer.labelTimeline[f][label_t], 
                                                    gptServer.dataTimeline[f][data_t]))
                {
                    gptServer.filterMask[f][t] = isFiltered(gptServer.label[f][label_t], 
                                                            gptServer.labelFilterType, 
                                                            gptServer.labelFilterReference);
                }
                else 
                {
                    gptServer.filterMask[f][data_t] = -1;
                }
            }
        }
    }
    
    gptServer.current_file_index = 0;
    gptServer.current_file = PIOFileInvalid;
    gptServer.current_timerange_index = 0;
    gptServer.current_dataset = PIODatasetInvalid;
    gptServer.current_timeline = PIOTimelineInvalid;
    
    return gptServer;
}




//GPTServer gptNewServer(char** pathToLabelFile, int numberOfDataFile, const char* pathToLabelDataset)
//{
//    int f = 0;
//    PIODatatype pioCommonDatatype = PIODatatypeInvalid;
//    GPTServer gptServer = GPTServerInvalid;
//    
//    // test file/dataset accessibility & similarity
//    for (f=0; f<numberOfDataFile; f++)
//    {
//        gptServer.current_file = pioOpenFile(pathToLabelFile[f], PINOCCHIO_READONLY);
//        if (PIOFileIsInvalid(gptServer.current_file)) 
//        {
//            fprintf(stderr, "PIOServer > Cannot open file %s.\n", pathToLabelFile[f]);
//            fflush(stderr);
//            gptCloseServer(&gptServer);
//            return GPTServerInvalid;
//        }
//        
//        gptServer.current_dataset = pioOpenDataset(PIOMakeObject(gptServer.current_file),
//                                                   pathToLabelDataset);
//        if (PIODatasetIsInvalid(gptServer.current_dataset))
//        {
//            fprintf(stderr, "PIOServer > Cannot open dataset %s in file %s.\n",
//                    pathToLabelDataset, pathToLabelFile[f]);
//            fflush(stderr);
//            gptCloseServer(&gptServer);
//            return GPTServerInvalid;
//        }
//        
//        gptServer.datatype = pioGetDatatype(gptServer.current_dataset);
//        if (PIODatatypeIsInvalid(gptServer.datatype))
//        {
//            fprintf(stderr, "PIOServer > Cannot get datatype of dataset %s in file %s.\n",
//                    pathToLabelDataset, pathToLabelFile[f]);
//            fflush(stderr);
//            gptCloseServer(&gptServer);
//            return GPTServerInvalid;            
//        }
//        
//        if (f == 0)
//        {
//            pioCommonDatatype = pioNewDatatype(gptServer.datatype.type, 
//                                               gptServer.datatype.dimension);
//            if (PIODatatypeIsInvalid(pioCommonDatatype))
//            {
//                fprintf(stderr, "PIOServer > Invalid datatype of dataset %s in file %s.\n",
//                        pathToLabelDataset, pathToLabelFile[f]);
//                fflush(stderr);
//             
//                gptCloseServer(&gptServer);
//                return GPTServerInvalid;            
//            }
//        }
//        else
//        {
//            if ((gptServer.datatype.dimension != pioCommonDatatype.dimension) ||
//                (gptServer.datatype.type      != pioCommonDatatype.type))
//            {
//                fprintf(stderr, 
//                        "PIOServer > Datatype of dataset %s in file %s do not match the one in file %s.\n",
//                        pathToLabelDataset, pathToLabelFile[f], pathToLabelFile[0]);
//                fflush(stderr);
//                
//                pioCloseDatatype(&pioCommonDatatype);
//                gptCloseServer(&gptServer);
//                return GPTServerInvalid;            
//            }
//        }
//        gptCloseServer(&gptServer);
//    }
//    
//    // initialize
//    gptServer.nfiles = numberOfDataFile;
//    gptServer.files = (char**) malloc(numberOfDataFile*sizeof(char*));
//    for (f=0; f<numberOfDataFile; f++) {
//        gptServer.files[f] = (char*) malloc((strlen(pathToLabelFile[f])+1)*sizeof(char));
//        sprintf(gptServer.files[f], "%s", pathToLabelFile[f]);
//    }
//    
//    gptServer.dataset = (char*) malloc((strlen(pathToLabelDataset)+1)*sizeof(char));
//    sprintf(gptServer.dataset, "%s", pathToLabelDataset);
//    
//    gptServer.ntimeranges = (int*) malloc(numberOfDataFile*sizeof(int));
//    gptServer.stored = (int*) malloc(numberOfDataFile*sizeof(int));
//    for (f=0; f<numberOfDataFile; f++)
//    {
//        gptServer.current_file = pioOpenFile(pathToLabelFile[f], PINOCCHIO_READONLY);
//        if (PIOFileIsInvalid(gptServer.current_file)) 
//        {
//            fprintf(stderr, "PIOServer > Cannot open file %s.\n", pathToLabelFile[f]);
//            fflush(stderr);
//            gptCloseServer(&gptServer);
//            return GPTServerInvalid;
//        }
//        
//        gptServer.current_dataset = pioOpenDataset(PIOMakeObject(gptServer.current_file), pathToLabelDataset);
//        if (PIODatasetIsInvalid(gptServer.current_dataset))
//        {
//            fprintf(stderr, "PIOServer > Cannot open dataset %s in file %s.\n",
//                    pathToLabelDataset, pathToLabelFile[f]);
//            fflush(stderr);
//            gptCloseServer(&gptServer);
//            return GPTServerInvalid;
//        }
//        
//        gptServer.ntimeranges[f] = gptServer.current_dataset.ntimeranges;
//        gptServer.stored[f] = gptServer.current_dataset.stored;
//        
//        pioCloseDataset(&(gptServer.current_dataset));
//        pioCloseFile(&(gptServer.current_file));
//    }
//    
//    gptServer.datatype = pioCommonDatatype;
//    
//    gptServer.current_file_index = 0;
//    gptServer.current_file = PIOFileInvalid;
//    gptServer.current_timerange_index = 0;
//    gptServer.current_dataset = PIODatasetInvalid;
//    gptServer.current_timeline = PIOTimelineInvalid;
//    
//    return gptServer;
//}

int gptGetServerDimension(GPTServer gptServer)
{
    return gptServer.datatype.dimension;
}

int gptReadNextData(GPTServer* gptServer, PIODatatype pioDatatype, void** buffer)
{
    int number = -1;
    
    if (gptServer->current_timerange_index == gptServer->lengthOfDataTimeline[gptServer->current_file_index])
    {
        if (PIODatasetIsValid(gptServer->current_dataset))
            pioCloseDataset(&(gptServer->current_dataset));
        if (PIOFileIsValid(gptServer->current_file))
            pioCloseFile(&(gptServer->current_file));
        gptServer->current_timerange_index = 0;
        gptServer->current_file_index++;
    }
    
    if (gptServer->current_file_index == gptServer->numberOfDataFiles)
    {
        gptServer->current_file_index = 0;
        gptServer->current_timerange_index = 0;
        return -1;
    }
    
    if (gptServer->current_timerange_index == 0)
    {
        gptServer->current_file = pioOpenFile(gptServer->pathToDataFile[gptServer->current_file_index],
                                           PINOCCHIO_READONLY);
        gptServer->current_dataset = pioOpenDataset(PIOMakeObject(gptServer->current_file),
                                                 gptServer->pathToDataDataset);
    }
    
    if (gptServer->filterMask[gptServer->current_file_index][gptServer->current_timerange_index] == 1)
    {
        number = pioRead(&(gptServer->current_dataset), 
                         gptServer->current_timerange_index,
                         pioDatatype, 
                         buffer);
    }
    else 
    {
        gptServer->current_timerange_index++;
        number = gptReadNextData(gptServer, pioDatatype, buffer);
    }
    
    return number;
}

//int gptDumpServer(GPTServer* gptServer, 
//                  PIODatatype pioDatatype, 
//                  void* already_allocated_buffer)
//{
//    int ds = 0;
//    
//    int tmp_number = -1;
//    int number = 0;
//        
//    size_t tmp_size = -1;
//    size_t size = 0;
//    
//    if (already_allocated_buffer)
//    {
//        for (ds=0; ds<gptServer->nfiles; ds++) 
//        {
//            gptServer->current_file = pioOpenFile(gptServer->files[ds], 
//                                                  PINOCCHIO_READONLY);
//            gptServer->current_dataset = pioOpenDataset(PIOMakeObject(gptServer->current_file), 
//                                                        gptServer->dataset);
//            
//            // dump dataset at correct position
//            tmp_number = pioDumpDataset(&(gptServer->current_dataset), 
//                                        pioDatatype, 
//                                        already_allocated_buffer+size);
//            
//            // close what needs to be closed
//            pioCloseDataset(&(gptServer->current_dataset));
//            pioCloseFile(&(gptServer->current_file));
//            
//            if (tmp_number < 0) return -1; // stop if something bad happened
//            
//            tmp_size = tmp_number*pioGetSize(pioDatatype);
//            size = size + tmp_size;
//            
//            number = number + tmp_number;
//        }    
//    }
//    else 
//    {
//        for (ds=0; ds<gptServer->nfiles; ds++) 
//        {
//            gptServer->current_file = pioOpenFile(gptServer->files[ds], 
//                                                  PINOCCHIO_READONLY);
//            gptServer->current_dataset = pioOpenDataset(PIOMakeObject(gptServer->current_file), 
//                                                        gptServer->dataset);
//            
//            // dump dataset at correct position
//            tmp_size = pioDumpDataset(&(gptServer->current_dataset), 
//                                        pioDatatype, 
//                                        NULL);
//            
//            // close what needs to be closed
//            pioCloseDataset(&(gptServer->current_dataset));
//            pioCloseFile(&(gptServer->current_file));
//            
//            if (tmp_size < 0) return -1; // stop if something bad happened
//            
//            size = size + tmp_size;            
//        }        
//        
//        return size;
//    }
//    
//    return number;
//}


int gptCloseServer(GPTServer* gptServer)
{
    int f = 0;
    
    // free pathToDataFile
    if (gptServer->pathToDataFile)
    {
        for (f=0; f<gptServer->numberOfDataFiles; f++) {
            if (gptServer->pathToDataFile[f]) free(gptServer->pathToDataFile[f]); 
            gptServer->pathToDataFile[f] = NULL;
        }
        free(gptServer->pathToDataFile);
    }
    gptServer->pathToDataFile = NULL;
    
    // free pathToDataDataset
    if (gptServer->pathToDataDataset) free(gptServer->pathToDataDataset);
    gptServer->pathToDataDataset = NULL;

    // free dataTimeline
    if (gptServer->dataTimeline)
    {
        for (f=0; f<gptServer->numberOfDataFiles; f++) {
            if (gptServer->dataTimeline[f]) free(gptServer->dataTimeline[f]); 
            gptServer->dataTimeline[f] = NULL;
        }
        free(gptServer->dataTimeline);
    }
    gptServer->dataTimeline = NULL;
    
    // free lengthOfDataTimeline
    if (gptServer->lengthOfDataTimeline) free(gptServer->lengthOfDataTimeline);
    gptServer->lengthOfDataTimeline = NULL;

    
    
    // free pathToLabelFile
    if (gptServer->pathToLabelFile)
    {
        for (f=0; f<gptServer->numberOfLabelFiles; f++) {
            if (gptServer->pathToLabelFile[f]) free(gptServer->pathToLabelFile[f]); 
            gptServer->pathToLabelFile[f] = NULL;
        }
        free(gptServer->pathToLabelFile);
    }
    gptServer->pathToLabelFile = NULL;
    
    // free pathToLabelDataset
    if (gptServer->pathToLabelDataset) free(gptServer->pathToLabelDataset);
    gptServer->pathToLabelDataset = NULL;
    
    // free labelTimeline
    if (gptServer->labelTimeline)
    {
        for (f=0; f<gptServer->numberOfLabelFiles; f++) {
            if (gptServer->labelTimeline[f]) free(gptServer->labelTimeline[f]); 
            gptServer->labelTimeline[f] = NULL;
        }
        free(gptServer->labelTimeline);
    }
    gptServer->labelTimeline = NULL;
    
    // free lengthOfLabelTimeline
    if (gptServer->lengthOfLabelTimeline) free(gptServer->lengthOfLabelTimeline);
    gptServer->lengthOfLabelTimeline = NULL;
    
    
    // free label
    if (gptServer->label)
    {
        for (f=0; f<gptServer->numberOfLabelFiles; f++) {
            if (gptServer->label[f]) free(gptServer->label[f]); 
            gptServer->label[f] = NULL;
        }
        free(gptServer->label);
    }
    gptServer->label = NULL;
    

    // free filterMask
    if (gptServer->filterMask)
    {
        for (f=0; f<gptServer->numberOfDataFiles; f++) {
            if (gptServer->filterMask[f]) free(gptServer->filterMask[f]); 
            gptServer->filterMask[f] = NULL;
        }
        free(gptServer->filterMask);
    }
    gptServer->filterMask = NULL;
    
    // free datatype
    if (PIODatatypeIsValid(gptServer->datatype))
        pioCloseDatatype(&(gptServer->datatype));
    
    // free labelDatatype
    if (PIODatatypeIsValid(gptServer->labelDatatype))
        pioCloseDatatype(&(gptServer->labelDatatype));
    
    // free current_file
    if (PIOFileIsValid(gptServer->current_file)) 
        pioCloseFile(&(gptServer->current_file));
        
    // free current_dataset
    if (PIODatasetIsValid(gptServer->current_dataset))
        pioCloseDataset(&(gptServer->current_dataset));
    
    // free current_datatype
    if (PIODatatypeIsValid(gptServer->current_datatype))
        pioCloseDatatype(&(gptServer->current_datatype));

    // free current_timeline
    if (PIOTimelineIsValid(gptServer->current_timeline)) 
        pioCloseTimeline(&(gptServer->current_timeline));
    
    // set to GPTServerInvalid
    *gptServer = GPTServerInvalid;
    
    return 1;
}

