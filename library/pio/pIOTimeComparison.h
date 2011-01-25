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

#ifndef _PINOCCHIO_TIMERANGE_H
#define _PINOCCHIO_TIMERANGE_H

#include "pIOTypes.h"

/**
	@brief Compare two time ranges
 
	@param[in] tr1 first time range
	@param[in] tr2 second time range
	@returns 
        - @ref PINOCCHIO_TIMERANGE_COMPARISON_SAME if \a tr1 equals \a tr2
        - @ref PINOCCHIO_TIMERANGE_COMPARISON_ASCENDING if \a tr1 starts before \a tr2
          or \a tr1 and \a tr2 starts simultaneously and \a tr1 ends before \a tr2
        - @ref PINOCCHIO_TIMERANGE_COMPARISON_DESCENDING if \a tr1 starts after \a tr2
          or \a tr1 and \a tr2 starts simultaneously and \a tr1 ends after \a tr2
    
    @image	html timecomparison.png "Comparing tr1 with tr2"
    @image	latex timecomparison.eps "Comparing tr1 with tr2" width=\textwidth
 
    @ingroup time
 */
PIOTimeRangeComparison pioCompareTimeRanges( PIOTimeRange tr1, PIOTimeRange tr2);

/**
 @brief Compare two timestamps
 
 @param[in] t1 first timestamp
 @param[in] t2 second timestamp

 @returns 
    - @ref PINOCCHIO_TIME_COMPARISON_SAME if \a t1 equals \a t2
    - @ref PINOCCHIO_TIME_COMPARISON_ASCENDING if \a t1 happens before \a t2
    - @ref PINOCCHIO_TIME_COMPARISON_DESCENDING if \a t1 happens after \a t2
  
 @ingroup time
 */
PIOTimeComparison pioCompareTimes( PIOTime t1, PIOTime t2);

/**
 @brief Compare timestamp and time range

 @param[in] t Timestamp
 @param[in] tr Time range

 @returns 
    - @ref PINOCCHIO_TIME_COMPARISON_SAME if \a t happens during \a tr
    - @ref PINOCCHIO_TIME_COMPARISON_ASCENDING if \a t happens before \a tr
    - @ref PINOCCHIO_TIME_COMPARISON_DESCENDING if \a t happens after \a tr
 
 @ingroup time
 */
PIOTimeRangeComparison pioCompareTimeAndTimeRange( PIOTime t, PIOTimeRange tr);

/**
 @brief Check inclusion of timestamp in time range

 @param[in] t Timestamp
 @param[in] tr Time range
	
 @returns 
        - TRUE if \a t happens during \a tr
        - FALSE otherwise
 
 @ingroup time
 */
int pioTimeInTimeRange( PIOTime t, PIOTimeRange tr);


/**
 @brief Check inclusion of time range in time range
 
 @param[in] tr1 First time range
 @param[in] tr2 Second time range
 
 @returns 
 - TRUE if \a tr1 is included in \a tr2
 - FALSE otherwise

 @ingroup time
 */
int pioTimeRangeInTimeRange( PIOTimeRange tr1, PIOTimeRange tr2);

/**
 @brief Get latest timestamp among two timestamps

 @param[in] t1 First timestamp
 @param[in] t2 Second timestamp
 
 @returns latest timestamp among them
 
 @ingroup time
 */
PIOTime pioGetTimeMax(PIOTime t1, PIOTime t2);

/**
 @brief Get earliest timestamp among two timestamps
 
 @param[in] t1 First timestamp
 @param[in] t2 Second timestamp
 
 @returns earliest timestamp among them
 
 @ingroup time
 */
PIOTime pioGetTimeMin(PIOTime t1, PIOTime t2);

/**
 @brief Get time range starting and ending at provided timestamps
 
 @param[in] start Start timestamp
 @param[in] stop End timestamp
 
 @returns
    - time range starting at @a start and ending at @a stop 
    - PIOTimeRangeEmpty if @a start is later than @a stop.
 
 @ingroup time
 */
PIOTimeRange pioGetTimeRangeBetweenTimes( PIOTime start, PIOTime stop);

/**
 @brief Get intersection of two time ranges
 
 @param[in] tr1 First time range
 @param[in] tr2 Second time range
 
 @returns 
    - intersection of the two time ranges
    - PIOTimeRangeEmpy if intersection is empty
    
 @ingroup time
 */
PIOTimeRange pioGetTimeRangeIntersection( PIOTimeRange tr1, PIOTimeRange tr2);

/**
 @brief Check intersection of time ranges
 
 @param[in] tr1 First time range
 @param[in] tr2 Second time range
 
 @returns 
 - FALSE if intersection of @a tr1 and @a tr2 is empty
 - TRUE otherwise
 
 @ingroup time
 */
int pioTimeRangeIntersectsTimeRange( PIOTimeRange tr1, PIOTimeRange tr2);

/**
	@brief Search time range in timeline
 
	@param[in] tr Time range
	@param[in] tl Array of time ranges sorted chronologically
	@param[in] n Number of time ranges in \a tl
	@returns 
        - Index of \a tr in \a tl when found
        - Negative value otherwise
 
    @ingroup time
 */
int pioFindTimeRangeInTimeLine( PIOTimeRange tr, PIOTimeRange* tl, int n);

/**
 @brief Compare two timelines

 @param[in] tr1 First array of time ranges sorted chronologically 
 @param[in] n1 Number of time ranges in \a tr1
 @param[in] tr2 Second array of time ranges sorted chronologically
 @param[in] n2 Number of time ranges in \a tr2

 @returns 
    - @ref PINOCCHIO_TIMELINE_COMPARISON_SAME if timeline are identical
    - @ref PINOCCHIO_TIMELINE_COMPARISON_SUPERSET if \a tr1 contains all time ranges of \a tr2
    - @ref PINOCCHIO_TIMELINE_COMPARISON_SUBSET if all time ranges of \a tr1 are contained in \a tr2
    - @ref PINOCCHIO_TIMELINE_COMPARISON_OTHER in any other cases
    
 @ingroup timeline
 */
PIOTimelineComparison pioCompareTimeLines (PIOTimeRange* tr1, int n1, PIOTimeRange* tr2, int n2);



#endif
