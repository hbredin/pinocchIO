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
 \defgroup timeline Timeline API
 \ingroup api
 
 @{
 */
    
#ifndef _PINOCCHIO_TIMELINE_H
#define _PINOCCHIO_TIMELINE_H

#include "pIOTypes.h"

/**
 @brief Check timeline invalidity
 
 Check whether the pinocchIO timeline handle is invalid.
 
 @param pioTimeline pinocchIO timeline handle
 @returns
 - TRUE if the pinocchIO timeline handle is invalid
 - FALSE otherwise
 */
#define PIOTimelineIsInvalid PIOObjectIsInvalid 


/**
 @brief Check timeline validity
 
 Check whether the pinocchIO timeline handle is valid.
 
 @param pioTimeline pinocchIO timeline handle
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
 */
int pioGetListOfTimelines(PIOFile pioFile, char*** pathsToTimelines);

/**
 @brief Get pinocchIO timeline from pinocchIO dataset
 
 Get the pinocchIO timeline to which the provided pinocchIO dataset is attached.
 
 @param[in] pioDataset  pinocchIO dataset handle
 @returns
    - a pinocchIO timeline handle when successful
    - \ref PIOTimelineInvalid otherwise
 */
PIOTimeline pioGetTimeline(PIODataset pioDataset);

#endif

/**
	@}
 */


