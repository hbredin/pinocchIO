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
import numpy
from datetime import timedelta

def recursive_add_h5dataset(listOfDatasets, object):
    if type(object) == h5py.highlevel.Dataset:
        listOfDatasets.append(object.name)
    if type(object) == h5py.highlevel.Group or type(object) == h5py.highlevel.File:
        for i in object.keys():
            recursive_add_h5dataset(listOfDatasets, object[i])

class PYOFile:
    def __init__(self, path):
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
        
class PYOTimerange(object):
    def __init__(self, time, duration, scale):
        super(PYOTimerange, self).__init__()
        self.time =     timedelta(seconds=1.0*time/scale)
        self.duration = timedelta(seconds=1.0*duration/scale)
        
    def equals(self, other):
        return self.time == other.time and self.duration == other.duration
        
class PYOTimeline(object):
    def __init__(self, pyoFile, path):
        super(PYOTimeline, self).__init__()
        # Path to timeline
        self.path = path
        # Read HDF5 dataset used to store timeline
        timeset = pyoFile.h5file['/timeline/' + path]
        # Number of time ranges in timeline
        self.ntimeranges = timeset.shape[0]
        # Time ranges
        self.timeranges = []
        for t in range(self.ntimeranges):
            self.timeranges.append(PYOTimerange(timeset[t][0], timeset[t][1], timeset[t][2]))
            
    def equals(self, other):
        if self.ntimeranges != other.ntimeranges:
            return False
        
        for t in range(self.ntimeranges):
            if self.timeranges[t].equals(other.timeranges[t]) == False:
                return False
            
        return True

class PYODataset(object):
    def __init__(self, pyoFile, path):
        super(PYODataset, self).__init__()
        
        # Path to dataset
        self.path = path
        
        # Read HDF5 dataset used to store actual data
        dataset = pyoFile.h5file['/dataset/' + path + '/data']
        # Description
        self.description = dataset.attrs['description']
        # Data base type
        self.basetype    = dataset.dtype.base
        # Data dimension
        self.dimension   = dataset.dtype.shape[0]
        
        # Timeline
        self.timeline = PYOTimeline(pyoFile, dataset.attrs['timeline'])

        # Read HDF5 dataset used to store data layout
        linkset = pyoFile.h5file['/dataset/' + path + '/link']
                        
        # Total number of data stored in pinocchIO dataset
        self.stored = 0
        for t in range(self.timeline.ntimeranges):
            self.stored += linkset[t][1]
        
        # Data, sorted in chronological order
        self.data = numpy.empty((self.stored, self.dimension), self.basetype)
        # Number of data, sorted in chronological order
        self.number = []
        self.position = []
        
        counter = 0
        for t in range(self.timeline.ntimeranges):
            position = linkset[t][0]
            self.position.append(counter)
            self.number.append(linkset[t][1])
            for n in range(self.number[t]):
                self.data[counter, :] = dataset[position+n]
                counter = counter + 1
        

if __name__ == "__main__":
    pass