#!/usr/bin/env python
# encoding: utf-8
"""
 Copyright 2010-2011 Herve BREDIN (bredin@limsi.fr)
 Contact: http://pinocchio.niderb.fr/
 
 This file is part of pinocchIO.
  
      pinocchIO is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.
  
      pinocchIO is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.
  
      You should have received a copy of the GNU General Public License
      along with pinocchIO. If not, see <http:www.gnu.org/licenses/>.
"""

import sys
import os
import h5py
import numpy as np
from datetime import timedelta, datetime

def recursive_add_h5dataset(listOfDatasets, object):
    if type(object) == h5py.highlevel.Dataset:
        listOfDatasets.append(object.name)
    if type(object) == h5py.highlevel.Group or type(object) == h5py.highlevel.File:
        for i in object.keys():
            recursive_add_h5dataset(listOfDatasets, object[i])

# -------------------------------------------------
class PYOFile(object):
    def __init__(self, path):
        super(PYOFile, self).__init__()
        self.h5file = h5py.File(path, 'r')
    
    
    def close(self):
        """Close pinocchIO file"""
        self.h5file.close()
    
    
    def datasets(self):
        h5datasets = []
        # get all HDF5 dataset paths contained in /dataset
        recursive_add_h5dataset(h5datasets, self.h5file['dataset'])
        datasets = []
        # each pinocchIO dataset is made of two datasets PATH/data and PATH/link
        for h5d in h5datasets:
            # get rid of PATH/link entries 
            if h5d.rsplit('/', 1)[1] == 'data':
                # remove /data suffix and /dataset prefix
                datasets.append('/' + h5d.rsplit('/', 1)[0].split('/', 2)[2])
        return datasets
    
    
    def timelines(self):
        h5timelines = []
        # get all HDF5 dataset paths contained in /timeline
        recursive_add_h5dataset(h5timelines, self.h5file['timeline'])
        timelines = []
        for h5d in h5timelines:
            # remove /timeline prefix
            timelines.append('/' + h5d.split('/', 2)[2])
        return timelines
    
    

