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



/**
 @brief Label/data server
 
 @ingroup server
 */
typedef struct {
    
    // ===================================
    // Labels
    // ===================================
    
    /** 
     @brief Label availibility flag.
     
     Getter: LBL_AVAILABLE(server) 
     */
    int servesLabels;
    
    // -------------
    // configuration
    // -------------
    
    /** 
     @brief Number of label pinocchIO files 
     
     Getter: LBL_NFILES(server)
     */
    int numberOfLabelFiles;  
    
    /** 
     @brief List of paths to label pinocchIO files.
     
     pathToLabelFile[f] is the path to the fth label pinocchIO file\n
     Getter: LBL_PATH(server, f)
     */
    char** pathToLabelFile;
    
    /** 
     @brief Path to label pinocchIO dataset
     
     Getter: LBL_DATASET(server)
     */
    char* pathToLabelDataset;    
    
    // ----------
    // statistics
    // ----------
    
    /**
     @brief Number of distinct label values
     
     Getter: LBL_NUMBER(server)
     */
    int  numberOfDistinctLabels;
    
    /**
     @brief List of label values
     
     listofDistinctLabels[i] is the value of the ith label.\n
     Getter: LBL_VALUE(server, i)
     */
    int* listOfDistinctLabels;
    
    /**
     @brief Distribution of label values
     
     labelCounts[i] is the number of label timeranges with ith label value.\n
     Getter: LBL_COUNT(server, i)
     */
    int* labelCounts;
    
    /**
     @brief  Distribution of label values, for each file
     
     labelCountsPerFile[f][i] is the number of label timeranges with ith label value in fth label file.\n
     Getter: LBL_COUNTF(server, f, i)
     */
    int** labelCountsPerFile;
    
    // -------
    // storage
    // -------
    
    /** 
     @brief Length of timelines of pinocchIO label datasets, for each file
     
     lengthOfLabelTimeline[f] is the length of the timeline of the label dataset in the fth file.\n
     Getter: LBL_NTIMERANGES(server, f)
    */
    int* lengthOfLabelTimeline;  

    /** 
     @brief Timelines of pinocchIO lable datasets, for each file
     
     labelTimeline[f][t] is the tth timerange in the timeline of the label dataset for the fth file.\n 
     Getter: LBL_TIMERANGE(server, f, t)
     */
    PIOTimeRange** labelTimeline; 
    
    /** 
     @brief Labels, in chronological order, for each file
     
     There can be more than one label per timerange.\n     
     Getter: LBL_LABEL(server, f, t, i) is the ith label for the tth timerange in the label dataset timeline of the fth file.
     */
    int** label;
    
    /**
     @brief Number of labels per timerange, for each file
     
     numberOfLabelsPerFilePerTimerange[f][t] is the number of labels for tth timerange in the label dataset timeline of the fth file.\n
     Getter: LBL_NLABELS(server, f, t)
     */
    int** numberOfLabelsPerFilePerTimerange;
    
    /**
     @brief Position in @a label of the first label for each timerange, for each file
     
     labels[f][ indexOfFirstLabelPerFilePerTimerange[f][t] ] is the first label of the tth timerange
     in the label dataset timeline of the fth file.
     */
    int** indexOfFirstLabelPerFilePerTimerange; 
    
    // ===================================
    // Data
    // ===================================
    
    /** 
     @brief Data availibility flag.
     
     Getter: DAT_AVAILABLE(server) 
     */
    int servesData;
    
    // -------------
    // configuration
    // -------------
    
    /** 
     @brief Number of data pinocchIO files 
     
     Getter: DAT_NFILES(server)
     */
    int numberOfDataFiles;
    
    /** 
     @brief List of paths to data pinocchIO files.
     
     pathToDataFile[f] is the path to the fth data pinocchIO file\n
     Getter: DAT_PATH(server, f)
     */
    char** pathToDataFile;
    
    /** 
     @brief Path to label pinocchIO dataset
     
     Getter: LBL_DATASET(server)
     */
    char* pathToDataDataset;   
    
    // ----------
    // statistics
    // ----------    
    
    /**
     @brief Distribution of data with respect to labels
     
     dataCountsForLabel[i] is the number of data entries with ith label value.\n
     Getter: DAT_COUNT(server, i)
     */
    int*  dataCountsForLabel;
    
    /**
     @brief  Distribution of data with respect to labels, for each file
     
     dataCountsForLabelPerFile[f][i] is the number of data entries with ith label value in fth label file.\n
     Getter: DAT_COUNTF(server, f, i)
     */
    int** dataCountsForLabelPerFile;    
    
    // -------
    // storage
    // -------
    
    /** 
     @brief Length of timelines of pinocchIO data datasets, for each file
     
     lengthOfDataTimeline[f] is the length of the timeline of the data dataset in the fth file.\n
     Getter: DAT_NTIMERANGES(server, f)
     */
    int* lengthOfDataTimeline;
    
    /** 
     @brief Timelines of pinocchIO data datasets, for each file
     
     dataTimeline[f][t] is the tth timerange in the timeline of the data dataset for the fth file.\n 
     Getter: DAT_TIMERANGE(server, f, t)
     */
    PIOTimeRange** dataTimeline;
    
    /**
     @brief Number of entries per timerange, for each file
     numberOfEntriesPerTimerangePerFile[f][t] is the number of entries stored in fth file for the tth timerange.
     */
    int** numberOfEntriesPerTimerangePerFile;
                
    // ===================================
    // Data filtering based on labels
    // ===================================
        
    // -------------
    // configuration
    // -------------
    
    /**
     @brief Type of label filter
     
     NONE, EQUALS_TO, DIFFERS_FROM, IS_GREATER_THAN, IS_SMALLER_THAN
     */
    GPTLabelFilterType labelFilterType;
    
    /**
     @brief Label filter reference value
     */
    int labelFilterReference;
    
    /**
     @brief Maximum number of samples per label
     */
    int maximumNumberOfSamplesPerLabel;

    // -------------------------
    // label/data correspondence
    // -------------------------
    
    /**
     @brief Index of the first label timerange intersecting a given data timerange
     
     firstCorrespondingLabelTimerange[f][t] is the index of label timerange corresponding to the tth data timerange of fth file.
     */
    int** firstCorrespondingLabelTimerange; 
    
    
    /**
     @brief Number of (consecutive) label timeranges intersecting a given data timerange
     
     numberOfCorrespondingLabelTimerange[f][t] is the number of label timeranges corresponding to the tth data timerange of fth file.
     */    
    int** numberOfCorrespondingLabelTimerange;
    
    
    // ---------
    // filtering
    // ---------
    /**
     @brief Indicates whether a data entry matches the label filter
     
     filtered[f][t] is set to 1 if the tth data timerange of fth file is served by the server.
     */
    int** filtered; 
    
    // ===================================
    // Internals
    // ===================================    
    
    PIODatatype datatype;
    PIODatatype labelDatatype;
    
    int         current_file_index;
    PIOFile     current_file;
    
    int         current_timerange_index;
    PIODataset  current_dataset;
    PIODatatype current_datatype;
    PIOTimeline current_timeline;   
    
    int         eof;
    
    int  current_data_labels_number;
    int* current_data_labels;
    
    
} GPTServer;

