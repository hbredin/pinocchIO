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

/**
 \defgroup objects Generic object
 \ingroup api
 
 @brief Some pinocchIO functions ask for PIOObject input arguments. 
 
 
 @{
 */

/// generic pinocchIO object
typedef struct {
	hid_t identifier; 
} PIOObject;

/**
	@brief Make generic pinocchIO object from pinocchIO file, timeline or dataset
	@param any pinocchIO file, timeline or dataset
	@returns Generic pinocchIO object
 */
#define PIOMakeObject( any )  ((PIOObject){ any.identifier })
 
/**
	@brief Test validity of pinocchIO object
	@param o pinocchIO object
	@returns 
        - 1 if object is valid
        - 0 if object is invalid
 */
#define PIOObjectIsValid(o)   ((o).identifier > 0)

/**
	@brief Test validity of pinocchIO object
	@param o pinocchIO object
	@returns
        - 1 if object is invalid
        - 0 if object is valid
 */
#define PIOObjectIsInvalid(o) (!PIOObjectIsValid(o))

/**
 @}
 */

/**
 \addtogroup file
 @{
 */


/**
 @brief File access rights

 pinocchIO provides a way for opening pinocchIO files in read-only mode
 or read-and-write mode (see \ref pioOpenFile).\n
 Note that \ref pioNewFile returns a pinocchIO file handle in read-and-write mode.
 */
typedef enum {
    /** Read-only mode */
	PINOCCHIO_READONLY,
    /** Read and write mode */
	PINOCCHIO_READNWRITE 
} PIOFileRights;

/**
	@brief pinocchIO file handle
 
    \ref PIOFile provides some kind of file object interface that can be manipulated
    by high-level pinocchIO functions.

    pinocchIO files are stored internally as HDF5 files.
    Developpers should not have to worry about \ref PIOFile internals.
    Their structure is strictly constrained by the pinocchIO library.
    However, we provide here a short description of the entrails of
    pinocchIO file handles.
 */
typedef struct {
    /**	HDF5 file identifier */
    hid_t identifier;
    /** Access rights granted to the user */
	PIOFileRights rights; 
    /** Path to the medium described by the pinocchIO file */
    char* medium; 
} PIOFile;

/**
	Invalid pinocchIO file
 */
#define PIOFileInvalid ((PIOFile) {-1, -1, NULL})

/**
 @}
 */

/**
 \addtogroup timeline
 @{
 */

/**
    pinocchIO time storage
 */
typedef struct {
	int64_t time; /**< time in number of units */
	int32_t scale; /**< number of units in one second */
} PIOTime;

/**
	Result of time comparison
 */
typedef enum {
	PINOCCHIO_TIME_COMPARISON_ASCENDING = -1, /**< Ascending order */
	PINOCCHIO_TIME_COMPARISON_SAME, /**< Simultaneity */
	PINOCCHIO_TIME_COMPARISON_DESCENDING /**< Descending order */
} PIOTimeComparison;

/**
	pinocchIO time range storage
 */
typedef struct {
    int64_t time; /**< start time in number of units */
    int64_t duration; /**< duration in number of units */
    int32_t scale; /**< number of units in one second */
} PIOTimeRange;

/**
	Result of time range comparison
 */
typedef enum {
	PINOCCHIO_TIMERANGE_COMPARISON_ASCENDING = -1, /**< Ascending order */
	PINOCCHIO_TIMERANGE_COMPARISON_SAME, /**< Equality */
	PINOCCHIO_TIMERANGE_COMPARISON_DESCENDING /**< Descending order */
} PIOTimeRangeComparison;

/**
	pinocchIO timeline
 */
typedef struct {
	hid_t identifier;           /**< HDF5 dataset identifier */
	int ntimeranges;            /**< number of time ranges */
	PIOTimeRange* timeranges;   /**< time ranges sorted in chronological order */
	char* path;                 /**< internal path to timeline in pinocchIO file */
	char* description;          /**< timeline textual description */
} PIOTimeline;

/**
	Invalid pinocchIO timeline
 */
#define PIOTimelineInvalid ((PIOTimeline) {-1, -1, NULL, NULL, NULL})

/**
 @}
 */

/**
 \addtogroup timecomparison
 @{
 */


/**
	Result of timeline comparison
 */
typedef enum {
    PINOCCHIO_TIMELINE_COMPARISON_SAME,     /**< Timelines are identical */
    PINOCCHIO_TIMELINE_COMPARISON_SUPERSET, /**< First timeline fully contains second timeline */
    PINOCCHIO_TIMELINE_COMPARISON_SUBSET,   /**< First timeline is fully included into second timeline */
    PINOCCHIO_TIMELINE_COMPARISON_OTHER     /**< Any other cases */
} PIOTimelineComparison;

/**
 @}
 */

/**
 \addtogroup datatype
 @{
 */


/**
	pinocchIO base type
 */
typedef enum {
    PINOCCHIO_TYPE_INT, /**< Integer */
    PINOCCHIO_TYPE_FLOAT, /**< Float */
    PINOCCHIO_TYPE_DOUBLE, /**< Double */
	PINOCCHIO_TYPE_CHAR /**< Char */
} PIOBaseType;

/**
	pinocchIO datatype
 */
typedef struct {
	hid_t identifier; /**< @brief HDF5 datatype identifier */
	PIOBaseType type; /**< array base type */
	int dimension; /**< array dimension */
} PIODatatype;

/**
	Invalid pinocchIO datatype
 */
#define PIODatatypeInvalid ((PIODatatype) {-1, -1, -1})

/**
 @}
 */

/**
 \addtogroup dataset
 @{
 */

/**
	pinocchIO dataset
 */
typedef struct {
	hid_t identifier; /**< HDF5 dataset identifier for path/data */
	hid_t link_identifier; /**< HDF5 dataset identifier ofr path/link */
	char* path; /**< internal path to dataset in pinocchIO file */
	char* description; /**< dataset textual description */
	int stored; /**< number of elements in dataset */
	int ntimeranges; /**< number of time ranges in dataset timeline */
	void* buffer; /**< internal buffer */
	size_t buffer_size; /**< internal buffer size */
} PIODataset;

/**
	Invalid pinocchIO dataset
 */
#define PIODatasetInvalid ((PIODataset) {-1, -1, NULL, NULL, -1, -1, NULL, 0})

#endif

/**
	@}
 */


