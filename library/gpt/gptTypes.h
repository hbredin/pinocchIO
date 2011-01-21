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

#include "pinocchIO/pinocchIO.h"

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
    
    int  numberOfDistinctLabels;
    int* listOfDistinctLabels;
    
    int* labelCounts;
    int** labelCountsPerFile;
        
    // number of pinocchIO files containing labels
    int numberOfLabelFiles;       
    // their paths
    char** pathToLabelFile;
    // internal path to the pinocchIO dataset containing labels
    char* pathToLabelDataset;
    
    // label storage
    
    /** labelTimeline[f][t] is the t^th label timerange of the f^th file */
    PIOTimeRange** labelTimeline; // 

    /** lengthOfLabelTimeline[f] is the number of label timeranges of the f^th file */
    int* lengthOfLabelTimeline;  
    
    /** label[f] contains all labels from the f^th file in chronological order */
    int** label;
    
    int** numberOfLabelsPerFilePerTimerange; // numberOfLabels[f][i] is the number of label available for ith timerange of fth file
    int** indexOfFirstLabelPerFilePerTimerange;  // indexOfLabels[f][i] is the index of first label for ith timerange of fth file
                          // label[f][indexOfLabels[f][i]] is therefore the first label of ith timerange of fth file

    // === FILTER ===

    GPTLabelFilterType labelFilterType; // NONE, EQUALS_TO, DIFFERS_FROM, IS_GREATER_THAN, IS_SMALLER_THAN
    int labelFilterReference;

    // labels are propagated from label timeline to data timeline
    int** propagatedLabel; // propagatedLabel[f][t] is the index of label timerange corresponding to the tth data timerange of fth file
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
/* servesData */                0,                  \
/* numberOfDataFiles */         -1,                 \
/* pathToDataFile */            NULL,               \
/* pathToDataDataset */         NULL,               \
/* dataTimeline */              NULL,               \
/* lengthOfDataTimeline */      NULL,               \
/* servesLabels */              0,                  \
/* numberOfDistinctLabels */    0,                  \
/* listOfDistinctLabels */      NULL,               \
/* labelCounts */               NULL,               \
/* labelCountsPerFile */        NULL,               \
/* numberOfLabelFiles */        -1,                 \
/* pathToLabelFile */           NULL,               \
/* pathToLabelDataset */        NULL,               \
/* labelTimeline */             NULL,               \
/* lengthOfLabelTimeline */     NULL,               \
/* label */                     NULL,               \
/* numberOfLabelsPerFilePerTimerange */     NULL,   \
/* indexOfFirstLabelPerFilePerTimerange */  NULL,   \
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


/**
 @brief pinocchIO time range
 
 A time range can be seen as a time segment, with a start \a time and a \a duration.
 
 pinocchIO stores time ranges internally using three integer values: \a time, \a duration and \a scale.
 
 See \ref PIOTime for more detail on how to efficiently choose the \a scale.
 
 @ingroup time
 */
typedef struct {
    /** pinocchIO timerange */
    PIOTimeRange timerange;
    /** file index */
    int fileIndex;
} GPTTimeRange;

// Macro for easy access to labels

#define LBL_AVAILABLE(server)               ((server).servesLabels)

#define LBL_NFILES(server)                  ((server).numberOfLabelFiles)
#define LBL_PATHS(server)                   ((server).pathToLabelFile)
#define LBL_PATH(server, f)                 ((LBL_PATHS((server)))[(f)])

#define LBL_DATASET(server)                 ((server).pathToLabelDataset)

#define LBL_NUMBER(server)                  ((server).numberOfDistinctLabels)
#define LBL_LIST(server)                    ((server).listOfDistinctLabels)
#define LBL_VALUE(server, i)                ((LBL_LIST(server))[(i)])
#define LBL_COUNT(server, i)                ((server).labelCounts[(i)])
#define LBL_COUNTF(server, f, i)            ((server).labelCountsPerFile[(f)][(i)])

#define LBL_LABELS( server, f, t)           ((server).label[(f)]+(server).indexOfFirstLabelPerFilePerTimerange[(f)][(t)])
#define LBL_NLABELS(server, f, t)           ((server).numberOfLabelsPerFilePerTimerange[(f)][(t)])
#define LBL_LABEL(  server, f, t, r)        ((LBL_LABELS((server), (f), (t)))[(r)])

#define LBL_TIMELINES(server)               ((server).labelTimeline)
#define LBL_TIMELINE(server, f)             ((LBL_TIMELINES((server)))[(f)])
#define LBL_NTIMERANGES(server, f)          ((server).lengthOfLabelTimeline[(f)])
#define LBL_TIMERANGE(server, f, t)         ((LBL_TIMELINE((server), (f)))[(t)])

// Macro for easy access to data labels

#define DAT_AVAILABLE(server)               ((server).servesData)

#define DAT_NFILES(server)                  ((server).numberOfDataFiles)
#define DAT_PATHS(server)                   ((server).pathToDataFile)
#define DAT_PATH(server, f)                 ((DAT_PATHS((server)))[(f)])

#define DAT_DATASET(server)                 ((server).pathToDataDataset)


#define DAT_MATCHING_LABEL(server, f, t)    ((server).propagatedLabel[(f)][(t)])
#define DAT_FILTERED(server, f, t)          ((server).filterMask[(f)][(t)])

#define DAT_LABELS( server, f, t)           (LBL_LABELS( (server), (f), DAT_MATCHING_LABEL((server), (f), (t))     ))
#define DAT_NLABELS(server, f, t)           (LBL_NLABELS((server), (f), DAT_MATCHING_LABEL((server), (f), (t))     ))
#define DAT_LABEL(  server, f, t, r)        (LBL_LABEL(  (server), (f), DAT_MATCHING_LABEL((server), (f), (t)), (r)))

#define DAT_TIMELINES(server)               ((server).dataTimeline)
#define DAT_TIMELINE(server, f)             ((DAT_TIMELINES((server)))[(f)])
#define DAT_NTIMERANGES(server, f)          ((server).lengthOfDataTimeline[(f)])
#define DAT_TIMERANGE(server, f, t)         ((DAT_TIMELINE((server), (f)))[(t)])


#endif

