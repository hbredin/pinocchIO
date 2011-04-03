from pinocchIO import PYOTimeline
from matplotlib.collections import LineCollection
from matplotlib import pyplot
import numpy as np

def FromFile(pyoFile, path, loadTimeline=True, assumeSorted=False):
    """
    Create dataset by reading from pinocchIO file
        - pyoFile as PYOFile
        - path as found in pyoFile.datasets() output
        - also loads timeline if loadTimeline is set to True (default)
        - if assumeSorted is True, it will be faster -- be sure to know what you are doing
    """
    
    # Read HDF5 dataset used to store data layout
    linkset = pyoFile.h5file['/dataset/' + path + '/link']
    # number[t] is the number of vectors for tth time range
    number = np.zeros(linkset.shape, dtype=np.int32)
    # position[t] is the position of first vector for tth time range
    position = np.zeros(linkset.shape, dtype=np.int32)
    for t in range(linkset.shape[0]):
        number[t] = linkset[t][1]
        position[t] = linkset[t][0]
    
    # Read HDF5 dataset used to store actual data
    dataset = pyoFile.h5file['/dataset/' + path + '/data']
    # Data base type
    basetype    = dataset.dtype.base
    dimension   = dataset.dtype.shape[0]
    
    if assumeSorted and dimension != 1:
        print "TODO: implement assumeSorted with dimension != 1"
    
    if assumeSorted and dimension == 1:
        data = dataset.value
        stored = dataset.shape[0]
    else:            
        # Total number of data stored in pinocchIO dataset
        stored = 0
        for t in range(linkset.shape[0]):
            stored += linkset[t][1]
        # Data, sorted in chronological order
        data = np.empty((stored, dimension), basetype)
        counter = 0
        for t in range(linkset.shape[0]):
            for n in range(number[t]):
                data[counter, :] = dataset[position[t]+n]
                counter += 1
        position = number.cumsum()-number[0]
    
    timelinePath = dataset.attrs['timeline']
    
    # Timelined
    if loadTimeline:
        timeline = PYOTimeline.FromFile(pyoFile, timelinePath)
    else:
        timeline = None
    
    return PYODataset(data, number=number, timeline=timeline)

def Paste(datasets):
    
    # TODO => check all timelines are the same
    # TODO => check all datasets are the same length
    
    try:
        data = np.concatenate([dataset.data for d, dataset in enumerate(datasets)], axis=1)            
    except Exception, e:
        sys.stderr.write("Pasted datasets must have identical number of entries.")
        data = None
            
    return PYODataset(data, number=datasets[0].number, timeline=datasets[0].timeline)


class PYODataset(object):
    def __init__(self, data, number=None, timeline=None):
        super(PYODataset, self).__init__()
        self.data = data
        if data == None:
            self.number = None
            self.position = None
            stored = 0
        else:
            stored, dimension = self.data.shape
            if number == None:
                self.number = np.ones((stored,), dtype=np.int32)
            else:
                self.number = np.array(np.copy(number), dtype=np.int32)
            self.position = np.copy(self.number)
            self.position[1:] = self.number[:-1].cumsum()
            self.position[0]  = 0
            
        if timeline:
            self.timeline = timeline
            if self.timeline.getNumberOfTimeranges() != self.number.shape[0]:
                sys.stderr.write("Number of time ranges (%d) in time line and dimension (%d) of data array do not match.\n" % (self.timeline.getNumberOfTimeranges(), self.number.shape[0]))
                return None
    
    
    def UpdateTimeline(self, timeline):
        """
        Create a new PYODataset by aligning it on a new timeline
        
        Given a time range in new timeline,
            If no original timeline timerange intersects it, new dataset will contain nothing for the time range
            If at least one timerange intersects it, new dataset will contain the concatenated data of all intersecting timeranges 
        """
        
        number = np.ones((timeline.getNumberOfTimeranges(),), dtype=np.int32)
        indexOfData = []
        for t, timerange in enumerate(timeline.timeranges):
            # find original time ranges that intersect this time range
            ids = self.timeline.indexOfTimerangesInPeriod(timerange, strict=False)
            # count corresponding data
            number[t] = self.number[ids].sum()
            # keep track of ordered ids
            for i in ids:
                indexOfData = indexOfData + self.indexOfDataByTimerangeIndex(i)
        
        data = np.copy( self.data[indexOfData, :] )
        return PYODataset(data, number=number, timeline=timeline)
    
    def indexOfDataByTimerangeIndex(self, index):
        number = self.number[index]
        position = self.position[index]
        return range(position, position+number)
    
    
    def getSlice(self, period, strict=False):
        
        if self.timeline == None:
            sys.stderr.write("Dataset has no timeline.")
            return None
        
        Is = self.timeline.indexOfTimerangesInPeriod(period, strict=strict)
        if len(Is) == 0:
            return None
        else:
            data = np.concatenate([ self.data[self.indexOfDataByTimerangeIndex(i), :]  for i in Is])
        timeline = self.timeline.getSlice(period, strict=strict)
        
        return PYODataset(data, timeline=timeline) 
    
    
    def getData(self):
        return self.data
    
    
    def getDimension(self):
        
        if self.data == None:
            dimension = -1
        else:
            stored, dimension = self.data.shape
        return dimension
    
    
    def getDataForTimestamp(self, timestamp, strict=False):
        if self.timeline == None:
            sys.stderr.write("Dataset has no timeline.")
            return None
            
        Is = self.timeline.indexOfTimerangesContainingTimestamp(timestamp, strict=strict)
        data = np.concatenate([ self.data[self.indexOfDataByTimerangeIndex(i), :]  for i in Is])
        
        return data
    
    
    def getExtent(self):
        if self.timeline == None:
            sys.stderr.write("Dataset has no timeline.")
            return None
        else:
            return self.timeline.getExtent()
    
    
    def plot( self, ax=None, dimension=0, origin=None ):
        
        if self.timeline == None:
            raise NameError('NoTimeline')
        
        if ax == None:
            ax = pyplot.gca()
        
        if origin == None:
            origin = self.timeline.timeranges[0].getStart()
        
        lines = []
        for tr, timerange in enumerate(self.timeline.timeranges):
            for i in range(self.number[tr]):
                y  = self.data[self.position[tr]+i, dimension]
                xL = (timerange.getStart() - origin).total_seconds()
                xR = (timerange.getStop()  - origin).total_seconds()
                lines.append( [ (xL, y), (xR, y)] )
                
        xLower  = lines[0][0][0]
        xHigher = lines[0][0][0]
        yLower  = lines[0][0][1]
        yHigher = lines[0][0][1]
        
        for l, line in enumerate(lines):
            xLower  =  min(xLower,  line[0][0])
            xHigher =  max(xHigher, line[1][0])
            yLower  =  min(yLower,  line[0][1])
            yHigher =  max(yHigher, line[0][1])
        
        ax.add_collection(LineCollection(lines))
        ax.set_xlim(xLower,                          xHigher)
        ax.set_ylim(yLower - 0.1 * (yHigher-yLower), yHigher + 0.1 * (yHigher-yLower))
        
        return ax
        
        
        
        