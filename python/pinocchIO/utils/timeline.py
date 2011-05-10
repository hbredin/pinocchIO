from pinocchIO import PYOTimeline, PYOTimerange
from datetime import timedelta

def sub( timeline, extent ):
    """
    Create a new PYOTimeline as a slice defined by 'extent' 
    """
    
    if timeline.isEmpty():
        return PYOTimeline.Empty()
    
    trIDs = timeline.indexOfTimerangesInPeriod(extent, strict=False)
    
    if len(trIDs) == 0:
        return PYOTimeline.Empty()
    
    timeranges = [timerange.intersection(extent) for i, timerange in enumerate(timeline) if i in trIDs]
    
    return PYOTimeline.PYOTimeline(timeranges)


def combine( timelines ):
    """
    Create timeline by combining multiple input timelines
         - timelines is an array of PYOTimeline objects
    
    Put graphically, that's what 'combine' does:
    Input 1 =      |-----|      |------|    |---------------|
    Input 2 =  |------| |---|      |------------|
    Ouput =    |---|--|-||--|---|--|---|----|---|-----------|
    """
    
    # # First check timeline equality
    # # do nothing if timelines are identical
    # identical = True
    # reference = timelines[0]
    # for t, timeline in enumerate(timelines):
    #     if not timeline == reference:
    #         identical = False
    #         break 
    # if identical:
    #     return PYOTimeline.PYOTimeline(reference.timeranges)
    
    # Concatenate start and stop timestamp of every time range of every time line
    timestamps = []
    for t, timeline in enumerate( timelines ):
        for r, timerange in enumerate( timeline ):
            timestamps.append( timerange.getStart() )
            timestamps.append( timerange.getStop()  )
    
    # Sort timestamps (in place)
    timestamps.sort()
    
    # Build the resulting timeline
    timeranges = []
    for r in range(1, len(timestamps)):
        if timestamps[r-1] < timestamps[r]:
            timerange = PYOTimerange.FromStartToStop(timestamps[r-1], timestamps[r])
            timeranges.append( timerange )
    
    return PYOTimeline.PYOTimeline( timeranges )


def dummy( numberOfTimeranges ):
    return PYOTimeline.PYOTimeline( [PYOTimerange.FromTimeset([n, 1, 1]) for n in range(numberOfTimeranges)] )


def sliding_window( duration, step, extent ):
    """
    duration in seconds
    step     in seconds
    extent   as PYOTimerange
    """
    start = extent.getStart()
    stop  = extent.getStop()
    
    tdDuration = timedelta(seconds=duration)
    tdHalfDuration = timedelta(seconds=.5*duration)
    tdStep     = timedelta(seconds=step)
    
    timeranges = []
    cur_start = start - tdHalfDuration
    
    while( cur_start + tdHalfDuration <= stop ):
        timeranges.append(PYOTimerange.PYOTimerange(cur_start, tdDuration))
        cur_start += tdStep
    
    return PYOTimeline.PYOTimeline( timeranges )


def constrained_sliding_window( length, step, original_timeline):
    
    # number of time ranges in original timeline
    numberOfTimeranges = original_timeline.getNumberOfTimeranges()
    
    timeranges = []
    for t in range(0, numberOfTimeranges-length+1, step):
        left  = original_timeline[t]
        right = original_timeline[t+length-1]
        timerange = PYOTimerange.FromStartToStop(left.getStart(), right.getStop())
        timeranges.append(timerange)
        
    return PYOTimeline.PYOTimeline( timeranges )
    
    
    