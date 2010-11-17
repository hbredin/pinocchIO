/*
 *  pIOTimeComparison.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 15/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

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

int pioTimeRangeIntersectsTimeRange( PIOTimeRange tr1, PIOTimeRange tr2)
{
	PIOTime t1, t2;
	t1.time = tr1.time; t1.scale = tr1.scale;
	t2.time = tr2.time; t2.scale = tr2.scale;
	return (pioTimeInTimeRange(t1, tr2) || pioTimeInTimeRange(t2, tr1));
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
