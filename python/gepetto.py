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
import numpy as np
import pynocchIO as pyo

class GPTServer:
    def __init__(self, files, dataset, prefix='', suffix='', fileByFile=False, assumeSorted=False):

        # add prefix (resp. suffix) before (resp. after) path to file
        # typically suffix would be '.pio'
        self.path2file = []
        for elt in files:
            self.path2file.append(prefix + elt + suffix)
            
        self.path2dataset = dataset
                
        # initialize data array
        if fileByFile:
            self.data = []
        else:
            self.data = None
        for path in self.path2file:
            # print 'Loading %s' % (path)
            f = pyo.PYOFile(path)
            d = pyo.PYODataset.FromFile(f, self.path2dataset, loadTimeline=False, assumeSorted=assumeSorted)
            f.close()
            
            if fileByFile:
                self.data.append(d.data)
            else:
                if self.data != None:
                    self.data = np.append(self.data, d.data, axis=0)
                else:
                    # get data dimension
                    self.dimension = d.dimension
                    # get data base type
                    self.basetype = d.basetype
                    # get data description
                    self.description = d.description
                    self.data = np.array(d.data, dtype=self.basetype, copy=True, ndmin=2)


if __name__ == "__main__":
    pass