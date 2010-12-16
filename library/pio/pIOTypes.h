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

#ifndef _PINOCCHIO_TYPES_H
#define _PINOCCHIO_TYPES_H

#include <hdf5.h>

#define ERROR_SWITCH_INIT herr_t (*old_func)(hid_t, void*); void *old_client_data;		
#define ERROR_SWITCH_OFF  H5Eget_auto2(H5E_DEFAULT, &old_func, &old_client_data); H5Eset_auto2(H5E_DEFAULT, NULL, NULL);
#define ERROR_SWITCH_ON   H5Eset_auto2(H5E_DEFAULT, old_func, old_client_data);

/// generic pinocchIO object
typedef struct {
	hid_t identifier;
} PIOObject;

#define PIOMakeObject( any )  ((PIOObject){ any.identifier }) 
#define PIOObjectIsValid(o)   ((o).identifier > 0)
#define PIOObjectIsInvalid(o) (!PIOObjectIsValid(o))

typedef enum {
	/// Read only
	PINOCCHIO_READONLY,
	/// Read and write
	PINOCCHIO_READNWRITE	
} PIOFileRights;

/// pinocchIO file
typedef struct {
	hid_t identifier;
	PIOFileRights rights;
	char* medium;
} PIOFile;

#define PIOFileInvalid ((PIOFile) {-1, -1, NULL})

/// PIOTime structure is used to store time.
typedef struct {
	/// time in number of units
	int64_t time;
	/// number of units in one second
	int32_t scale;
} PIOTime;

typedef enum {
	PINOCCHIO_TIME_COMPARISON_ASCENDING = -1,
	PINOCCHIO_TIME_COMPARISON_SAME,
	PINOCCHIO_TIME_COMPARISON_DESCENDING,
} PIOTimeComparison;

/// PIOTimeRange structure is used to store time range.
typedef struct {
	/// start time in number of units
	int64_t time;
	/// range duration in number of units
	int64_t duration;
	/// number of units in one second
	int32_t scale;
} PIOTimeRange;

typedef enum {
	PINOCCHIO_TIMERANGE_COMPARISON_ASCENDING = -1,
	PINOCCHIO_TIMERANGE_COMPARISON_SAME,
	PINOCCHIO_TIMERANGE_COMPARISON_DESCENDING,
} PIOTimeRangeComparison;

/// pinocchIO timeline
typedef struct {
	hid_t identifier;
	int ntimeranges;
	PIOTimeRange* timeranges;
	char* path;
	char* description;
} PIOTimeline;

#define PIOTimelineInvalid ((PIOTimeline) {-1, -1, NULL, NULL, NULL})

typedef enum {
	/// Timelines are exactly the same
	PINOCCHIO_TIMELINE_COMPARISON_SAME,
	/// First timeline fully contains second timeline
	PINOCCHIO_TIMELINE_COMPARISON_SUPERSET,
	/// First timeline is fully included into second timeline 
	PINOCCHIO_TIMELINE_COMPARISON_SUBSET,
	/// Any other cases
	PINOCCHIO_TIMELINE_COMPARISON_OTHER
} PIOTimelineComparison;

typedef enum {
	/// Integer datatype
	PINOCCHIO_TYPE_INT,
	/// Float datatype
	PINOCCHIO_TYPE_FLOAT,
	/// Double datatype
	PINOCCHIO_TYPE_DOUBLE,
	/// String datatype,
	PINOCCHIO_TYPE_CHAR
} PIOBaseType;

/// pinocchIO datatype
typedef struct {
	hid_t identifier;
	PIOBaseType type;
	int dimension;
} PIODatatype;

#define PIODatatypeInvalid ((PIODatatype) {-1, -1, -1})

/// pinocchIO dataset
typedef struct {
	hid_t identifier; // data_identifier
	hid_t link_identifier;
	char* path;
	char* description;
	int stored; // number of stored elements
	int ntimeranges; // number of timeranges in dataset timeline ( = extent of link dataset)
    void* buffer;
    size_t buffer_size;
} PIODataset;

#define PIODatasetInvalid ((PIODataset) {-1, -1, NULL, NULL, -1, -1, NULL, 0})

/// pinocchIO server
typedef struct {
    int nfiles;
    char** files;
    char* dataset;
    int* ntimeranges;
    int* stored;
    PIODatatype datatype;
    
    int         current_file_index;
    PIOFile     current_file;
    
    int         current_timerange_index;
    PIODataset  current_dataset;
    PIOTimeline current_timeline;    
} PIOServer;

#define PIOServerInvalid ((PIOServer) {-1, NULL, NULL, NULL, NULL, PIODatatypeInvalid, -1, PIOFileInvalid, -1, PIODatasetInvalid, PIOTimelineInvalid})


#endif

