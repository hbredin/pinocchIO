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

#include <stdlib.h>
#include <string.h>

#include "gptServer.h"
#include "list_utils.h"

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
    int label_t = 0; // label timerange counter
    int data_t = 0;  // data timerange counter
    
    GPTServer gptServer = GPTServerInvalid;
    
    gptServer.servesData = (numberOfDataFiles > 0);
    gptServer.servesLabels = (numberOfLabelFiles > 0);
    gptServer.labelFilterType = labelFilterType;
    gptServer.labelFilterReference = labelFilterReference;
    
    // if server serves data AND label
    // make sure there is as many data files than label files
    if (gptServer.servesData && gptServer.servesLabels)
    {
        if (numberOfDataFiles != numberOfLabelFiles)
        {
            fprintf(stderr, "Number of data files (%d) and number of label files (%d) do not match.\n",
                    numberOfDataFiles, numberOfLabelFiles);
            fflush(stderr);
            gptCloseServer(&gptServer);
            return GPTServerInvalid;            
        }
    }
    
    // if server applies filters on labels
    // make sure server serves labels
    if ((gptServer.labelFilterType != GEPETTO_LABEL_FILTER_TYPE_UNDEFINED) && 
        (!gptServer.servesLabels))
    {
        fprintf(stderr, "Cannot apply filter if no label is available.\n");
        fflush(stderr);
        gptCloseServer(&gptServer);
        return GPTServerInvalid;
    }
            
    // initialize server label stuff
    if (gptServer.servesLabels)
    {
        // set number of labels files
        gptServer.numberOfLabelFiles = numberOfLabelFiles;
        // store their paths
        gptServer.pathToLabelFile = (char**) malloc(gptServer.numberOfLabelFiles*sizeof(char*));        
        for (f=0; f<gptServer.numberOfLabelFiles; f++) gptServer.pathToLabelFile[f] = NULL;
        // store the path to labels dataset
        gptServer.pathToLabelDataset = (char*) malloc((strlen(pathToLabelDataset)+1)*sizeof(char));
        sprintf(gptServer.pathToLabelDataset, "%s", pathToLabelDataset);
        
        // label timelines
        gptServer.labelTimeline         = (PIOTimeRange**) malloc(gptServer.numberOfLabelFiles*sizeof(PIOTimeRange*));
        for (f=0; f<gptServer.numberOfLabelFiles; f++) gptServer.labelTimeline[f] = NULL;
        gptServer.lengthOfLabelTimeline = (int*)  malloc(gptServer.numberOfLabelFiles*sizeof(int));
        // labels
        gptServer.label                 = (int**) malloc(gptServer.numberOfLabelFiles*sizeof(int*));   
        for (f=0; f<gptServer.numberOfLabelFiles; f++) gptServer.label[f] = NULL;
        // labels are integers
        gptServer.labelDatatype = pioNewDatatype(PINOCCHIO_TYPE_INT, 1);        
        // label counts
        gptServer.labelCountsTotal = NULL;
        gptServer.labelCountsPerFile = (listOfLabels_t**) malloc(gptServer.numberOfLabelFiles*sizeof(listOfLabels_t*));
        for (f=0; f<gptServer.numberOfLabelFiles; f++) gptServer.labelCountsPerFile[f] = NULL;
        
        for (f=0; f<gptServer.numberOfLabelFiles; f++)
        {
            // store path to label file
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
            for (label_t=0; label_t<gptServer.lengthOfLabelTimeline[f]; label_t++) 
                gptServer.labelTimeline[f][label_t] = gptServer.current_timeline.timeranges[label_t];
            
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
            
            // update label counts
            for (label_t=0; label_t<gptServer.lengthOfLabelTimeline[f]; label_t++) 
            {
                gptServer.labelCountsPerFile[f] = updateLabelCount(gptServer.labelCountsPerFile[f],
                                                                   gptServer.label[f][label_t],
                                                                   1);
                gptServer.labelCountsTotal      = updateLabelCount(gptServer.labelCountsTotal,
                                                                   gptServer.label[f][label_t],
                                                                   1);
            }
            
            // prepare for next file
            pioCloseTimeline(&(gptServer.current_timeline));
            pioCloseDatatype(&(gptServer.current_datatype));
            pioCloseDataset(&(gptServer.current_dataset));
            pioCloseFile(&(gptServer.current_file));
        }        
    }

    if (gptServer.servesData)
    {
        // initialize data-related stuff
        gptServer.numberOfDataFiles = numberOfDataFiles;
        gptServer.pathToDataFile = (char**) malloc(gptServer.numberOfDataFiles*sizeof(char*));
        for (f=0; f<gptServer.numberOfDataFiles; f++) gptServer.pathToDataFile[f] = NULL;
        
        gptServer.pathToDataDataset = (char*) malloc((strlen(pathToDataDataset)+1)*sizeof(char));
        sprintf(gptServer.pathToDataDataset, "%s", pathToDataDataset);
        
        gptServer.dataTimeline = (PIOTimeRange**) malloc(gptServer.numberOfDataFiles*sizeof(PIOTimeRange*));
        for (f=0; f<gptServer.numberOfDataFiles; f++) gptServer.dataTimeline[f] = NULL;
        
        gptServer.lengthOfDataTimeline = (int*) malloc(gptServer.numberOfDataFiles*sizeof(int));
        
        // label counts
        gptServer.dataCountsPerLabelTotal = NULL;
        gptServer.dataCountsPerLabelPerFile = (listOfLabels_t**) malloc(gptServer.numberOfDataFiles*sizeof(listOfLabels_t*));
        for (f=0; f<gptServer.numberOfDataFiles; f++) gptServer.dataCountsPerLabelPerFile[f] = NULL;
        
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
            for (data_t=0; data_t<gptServer.lengthOfDataTimeline[f]; data_t++) 
                gptServer.dataTimeline[f][data_t] = gptServer.current_timeline.timeranges[data_t];
            
            // prepare for next file
            pioCloseTimeline(&(gptServer.current_timeline));
            pioCloseDatatype(&(gptServer.current_datatype));
            pioCloseDataset(&(gptServer.current_dataset));
            pioCloseFile(&(gptServer.current_file));
        }

        // initialize filter mask and propagate labels
        gptServer.propagatedLabel = (int**) malloc(gptServer.numberOfDataFiles*sizeof(int*));
        gptServer.filterMask = (int**) malloc(gptServer.numberOfDataFiles*sizeof(int*));
        
        for (f=0; f<gptServer.numberOfDataFiles; f++) 
        {
            gptServer.propagatedLabel[f] = (int*) malloc(gptServer.lengthOfDataTimeline[f]*sizeof(int));
            gptServer.filterMask[f] = (int*) malloc(gptServer.lengthOfDataTimeline[f]*sizeof(int));
            if (gptServer.labelFilterType == GEPETTO_LABEL_FILTER_TYPE_UNDEFINED)
            {
                for (data_t=0; data_t<gptServer.lengthOfDataTimeline[f]; data_t++) 
                {
                    gptServer.propagatedLabel[f][data_t] = -1;
                    gptServer.filterMask[f][data_t] = 1;
                }
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
                        gptServer.propagatedLabel[f][data_t] = gptServer.label[f][label_t];
                        gptServer.filterMask[f][data_t] = isFiltered(gptServer.label[f][label_t], 
                                                                     gptServer.labelFilterType, 
                                                                     gptServer.labelFilterReference);
                    }
                    else 
                    {
                        gptServer.propagatedLabel[f][data_t] = -1;
                        gptServer.filterMask[f][data_t] = -1;
                    }
                    
                    gptServer.dataCountsPerLabelPerFile[f] = updateLabelCount(gptServer.dataCountsPerLabelPerFile[f],
                                                                       gptServer.propagatedLabel[f][data_t],
                                                                       1);
                    gptServer.dataCountsPerLabelTotal      = updateLabelCount(gptServer.dataCountsPerLabelTotal,
                                                                       gptServer.propagatedLabel[f][data_t],
                                                                       1);
                }
            }
        }
    }
        
    gptServer.current_file_index = 0;
    gptServer.current_file = PIOFileInvalid;
    gptServer.current_timerange_index = 0;
    gptServer.current_dataset = PIODatasetInvalid;
    gptServer.current_timeline = PIOTimelineInvalid;
    
    gptServer.eof = 0;
    
    return gptServer;
}

