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

/// Time range comparison. 
///
/// \param[in] tr1	First time range.
/// \param[in] tr2	Second time range.
///
/// \returns	int
///		- PINOCCHIO_TIMERANGE_ASCENDING - means \a tr1 starts before \a tr2 (or \a tr1 and \a tr2 start times are identical but \a tr1 is shorter than \a tr2)
///		- PINOCCHIO_TIMERANGE_SAME - means \a tr1 and \a tr2 are identical
///		- PINOCCHIO_TIMERANGE_DESCENDING - means \a tr1 starts after \a tr2 (or \a tr1 ans \a tr2 start times are identical but \a tr2 is longer than \a tr2)
///
PIOTimeRangeComparison pioCompareTimeRanges( PIOTimeRange tr1, PIOTimeRange tr2);

/// Time comparison. 
///
/// \param[in] t1	First time.
/// \param[in] t2	Second time.
///
/// \returns	int
///		- PINOCCHIO_TIME_ASCENDING - means \a t1 is before \a t2 
///		- PINOCCHIO_TIME_SAME - means \a t1 and \a t2 are identical
///		- PINOCCHIO_TIME_DESCENDING - means \a t1 is after \a t2
///
PIOTimeComparison pioCompareTimes( PIOTime t1, PIOTime t2);

/// Time and timerange comparison.
///
/// \param[in] t	Time.
/// \param[in] tr	Timerange.
///
/// \returns	int
///		- PINOCCHIO_TIME_ASCENDING - means \a t happens before \a tr 
///		- PINOCCHIO_TIME_SAME - means \a t happens during \a tr
///		- PINOCCHIO_TIME_DESCENDING - means \a t happens after \a tr
///
PIOTimeRangeComparison pioCompareTimeAndTimeRange( PIOTime t, PIOTimeRange tr);

/// Time inclusion in timerange.
///
/// \param[in] t	Time.
/// \param[in] tr	Timerange.
///
/// \returns	int
///		- 1 - means \a t is included in \a tr 
///		- 0 - means \a t is not included in \a tr
///
int pioTimeInTimeRange( PIOTime t, PIOTimeRange tr);

/// Timerange inclusion.
///
/// \param[in] tr1	First time range.
/// \param[in] tr2	Second time range.
///
/// \returns	int
///		- 1 - means \a tr1 is entirely included in \a tr2 
///		- 0 - means \a tr1 is not entirely included in \a tr2
///
int pioTimeRangeInTimeRange( PIOTimeRange tr1, PIOTimeRange tr2);

/// Timerange intersection.
///
/// \param[in] tr1	First time range.
/// \param[in] tr2	Second time range.
///
/// \returns	int
///		- 1 - means intersection of \a tr1 and \a tr2 is not empty 
///		- 0 - means intersection of \a tr1 and \a tr2 is empty
///
int pioTimeRangeIntersectsTimeRange( PIOTimeRange tr1, PIOTimeRange tr2);

/// Search timerange in timeline
///
/// \param[in]	tr	timerange
/// \param[in]	tl	timeline
/// \param[in]	n	number of timeranges in timeline
/// 
///	\returns	int
///		- position of timerange in timeline when found
///		- negative value otherwise
int pioFindTimeRangeInTimeLine( PIOTimeRange tr, PIOTimeRange* tl, int n);

/// Compare two timelines
///
/// \param[in] tr1	First timeline
/// \param[in] n1	Number of timeranges in first timeline
/// \param[in] tr2	Second timeline
/// \param[in] n2	Number of timeranges in second timeline
/// 
/// \returns	int
///		
PIOTimelineComparison pioCompareTimeLines (PIOTimeRange* tr1, int n1, PIOTimeRange* tr2, int n2);

#endif
