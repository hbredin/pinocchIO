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

class PYOFile:
    def __init__(self, path):
        self.h5file = h5py.File(path, 'r')

    def close(self):
        """Close pinocchIO file"""
        self.h5file.close()

class PYOTimerange(object):
    def __init__(self, time, duration, scale):
        super(PYOTimerange, self).__init__()
        self.start = 1.0 * time / scale
        self.stop  = 1.0 * (time + duration) / scale
        self.duration = 1.0 * duration / scale

class PYODataset:
    def __init__(self, pyoFile, path):
        
        # Read HDF5 dataset used to store actual data
        dataset = pyoFile.h5file['/dataset/' + path + '/data']
        # Read HDF5 dataset used to store data layout
        linkset = pyoFile.h5file['/dataset/' + path + '/link']
        # Read path to timeline
        path2timeline = dataset.attrs['timeline']
        # Read HDF5 dataset used to store timeline
        timeset = pyoFile.h5file['/timeline/' + path2timeline]
        
        # Data base type
        self.basetype    = dataset.dtype.base
        # Data dimension
        self.dimension   = dataset.dtype.shape[0]
        # Number of time ranges in timeline
        self.ntimeranges = timeset.shape[0]
        # Timeline
        self.timeline = []
        for t in range(self.ntimeranges):
            self.timeline.append(PYOTimerange(timeset[t][0], timeset[t][1], timeset[t][2]))
        
        # Total number of data stored in pinocchIO dataset
        self.stored = 0
        for t in range(self.ntimeranges):
            self.stored += linkset[t][1]
        
        # Data, sorted in chronological order
        self.data = numpy.empty((self.stored, self.dimension), self.basetype)
        # Number of data, sorted in chronological order
        self.number = []
        self.position = []
        
        counter = 0
        for t in range(self.ntimeranges):
            position = linkset[t][0]
            self.position.append(counter)
            self.number.append(linkset[t][1])
            for n in range(self.number[t]):
                self.data[counter, :] = dataset[position+n]
                counter = counter + 1
        
