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

/**
 @defgroup time Time API
 @ingroup api
 
 @brief Functions dealing with pinocchIO timestamps and time ranges
 
*/

/**
 @defgroup timeline Timeline API
 @ingroup api

 @brief Functions dealing with pinocchIO timelines

 @{
 */
    
#ifndef _PINOCCHIO_TIMELINE_H
#define _PINOCCHIO_TIMELINE_H

#include "pIOTypes.h"

/**
 @brief Check timeline invalidity
 
 Check whether the pinocchIO timeline handle is invalid.
 
 @param[in] pioTimeline pinocchIO timeline handle
 @returns
 - TRUE if the pinocchIO timeline handle is invalid
 - FALSE otherwise
 */
#define PIOTimelineIsInvalid PIOObjectIsInvalid 


/**
 @brief Check timeline validity
 
 Check whether the pinocchIO timeline handle is valid.
 
 @param[in] pioTimeline pinocchIO timeline handle
 @returns
 - TRUE if the pinocchIO timeline handle is valid
 - FALSE otherwise
 */
#define PIOTimelineIsValid   PIOObjectIsValid 

/**
 @brief Create new pinocchIO timeline
 
 Create a new pinocchIO timeline in pinocchIO file \a pioFile at internal location \a path.
 
 - There must not be any existing timeline at internal location \a path.
 
 @param[in] pioFile pinocchIO file handle
 @param[in] path Internal path to the new timeline
 @param[in] description Textual description of the new timeline
 @param[in] numberOfTimeRanges Number of time ranges in the new timeline
 @param[in] timeranges Array of time ranges, sorted in chronological order
 @returns 
 - a pinocchIO timeline handle when successful
 - \ref PIOTimelineInvalid otherwise
 
 @note
 Use pioCloseTimeline() to close the timeline when no longer needed. 
 */
PIOTimeline pioNewTimeline(PIOFile pioFile, const char* path, const char* description,
							 int numberOfTimeRanges, PIOTimeRange* timeranges);

/**
 @brief Open pinocchIO timeline
 
 Open the pinocchIO file at internal location \a path in file containing \a pioObjectInFile.\n
 \a pioObjectInFile can be the file itself, a dataset stored in the same file, or another
 timeline stored in the same file. Just use \ref PIOMakeObject beforehand to convert it to a PIOObject.
 
 @param[in] pioObjectInFile PIOObject stored in the same file as requested timeline 
 @param[in] path Path to the existing pinocchIO timeline
 @returns
 - a pinocchIO timeline handle when successful
 - \ref PIOTimelineInvalid otherwise

 @note
 Use pioCloseTimeline() to close the timeline when no longer needed. 
 */
PIOTimeline pioOpenTimeline(PIOObject pioObjectInFile, const char* path);

/**
 @brief Close pinocchIO timeline
 
 Close a previously opened pinocchIO timeline.
 Upon success, the pinocchIO timeline handle will be set to \ref PIOTimelineInvalid.
 
 @param[in,out] pioTimeline pinocchIO timeline handle
 @returns
 - 1 when file is successfully closed
 - 0 otherwise
 */
int pioCloseTimeline( PIOTimeline* pioTimeline );

/**
 @}
 */

/**
 @brief Remove pinocchIO timeline
 
 Remove the pinocchIO timeline at internal location \a path from file containing \a pioObject.\n
 \a pioObject can be the file itself, another timeline stored in the same file, or a
 dataset stored in the same file. Just use \ref PIOMakeObject beforehand to convert it to a PIOObject.
 
 @param[in] pioObject PIOObject stored in the same file as requested timeline 
 @param[in] path Path to the pinocchIO timeline to be removed
 @returns
 - 1 if dataset was successfully removed
 - 0 otherwise
 
 @note
 In case the timeline is used by at least one dataset, it cannot be removed
 and pioRemoveTimeline() will return 0. Use pioRemoveDataset() to remove 
 dataset if needed.
 
 @ingroup file
 */    
int pioRemoveTimeline(PIOObject pioObject, const char* path);


/**
 @brief Get pinocchIO timeline from pinocchIO dataset
 
 Get the pinocchIO timeline to which the provided pinocchIO dataset is attached.
 
 @param[in] pioDataset  pinocchIO dataset handle
 @returns
    - a pinocchIO timeline handle when successful
    - \ref PIOTimelineInvalid otherwise
 
 @note
 Use pioCloseTimeline() to close the timeline when no longer needed. 
 
 @ingroup dataset
 */
PIOTimeline pioGetTimeline(PIODataset pioDataset);

/**
 @brief Get list of pinocchIO timelines
 
 Get the list of internal paths to all pinocchIO timelines stored in \a pioFile.
 
 @param[in] pioFile pinocchIO file handle
 @param[out] pathsToTimelines Array of paths to timelines
 @returns number of timelines
 
 \note
 \a pathsToTimelines is allocated by \ref pioGetListOfTimelines. It must be freed
 when no longer useful to avoid memory leaks:
 \verbatim
 char** pathsToTimelines = NULL;
 int numberOfTimelines;
 
 // obtain list of path to timelines
 numberOfTimelines = pioGetListOfTimelines(pioFile, &pathsToTimelines);
 // pathsToTimelines[i] contains paths to ith timeline
 // ... do what needs to be done ...
 
 // free memory
 for( int i=0; i<numberOfTimelines; i++) free(pathsToTimelines[i]);
 free(pathsToTimelines); 
 \endverbatim
 
 @ingroup file
 */
int pioGetListOfTimelines(PIOFile pioFile, char*** pathsToTimelines);



/**
 @brief Copy a timeline from one file to another one
 
 Copy timeline at location @a path from @a input file into @a output file.  
 
 @param path[in] Path to timeline
 @param input[in] Input file
 @param output[in] Output file
 @returns 
    - TRUE when successful
    - FALSE otherwise
 
 @note
 In case a timeline already exists at the same @a path in the @a output file,
 pioCopyTimeline() will check whether it is identical to the input timeline.
 If so, it will do nothing and return TRUE. Otherwise, it will return FALSE.
 
 @ingroup file
 */
int pioCopyTimeline(const char* path, PIOFile input, PIOFile output);


#endif

