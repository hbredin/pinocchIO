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

static int isFiltered(int* labels, 
                      int nLabels,
                      GPTLabelFilterType labelFilterType,
                      int labelFilterReference)
{
    int i;
    int result = 0;
    
    for (i=0; i<nLabels; i++) 
    {
        switch (labelFilterType)
        {
            case GEPETTO_LABEL_FILTER_TYPE_NONE:
                result = result || 1;
                break;
            case GEPETTO_LABEL_FILTER_TYPE_EQUALS_TO:
                result = result || (labels[i] == labelFilterReference);
                break;
            case GEPETTO_LABEL_FILTER_TYPE_DIFFERS_FROM:
                result = result || (labels[i] != labelFilterReference);
                break;
            case GEPETTO_LABEL_FILTER_TYPE_GREATER_THAN:
                result = result || (labels[i] > labelFilterReference);
                break;
            case GEPETTO_LABEL_FILTER_TYPE_SMALLER_THAN:
                result = result || (labels[i] < labelFilterReference);
                break;
            default:
                break;
        }        
    }
    
    return result;
}

static int compareLabels (void const *a, void const *b)
{
    int const *pa = a;
    int const *pb = b;    
    return *pa - *pb;
}

static int getStatsOnLabels( GPTServer server, int* labels, int* labelCounts, int** labelCountsPerFile)
{
    int f, t, i;
    int count;
    listOfLabels_t* list = NULL;
    listOfLabels_t** listPerFile = NULL;
    
    listPerFile = (listOfLabels_t**) malloc(LBL_NFILES(server)*sizeof(listOfLabels_t*));
    
    count = 0; // number of distinct labels so far
    for(f=0; f<LBL_NFILES(server); f++)
    {
        listPerFile[f] = NULL;
        for(t=0; t<LBL_NTIMERANGES(server, f); t++)
        {
            for(i=0; i<LBL_NLABELS(server, f, t); i++)
            {
                if (!isLabelInList(list, LBL_LABEL(server, f, t, i)))
                {
                    if (labels) labels[count] = LBL_LABEL(server, f, t, i);
                    count++;
                }
                list           = updateLabelCount(list,           LBL_LABEL(server, f, t, i), 1);
                listPerFile[f] = updateLabelCount(listPerFile[f], LBL_LABEL(server, f, t, i), 1);

            }
        }
    }
    
    if (labels)
    {
        // sort labels
        qsort(labels, count, sizeof(int), compareLabels);
        if (labelCounts)
        {
            for (i=0; i<count; i++)
                labelCounts[i] = getLabelCount(list, labels[i]);
        }
        
        if (labelCountsPerFile)
        {
            for (f=0; f<LBL_NFILES(server); f++)
                for (i=0; i<count; i++)
                    labelCountsPerFile[f][i] = getLabelCount(listPerFile[f], labels[i]);
        }
    }
    
    destroyListOfLabels(list);
    for (f=0; f<LBL_NFILES(server); f++) 
        destroyListOfLabels(listPerFile[f]);
    free(listPerFile);
    
    return count;
}



