from pinocchIO import PYOTimerange
from matplotlib.collections import LineCollection
from matplotlib import pyplot

def FromFile(pyoFile, path):
    """
    Create timeline by reading from pinocchIO file
        - pyoFile as PYOFile
        - path as found in pyoFile.timelines() output
    """
    
    # Read HDF5 dataset used to store timeline
    timeset = pyoFile.h5file['/timeline/' + path]
    
    # Number of time ranges in timeline
    ntimeranges = timeset.shape[0]
    
    # Time ranges
    timeranges = []
    for t in range(ntimeranges):
        timeranges.append(PYOTimerange.FromTimeset(timeset[t]))
    
    return PYOTimeline(timeranges)


def Combination( timelines ):
    """
    Create timeline by combining multiple input timelines
         - timelines is an array of PYOTimeline objects
         
    Put graphically, that's what Combination does:
    Input 1 =      |-----|      |------|    |---------------|
    Input 2 =  |------| |---|      |------------|
    Ouput =    |---|--|-||--|---|--|---|----|---|-----------|
    """
    
    # Concatenate start and stop timestamp of every time range of every time line
    timestamps = []
    for t, timeline in enumerate( timelines ):
        for r, timerange in enumerate( timeline.timeranges ):
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
            
    return PYOTimeline( timeranges )
    

class PYOTimeline(object):
    
    def __init__(self, timeranges):
        super(PYOTimeline, self).__init__()
        self.timeranges = timeranges
    
    
    def indexOfTimerangesInPeriod(self, period, strict=False):
        """
        Find index of every time range (strictly?) contained by the provided period timerange
        """
        
        if strict:
            Is = [ i for i, timerange in enumerate(self.timeranges) if period.includes(timerange, strict=True)]
        else:
            Is = [ i for i, timerange in enumerate(self.timeranges) if period.intersects(timerange)]
        
        return Is
    
    
    def getSlice(self, period, strict=False):
        """
        Create a new PYOTimeline as a slice defined by 'period' 
        """
        Is = self.indexOfTimerangesInPeriod(period, strict=strict)
        return PYOTimeline([timerange for i, timerange in enumerate(self.timeranges) if i in Is])
    
    
    def getNumberOfTimeranges(self):
        """
        Returns number of time ranges in timeline
        """
        return len(self.timeranges)
    
    
    def equals(self, other):
        """
        Returns True if other timeline is identical
            and False otherwise
        """
        if self.getNumberOfTimeranges() != other.getNumberOfTimeranges():
            return False
        
        for t in range(self.getNumberOfTimeranges):
            if self.timeranges[t].equals(other.timeranges[t]) == False:
                return False
            
        return True
    
    
    def indexOfTimerangesContainingTimestamp(self, timestamp, strict=False):
        """
        Find index of every time range containing the provided timestamp
        """
        
        Is = []
        for i, timerange in enumerate(self.timeranges):
            if timerange.contains(timestamp, strict=strict):
                # timerange contains timestamp. Yeaaaay!
                Is.append(i)
        
        return Is
    
    
    def getExtent(self):
        """
        Returns extent of current timeline (as PYOTimerange)
        The extent of a timeline is simply the shortest time range that contains all time ranges in timeline
        """
        
        
        if self.getNumberOfTimeranges() > 0:
            start = min( [timerange.getStart() for i, timerange in enumerate(self.timeranges)])
            stop  = max( [timerange.getStop()  for i, timerange in enumerate(self.timeranges)])
            duration = stop - start
        else:
            start = datetime.now()
            duration = timedelta(seconds=0)
        
        return PYOTimerange.PYOTimerange(start, duration)
    
    
    def plot( self, ax=None, origin=None, levels=[0.0], width=0.1, colors=['blue', 'red'] ):
        
        if origin == None:
            origin = self.timeranges[0].getStart()
        
        if ax == None:
            ax = pyplot.gca()
        
        lines = []
        nLevels = len(levels)
        for tr, timerange in enumerate(self.timeranges):
            y  = levels[tr % nLevels]
            xL = (timerange.getStart() - origin).total_seconds()
            xR = (timerange.getStop()  - origin).total_seconds()
            lines.append( [ (xL, y-width), (xL, y+width)])
            lines.append( [ (xL, y),       (xR, y)      ])
            lines.append( [ (xR, y-width), (xR, y+width)])
        
        xLower  = lines[0][0][0]
        xHigher = lines[0][0][0]
        
        for l, line in enumerate(lines):
            xLower  =  min(xLower,  line[0][0])
            xHigher =  max(xHigher, line[1][0])
        
        ax.add_collection(LineCollection(lines, colors=sorted(colors*3)))
        ax.set_xlim(xLower,              xHigher)
        ax.set_ylim(min(levels)-10*width, max(levels)+10*width)
        
        return ax
    
