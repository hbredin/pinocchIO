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

#ifndef _GEPETTO_TYPES_H
#define _GEPETTO_TYPES_H

#include "pinocchIO.h"
#include "list_utils.h"

/**
 @brief Type of filter
 
 Depending on their associated label, Gepetto servers are able to filter out data
 entries to serve.
 
 @ingroup server
 */
typedef enum {
    /** Undefined filter */
	GEPETTO_LABEL_FILTER_TYPE_UNDEFINED = -1,
    /** No filter. Server serves all data entries */
	GEPETTO_LABEL_FILTER_TYPE_NONE, 
    /** "Equals to" filter. Server only serves data entries whose label equals to a predefined value */
	GEPETTO_LABEL_FILTER_TYPE_EQUALS_TO, 
    /** "Differs from" filter. Server only serves data entries whose label differs from a predefined value */
	GEPETTO_LABEL_FILTER_TYPE_DIFFERS_FROM, 
    /** "Greater than" filter. Server only serves data entries whose label is greater than a predefined value */
	GEPETTO_LABEL_FILTER_TYPE_GREATER_THAN, 
    /** "Smaller than" filter. Server only serves data entries whose label is smaller than a predefined value */
	GEPETTO_LABEL_FILTER_TYPE_SMALLER_THAN
} GPTLabelFilterType;

/// gepetto server
typedef struct {
    
    // === DATA ===
    
    // does server serve data?
    int servesData;
    
    //
    listOfLabels_t*  dataCountsPerLabelTotal;
    listOfLabels_t** dataCountsPerLabelPerFile;
    
    // number of pinocchIO files containing the actual data
    int numberOfDataFiles;
    // their paths
    char** pathToDataFile;
    // internal path to the pinocchIO dataset containing the actual data
    char* pathToDataDataset;   
    
    PIOTimeRange** dataTimeline;
    int* lengthOfDataTimeline;
    
    // === LABELS ===
    
    // does server server labels?
    int servesLabels;
    
    // 
    listOfLabels_t*  labelCountsTotal;
    listOfLabels_t** labelCountsPerFile;
    
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
    int** propagatedLabel;
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
    
    int         eof;
    
} GPTServer;

/**
	@brief Invalid Gepetto server
 */
#define GPTServerInvalid ((GPTServer) {             \
/* servesData */                -1,                 \
/* dataCountsPerLabelTotal */   NULL,               \
/* dataCountsPerLabelPerFile */ NULL,               \
/* numberOfDataFiles */         -1,                 \
/* pathToDataFile */            NULL,               \
/* pathToDataDataset */         NULL,               \
/* dataTimeline */              NULL,               \
/* lengthOfDataTimeline */      NULL,               \
/* servesLabels */              -1,                 \
/* labelCountsTotal */          NULL,               \
/* labelCountsPerFile */        NULL,               \
/* numberOfLabelFiles */        -1,                 \
/* pathToLabelFile */           NULL,               \
/* pathToLabelDataset */        NULL,               \
/* labelTimeline */             NULL,               \
/* lengthOfLabelTimeline */     NULL,               \
/* label */                     NULL,               \
/* labelFilterType */           -1,                 \
/* labelFilterReference */      -1,                 \
/* propagatedLabel */           NULL,               \
/* filterMask */                NULL,               \
/* datatype */                  PIODatatypeInvalid, \
/* labelDatatype */             PIODatatypeInvalid, \
/* current_file_index */        -1,                 \
/* curent_file */               PIOFileInvalid,     \
/* current_timerange_index */   -1,                 \
/* current_dataset */           PIODatasetInvalid,  \
/* current_datatype */          PIODatatypeInvalid, \
/* current_timeline */          PIOTimelineInvalid, \
/* eof */                       -1                  \
})


#endif

