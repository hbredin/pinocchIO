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

#ifndef MAX
/**
 @internal
 @brief Maximum of two numbers
 @param a First number
 @param b Second number
 @returns 
    - @a a if @a a > @a b
    - @a b otherwise
 */
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

/**
 @internal
 @brief Apply filter on given label
 @param[in] label Input label
 @param[in] labelFilterType Label filter type
 @param[in] labelFilterReference Label filter reference value
 @returns 
    - 1 if label value matches filter
    - 0 otherwise
 */
static int applyFilter(int label,
                      GPTLabelFilterType labelFilterType,
                      int labelFilterReference)
{
    int result;
        switch (labelFilterType)
        {
            case GEPETTO_LABEL_FILTER_TYPE_NONE:
                result = 1;
                break;
            case GEPETTO_LABEL_FILTER_TYPE_EQUALS_TO:
                result = (label == labelFilterReference);
                break;
            case GEPETTO_LABEL_FILTER_TYPE_DIFFERS_FROM:
                result = (label != labelFilterReference);
                break;
            case GEPETTO_LABEL_FILTER_TYPE_GREATER_THAN:
                result = (label > labelFilterReference);
                break;
            case GEPETTO_LABEL_FILTER_TYPE_SMALLER_THAN:
                result = (label < labelFilterReference);
                break;
            default:
                break;
        }        
    
    return result;
}

/**
 @internal
 @brief Apply server filter on given 
 @param[in] server Gepetto server
 @param[in] f File index
 @param[in] data_t Data timerange index
 @returns 
    - TRUE if data matches filter
    - FALSE otherwise
 */
static int isFiltered(GPTServer server,
                    int f,
                    int data_t)
{
    int result = 0;
    int n, label_t, r;
    
    for (n=0; n<server.numberOfCorrespondingLabelTimerange[f][data_t]; n++) 
    {
        label_t = server.firstCorrespondingLabelTimerange[f][data_t] + n;
        for (r=0; r<LBL_NLABELS(server, f, label_t); r++) 
            result = result || applyFilter(LBL_LABEL(server, f, label_t, r), 
                                           server.labelFilterType,
                                           server.labelFilterReference); 
    }
    return result;
}

/**
 @internal
 
 @brief Check if data has a given label
 @param[in] server Gepetto server
	@param f File index
	@param data_t Data timerange index
	@param label Label value
	@returns 
        - TRUE if data has given label
        - FALSE if it has not
 */
static int hasLabel(GPTServer server, int f, int data_t, int label)
{
    int result = 0;
    int n, label_t, r;
    
    for (n=0; n<server.numberOfCorrespondingLabelTimerange[f][data_t]; n++) 
    {
        label_t = server.firstCorrespondingLabelTimerange[f][data_t] + n;
        for (r=0; r<LBL_NLABELS(server, f, label_t); r++) 
            result = result || (LBL_LABEL(server, f, label_t, r) == label);
    }
    return result;
}

/**
 @internal
 @brief Compare two integer labels
 @param[in] a First label value
 @param[in] b Second label value
 @returns 
    - positive value if @a a is greater than @a b
    - negative value otherwise
 */
static int compareLabels (void const *a, void const *b)
{
    int const *pa = a;
    int const *pb = b;
    return *pa - *pb;
}

/**
 @internal

	@brief Init label configuration variables
 
    Allocate and set:
        - server.servesLabel
        - server.numberOfLabelFiles
        - server.pathToLabelFile
        - server.pathToLabelDataset
 
	@param[in,out] server Gepetto server
	@param[in] numberOfLabelFiles Number of label pinocchIO files
	@param[in] pathToLabelFile Paths to each of them
	@param[in] pathToLabelDataset pinocchIO path to label dataset
 
	@returns 
        - 1 when succesful
        - negative value otherwise
 */