/**
 @brief Invalid Gepetto server
 
 @ingroup server
 */
#define GPTServerInvalid ((GPTServer) {             \
/* servesLabels */              0,                  \
/* numberOfLabelFiles */        -1,                 \
/* pathToLabelFile */           NULL,               \
/* pathToLabelDataset */        NULL,               \
/* numberOfDistinctLabels */    0,                  \
/* listOfDistinctLabels */      NULL,               \
/* labelCounts */               NULL,               \
/* labelCountsPerFile */        NULL,               \
/* lengthOfLabelTimeline */     NULL,               \
/* labelTimeline */             NULL,               \
/* label */                     NULL,               \
/* numberOfLabelsPerFilePerTimerange */     NULL,   \
/* indexOfFirstLabelPerFilePerTimerange */  NULL,   \
/* servesData */                0,                  \
/* numberOfDataFiles */         -1,                 \
/* pathToDataFile */            NULL,               \
/* pathToDataDataset */         NULL,               \
/* dataCountsForLabel */        NULL,               \
/* dataCountsForLabelPerFile */ NULL,               \
/* lengthOfDataTimeline */      NULL,               \
/* dataTimeline */              NULL,               \
/* numberOfEntriesPerTimerangePerFile */  NULL,     \
/* labelFilterType */           -1,                 \
/* labelFilterReference */      -1,                 \
/* maximumNumberOfSamplesPerLabel */ -1,            \
/* firstCorrespondingLabelTimerange */    NULL,     \
/* numberOfCorrespondingLabelTimerange */ NULL,     \
/* filtered */                  NULL,               \
/* datatype */                  PIODatatypeInvalid, \
/* labelDatatype */             PIODatatypeInvalid, \
/* current_file_index */        -1,                 \
/* curent_file */               PIOFileInvalid,     \
/* current_timerange_index */   -1,                 \
/* current_dataset */           PIODatasetInvalid,  \
/* current_datatype */          PIODatatypeInvalid, \
/* current_timeline */          PIOTimelineInvalid, \
/* eof */                       -1,                 \
/* current_data_labels_number */ -1,                \
/* current_data_labels */       NULL                \
})

/**
 @brief pinocchIO time range
 
 A time range can be seen as a time segment, with a start \a time and a \a duration.
 
 pinocchIO stores time ranges internally using three integer values: \a time, \a duration and \a scale.
 
 See @ref PIOTime for more detail on how to efficiently choose the \a scale.
 
 @ingroup time
 */
typedef struct {
    /** pinocchIO timerange */
    PIOTimeRange timerange;
    /** file index */
    int fileIndex;
} GPTTimeRange;

// Label-related getters

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

// Data-related getters

#define DAT_AVAILABLE(server)               ((server).servesData)

#define DAT_NFILES(server)                  ((server).numberOfDataFiles)
#define DAT_PATHS(server)                   ((server).pathToDataFile)
#define DAT_PATH(server, f)                 ((DAT_PATHS((server)))[(f)])

#define DAT_DATASET(server)                 ((server).pathToDataDataset)

#define DAT_COUNT(server, i)                ((server).dataCountsForLabel[(i)])
#define DAT_COUNTF(server, f, i)            ((server).dataCountsForLabelPerFile[(f)][(i)])

#define DAT_FILTERED(server, f, t)          ((server).filtered[(f)][(t)])

#define DAT_TIMELINES(server)               ((server).dataTimeline)
#define DAT_TIMELINE(server, f)             ((DAT_TIMELINES((server)))[(f)])
#define DAT_NTIMERANGES(server, f)          ((server).lengthOfDataTimeline[(f)])
#define DAT_TIMERANGE(server, f, t)         ((DAT_TIMELINE((server), (f)))[(t)])


#endif

