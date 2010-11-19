/*
 *  gptTypes.h
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 18/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GEPETTO_TYPES_H
#define _GEPETTO_TYPES_H

#include "pinocchIO/pinocchIO.h"


typedef enum {
    GEPETTO_LABEL_FILTER_TYPE_NONE,
    GEPETTO_LABEL_FILTER_TYPE_EQUALS_TO,
    GEPETTO_LABEL_FILTER_TYPE_DIFFERS_FROM,
    GEPETTO_LABEL_FILTER_TYPE_GREATER_THAN,
    GEPETTO_LABEL_FILTER_TYPE_SMALLER_THAN
} GPTLabelFilterType;

/// gepetto server
typedef struct {
    
    // === DATA ===
    
    // number of pinocchIO files containing the actual data
    int numberOfDataFiles;
    // their paths
    char** pathToDataFile;
    // internal path to the pinocchIO dataset containing the actual data
    char* pathToDataDataset;   
    
    PIOTimeRange** dataTimeline;
    int* lengthOfDataTimeline;
    
    // === LABELS ===
    
    // number of pinocchIO files containing labels
    int numberOfLabelFiles;       
    // their paths
    char** pathToLabelFile;
    // internal path to the pinocchIO dataset containing labels
    char* pathToLabelDataset;
    
    // label storage
    PIOTimeRange** labelTimeline;
    int* lengthOfLabelTimeline;
    int** label;

    // === FILTER ===

    GPTLabelFilterType labelFilterType; // NONE, EQUALS_TO, DIFFERS_FROM, IS_GREATER_THAN, IS_SMALLER_THAN
    int labelFilterReference;

    // labels are propagated from label timeline to data timeline
    int** filterMask; 
    
    // === SAMPLING ===
    
    
    // === INTERNALS ===
    
    PIODatatype datatype;
    PIODatatype labelDatatype;
    
    int         current_file_index;
    PIOFile     current_file;
    
    int         current_timerange_index;
    PIODataset  current_dataset;
    PIODatatype current_datatype;
    PIOTimeline current_timeline;    

} GPTServer;

#define GPTServerInvalid ((GPTServer) {           \
/* numberOfDataFiles */       -1,                 \
/* pathToDataFile */          NULL,               \
/* pathToDataDataset */       NULL,               \
/* dataTimeline */            NULL,               \
/* lengthOfDataTimeline */    NULL,               \
/* numberOfLabelFiles */      -1,                 \
/* pathToLabelFile */         NULL,               \
/* pathToLabelDataset */      NULL,               \
/* labelTimeline */           NULL,               \
/* lengthOfLabelTimeline */   NULL,               \
/* label */                   NULL,               \
/* labelFilterType */         -1,                 \
/* labelFilterReference */    -1,                 \
/* filterMask */              NULL,               \
/* datatype */                PIODatatypeInvalid, \
/* labelDatatype */           PIODatatypeInvalid, \
/* current_file_index */      -1,                 \
/* curent_file */             PIOFileInvalid,     \
/* current_timerange_index */ -1,                 \
/* current_dataset */         PIODatasetInvalid,  \
/* current_datatype */        PIODatatypeInvalid, \
/* current_timeline */        PIOTimelineInvalid  \
})


#endif