# -------------------------------------------------
class PYOTimerange(object):
    def __init__(self):
        super(PYOTimerange, self).__init__()
        self.start = None
        self.duration = None
    
    
    def FromTimeset(self, timeset, origin=datetime(year=1981, month=4, day=17)):
        
        time = timeset[0]
        duration = timeset[1]
        scale = timeset[2]
        
        self.start = origin+timedelta(seconds=1.0*time/scale)
        self.duration = timedelta(seconds=1.0*duration/scale)
        
        return self
    
    
    def FromStartWithDuration(self, start, duration):
        
        self.start = start
        self.duration = duration
        
        return self
    
    
    def FromStartToStop(self, start, stop):
        
        self.start = start
        self.duration = stop-start
        
        return self
    
    
    def getStart(self):
        return self.start
    
    
    def getStop(self):
        return self.start + self.duration
    
    
    def getDuration(self):
        return self.duration
    
    
    def getMiddle(self):
        return self.start + timedelta(seconds=.5*self.duration.total_seconds())
    
    
    def equals(self, timerange):
        return cmp(self.getStart(), timerange.getStart()) == 0 and cmp(self.getStop(), timerange.getStop()) == 0
    
    
    def contains(self, timestamp, strict=False):
        """
        Returns True if timestamp (as datetime.datetime) happens during time range
            and False otherwise
        """
        if strict:
            return cmp(self.getStart(), timestamp) < 0 and cmp(self.getStop(), timestamp) > 0
        else:
            return cmp(self.getStart(), timestamp) <= 0 and cmp(self.getStop(), timestamp) >= 0
    
    
    def intersection(self, timerange):
        """
        Returns intersection with other time range (as PYOTimerange) if they do intersect
            and None otherwise
        """
        
        start = max(self.getStart(), timerange.getStart())
        stop  = min(self.getStop(),  timerange.getStop())
        duration = stop - start
        if duration.total_seconds() > 0:
            return PYOTimerange().FromStartWithDuration(start, duration)
        else:
            return None
    
    
    def intersects(self, timerange):
        """
        Returns True if intersection with other time range is not empty
            and False otherwise
        """
        return self.intersection(timerange) != None
    
    
    def includes(self, timerange, strict=False):
        """
        Returns True if other time range is fully included in time range
            and False otherwise
        """
        if strict:
            return cmp(self.getStart(), timerange.getStart()) < 0 and cmp(self.getStop(), timerange.getStop()) > 0
        else:
            return cmp(self.getStart(), timerange.getStart()) <= 0 and cmp(self.getStop(), timerange.getStop()) >= 0
    
    
    def included(self, timerange, strict=False):
        """
        Returns True if time range is included in other time range
        """
        return timerange.includes(self, strict=strict)
    
    
    def compare(self, timerange):
        """
        Compare relative position of time range [******] with other time range [------]
        
                      [******]
                      |      | [------]    => -6
                      |      [------]      => -5
                      |  [---|--]          => -4
                      | [----]             => -3
                      | [--] |             => -2
                      [------|--]          => -1
                      [------]             =>  0
                      [----] |             =>  1
                   [--|------|--]          =>  2
                   [--|------]             =>  3
                  [---|--]   |             =>  4
                [-----]      |             =>  5
              [-----] |      |             =>  6
                      [******]
        """
        # =============================================== #
        #                  [***...                        #
        #               [---...                           #
        # =============================================== #
        if cmp(timerange.getStart(), self.getStart()) < 0:
            return -timerange.compare(self)
        
        # =============================================== #
        #                  [***...                        #
        #                  [---...                        #
        # =============================================== #
        if cmp(timerange.getStart(), self.getStart()) == 0:
            
            # =============================================== #
            #                  [******]                       #
            #                  [------]                       #
            # =============================================== #
            if cmp(timerange.getStop(), self.getStop()) == 0:
                return 0
            
            # =============================================== #
            #                  [******]                       #
            #                  [---]                          #
            # =============================================== #
            if cmp(timerange.getStop(), self.getStop()) < 0:
                return -1
            
            # =============================================== #
            #                  [******]                       #
            #                  [---------]                    #
            # =============================================== #
            if cmp(timerange.getStop(), self.getStop()) > 0:
                return 1
            
        # =============================================== #
        #                  [***...                        #
        #                     [---...                     #
        # =============================================== #
        if cmp(timerange.getStart(), self.getStart()) > 0:
            
            # =============================================== #
            #             ...***]                             #
            #                   |  [---...                    #
            # =============================================== #
            if cmp(timerange.getStart(), self.getStop()) > 0:
                return -6
            
            # =============================================== #
            #             ...***]                             #
            #                   [---...                       #
            # =============================================== #
            if cmp(timerange.getStart(), self.getStop()) == 0:
                return -5
            
            # =============================================== #
            #             [******]                            #
            #                 [--|---]                        #
            # =============================================== #
            if cmp(timerange.getStop(), self.getStop()) > 0:
                return -4
            
            # =============================================== #
            #             [******]                            #
            #               [----]                            #
            # =============================================== #
            if cmp(timerange.getStop(), self.getStop()) == 0:
                return -3
            
            # =============================================== #
            #             [******]                            #
            #               [--]                              #
            # =============================================== #
            if cmp(timerange.getStop(), self.getStop()) < 0:
                return -2
    
    
    def __repr__(self):
        return "<PYOTimerange [%s --> %s]>" % (self.getStart(), self.getStop())
    
    
    def __str__(self):
        return "[%s --> %s]" % (self.getStart(), self.getStop())
    
    

# -------------------------------------------------
class PYOTimeline(object):
    
    def __init__(self):
        super(PYOTimeline, self).__init__()
        self.path = None
        self.timeranges = []
    
    
    def FromFile(self, pyoFile, path):
        
        # Path to timeline
        self.path = path
        
        # Read HDF5 dataset used to store timeline
        timeset = pyoFile.h5file['/timeline/' + path]
        
        # Number of time ranges in timeline
        ntimeranges = timeset.shape[0]
        
        # Time ranges
        self.timeranges = []
        for t in range(ntimeranges):
            self.timeranges.append(PYOTimerange().FromTimeset(timeset[t]))
        
        return self
    
    
    def FromTimeranges(self, timeranges):
        
        self.timeranges = timeranges
        return self
    
    
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
        Is = self.indexOfTimerangesInPeriod(period, strict=strict)
        return PYOTimeline().FromTimeranges([timerange for i, timerange in enumerate(self.timeranges) if i in Is])
    
    
    def getNumberOfTimeranges(self):
        return len(self.timeranges)
    
    
    def equals(self, other):
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
        
        if self.getNumberOfTimeranges() > 0:
            start = min( [timerange.getStart() for i, timerange in enumerate(self.timeranges)])
            stop  = max( [timerange.getStop()  for i, timerange in enumerate(self.timeranges)])
            duration = stop - start
        else:
            start = datetime.now()
            duration = timedelta(seconds=0)
        
        return PYOTimerange().FromStartWithDuration(start, duration)
    
    