static int initLabelConfiguration( GPTServer* server, int numberOfLabelFiles, char** pathToLabelFile, const char* pathToLabelDataset)
{
    int f; // file counter
    
    // label availability
    LBL_AVAILABLE(*server) = (numberOfLabelFiles > 0);
    
    LBL_NFILES(*server) = numberOfLabelFiles;
    
    if (LBL_AVAILABLE(*server)) 
    {
        LBL_PATHS(*server) = (char**) malloc(LBL_NFILES(*server)*sizeof(char*));     
        for (f=0; f<LBL_NFILES(*server); f++)
        {
            LBL_PATH(*server, f) = (char*) malloc((strlen(pathToLabelFile[f])+1)*sizeof(char));
            sprintf(LBL_PATH(*server, f), "%s", pathToLabelFile[f]);
        }
        
        LBL_DATASET(*server) = (char*) malloc((strlen(pathToLabelDataset)+1)*sizeof(char));
        sprintf(LBL_DATASET(*server), "%s", pathToLabelDataset);
    }
    
    return 1;
}

/**
 @internal
 @brief Init label storage variables
 
 Allocate and set:
    - server.lengthOfLabelTimeline
    - server.labelTimeline
    - server.numberOfLabelsPerFilePerTimerange
    - server.indexOfFirstLabelPerFilePerTimerange
    - server.label
    - server.labelDatatype
 
 @param[in, out] server Gepetto server
 @returns
    - 1 when successful
    - -1 in case of failure (server is closed, also).
 */
