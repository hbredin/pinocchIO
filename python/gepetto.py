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
    def __init__(self, files, dataset, prefix='', suffix=''):

        base = []
        # files can be
        #   - a path to a file containing a list of paths to files
        if type(files) == str:
            f = open(files, 'r')
            for line in f:
                if line.strip() != '':
                    base.append(line.strip())
            f.close()
        #   - a list of files (or base names)
        else:
            for elt in files:
                base.append(elt)
                
        # up to here, base is a list of paths to files
        
        # add prefix (resp. suffix) before (resp. after) path to file
        # typically suffix would be '.pio'
        self.path2file = []
        for elt in base:
            self.path2file.append(prefix + elt + suffix)
            
        self.path2dataset = dataset
                
        # initialize data array
        self.data = None
        for path in self.path2file:
            print 'Loading %s' % (path)
            f = pyo.PYOFile(path)
            d = pyo.PYODataset(f, self.path2dataset)
            f.close()
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
            del d.data


if __name__ == "__main__":
    pass