# -------------------------------------------------
class PYODataset(object):
    def __init__(self):
        super(PYODataset, self).__init__()
        self.path = None
        self.description = None
        self.timelinePath = None
        self.timeline = None
    
    
    def FromFile(self, pyoFile, path, loadTimeline=True, assumeSorted=False):
        
        # Path to dataset
        self.path = path
        
        # Read HDF5 dataset used to store data layout
        linkset = pyoFile.h5file['/dataset/' + path + '/link']
        # number[t] is the number of vectors for tth time range
        self.number = np.zeros(linkset.shape, dtype=np.int32)
        # position[t] is the position of first vector for tth time range
        self.position = np.zeros(linkset.shape, dtype=np.int32)
        for t in range(linkset.shape[0]):
            self.number[t] = linkset[t][1]
            self.position[t] = linkset[t][0]
        
        # Read HDF5 dataset used to store actual data
        dataset = pyoFile.h5file['/dataset/' + path + '/data']
        # Description
        self.description = dataset.attrs['description']
        # Data base type
        basetype    = dataset.dtype.base
        dimension   = dataset.dtype.shape[0]
        
        if assumeSorted and dimension != 1:
            print "TODO: implement assumeSorted with dimension != 1"
        
        if assumeSorted and dimension == 1:
            self.data = dataset.value
            stored = dataset.shape[0]
        else:            
            # Total number of data stored in pinocchIO dataset
            stored = 0
            for t in range(linkset.shape[0]):
                stored += linkset[t][1]
            # Data, sorted in chronological order
            self.data = np.empty((stored, dimension), basetype)
            counter = 0
            for t in range(linkset.shape[0]):
                for n in range(self.number[t]):
                    self.data[counter, :] = dataset[self.position[t]+n]
                    counter += 1
            self.position = self.number.cumsum()-self.number[0]
        
        self.timelinePath = dataset.attrs['timeline']
        
        # Timelined
        if loadTimeline:
            self.timeline = PYOTimeline().FromFile(pyoFile, self.timelinePath)
        else:
            self.timeline = None
        
        return self
    
    
    def FromData(self, data, timeline=None):
        
        self.data = data
        
        if data == None:
            self.number = None
            self.position = None
            stored = 0
        else:
            stored, dimension = self.data.shape
            self.number = np.ones((stored,), dtype=np.int32)
            self.position = self.number.cumsum() - 1
        
        if timeline:
            self.timeline = timeline
            if self.timeline.getNumberOfTimeranges() != stored:
                sys.stderr.write("Number of time ranges (%d) in time line and dimension (%d) of data array do not match.\n" % (self.timeline.getNumberOfTimeranges(), stored))
                return None
        
        return self
    
    
    def Paste(self, datasets):
        
        # TODO => check all timelines are the same
        # TODO => check all datasets are the same length
        
        try:
            data = np.concatenate([dataset.data for d, dataset in enumerate(datasets)], axis=1)            
        except Exception, e:
            sys.stderr.write("Pasted datasets must have identical number of entries.")
            data = None
                
        timeline = datasets[0].timeline
        return self.FromData(data, timeline=timeline)
    
    
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
            data = None
        else:
            data = np.concatenate([ self.data[self.indexOfDataByTimerangeIndex(i), :]  for i in Is])
        timeline = self.timeline.getSlice(period, strict=strict)
        
        return PYODataset().FromData(data, timeline=timeline) 
    
    
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
    
    

# -------------------------------------------------
if __name__ == "__main__":
    pass