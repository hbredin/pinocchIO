from pinocchIO import PYOTimeline
import pinocchIO.utils.timeline

from matplotlib.collections import LineCollection
from matplotlib import pyplot
import numpy as np


def Empty():
    return PYODataset(np.array(()), np.array((), dtype=np.int32), PYOTimeline.Empty())


def _fromFile(pyoFile, path, loadTimeline=True, assumeSorted=False):
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
        timeline = pyoFile.getTimeline(timelinePath)
    else:
        timeline = pinocchIO.utils.timeline.dummy( number.shape[0] )
    
    return PYODataset(data, number, timeline)


class PYODataset(object):
    
    def __init__(self, data, number, timeline):
        super(PYODataset, self).__init__()
        
        if data == None or data.shape == (0,) or number == None or timeline == None or number.shape[0] != timeline.getNumberOfTimeranges():
            self._data = np.array(())
            self._number = np.array((), dtype=np.int32)
            self._position = np.array((), dtype=np.int32)
            self._timeline = PYOTimeline.Empty()
            return
            
        self._data = data
        self._timeline = timeline
        
        stored, dimension = self._data.shape
        self._number = np.array(np.copy(number), dtype=np.int32)
        self._position = np.copy(self._number)
        self._position[1:] = self._number[:-1].cumsum()
        self._position[0]  = 0
    
    
    def isEmpty(self):
        # A dataset is considered empty if its timeline is empty
        # or if it contains no data
        return self._timeline.isEmpty() or np.sum(self._number) == 0
    
    
    def _indexOfDataByTimerangeIndex(self, index):
        number = self._number[index]
        position = self._position[index]
        return range(position, position+number)
    
    
    def __getitem__(self, t):
        return self._data[self._indexOfDataByTimerangeIndex(t), :]
    
    
    def getData(self):
        return self._data
    
    
    def getDimension(self):
        
        if self.isEmpty():
            return -1
        
        stored, dimension = self._data.shape
        return dimension
    
    
    def getTimeline(self):
        return self._timeline
    
    
    def getNumber(self):
        return self._number
        
    
    def getPosition(self):
        return self._position
    
    
    # def __getattr__(self, name):
    #     if name == 'data':
    #         return self.getData()
    #     elif name == 'dimension':
    #         return self.getDimension()
    #     elif name == 'number':
    #         return self.getNumber()
    #     elif name == 'timeline':
    #         return self.getTimeline()
    #     else:
    #         raise AttributeError()
        
    
    def getDataForTimestamp(self, timestamp, strict=False):
        
        Is = self._timeline.indexOfTimerangesContainingTimestamp(timestamp, strict=strict)
        data = np.concatenate([self[i] for i in Is])
        
        return data
    
    
    def getExtent(self):
        if self._timeline == None:
            sys.stderr.write("Dataset has no timeline.")
            return None
        else:
            return self._timeline.getExtent()
    
    
    # def __mul__(self, other):
    #     
    #     if type(other) == PYOTimeline:
    #         raise ValueError("Cannot multiply two datasets.")
    #     else:
    #         return PYODataset(self.data * other, number=self._number, timeline=self.timeline)
    # 
    # def __add__(self, other):
    #     
    #     if type(other) == PYOTimeline:
    #         
    #         # Combine timelines
    #         timeline = PYOTimeline.Combination([self.timeline, other.timeline])
    #         
    #         # Update self and other timelines
    #         ds1 = self.UpdateTimeline( timeline )
    #         ds2 = other.UpdateTimeline( timeline )
    #         
    #         # Check self and other 'number' compatibility
    #         if False in (ds1.number == ds2.number):
    #             raise ValueError("Number of entries do not match for each time range")
    #         else:
    #             number = ds1.number
    #         
    #         # Add data
    #         data = ds1.data + ds2.data
    #         
    #         return PYODataset(data, number=number, timeline=timeline)
    #         
    #     else:
    #         return PYODataset(self.data + other, number=self._number, timeline=self.timeline)
    
    
    def plot( self, ax=None, dimension=0, origin=None ):
        
        if self._timeline == None:
            raise ValueError('Cannot plot a dataset without a timeline')
        
        if ax == None:
            ax = pyplot.gca()
        
        if origin == None:
            origin = self._timeline[0].getStart()
        
        lines = []
        for tr, timerange in enumerate(self._timeline):
            for i in range(self._number[tr]):
                y  = self._data[self._position[tr]+i, dimension]
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
        
        
        
        