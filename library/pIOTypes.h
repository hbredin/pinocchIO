/*
 *  pIOTypes.h
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 14/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

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
} PIODataset;

#define PIODatasetInvalid ((PIODataset) {-1, -1, NULL, NULL, -1, -1})


#endif

