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
 \defgroup objects Object API
 \ingroup api
 
 @brief Functions dealing with pinocchIO generic object
 
 @{
 */

/**
	@brief pinocchIO generic object
 */
typedef struct {
    /**	HDF5 identifier */
	hid_t identifier; 
} PIOObject;

/**
 @brief Make pinocchIO object

 Make pinocchIO object from pinocchIO file, timeline or dataset
 
 @param[in] any pinocchIO file, timeline or dataset
 @returns pinocchIO generic object
 */
#define PIOMakeObject( any )  ((PIOObject){ (any).identifier })

/**
 @brief Check object validity
 
 Check whether the pinocchIO object handle is valid.
 
 @param[in] o pinocchIO object
 @returns 
    - TRUE if object is valid
    - FALSE otherwise
 */
#define PIOObjectIsValid(o)   ((o).identifier > 0)

/**
 @brief Check object invalidity

 Check whether the pinocchIO object handle is invalid.
 
 @param[in] o pinocchIO object
 @returns
    - TRUE if object is invalid
    - FALSE otherwise
 */
#define PIOObjectIsInvalid(o) (!PIOObjectIsValid(o))

/**
 @}
 */

/**
 @brief File access rights

 pinocchIO provides a way for opening pinocchIO files in read-only mode
 or read-and-write mode (see pioOpenFile()).\n
 Note that pioNewFile() returns a pinocchIO file handle in read-and-write mode.

 @ingroup file
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
 
    @ingroup file
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
	@brief Invalid pinocchIO file
 
    Invalid pinocchIO file handle returned by pioNewFile() and pioOpenFile()
    in case of failure.
 
    @ingroup file
 */
#define PIOFileInvalid ((PIOFile) {-1, -1, NULL})

/**
 @brief pinocchIO timestamp
 
 pinocchIO stores timestamps internally using two integer values: \a time and \a scale.
 
 To obtain the actual value in second of the timestamp, simply divide \a time by \a scale.
 For instance, (340, 1000) corresponds to 340ms.
 
 The \a scale can be adapted to the medium being processed:
 - when processing every frame of a video with a 25 fps frame rate,
 one can simply set the timestamp \a scale to 25 and \a time to the frame index.
 - for MFCC extracted using a sliding window with a 20ms step, one can use a 
 \a scale of 50 (=1/0.020) and increment \a time by 1 when the window goes to the
 next position
  
 @ingroup time
 */
typedef struct {
    /** Time in number of units */
	int64_t time;
    /** Units per second */
	int32_t scale;
} PIOTime;

/**
 @brief Timestamps comparison result
 @ingroup time
 */
typedef enum {
    /** Ascending order */
	PINOCCHIO_TIME_COMPARISON_ASCENDING = -1,
    /** Equality */
	PINOCCHIO_TIME_COMPARISON_SAME, 
    /** Descending order */
	PINOCCHIO_TIME_COMPARISON_DESCENDING
} PIOTimeComparison;


/**
 @brief pinocchIO time range
 
 A time range can be seen as a time segment, with a start \a time and a \a duration.
 
 pinocchIO stores time ranges internally using three integer values: \a time, \a duration and \a scale.

 See \ref PIOTime for more detail on how to efficiently choose the \a scale.
  
 @ingroup time
 */
typedef struct {
    /** Start time in number of units */
    int64_t time;
    /** Duration in number of units */
    int64_t duration;
    /** Units per second */
    int32_t scale;
} PIOTimeRange;

/**
 @brief Invalid pinocchIO time range
 @ingroup time
 */
#define PIOTimeRangeInvalid ((PIOTimeRange) {-1, -1, -1})

/**
 @brief Empty pinocchIO time range
 @ingroup time
 */
#define PIOTimeRangeEmpty ((PIOTimeRange) {-1, 0, -1})

/**
 @brief Time ranges comparison result
 @ingroup time
 */
typedef enum {
    /** Ascending order */
	PINOCCHIO_TIMERANGE_COMPARISON_ASCENDING = -1,
    /** Equality */
	PINOCCHIO_TIMERANGE_COMPARISON_SAME,
    /** Descending order */
	PINOCCHIO_TIMERANGE_COMPARISON_DESCENDING
} PIOTimeRangeComparison;


/**
 @brief pinocchIO timeline handle
 
 \ref PIOTimeline provides some kind of timeline object interface that can be manipulated
 by high-level pinocchIO functions.
 
 pinocchIO timelines are stored internally as HDF5 datasets.
 Developpers should not have to worry about \ref PIOTimeline internals.
 Their structure is strictly constrained by the pinocchIO library.
 However, we provide here a short description of the entrails of
 pinocchIO timeline handles.
 
 @ingroup timeline
 */
typedef struct {
    /** HDF5 dataset identifier */
	hid_t identifier;
    /** number of time ranges in timeline */
	int ntimeranges;
    /** time ranges sorted in chronological order */
	PIOTimeRange* timeranges;
    /** internal path to timeline in pinocchIO file */
	char* path;
    /** timeline textual description */
	char* description;
} PIOTimeline;

/**
 @brief Invalid pinocchIO timeline
 
 Invalid pinocchIO timeline handle returned by pioNewTimeline() and pioOpenTimeline()
 in case of failure.
 
 @ingroup timeline
 */
#define PIOTimelineInvalid ((PIOTimeline) {-1, -1, NULL, NULL, NULL})

/**
 @brief Timelines comparison result
 @ingroup timeline
 */
typedef enum {
    /** Equality */
    PINOCCHIO_TIMELINE_COMPARISON_SAME,
    /** Superset */
    PINOCCHIO_TIMELINE_COMPARISON_SUPERSET,
    /** Subset */
    PINOCCHIO_TIMELINE_COMPARISON_SUBSET,
    /** Any other case */
    PINOCCHIO_TIMELINE_COMPARISON_OTHER
} PIOTimelineComparison;

/**
	@brief pinocchIO base type
 
    pinocchIO handles a limited number of data type, the list of which is given
    below.
 
    @ingroup datatype
 */
typedef enum {
    /** Integer */
    PINOCCHIO_TYPE_INT,
    /** Float */
    PINOCCHIO_TYPE_FLOAT,
    /** Double */
    PINOCCHIO_TYPE_DOUBLE,
    /** Char */
	PINOCCHIO_TYPE_CHAR
} PIOBaseType;

/**
 @brief pinocchIO datatype handle
 
 pinocchIO datasets can be used to store various type of data as long as they 
 can be interpreted as multi-dimensional arrays.
 
 However, each pinocchIO dataset can store only one kind of data at a time --
 which is defined when the dataset is created using a pinocchIO datatype.
 
 @a dimension and @a type of values (see @ref PIOBaseType) are set once 
 and for all when the dataset is created. 
 
 However, the number of vectors can vary from one time range to another.
 For instance, SIFT descriptors are usually stored as 128-dimensional float vectors.
 However, there might 250 of them for the first frame of the video, and twice as 
 much in the next one.
 
 Dataset can also be used to store textual descriptors as string. 
 Simply create a dataset of 1-dimensional char vectors and store N vectors where N is the length of the string.
\verbatim
    char* text = ...
    ...
    PIODatatype textDatatype = pioNewDatatype(PINOCCHIO_TYPE_CHAR, 1);
    ...
    PIODataset textDataset = pioNewDataset(..., textDatatype);
    pioWrite(textDataset, ..., text, strlen(text), textDatatype);
    ...
    pioCloseDatatype(&textDatatype);
    pioCloseDataset(&textDataset);
    ...
\endverbatim
 
 \ref PIODatatype provides some kind of datatype object interface that can be manipulated
 by high-level pinocchIO functions.
 
 pinocchIO datatypes are stored internally as HDF5 datatypes.
 Developpers should not have to worry about \ref PIODatatype internals.
 Their structure is strictly constrained by the pinocchIO library.
 However, we provide here a short description of the entrails of
 pinocchIO datatype handles.
 
 @ingroup datatype
 */
typedef struct {
    /** HDF5 datatype identifier */
	hid_t identifier;
    /** base type */
	PIOBaseType type;
    /** array dimension */
	int dimension;
} PIODatatype;

/**
 @brief Invalid pinocchIO datatype
 
 Invalid pinocchIO datatype handle returned by pioNewDatatype()
 in case of failure.

 @ingroup datatype
 */
#define PIODatatypeInvalid ((PIODatatype) {-1, -1, -1})


/**
 @brief pinocchIO dataset handle
 
 \ref PIODataset provides some kind of dataset object interface that can be manipulated
 by high-level pinocchIO functions.
 
 pinocchIO datasets are stored internally as a pair of HDF5 datasets.
 Developpers should not have to worry about \ref PIODataset internals.
 Their structure is strictly constrained by the pinocchIO library.
 However, we provide here a short description of the entrails of
 pinocchIO dataset handles.
 
 @ingroup dataset
 */
typedef struct {
    /** HDF5 dataset identifier for actual data */
	hid_t identifier; 
    /** HDF5 dataset identifier for number of entries per time range */
	hid_t link_identifier;
	/** internal path to dataset in pinocchIO file */
    char* path;
    /** dataset textual description */
	char* description;
    /** total number of entries stored in dataset */
	int stored;
    /** number of time ranges in dataset timeline */
	int ntimeranges; 
    /** internal buffer - mostly used by pioWrite() */
	void* buffer;
    /** size of internal buffer, in bytes */
	size_t buffer_size;
} PIODataset;

/**
 @brief Invalid pinocchIO dataset
 
 Invalid pinocchIO dataset handle returned by pioNewDataset() and pioOpenDataset()
 in case of failure.

 @ingroup dataset
 */
#define PIODatasetInvalid ((PIODataset) {-1, -1, NULL, NULL, -1, -1, NULL, 0})

#endif
