// 
// Copyright 2010 Herve BREDIN (bredin@limsi.fr)
// Contact: http://pinocchio.niderb.fr/
// 
// This file is part of pinocchIO.
//  
//      pinocchIO is free software: you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation, either version 3 of the License, or
//      (at your option) any later version.
//  
//      pinocchIO is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//  
//      You should have received a copy of the GNU General Public License
//      along with pinocchIO. If not, see <http://www.gnu.org/licenses/>.
// 


/**
\mainpage pinocchIO
 
 pinocchIO is a \link fileformat file format\endlink, a \link api C library \endlink
 and a set of \link cli tools \endlink for storing audiovisual document descriptors.
 
 pinocchIO is meant to be an <b>easy way for researchers</b> working in the wide area
 of content-based multimedia analysis to <b>store and share</b> any kind of <b>time-indexed
 description</b> of the content.
 
 The notion of <b>time-indexed description</b> includes (but is not restricted to)
 labelled and unlabelled <b>segmentations</b> of a multimedia document, <b>low-level descriptors</b> 
 directly extracted from an audio signal or the frames of a video, as well as manual
 or automatic semantic <b>annotations</b> of the document.
 
 For instance, a researcher working on the audiovisual indexing of persons in TV news
 can use pinocchIO to store in one single pinocchIO file the output of multiple
 algorithms applied on a selected video:
    - color histograms extracted every few frames,
    - shot boundary detection,
    - face detection,
    - optical character recognition, 
    - speech/non speech segmentation,
    - automatic speech recognition,
    - speaker diarization,
    - ...
 
 pinocchIO does <b>NOT</b> provide the necessary tools to automatically extract these 
 from an audiovisual document. It is a way for the members of the community to store
 and share the output of their own tools.

 \section documentation Documentation

 -# \subpage gettingstarted "Getting started with pinocchIO"
 -# \subpage fileformat "An overview of pinocchIO file format"
 -# \subpage cli "pinocchIO made easy with command line tools" 
 -# \subpage api "pinocchIO for developers"
 -# \subpage gepetto "Gepetto, a pinocchIO server"
 
 
 \section license License
 
\verbatim
 pinocchIO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 pinocchIO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with pinocchIO. If not, see <http://www.gnu.org/licenses/>.
\endverbatim 

 \author Hervé BREDIN <lastname@limsi.fr>
 
 */

/**
 \page gettingstarted Getting started

 \section require Requirements
 
 \par HDF5
 pinocchIO file format is based on the HDF5 file format. Therefore, the HDF5
 library is a mandatory requirement in order to compile and use pinocchIO.\n
 Source code can be downloaded from the HDFgroup website. 
 pinocchIO has been tested with version 1.8.5-patch1 and the corresponding tar
 archive should be available in \e dependencies/hdf5-1.8.5-patch1.tar.gz
 
 \par libconfig
 A pinocchIO data server called gepetto is also part of the pinochIO suite.
 gepetto configuration files follows a well-defined format provided by 
 libconfig library. Source code can be downloaded from 
 the libconfig website. geppeto has been tested with version 1.4.6 and the 
 corresponding tar archive should be available in 
 \e dependencies/libconfig-1.4.6.tar.gz
  
 \par cmake
 pinocchIO uses \b cmake as its primary compilation system.
 It was successfully compiled on Linux and Mac OS X 10.6 with cmake version 2.8.3
 
 \section get Download pinocchIO
 Two options are available:
 - Download source code from http://pinocchio.niderb.fr
 - Or get development code directly from the Mercurial repository
 \verbatim
    $ hg clone https://hg.niderb.fr/Development/pinocchIO/\endverbatim
 
 \section install Install pinocchIO
 
 - Create build directory
 \verbatim
    $ cd pinocchio
    $ mkdir build
    $ cd build\endverbatim
 - Generate Makefile using cmake
 \verbatim
    $ cmake .. \endverbatim
 - Compile
 \verbatim
    $ make \endverbatim
 - Install
 \verbatim
    $ sudo make install \endverbatim
 */

/** 
 \page api Application Programming Interface
 
 - \ref file
 - \ref timeline
 - \ref dataset
 - \ref attribute
 - \ref gepetto
 
 */

/**
 \defgroup api API
 */

#ifndef _PINOCCHIO_H
#define _PINOCCHIO_H

#include "pIOTypes.h"
#include "pIOFile.h"
#include "pIOAttributes.h"
#include "pIOTimeline.h"
#include "pIOTimeComparison.h"
#include "pIODatatype.h"
#include "pIODataset.h"
#include "pIOWrite.h"
#include "pIORead.h"

#endif