static int initLabelStorage(GPTServer* server)
{
    int f;
    int label_t;
    size_t labelBufferSize = -1;

    // label timelines
    LBL_TIMELINES(*server) = (PIOTimeRange**) malloc(LBL_NFILES(*server)*sizeof(PIOTimeRange*));
    for (f=0; f<LBL_NFILES(*server); f++) LBL_TIMELINE(*server,f) = NULL;
    server->lengthOfLabelTimeline = (int*) malloc(LBL_NFILES(*server)*sizeof(int));
    
    // labels
    server->label                 = (int**) malloc(LBL_NFILES(*server)*sizeof(int*));   
    for (f=0; f<LBL_NFILES(*server); f++) server->label[f] = NULL;
    
    // Number of labels per file per timerange
    server->numberOfLabelsPerFilePerTimerange        = (int**) malloc(LBL_NFILES(*server)*sizeof(int*));
    for (f=0; f<LBL_NFILES(*server); f++) server->numberOfLabelsPerFilePerTimerange[f] = NULL;
    
    // Index of first label per file per timerange
    server->indexOfFirstLabelPerFilePerTimerange        = (int**) malloc(LBL_NFILES(*server)*sizeof(int*));
    for (f=0; f<LBL_NFILES(*server); f++) server->indexOfFirstLabelPerFilePerTimerange[f] = NULL;
    
    // labels are integers
    server->labelDatatype = pioNewDatatype(PINOCCHIO_TYPE_INT, 1);        
    
    for (f=0; f<LBL_NFILES(*server); f++)
    {
        // check if label file is readable
        server->current_file = pioOpenFile(LBL_PATH(*server, f), PINOCCHIO_READONLY);
        if (PIOFileIsInvalid(server->current_file)) 
        {
            fprintf(stderr, "Geppeto cannot open label file %s.\n", LBL_PATH(*server, f));
            fflush(stderr);
            gptCloseServer(server);
            return -1;
        }
        
        // check if label dataset is readable
        server->current_dataset = pioOpenDataset(PIOMakeObject(server->current_file),
                                                   LBL_DATASET(*server));
        if (PIODatasetIsInvalid(server->current_dataset))
        {
            fprintf(stderr, "Gepetto cannot open label dataset %s in file %s.\n",
                    LBL_DATASET(*server), LBL_PATH(*server, f));
            fflush(stderr);
            gptCloseServer(server);
            return -1;
        }
        
        // check if label datatype is mono-dimensional
        server->current_datatype = pioGetDatatype(server->current_dataset);
        if (PIODatatypeIsInvalid(server->current_datatype))
        {
            fprintf(stderr, "Gepetto cannot get datatype of label dataset %s in file %s.\n",
                    LBL_DATASET(*server), LBL_PATH(*server, f));
            fflush(stderr);
            gptCloseServer(server);
            return -1;            
        }
        if (server->current_datatype.dimension != 1)
        {
            fprintf(stderr, 
                    "Gepetto found that the datatype of label dataset %s in file %s is not mono-dimensional.\n",
                    LBL_DATASET(*server), LBL_PATH(*server, f));
            fflush(stderr);                
            gptCloseServer(server);
            return -1;            
        }
        
        // load label timeline
        server->current_timeline = pioGetTimeline(server->current_dataset);
        if (PIOTimelineIsInvalid(server->current_timeline))
        {
            fprintf(stderr, "Geppeto cannot open timeline of label dataset %s in file %s.\n",
                    LBL_DATASET(*server), LBL_PATH(*server, f));
            fflush(stderr);
            gptCloseServer(server);
            return -1;
            
        }
        LBL_NTIMERANGES(*server, f) = server->current_timeline.ntimeranges;
        LBL_TIMELINE(*server, f) = (PIOTimeRange*) malloc(LBL_NTIMERANGES(*server, f)*sizeof(PIOTimeRange));
        for (label_t=0; label_t<LBL_NTIMERANGES(*server, f); label_t++) 
            LBL_TIMERANGE(*server, f, label_t) = server->current_timeline.timeranges[label_t];
        
        // load label data
        labelBufferSize = pioDumpDataset(&(server->current_dataset), server->labelDatatype, NULL, NULL);
        if (labelBufferSize <= 0)
        {
            fprintf(stderr, 
                    "Gepetto cannot find any label in dataset %s in file %s.\n",
                    LBL_DATASET(*server), LBL_PATH(*server, f));
            fflush(stderr);
            gptCloseServer(server);
            return -1;                
        }
        
        server->label[f] = (int*) malloc(labelBufferSize);
        server->numberOfLabelsPerFilePerTimerange[f] = (int*) malloc(LBL_NTIMERANGES(*server, f)*sizeof(int));
        
        if (pioDumpDataset(&(server->current_dataset), server->labelDatatype, server->label[f], server->numberOfLabelsPerFilePerTimerange[f]) < 0)
        {
            fprintf(stderr, 
                    "Gepetto cannot dump label dataset %s in file %s.\n",
                    LBL_DATASET(*server), LBL_PATH(*server, f));
            fflush(stderr);
            gptCloseServer(server);
            return -1;
        }
        
        server->indexOfFirstLabelPerFilePerTimerange[f]  = (int*) malloc(LBL_NTIMERANGES(*server, f)*sizeof(int));
        server->indexOfFirstLabelPerFilePerTimerange[f][0] = 0;
        for (label_t=1; label_t<LBL_NTIMERANGES(*server, f); label_t++)
            server->indexOfFirstLabelPerFilePerTimerange[f][label_t] = server->indexOfFirstLabelPerFilePerTimerange[f][label_t-1] + LBL_NLABELS(*server, f, label_t-1);
        
        // prepare for next file
        pioCloseTimeline(&(server->current_timeline));
        pioCloseDatatype(&(server->current_datatype));
        pioCloseDataset(&(server->current_dataset));
        pioCloseFile(&(server->current_file));
    }    
    
    return 1;
}

/**
 @internal
 @brief Get statistics on labels
	@param[in] server Gepetto server
	@param[out] labels List of distinct label values, sorted in ascending order
	@param[out] labelCounts Label counts for each label value
	@param[out] labelCountsPerFile Label counts for each label value, per file
	@returns number of distinct label values
 */