// get data dimension
int gptGetServerDimension(GPTServer gptServer)
{
    if (gptServer.servesData)
        return gptServer.datatype.dimension;
    else
        return -1;
}

int gptReadNext(GPTServer* gptServer, PIODatatype pioDatatype, void** buffer, int* label)
{
    int number = -1;
    
    if (!gptServer->servesData) return -1;
    
    // if last timerange is processed, go to first timerange of next file
    if (gptServer->current_timerange_index == gptServer->lengthOfDataTimeline[gptServer->current_file_index])
    {
        if (PIODatasetIsValid(gptServer->current_dataset))
            pioCloseDataset(&(gptServer->current_dataset));
        if (PIOFileIsValid(gptServer->current_file))
            pioCloseFile(&(gptServer->current_file));
        gptServer->current_timerange_index = 0;
        gptServer->current_file_index++;
    }
    
    // if last file is processed, stop
    if (gptServer->current_file_index == gptServer->numberOfDataFiles)
    {
        gptServer->current_file_index = 0;
        gptServer->current_timerange_index = 0;
        gptServer->eof = 1;
        return -1;
    }
    
    // open next file/dataset if necessary
    if (gptServer->current_timerange_index == 0)
    {
        gptServer->current_file = pioOpenFile(gptServer->pathToDataFile[gptServer->current_file_index],
                                           PINOCCHIO_READONLY);
        gptServer->current_dataset = pioOpenDataset(PIOMakeObject(gptServer->current_file),
                                                 gptServer->pathToDataDataset);
    }
    
    // if not filtered, read next data
    if (gptServer->filterMask[gptServer->current_file_index][gptServer->current_timerange_index])
    {
        number = pioRead(&(gptServer->current_dataset), 
                         gptServer->current_timerange_index,
                         pioDatatype, 
                         buffer);
        if (label) 
            *label = gptServer->propagatedLabel[gptServer->current_file_index][gptServer->current_timerange_index];
        gptServer->current_timerange_index++;
    }
    // otherwise, go to next timerange
    else 
    {
        gptServer->current_timerange_index++;
        number = gptReadNext(gptServer, pioDatatype, buffer, label);
    }
    
    // return number of data
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
    
    // free data counts
    if (gptServer->dataCountsPerLabelTotal)
        destroyListOfLabels(gptServer->dataCountsPerLabelTotal);
    gptServer->dataCountsPerLabelTotal = NULL;
    
    if (gptServer->dataCountsPerLabelPerFile)
    {
        for (f=0; f<gptServer->numberOfDataFiles; f++) {
            destroyListOfLabels(gptServer->dataCountsPerLabelPerFile[f]);
            gptServer->dataCountsPerLabelPerFile[f] = NULL;
        }
        free(gptServer->dataCountsPerLabelPerFile);
    }
    gptServer->dataCountsPerLabelPerFile = NULL;
    
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

    // free label counts
    if (gptServer->labelCountsTotal)
        destroyListOfLabels(gptServer->labelCountsTotal);
    gptServer->labelCountsTotal = NULL;
    
    if (gptServer->labelCountsPerFile)
    {
        for (f=0; f<gptServer->numberOfLabelFiles; f++) {
            destroyListOfLabels(gptServer->labelCountsPerFile[f]);
            gptServer->labelCountsPerFile[f] = NULL;
        }
        free(gptServer->labelCountsPerFile);
    }
    gptServer->labelCountsPerFile = NULL;
    
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
    
    // free propagatedLabel
    if (gptServer->propagatedLabel)
    {
        for (f=0; f<gptServer->numberOfDataFiles; f++) {
            if (gptServer->propagatedLabel[f]) free(gptServer->propagatedLabel[f]); 
            gptServer->propagatedLabel[f] = NULL;
        }
        free(gptServer->propagatedLabel);
    }
    gptServer->propagatedLabel = NULL;
    
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

int gptPrintStatistics(GPTServer server, FILE* file)
{    
    int f, i;
    int numberOfLabels = 0;
    int* labels = NULL;
    listOfLabels_t* labelCounts = NULL;
    
    numberOfLabels = numberOfLabelsInList(server.labelCountsTotal);
    labels = (int*) malloc(numberOfLabels*sizeof(int));
    labelCounts = server.labelCountsTotal;
    i = 0;
    while (labelCounts) 
    {
        labels[i] = labelCounts->value;
        labelCounts = labelCounts->next;
        i++;
    }
    
    for (i=0; i<numberOfLabels; i++)
        fprintf(file, "-------");
    fprintf(file, "  ");
    for (i=0; i<numberOfLabels; i++)
        fprintf(file, "-------");
    fprintf(file, "\n");
    
    for (i=0; i<numberOfLabels; i++)
        fprintf(file, "%7d", labels[i]);
    fprintf(file, "  ");
    for (i=0; i<numberOfLabels; i++)
        fprintf(file, "%7d", labels[i]);
    fprintf(file, "\n");
    
    for (i=0; i<numberOfLabels; i++)
        fprintf(file, "-------");
    fprintf(file, "  ");
    for (i=0; i<numberOfLabels; i++)
        fprintf(file, "-------");
    fprintf(file, "\n");
    
    for (i=0; i<numberOfLabels; i++)
    {
        listOfLabels_t* element = isLabelInList(server.labelCountsTotal, labels[i]);
        fprintf(file, "%7d", element->count);
    }
    fprintf(file, "  ");
    for (i=0; i<numberOfLabels; i++)
    {
        listOfLabels_t* element = isLabelInList(server.dataCountsPerLabelTotal, labels[i]);
        fprintf(file, "%7d", element->count);
    }
    fprintf(file, "\n");
    
    for (i=0; i<numberOfLabels; i++)
        fprintf(file, "-------");
    fprintf(file, "  ");
    for (i=0; i<numberOfLabels; i++)
        fprintf(file, "-------");
    fprintf(file, "\n");
    

    for (f=0; f<server.numberOfLabelFiles; f++) {
        for (i=0; i<numberOfLabels; i++)
        {
            listOfLabels_t* element = isLabelInList(server.labelCountsPerFile[f], labels[i]);
            if (!element)
                fprintf(file, "     --");
            else
                fprintf(file, "%7d", element->count);
        }
        fprintf(file, "\n");
        
    }
    
    return 1;
}