GPTServer gptNewServer(int numberOfDataFiles, char** pathToDataFile, const char* pathToDataDataset,
                       GPTLabelFilterType labelFilterType, int labelFilterReference,
                       int numberOfLabelFiles, char** pathToLabelFile, const char* pathToLabelDataset)
{
    int f = 0; // file counter
    int label_t = 0; // label timerange counter
    int data_t = 0;  // data timerange counter
    
    size_t labelBufferSize = -1;
    
    GPTServer gptServer = GPTServerInvalid;
    
    gptServer.servesData = (numberOfDataFiles > 0);
    DAT_NFILES(gptServer) = numberOfDataFiles;
    gptServer.servesLabels = (numberOfLabelFiles > 0);
    // set number of labels files
    LBL_NFILES(gptServer) = numberOfLabelFiles;
    gptServer.labelFilterType = labelFilterType;
    gptServer.labelFilterReference = labelFilterReference;
    
    // if server serves data AND label
    // make sure there is as many data files than label files
    if (DAT_AVAILABLE(gptServer) && LBL_AVAILABLE(gptServer))
    {
        if (DAT_NFILES(gptServer) != LBL_NFILES(gptServer))
        {
            fprintf(stderr, "Number of data files (%d) and number of label files (%d) do not match.\n",
                    DAT_NFILES(gptServer), LBL_NFILES(gptServer));
            fflush(stderr);
            gptCloseServer(&gptServer);
            return GPTServerInvalid;            
        }
    }
    
    // if server applies filters on labels
    // make sure server serves labels
    if ((gptServer.labelFilterType != GEPETTO_LABEL_FILTER_TYPE_UNDEFINED) && 
        (!LBL_AVAILABLE(gptServer)))
    {
        fprintf(stderr, "Cannot apply filter if no label is available.\n");
        fflush(stderr);
        gptCloseServer(&gptServer);
        return GPTServerInvalid;
    }
            
    // initialize server label stuff
    if (LBL_AVAILABLE(gptServer))
    {
        // store their paths
        LBL_PATHS(gptServer) = (char**) malloc(LBL_NFILES(gptServer)*sizeof(char*));        
        for (f=0; f<LBL_NFILES(gptServer); f++) LBL_PATH(gptServer, f) = NULL;
        
        // store the path to labels dataset
        LBL_DATASET(gptServer) = (char*) malloc((strlen(pathToLabelDataset)+1)*sizeof(char));
        sprintf(LBL_DATASET(gptServer), "%s", pathToLabelDataset);
        
        // label timelines
        LBL_TIMELINES(gptServer) = (PIOTimeRange**) malloc(LBL_NFILES(gptServer)*sizeof(PIOTimeRange*));
        for (f=0; f<LBL_NFILES(gptServer); f++) LBL_TIMELINE(gptServer,f) = NULL;
        gptServer.lengthOfLabelTimeline = (int*) malloc(LBL_NFILES(gptServer)*sizeof(int));
        
        // labels
        gptServer.label                 = (int**) malloc(LBL_NFILES(gptServer)*sizeof(int*));   
        for (f=0; f<LBL_NFILES(gptServer); f++) gptServer.label[f] = NULL;
        
        // Number of labels per file per timerange
        gptServer.numberOfLabelsPerFilePerTimerange        = (int**) malloc(LBL_NFILES(gptServer)*sizeof(int*));
        for (f=0; f<LBL_NFILES(gptServer); f++) gptServer.numberOfLabelsPerFilePerTimerange[f] = NULL;
        
        // Index of first label per file per timerange
        gptServer.indexOfFirstLabelPerFilePerTimerange        = (int**) malloc(LBL_NFILES(gptServer)*sizeof(int*));
        for (f=0; f<LBL_NFILES(gptServer); f++) gptServer.indexOfFirstLabelPerFilePerTimerange[f] = NULL;
        
        // labels are integers
        gptServer.labelDatatype = pioNewDatatype(PINOCCHIO_TYPE_INT, 1);        
//        // label counts
//        gptServer.labelCountsTotal = NULL;
//        gptServer.labelCountsPerFile = (listOfLabels_t**) malloc(LBL_NFILES(gptServer)*sizeof(listOfLabels_t*));
//        for (f=0; f<LBL_NFILES(gptServer); f++) gptServer.labelCountsPerFile[f] = NULL;
        
        for (f=0; f<LBL_NFILES(gptServer); f++)
        {
            // store path to label file
            LBL_PATH(gptServer, f) = (char*) malloc((strlen(pathToLabelFile[f])+1)*sizeof(char));
            sprintf(LBL_PATH(gptServer, f), "%s", pathToLabelFile[f]);
            
            // check if label file is readable
            gptServer.current_file = pioOpenFile(LBL_PATH(gptServer, f), PINOCCHIO_READONLY);
            if (PIOFileIsInvalid(gptServer.current_file)) 
            {
                fprintf(stderr, "Geppeto cannot open label file %s.\n", LBL_PATH(gptServer, f));
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;
            }
            
            // check if label dataset is readable
            gptServer.current_dataset = pioOpenDataset(PIOMakeObject(gptServer.current_file),
                                                       LBL_DATASET(gptServer));
            if (PIODatasetIsInvalid(gptServer.current_dataset))
            {
                fprintf(stderr, "Gepetto cannot open label dataset %s in file %s.\n",
                        LBL_DATASET(gptServer), LBL_PATH(gptServer, f));
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;
            }
            
            // check if label datatype is mono-dimensional
            gptServer.current_datatype = pioGetDatatype(gptServer.current_dataset);
            if (PIODatatypeIsInvalid(gptServer.current_datatype))
            {
                fprintf(stderr, "Gepetto cannot get datatype of label dataset %s in file %s.\n",
                        LBL_DATASET(gptServer), LBL_PATH(gptServer, f));
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;            
            }
            if (gptServer.current_datatype.dimension != 1)
            {
                fprintf(stderr, 
                        "Gepetto found that the datatype of label dataset %s in file %s is not mono-dimensional.\n",
                        LBL_DATASET(gptServer), LBL_PATH(gptServer, f));
                fflush(stderr);                
                gptCloseServer(&gptServer);
                return GPTServerInvalid;            
            }
            
            // load label timeline
            gptServer.current_timeline = pioGetTimeline(gptServer.current_dataset);
            if (PIOTimelineIsInvalid(gptServer.current_timeline))
            {
                fprintf(stderr, "Geppeto cannot open timeline of label dataset %s in file %s.\n",
                        LBL_DATASET(gptServer), LBL_PATH(gptServer, f));
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;
                
            }
            LBL_NTIMERANGES(gptServer, f) = gptServer.current_timeline.ntimeranges;
            LBL_TIMELINE(gptServer, f) = (PIOTimeRange*) malloc(LBL_NTIMERANGES(gptServer, f)*sizeof(PIOTimeRange));
            for (label_t=0; label_t<LBL_NTIMERANGES(gptServer, f); label_t++) 
                LBL_TIMERANGE(gptServer, f, label_t) = gptServer.current_timeline.timeranges[label_t];
            
            // load label data
            labelBufferSize = pioDumpDataset(&(gptServer.current_dataset), gptServer.labelDatatype, NULL, NULL);
            if (labelBufferSize <= 0)
            {
                fprintf(stderr, 
                        "Gepetto cannot find any label in dataset %s in file %s.\n",
                        LBL_DATASET(gptServer), LBL_PATH(gptServer, f));
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;                
            }
            
            gptServer.label[f] = (int*) malloc(labelBufferSize);
            gptServer.numberOfLabelsPerFilePerTimerange[f] = (int*) malloc(LBL_NTIMERANGES(gptServer, f)*sizeof(int));

            if (pioDumpDataset(&(gptServer.current_dataset), gptServer.labelDatatype, gptServer.label[f], gptServer.numberOfLabelsPerFilePerTimerange[f]) < 0)
            {
                fprintf(stderr, 
                        "Gepetto cannot dump label dataset %s in file %s.\n",
                        LBL_DATASET(gptServer), LBL_PATH(gptServer, f));
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;
            }
            
            gptServer.indexOfFirstLabelPerFilePerTimerange[f]  = (int*) malloc(LBL_NTIMERANGES(gptServer, f)*sizeof(int));
            gptServer.indexOfFirstLabelPerFilePerTimerange[f][0] = 0;
            for (label_t=1; label_t<LBL_NTIMERANGES(gptServer, f); label_t++)
                gptServer.indexOfFirstLabelPerFilePerTimerange[f][label_t] = gptServer.indexOfFirstLabelPerFilePerTimerange[f][label_t-1] + LBL_NLABELS(gptServer, f, label_t-1);
            
            // update label counts
//            for (label_t=0; label_t<LBL_NTIMERANGES(gptServer, f); label_t++) 
//            {
//                for (i=0; i<LBL_NLABELS(gptServer, f, label_t); i++) 
//                {
//                    gptServer.labelCountsPerFile[f] = updateLabelCount(gptServer.labelCountsPerFile[f],
//                                                                       LBL_LABEL(gptServer, f, label_t, i),
//                                                                       1);
//                    gptServer.labelCountsTotal      = updateLabelCount(gptServer.labelCountsTotal,
//                                                                       LBL_LABEL(gptServer, f, label_t, i),
//                                                                       1);                    
//                }
//            }
            
            // prepare for next file
            pioCloseTimeline(&(gptServer.current_timeline));
            pioCloseDatatype(&(gptServer.current_datatype));
            pioCloseDataset(&(gptServer.current_dataset));
            pioCloseFile(&(gptServer.current_file));
        }    
        
        // list of distinct labels + total/per file counts
        LBL_NUMBER(gptServer) = getStatsOnLabels(gptServer, NULL, NULL, NULL);
        LBL_LIST(gptServer) = (int*) malloc(LBL_NUMBER(gptServer)*sizeof(int));
        gptServer.labelCounts = (int*) malloc(LBL_NUMBER(gptServer)*sizeof(int));
        gptServer.labelCountsPerFile = (int**) malloc(LBL_NFILES(gptServer)*sizeof(int*));
        for (f=0; f<LBL_NFILES(gptServer); f++)
            gptServer.labelCountsPerFile[f] = (int*) malloc(LBL_NUMBER(gptServer)*sizeof(int));
        getStatsOnLabels(gptServer, LBL_LIST(gptServer), gptServer.labelCounts, gptServer.labelCountsPerFile);
    }

    if (DAT_AVAILABLE(gptServer))
    {
        // initialize data-related stuff
        DAT_PATHS(gptServer) = (char**) malloc(DAT_NFILES(gptServer)*sizeof(char*));
        for (f=0; f<DAT_NFILES(gptServer); f++) DAT_PATH(gptServer, f) = NULL;
        
        DAT_DATASET(gptServer) = (char*) malloc((strlen(pathToDataDataset)+1)*sizeof(char));
        sprintf(DAT_DATASET(gptServer), "%s", pathToDataDataset);
        
        gptServer.dataTimeline = (PIOTimeRange**) malloc(DAT_NFILES(gptServer)*sizeof(PIOTimeRange*));
        for (f=0; f<DAT_NFILES(gptServer); f++) DAT_TIMELINE(gptServer, f) = NULL;
        
        gptServer.lengthOfDataTimeline = (int*) malloc(DAT_NFILES(gptServer)*sizeof(int));
        
//        // label counts
//        gptServer.dataCountsPerLabelTotal = NULL;
//        gptServer.dataCountsPerLabelPerFile = (listOfLabels_t**) malloc(DAT_NFILES(gptServer)*sizeof(listOfLabels_t*));
//        for (f=0; f<DAT_NFILES(gptServer); f++) gptServer.dataCountsPerLabelPerFile[f] = NULL;
        
        // test, initialize and (partially, timeline only) load data
        for (f=0; f<DAT_NFILES(gptServer); f++)
        {
            DAT_PATH(gptServer, f) = (char*) malloc((strlen(pathToDataFile[f])+1)*sizeof(char));
            sprintf(DAT_PATH(gptServer, f), "%s", pathToDataFile[f]);
            
            // test fth data file
            gptServer.current_file = pioOpenFile(DAT_PATH(gptServer, f), PINOCCHIO_READONLY);
            if (PIOFileIsInvalid(gptServer.current_file)) 
            {
                fprintf(stderr, "Geppeto cannot open data file %s.\n", DAT_PATH(gptServer, f));
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;
            }
            
            // test fth data dataset
            gptServer.current_dataset = pioOpenDataset(PIOMakeObject(gptServer.current_file),
                                                       DAT_DATASET(gptServer));
            if (PIODatasetIsInvalid(gptServer.current_dataset))
            {
                fprintf(stderr, "Gepetto cannot open data dataset %s in file %s.\n",
                        DAT_DATASET(gptServer), DAT_PATH(gptServer, f));
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;
            }
            
            // test fth data datatype
            gptServer.current_datatype = pioGetDatatype(gptServer.current_dataset);
            if (PIODatatypeIsInvalid(gptServer.current_datatype))
            {
                fprintf(stderr, "Gepetto cannot get datatype of data dataset %s in file %s.\n",
                        DAT_DATASET(gptServer), DAT_PATH(gptServer, f));
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
                            DAT_DATASET(gptServer), DAT_PATH(gptServer, f));
                    fflush(stderr);
                    
                    gptCloseServer(&gptServer);
                    return GPTServerInvalid;            
                }
            }
            else
            {
                // test fth data datatype
                if ((gptServer.current_datatype.dimension != gptServer.datatype.dimension) ||
                    (gptServer.current_datatype.type      != gptServer.datatype.type))
                {
                    fprintf(stderr, 
                            "Gepetto found that the datatype of dataset %s in file %s does not match the one in first file %s.\n",
                            DAT_DATASET(gptServer), DAT_PATH(gptServer, f), DAT_PATH(gptServer, 0));
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
                        DAT_DATASET(gptServer), DAT_PATH(gptServer, f));
                fflush(stderr);
                gptCloseServer(&gptServer);
                return GPTServerInvalid;
                
            }
            DAT_NTIMERANGES(gptServer, f) = gptServer.current_timeline.ntimeranges;
            DAT_TIMELINE(gptServer, f) = (PIOTimeRange*) malloc(DAT_NTIMERANGES(gptServer, f)*sizeof(PIOTimeRange));
            for (data_t=0; data_t<DAT_NTIMERANGES(gptServer, f); data_t++) 
                DAT_TIMERANGE(gptServer, f, data_t) = gptServer.current_timeline.timeranges[data_t];
            
            // prepare for next file
            pioCloseTimeline(&(gptServer.current_timeline));
            pioCloseDatatype(&(gptServer.current_datatype));
            pioCloseDataset(&(gptServer.current_dataset));
            pioCloseFile(&(gptServer.current_file));
        }

        // initialize filter mask and propagate labels

        // labels
        gptServer.propagatedLabel = (int**) malloc(DAT_NFILES(gptServer)*sizeof(int*));   
        for (f=0; f<DAT_NFILES(gptServer); f++) gptServer.propagatedLabel[f] = NULL;
    
        gptServer.filterMask = (int**) malloc(DAT_NFILES(gptServer)*sizeof(int*));
        
        for (f=0; f<DAT_NFILES(gptServer); f++) 
        {
            gptServer.propagatedLabel[f] = (int*) malloc(DAT_NTIMERANGES(gptServer, f)*sizeof(int));
            
            
            gptServer.filterMask[f] = (int*) malloc(DAT_NTIMERANGES(gptServer, f)*sizeof(int));
            if (gptServer.labelFilterType == GEPETTO_LABEL_FILTER_TYPE_UNDEFINED)
            {
                for (data_t=0; data_t<DAT_NTIMERANGES(gptServer, f); data_t++) 
                {
                    DAT_MATCHING_LABEL(gptServer, f, data_t) = -1;
                    DAT_FILTERED(gptServer, f, data_t) = 1;
                }
            }
            else 
            {
                label_t = 0;
                for (data_t=0; data_t<DAT_NTIMERANGES(gptServer, f); data_t++) 
                {
                    // look for first label timerange intersecting data timerange
                    while (
                           // not yet reached the end of label timeline
                           (label_t < LBL_NTIMERANGES(gptServer, f))
                           &&
                           // label timerange is before data timerange
                           (pioCompareTimeRanges(LBL_TIMERANGE(gptServer, f, label_t), 
                                                 DAT_TIMERANGE(gptServer, f, data_t)) != PINOCCHIO_TIMERANGE_COMPARISON_DESCENDING) 
                           &&
                           // label timerange do not intersect data timerange
                           (!pioTimeRangeIntersectsTimeRange(LBL_TIMERANGE(gptServer, f, label_t), 
                                                             DAT_TIMERANGE(gptServer, f, data_t)))
                           )
                    {
                        // try next label
                        label_t++;
                    }
                    
                    // if found, update mask based on 
                    if (pioTimeRangeIntersectsTimeRange(LBL_TIMERANGE(gptServer, f, label_t), 
                                                        DAT_TIMERANGE(gptServer, f, data_t)))
                    {
                        DAT_MATCHING_LABEL(gptServer, f, data_t) = label_t;                        
                        DAT_FILTERED(gptServer, f, data_t) = isFiltered(DAT_LABELS(gptServer, f, data_t),
                                                                        DAT_NLABELS(gptServer, f, data_t),
                                                                        gptServer.labelFilterType, 
                                                                        gptServer.labelFilterReference);
                    }
                    else 
                    {
                        DAT_MATCHING_LABEL(gptServer, f, data_t) = -1;
                        DAT_FILTERED(gptServer, f, data_t) = -1;
                    }
                    
//                    if (DAT_MATCHING_LABEL(gptServer, f, data_t) >= 0)
//                    {
//                        for (i=0; i<DAT_NLABELS(gptServer, f, data_t); i++) 
//                        {
//                            gptServer.dataCountsPerLabelPerFile[f] = updateLabelCount(gptServer.dataCountsPerLabelPerFile[f],
//                                                                                      DAT_LABEL(gptServer, f, data_t, i),
//                                                                                      1);
//                            gptServer.dataCountsPerLabelTotal = updateLabelCount(gptServer.dataCountsPerLabelTotal,
//                                                                                      DAT_LABEL(gptServer, f, data_t, i),
//                                                                                      1);
//                        }
//                    }
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

int gptCloseServer(GPTServer* gptServer)
{
    int f = 0;
    
//    // free data counts
//    if (gptServer->dataCountsPerLabelTotal)
//        destroyListOfLabels(gptServer->dataCountsPerLabelTotal);
//    gptServer->dataCountsPerLabelTotal = NULL;
//    
//    if (gptServer->dataCountsPerLabelPerFile)
//    {
//        for (f=0; f<gptServer->numberOfDataFiles; f++) {
//            destroyListOfLabels(gptServer->dataCountsPerLabelPerFile[f]);
//            gptServer->dataCountsPerLabelPerFile[f] = NULL;
//        }
//        free(gptServer->dataCountsPerLabelPerFile);
//    }
//    gptServer->dataCountsPerLabelPerFile = NULL;
    
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

    // free list of distinct labels
    free(LBL_LIST(*gptServer)); 
    // free label counts
    free(gptServer->labelCounts);
    // free per file label counts
    for (f=0; f<LBL_NFILES(*gptServer); f++)
    {
        free(gptServer->labelCountsPerFile[f]);
        gptServer->labelCountsPerFile[f] = NULL;
    }
    free(gptServer->labelCountsPerFile); 
    
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
    
    // free numberOfLabels
    if (gptServer->numberOfLabelsPerFilePerTimerange)
    {
        for (f=0; f<gptServer->numberOfLabelFiles; f++) {
            if (gptServer->numberOfLabelsPerFilePerTimerange[f]) free(gptServer->numberOfLabelsPerFilePerTimerange[f]); 
            gptServer->numberOfLabelsPerFilePerTimerange[f] = NULL;
        }
        free(gptServer->numberOfLabelsPerFilePerTimerange);        
    }

    // free indexOfLabels
    if (gptServer->indexOfFirstLabelPerFilePerTimerange)
    {
        for (f=0; f<gptServer->numberOfLabelFiles; f++) {
            if (gptServer->indexOfFirstLabelPerFilePerTimerange[f]) free(gptServer->indexOfFirstLabelPerFilePerTimerange[f]); 
            gptServer->indexOfFirstLabelPerFilePerTimerange[f] = NULL;
        }
        free(gptServer->indexOfFirstLabelPerFilePerTimerange);        
    }
    
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