static int getStatsOnLabels(GPTServer server, int* labels, int* labelCounts, int** labelCountsPerFile)
{
    int f, t, i;
    int count;
    listOfLabels_t* list = NULL;
    listOfLabels_t** listPerFile = NULL;
    
    listPerFile = (listOfLabels_t**) malloc(LBL_NFILES(server)*sizeof(listOfLabels_t*));
    for (f=0; f<LBL_NFILES(server); f++) listPerFile[f] = NULL;
    
    
    count = 0; // number of distinct labels so far
    for(f=0; f<LBL_NFILES(server); f++)
    {
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

/**
 @brief Init label statistics variables
 
 Allocate and set:
    - server.numberOfDistinctLabels
    - server.listOfDistinctLabels
    - server.labelCounts
    - server.labelCountsPerFile
 
 @param server Gepetto server
 @returns
    - 1 when successful
    - -1 otherwise
 */
static int initLabelStatistics(GPTServer* server)
{
    int f;
    LBL_NUMBER(*server) = getStatsOnLabels(*server, NULL, NULL, NULL);
    LBL_LIST(*server) = (int*) malloc(LBL_NUMBER(*server)*sizeof(int));
    server->labelCounts = (int*) malloc(LBL_NUMBER(*server)*sizeof(int));
    server->labelCountsPerFile = (int**) malloc(LBL_NFILES(*server)*sizeof(int*));
    for (f=0; f<LBL_NFILES(*server); f++)
        server->labelCountsPerFile[f] = (int*) malloc(LBL_NUMBER(*server)*sizeof(int));
    getStatsOnLabels(*server, LBL_LIST(*server), server->labelCounts, server->labelCountsPerFile);
    return 1;
}

/**
 @brief Init data configuration variables
 
 Allocate and set:
 - server.servesData
 - server.numberOfDataFiles
 - server.pathToDataFile
 - server.pathToDataDataset
 
 @param server Gepetto server
 @param numberOfDataFiles Number of data pinocchIO files
 @param pathToDataFile Paths to each of them
 @param pathToDataDataset pinocchIO path to data dataset
 
 @returns 1
 */
static int initDataConfiguration( GPTServer* server, int numberOfDataFiles, char** pathToDataFile, const char* pathToDataDataset)
{
    int f; // file counter
    
    // data availability
    DAT_AVAILABLE(*server) = (numberOfDataFiles > 0);
    
    DAT_NFILES(*server) = numberOfDataFiles;
    
    if (DAT_AVAILABLE(*server)) 
    {
        DAT_PATHS(*server) = (char**) malloc(DAT_NFILES(*server)*sizeof(char*));     
        for (f=0; f<DAT_NFILES(*server); f++)
        {
            DAT_PATH(*server, f) = (char*) malloc((strlen(pathToDataFile[f])+1)*sizeof(char));
            sprintf(DAT_PATH(*server, f), "%s", pathToDataFile[f]);
        }
        
        DAT_DATASET(*server) = (char*) malloc((strlen(pathToDataDataset)+1)*sizeof(char));
        sprintf(DAT_DATASET(*server), "%s", pathToDataDataset);
    }
    
    return 1;
}

/**
 @internal
 @brief Init data storage variables
 
 Allocate and set:
 - server.lengthOfDataTimeline
 - server.dataTimeline
 - server.datatype
 
 @param[in, out] server Gepetto server
 @returns
 - 1 when successful
 - -1 in case of failure (server is closed, also).
 */
static int initDataStorage(GPTServer* gptServer)
{
    int f;
    int data_t;
    
    gptServer->dataTimeline = (PIOTimeRange**) malloc(DAT_NFILES(*gptServer)*sizeof(PIOTimeRange*));
    for (f=0; f<DAT_NFILES(*gptServer); f++) DAT_TIMELINE(*gptServer, f) = NULL;
    
    gptServer->lengthOfDataTimeline = (int*) malloc(DAT_NFILES(*gptServer)*sizeof(int));
    
    // test, initialize and (partially, timeline only) load data
    for (f=0; f<DAT_NFILES(*gptServer); f++)
    {            
        // test fth data file
        gptServer->current_file = pioOpenFile(DAT_PATH(*gptServer, f), PINOCCHIO_READONLY);
        if (PIOFileIsInvalid(gptServer->current_file)) 
        {
            fprintf(stderr, "Geppeto cannot open data file %s.\n", DAT_PATH(*gptServer, f));
            fflush(stderr);
            gptCloseServer(gptServer);
            return -1;
        }
        
        // test fth data dataset
        gptServer->current_dataset = pioOpenDataset(PIOMakeObject(gptServer->current_file),
                                                   DAT_DATASET(*gptServer));
        if (PIODatasetIsInvalid(gptServer->current_dataset))
        {
            fprintf(stderr, "Gepetto cannot open data dataset %s in file %s.\n",
                    DAT_DATASET(*gptServer), DAT_PATH(*gptServer, f));
            fflush(stderr);
            gptCloseServer(gptServer);
            return -1;
        }
        
        // test fth data datatype
        gptServer->current_datatype = pioGetDatatype(gptServer->current_dataset);
        if (PIODatatypeIsInvalid(gptServer->current_datatype))
        {
            fprintf(stderr, "Gepetto cannot get datatype of data dataset %s in file %s.\n",
                    DAT_DATASET(*gptServer), DAT_PATH(*gptServer, f));
            fflush(stderr);
            gptCloseServer(gptServer);
            return -1;            
        }
        
        if (f == 0)
        {
            // initialize data datatype
            gptServer->datatype = pioNewDatatype(gptServer->current_datatype.type,
                                                gptServer->current_datatype.dimension);
            if (PIODatatypeIsInvalid(gptServer->datatype))
            {
                fprintf(stderr, "Gepetto found that datatype of dataset %s in first file %s is invalid.\n",
                        DAT_DATASET(*gptServer), DAT_PATH(*gptServer, f));
                fflush(stderr);
                
                gptCloseServer(gptServer);
                return -1;            
            }
        }
        else
        {
            // test fth data datatype
            if ((gptServer->current_datatype.dimension != gptServer->datatype.dimension) ||
                (gptServer->current_datatype.type      != gptServer->datatype.type))
            {
                fprintf(stderr, 
                        "Gepetto found that the datatype of dataset %s in file %s does not match the one in first file %s.\n",
                        DAT_DATASET(*gptServer), DAT_PATH(*gptServer, f), DAT_PATH(*gptServer, 0));
                fflush(stderr);                
                gptCloseServer(gptServer);
                return -1;            
            }
        }
        
        // load data timeline
        gptServer->current_timeline = pioGetTimeline(gptServer->current_dataset);
        if (PIOTimelineIsInvalid(gptServer->current_timeline))
        {
            fprintf(stderr, "Geppeto cannot open timeline of data dataset %s in file %s.\n",
                    DAT_DATASET(*gptServer), DAT_PATH(*gptServer, f));
            fflush(stderr);
            gptCloseServer(gptServer);
            return -1;
            
        }
        DAT_NTIMERANGES(*gptServer, f) = gptServer->current_timeline.ntimeranges;
        DAT_TIMELINE(*gptServer, f) = (PIOTimeRange*) malloc(DAT_NTIMERANGES(*gptServer, f)*sizeof(PIOTimeRange));
        for (data_t=0; data_t<DAT_NTIMERANGES(*gptServer, f); data_t++) 
            DAT_TIMERANGE(*gptServer, f, data_t) = gptServer->current_timeline.timeranges[data_t];
        
        // prepare for next file
        pioCloseTimeline(&(gptServer->current_timeline));
        pioCloseDatatype(&(gptServer->current_datatype));
        pioCloseDataset(&(gptServer->current_dataset));
        pioCloseFile(&(gptServer->current_file));
    }
    
    
    return 1;
    
}

/**
 @internal
 @brief Init data filtering variables
 
 Allocate and set:
    - server.labelFilterType
    - server.labelFilterReference
    - server.firstCorrespondingLabelTimerange
    - server.numberOfCorrespondingLabelTimerange
 
 @param[in, out] server Gepetto server
 @param[in] type Type of label filter
 @param[in] referene Label filter reference value
 @returns 
    - 1 when successful
    - negative value otherwise
 */
static int initDataFiltering(GPTServer* server, GPTLabelFilterType type, int reference, int maximumNumberOfSamplesPerLabel)
{
    int f;
    int label_t;
    int data_t;
    
    server->labelFilterType = type;
    server->labelFilterReference = reference;
    server->maximumNumberOfSamplesPerLabel = maximumNumberOfSamplesPerLabel;
    
    server->firstCorrespondingLabelTimerange = (int**) malloc(DAT_NFILES(*server)*sizeof(int*));   
    server->numberOfCorrespondingLabelTimerange = (int**) malloc(DAT_NFILES(*server)*sizeof(int*));   
    
    for (f=0; f<DAT_NFILES(*server); f++) 
    {
        // initialize with default "no available label" behavior
        server->firstCorrespondingLabelTimerange[f] = (int*) malloc(DAT_NTIMERANGES(*server, f)*sizeof(int));
        server->numberOfCorrespondingLabelTimerange[f] = (int*) malloc(DAT_NTIMERANGES(*server, f)*sizeof(int));
        for (data_t=0; data_t<DAT_NTIMERANGES(*server, f); data_t++) 
        {
            server->firstCorrespondingLabelTimerange[f][data_t] = -1;
            server->numberOfCorrespondingLabelTimerange[f][data_t] = 0;
        }
        
        if (LBL_AVAILABLE(*server))
        {
            for (data_t=0; data_t<DAT_NTIMERANGES(*server, f); data_t++) 
            {                    
                // start looking at first matching timerange of previous data timerange
                if (data_t == 0) label_t = 0;
                else label_t = MAX(server->firstCorrespondingLabelTimerange[f][data_t-1], 0);
                
                // look for first matching timerange
                while (// not yet reached the end of label timeline
                       (label_t < LBL_NTIMERANGES(*server, f)) &&
                       // label timerange still happens BEFORE data timerange
                       (pioCompareTimeRanges(LBL_TIMERANGE(*server, f, label_t), 
                                             DAT_TIMERANGE(*server, f, data_t)) != PINOCCHIO_TIMERANGE_COMPARISON_DESCENDING) &&
                       // label timerange still DOES NOT intersect data timerange
                       (!pioTimeRangeIntersectsTimeRange(LBL_TIMERANGE(*server, f, label_t), 
                                                         DAT_TIMERANGE(*server, f, data_t))))
                    label_t++;
                
                // if there is at least one matching timerange
                if ((label_t < LBL_NTIMERANGES(*server, f)) &&
                    (pioTimeRangeIntersectsTimeRange(LBL_TIMERANGE(*server, f, label_t), 
                                                     DAT_TIMERANGE(*server, f, data_t))))
                {
                    // store first matching timerange
                    server->firstCorrespondingLabelTimerange[f][data_t] = label_t;
                    
                    // count number of matching timeranges
                    while (                           
                           // not yet reached the end of label timeline
                           (label_t < LBL_NTIMERANGES(*server, f))
                           &&
                           (pioTimeRangeIntersectsTimeRange(LBL_TIMERANGE(*server, f, label_t), 
                                                            DAT_TIMERANGE(*server, f, data_t)))) 
                    {
                        server->numberOfCorrespondingLabelTimerange[f][data_t]++;    
                        label_t++;
                    }
                }
            }
        }
    }
    
    server->filtered = (int**) malloc(DAT_NFILES(*server)*sizeof(int*));
    for (f=0; f<DAT_NFILES(*server); f++) 
    {
        // initialize with "no filter" (i.e. keep everything)
        server->filtered[f] = (int*) malloc(DAT_NTIMERANGES(*server, f)*sizeof(int));
        for (data_t=0; data_t<DAT_NTIMERANGES(*server, f); data_t++) 
            server->filtered[f][data_t] = 1;

        if (LBL_AVAILABLE(*server))
            for (data_t=0; data_t<DAT_NTIMERANGES(*server, f); data_t++) 
                server->filtered[f][data_t] = isFiltered(*server, f, data_t);
        
    }
    
    return 1;
}

static int performDataSampling(GPTServer* server)
{
    int data_t;
    int n;
    int f;
    
    int** numberOfSamplesPerLabelPerFile;
    int* numberOfSamplesPerLabel;
    
    numberOfSamplesPerLabel = (int*) malloc(LBL_NUMBER(*server)*sizeof(int));
    numberOfSamplesPerLabelPerFile = (int**) malloc(LBL_NUMBER(*server)*sizeof(int*));
    
    for (n=0; n<LBL_NUMBER(*server); n++)
    {
        numberOfSamplesPerLabel[n] = 0;
     
        numberOfSamplesPerLabelPerFile[n] = (int*) malloc(DAT_NFILES(*server)*sizeof(int));
        for (f=0; f<DAT_NFILES(*server); f++)
            numberOfSamplesPerLabelPerFile[n][f] = 0;        
        
        // if there are too many samples for nth label
        if (server->maximumNumberOfSamplesPerLabel < DAT_COUNT(*server, n))
        {
            // add samples from every file until number of samples is reached
            while (numberOfSamplesPerLabel[n] < server->maximumNumberOfSamplesPerLabel) 
            {
                for (f=0; f<DAT_NFILES(*server); f++) 
                {
                    // make sure we do not add more samples than are available
                    if (numberOfSamplesPerLabelPerFile[n][f] < DAT_COUNTF(*server, f, n))
                    {
                        numberOfSamplesPerLabelPerFile[n][f]++;
                        numberOfSamplesPerLabel[n]++;
                    }
                }
            }
        }
        // if there are NOT too many samples for nth label
        // keep everything
        else {
            numberOfSamplesPerLabel[n] = 0;

            for (f=0; f<DAT_NFILES(*server); f++)
            {
                numberOfSamplesPerLabelPerFile[n][f] = DAT_COUNTF(*server, f, n);
                numberOfSamplesPerLabel[n] += numberOfSamplesPerLabelPerFile[n][f];
            }
        }
    }
    
    // do the actual downsampling by altering server.filtered
    for (n=0; n<LBL_NUMBER(*server); n++) 
    {
        for (f=0; f<DAT_NFILES(*server); f++) 
        {
            int keptSoFar = 0;
            int metSoFar = 0;
            if (numberOfSamplesPerLabelPerFile[n][f] < DAT_COUNTF(*server, f, n))
            {
                for (data_t=0; data_t < DAT_NTIMERANGES(*server, f); data_t++) 
                {
                    if (hasLabel(*server, f, data_t, LBL_VALUE(*server, n))) 
                    {
                        metSoFar++;
                        if (keptSoFar*DAT_COUNTF(*server, f, n) < numberOfSamplesPerLabelPerFile[n][f]*metSoFar)
                            keptSoFar++;
                        else 
                            server->filtered[f][data_t] = 0;
                    }
                }
            }
        }
    }
    
    free(numberOfSamplesPerLabel);
    for (n=0; n<LBL_NUMBER(*server); n++) free(numberOfSamplesPerLabelPerFile[n]);
    free(numberOfSamplesPerLabelPerFile);
    
    return 1;
}

/**
 @brief Init data statistics variables
 
 Allocate and set:
 - server.dataCountsForLabel
 - server.dataCountsForLabelPerFile
 
 @param[in] server Gepetto server
 @returns
 - 1 when successful
 - -1 otherwise
 */
static int initDataStatistics(GPTServer* server)
{
    int f;
    int data_t;
    int label_t;
    int i;
    int r;
    int n;
    listOfLabels_t* localList;
    
    server->dataCountsForLabel = (int*) malloc(LBL_NUMBER(*server)*sizeof(int));
    for (n=0; n<LBL_NUMBER(*server); n++) server->dataCountsForLabel[n] = 0;
    
    server->dataCountsForLabelPerFile = (int**) malloc(DAT_NFILES(*server)*sizeof(int*));
    for (f=0; f<DAT_NFILES(*server); f++)
    {
        server->dataCountsForLabelPerFile[f] = (int*) malloc(LBL_NUMBER(*server)*sizeof(int));
        for (n=0; n<LBL_NUMBER(*server); n++) 
            server->dataCountsForLabelPerFile[f][n] = 0;        
    }
    
    for (f=0; f<DAT_NFILES(*server); f++)
    {
        for (data_t=0; data_t<DAT_NTIMERANGES(*server, f); data_t++) 
        {
            localList = NULL;
            
            // get each data timerange label counts
            for (i=0; i<server->numberOfCorrespondingLabelTimerange[f][data_t]; i++) 
            {
                label_t = server->firstCorrespondingLabelTimerange[f][data_t]+i;
                for (r=0; r<LBL_NLABELS(*server, f, label_t); r++) 
                    localList = updateLabelCount(localList, LBL_LABEL(*server, f, label_t, r), 1);
            }   
            
            // add +1 for each data timerange with at least one corresponding label
            for (n=0; n<LBL_NUMBER(*server); n++) 
            {
                if (getLabelCount(localList, LBL_VALUE(*server, n)) > 0)
                {
                    server->dataCountsForLabel[n]++;
                    server->dataCountsForLabelPerFile[f][n]++;
                }
            }
            
            destroyListOfLabels(localList);
        }
    }
    return 1;
}

GPTServer gptNewServer(int numberOfDataFiles, char** pathToDataFile, const char* pathToDataDataset,
                       GPTLabelFilterType labelFilterType, int labelFilterReference, int maximumNumberOfSamplesPerLabel,
                       int numberOfLabelFiles, char** pathToLabelFile, const char* pathToLabelDataset)
{        
    GPTServer gptServer = GPTServerInvalid;

    if (initLabelConfiguration(&gptServer, numberOfLabelFiles, pathToLabelFile, pathToLabelDataset) < 0)
        return GPTServerInvalid;
    
    if (initDataConfiguration( &gptServer, numberOfDataFiles,  pathToDataFile,  pathToDataDataset) < 0)
        return GPTServerInvalid;
        
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
    if ((labelFilterType != GEPETTO_LABEL_FILTER_TYPE_NONE) && 
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
        if (initLabelStorage(&gptServer) < 0)
            return GPTServerInvalid;
                

        if (initLabelStatistics(&gptServer) < 0)
            return GPTServerInvalid;        
    }

    if (DAT_AVAILABLE(gptServer))
    {        
        if (initDataStorage(&gptServer) < 0)
            return GPTServerInvalid;

        if (initDataFiltering(&gptServer, labelFilterType, labelFilterReference, maximumNumberOfSamplesPerLabel) < 0)
            return GPTServerInvalid;
        
        if (LBL_AVAILABLE(gptServer))
        {
            if (initDataStatistics(&gptServer) < 0)
                return GPTServerInvalid;
            
            if (gptServer.maximumNumberOfSamplesPerLabel > 0)
                if (performDataSampling(&gptServer) < 0)
                    return GPTServerInvalid;
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
    
    // free firstCorrespondingLabelTimerange
    if (gptServer->firstCorrespondingLabelTimerange)
    {
        for (f=0; f<gptServer->numberOfDataFiles; f++) {
            if (gptServer->firstCorrespondingLabelTimerange[f]) free(gptServer->firstCorrespondingLabelTimerange[f]); 
            gptServer->firstCorrespondingLabelTimerange[f] = NULL;
        }
        free(gptServer->firstCorrespondingLabelTimerange);
    }
    gptServer->firstCorrespondingLabelTimerange = NULL;
    
    // free filtered
    if (gptServer->filtered)
    {
        for (f=0; f<gptServer->numberOfDataFiles; f++) {
            if (gptServer->filtered[f]) free(gptServer->filtered[f]); 
            gptServer->filtered[f] = NULL;
        }
        free(gptServer->filtered);
    }
    gptServer->filtered = NULL;
    
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


