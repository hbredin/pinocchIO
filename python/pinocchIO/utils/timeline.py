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


def sliding_window( duration, step, extent, position='center' ):
    """
    duration in seconds
    step     in seconds
    extent   as PYOTimerange
    
    sliding    [========================]
    window               [=========================]
               <--------duration-------->
               |--step-->
    
    position='center' 
              |-------------------------- extent --------------------------|
          [===|===] .................................................. [===|===]
    
    position='in'
              |-------------------------- extent --------------------------|
              [===|===] .......................................... [===|===]
    
    position='out'
              |-------------------------- extent --------------------------|
      [===|===] .......................................................... [===|===]
      
    """
    
    tdDuration     = timedelta(seconds=duration)
    tdHalfDuration = timedelta(seconds=.5*duration)
    tdStep         = timedelta(seconds=step)
    
    if position == 'in':
        start = extent.getStart()
        stop  = extent.getStop() - tdDuration
    elif position == 'out':
        start = extent.getStart() - tdDuration
        stop  = extent.getStop()
    else: 
        # 'center' or anything different from 'in' and 'out'
        start = extent.getStart() - tdHalfDuration
        stop  = extent.getStop()  - tdHalfDuration
    
    
    timeranges = []
    cur_start = start
    
    while( cur_start + tdDuration <= stop ):
        timeranges.append(PYOTimerange.PYOTimerange(cur_start, tdDuration))
        cur_start += tdStep
    
    return PYOTimeline.PYOTimeline( timeranges )


def constrained_sliding_window( length, original_timeline, position='left'):
    """
    duration in seconds
    step     in seconds
    extent   as PYOTimerange
    
    sliding    [========================]
    window               [=========================]
               <--------duration-------->
               |--step-->
    
    position='left' with length=3
              |--|--|--|--|--|--|--|--|-- extent --|--|--|--|--|--|--|--|--|
              [    0   ] ........................................ [   N-3  ]
                 [    1   ]                                          [ N-2 ]xx
                    [    2   ]                                          [-1]xx|xx
    
    position='right' with length=3
              |--|--|--|--|--|--|--|--|-- extent --|--|--|--|--|--|--|--|--|
         xx|xx[ 0] .............................................. [   N-1  ]
            xx[  1  ]                                          [   N-2  ]   
              [    2   ]                                    [   N-3  ]                                                                                         [     ]
                                                                                                                                            [  ]
    
    position='center' with length=3
              |--|--|--|--|--|--|--|--|-- extent --|--|--|--|--|--|--|--|--|
            xx[ 0   ] .............................................. [ N-1 ]xx
              [    1   ]                                          [   N-2  ]   
                 [   2    ]                                    [   N-3  ]                                                                                         [     ]
      
    """
    
    N = original_timeline.getNumberOfTimeranges()
    L = int(length)
    
    timeranges = []
    for t in range(N):
        if position == 'left':
            a = t
            b = t+L-1
        elif position == 'right':
            a = t-L+1
            b = t
        else:
            # position == 'center' or any value different from 'left' and 'right'
            a = t-(L-1)/2
            b = t+( L )/2
        
        a = max(0, a  )
        b = min(b, N-1)
        
        timerange = PYOTimerange.FromStartToStop(original_timeline[a].getStart(), original_timeline[b].getStop())
        timeranges.append( timerange )
    
    return PYOTimeline.PYOTimeline( timeranges )


