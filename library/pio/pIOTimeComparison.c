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

#include "pIOTimeComparison.h"

// int64_t comparison
int compare_int64_t( int64_t ll1, int64_t ll2 )
{
	if (( ll1 < ll2  )) return -1;
	if (( ll1 == ll2 )) return 0;
	if (( ll1 > ll2  )) return 1;
	return 0; 
}

PIOTimeRangeComparison pioCompareTimeRanges( PIOTimeRange tr1, PIOTimeRange tr2)
{
	int compareStart = compare_int64_t(tr1.time * tr2.scale, 
									   tr2.time * tr1.scale);
	int compareStop  = compare_int64_t((tr1.time+tr1.duration) * tr2.scale, 
									   (tr2.time+tr2.duration) * tr1.scale);
	
	switch (compareStart) {
			// tr1 begins before tr2
		case -1:
			return PINOCCHIO_TIMERANGE_COMPARISON_ASCENDING;
			break;
			
		case 0:
			switch (compareStop) {
					// tr1 stops before tr2
				case -1:
					return PINOCCHIO_TIMERANGE_COMPARISON_ASCENDING;
					break;
					
				case 0:
					return PINOCCHIO_TIMERANGE_COMPARISON_SAME;
					break;
					
				case 1:
					return PINOCCHIO_TIMERANGE_COMPARISON_DESCENDING;
					break;				
			}
			break;
			
			// tr2 begins before tr1
		case 1:
			return PINOCCHIO_TIMERANGE_COMPARISON_DESCENDING;
			break;		
	}
	return 0;
}

PIOTimeComparison pioCompareTimes( PIOTime t1, PIOTime t2)
{
	PIOTimeRange tr1; 
	PIOTimeRange tr2;
	
	tr1.time = t1.time;
	tr1.scale = t1.scale;
	tr1.duration = 0;
	
	tr2.time = t2.time;
	tr2.scale = t2.scale;
	tr2.duration = 0;
	
	return (PIOTimeComparison)pioCompareTimeRanges(tr1, tr2);
}

int pioTimeInTimeRange( PIOTime t, PIOTimeRange tr)
{
	return ((  tr.time              * t.scale <= t.time * tr.scale ) && 
			( (tr.time+tr.duration) * t.scale >= t.time * tr.scale ));	
}

int pioTimeRangeInTimeRange( PIOTimeRange tr1, PIOTimeRange tr2)
{
	return ((	tr1.time				* tr2.scale >=  tr2.time				* tr1.scale) && 
			(  (tr1.time+tr1.duration)	* tr2.scale <= (tr2.time+tr2.duration)	* tr1.scale));
}

PIOTime pioGetTimeMax(PIOTime t1, PIOTime t2)
{
    if (t1.time * t2.scale > t2.time * t1.scale) return t1;
    else return t2;
}

PIOTime pioGetTimeMin(PIOTime t1, PIOTime t2)
{
    if (t1.time * t2.scale < t2.time * t1.scale) return t1;
    else return t2;    
}

PIOTimeRange pioGetTimeRangeBetweenTimes( PIOTime start, PIOTime stop)
{
    if (pioCompareTimes(start, stop) == PINOCCHIO_TIME_COMPARISON_DESCENDING)
        return PIOTimeRangeEmpty;
    
    if (start.scale == stop.scale)
        return (PIOTimeRange){start.time, stop.time-start.time, start.scale};
    
    return (PIOTimeRange){start.time*stop.scale, stop.time*start.scale - start.time*stop.scale, start.scale*stop.scale};
}

PIOTimeRange pioGetTimeRangeIntersection( PIOTimeRange tr1, PIOTimeRange tr2)
{
    PIOTime start1, start2, stop1, stop2;
    
    start1 = (PIOTime){tr1.time, tr1.scale};
    start2 = (PIOTime){tr2.time, tr2.scale};
    stop1  = (PIOTime){tr1.time+tr1.duration, tr1.scale};
    stop2  = (PIOTime){tr2.time+tr2.duration, tr2.scale};
    
    return pioGetTimeRangeBetweenTimes(pioGetTimeMax(start1, start2),
                                       pioGetTimeMin(stop1, stop2));
}

int pioTimeRangeIntersectsTimeRange( PIOTimeRange tr1, PIOTimeRange tr2)
{
    PIOTimeRange intersection = pioGetTimeRangeIntersection(tr1, tr2);
    
    return (intersection.duration > 0);
}

#pragma mark Timelines functions

// dichotomic search
int helper_pioFindTimeRangeInTimeLine( PIOTimeRange tr, PIOTimeRange* tl, int *left, int* right )
{
	int pivot;
	
	if (( *left >= *right )) return *left;
	
	pivot = (*left + *right) / 2;
	switch (pioCompareTimeRanges(tr, tl[pivot]))
	{
			// pivot is (strictly) bigger than tr
		case PINOCCHIO_TIMERANGE_COMPARISON_ASCENDING:
			*right = pivot-1;
			break;
			// pivot is (strictly) smaller than tr
		case PINOCCHIO_TIMERANGE_COMPARISON_DESCENDING:
			*left = pivot+1;
			break;
			// found it!
		case PINOCCHIO_TIMERANGE_COMPARISON_SAME:
			*left = pivot; *right = pivot;
			break;
	}
	return helper_pioFindTimeRangeInTimeLine( tr, tl, left, right);		
}

int pioFindTimeRangeInTimeLine( PIOTimeRange tr, PIOTimeRange* tl, int n)
{
	int found = -1;
	
	int left = 0; 
	int right = n;
	
	found = helper_pioFindTimeRangeInTimeLine(tr, tl, &left, &right );
	
	if (( pioCompareTimeRanges(tr, tl[found]) != PINOCCHIO_TIMERANGE_COMPARISON_SAME ))
		found = -1;
	
	return found;
}

PIOTimelineComparison pioCompareTimeLines (PIOTimeRange* tr1, int n1, PIOTimeRange* tr2, int n2)
{
	PIOTimelineComparison result;
	int t;
	
	if (( n1 > n2 ))
	{
		// switch roles of tr1 and tr2
		result = pioCompareTimeLines( tr2, n2, tr1, n1 );
		// result can be either PINOCCHIO_TIMELINE_SUBSET or PINOCCHIO_TIMELINE_OTHER
		if (result == PINOCCHIO_TIMELINE_COMPARISON_SUBSET)
			result = PINOCCHIO_TIMELINE_COMPARISON_SUPERSET;
	}
	else if (n1 == n2)
	{
		// check all timeranges
		result = PINOCCHIO_TIMELINE_COMPARISON_SAME;
		for (t=0; t<n1; t++) {
			if (( pioCompareTimeRanges(tr1[t], tr2[t]) != PINOCCHIO_TIMERANGE_COMPARISON_SAME ))
			{
				result = PINOCCHIO_TIMELINE_COMPARISON_OTHER;
				break;
			}
		}
	}
	else
	{
		// make sure all timeranges of smaller timeline is included in bigger one
		result = PINOCCHIO_TIMELINE_COMPARISON_SUBSET;
		for (t=0; t<n1; t++)
		{
			// if timerange 1 is not included in timeline 2, stops 
			if (( pioFindTimeRangeInTimeLine(tr1[t], tr2, n2) < 0 ))
			{
				result = PINOCCHIO_TIMELINE_COMPARISON_OTHER;
				break;
			}
		}
	}
	
	return result;
}
