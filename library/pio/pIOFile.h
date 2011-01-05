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
 \page fileformat An overview of pinocchIO file format

 \section Outline
 -# \ref oneone
 -# \ref onemultiple
    -# \ref filesystem
    -# \ref description
 -# \ref timelinevsdataset
 -# \ref attributes
 
 pinocchIO file format can be summarized by the following equation.
 
 <center><b>one medium + multiple descriptions = one pinocchIO file</b></center>
 
 The idea behind pinocchIO is to be able to store and organize all the information
 related to a given medium into one single file.
 
 \section oneone One medium <--> One pinocchIO file
 
 Each pinocchIO file must only contain descriptions related to one medium.
 Therefore, any pinocchIO file contains a <b>specific area</b> where <b>the path 
 to the medium is stored</b> as soon as the pinocchIO file is created.
 
 For instance, one can used the \link pioinit \endlink command line tool to
 create a new pinocchIO file <i>MyVideo.pio</i> corresponding to the video <i>MyVideo.avi</i>
\verbatim
 $ pioinit /path/to/MyVideo.avi /another/path/to/MyVideo.pio
\endverbatim
 
 \section onemultiple One pinocchIO file <--> Multiple descriptions
 
 Traditionnally, one would store one kind of descriptors into one file and another
 one into another file -- thus leading to a complex and cluttered collection of files.
 
 For instance, one would store 
 <a href="http://en.wikipedia.org/wiki/Mel-frequency_cepstrum">MFCC</a> coefficients
 extracted from the audio stream of a video into one file (e.g. <i>audio.mfcc</i>),
 <a href="http://en.wikipedia.org/wiki/Scale-invariant_feature_transform">SIFT</a>
 descriptors computed from every frame of the video into another file (e.g <i>MyVideo.sift</i>),
 and two versions of speaker diarization results into two other files 
 (e.g. <i>MyVideo.spk.v1</i> and <i>MyVideo.spk.v2alpha</i>).
 
 \subsection filesystem Internal file system
 
 pinocchIO files reproduce the file system paradigm internally.
 Therefore, it is possible to store all of these descriptors into one unique file
 (e.g. <i>MyVideo.pio</i>). 
 
 To continue with the previous example, one could choose to store MFCC coefficients
 at internal path <i>/audio/mfcc</i>, SIFT descriptors at <i>/video/sift</i> and speaker diarization
 results into <i>/audio/SpeakerDiarization/v1</i>
 and <i>/audio/SpeakerDiarization/v2a</i>.
  
 \subsection description Mandatory description 
 
 In the above example, two speaker diarization approaches are available -- 
 called <i>v1</i> (version 1), and <i>v2alpha</i> (version 2, alpha).
 It might as well be <i>potato</i> and <i>asparagus</i>! In a month or two, will you
 remember what was the difference between them?

 pinocchIO allows to store a textual description with every single descriptor
 store in a pinocchIO files. For instance, <i>v1</i> description could be\n 
 <center><i>Speaker diarization using  <a href="http://en.wikipedia.org/wiki/Mixture_model">GMMs</a> with 512 gaussians</i></center>
 while <i>v2alpha</i> would be described by\n
 <center><i>Speaker diarization using 4-states <a href="http://en.wikipedia.org/wiki/Hidden_Markov_model">HMMs</a></i></center>
 
 pinocchIO even goes one step further by <b>preventing</b> any descriptor to be <b>created without any
 textual description</b>, and <b>systematically asks for one</b> when it is created.
 
 Overall, the final pinocchIO file <i>MyVideo.pio</i> might look like that:
 
\verbatim
==============================
==== /path/to/MyVideo.avi ====
==============================
/audio/
  mfcc ....................... MFCC coefficients
  SpeakerDiarization/
    v1 ....................... Speaker diarization using
                               GMM with 512 gaussians
    v2alpha .................. Speaker diarization using
                               4-states HMMs
/video/
  sift ....................... SIFT descriptors
\endverbatim

 \section timelinevsdataset Timelines vs. datasets
    
 Depending on the nature of the description, two types of path can be created
 within a pinocchIO files.
 
 <b>Timelines</b> can be used to store raw temporal segmentation of the medium, such 
 as shot boundary detection or story segmentation for instance.
 
 <b>Datasets</b>, on the other hand, are meant to store actual data extracted from the medium.
 This includes MFCC coefficients extracted every few milliseconds, speaker diarization
 results (who is speaking and when), timestamped subtitles, etc.
  
 \par Definition:
 A timeline is a list of chronologically ordered time ranges
 
 \image	html timeline.png "Three valid timelines"
 \image	latex timeline.eps "Three valid timelines" width=\textwidth

 The first timeline is a partition of the medium duration. Think of it as the output of 
 a shot boundary detection algorithm, for instance.
 
 The second timeline shows that it is acceptable for a timeline to contain overlapping
 time ranges and <i>holes</i>. This timeline could be used as the (unlabelled) output of a
 speaker diarization system where each segment corresponds to a speaker: two speakers
 can obviously speak simultaneously and there are times when nobody is speaking. 
 
 The third timeline shows that time ranges do not have to be included in the duration
 of the medium. For instance, MFCC coefficients are often computed on a sliding window
 centered on the current temporal position -- consequently, the very first position of
 the window might start before the beginning of the medium.
  
 See \ref ascii2pio and \ref timeline for more information on how to create a timeline.
 
 \par Definition:
 A dataset is a collection of sets of D-dimensional vectors aligned with a timeline.</b></center>
 
 \image	html dataset.png "A dataset aligned with the first timeline"
 \image	latex dataset.eps "A dataset aligned with the first timeline" width=\textwidth
 
 Dimension D and type of values (char, integer, float or double) are set once and 
 for all when the dataset is created.
 However, the number of vectors can vary from one time range to another.

 For instance, SIFT descriptors are usually stored as 128-dimensional float vectors.
 However, their number can vary from one frame to another.
 
 Dataset can also be used to store textual descriptors as string.
 Simply create a dataset of 1-dimensional char vectors and store N vectors where
 N is the length of the string.
 
 - See \ref datatype "datatype API" for more information on datatype.
 - See \ref ascii2pio and \ref dataset "dataset API" for more information on how to create a dataset.
 - See \ref write "writing API" for more information on how to write data into a dataset.
 - See \ref piodump and \ref read "reading API" for more information on how to read data from a dataset.

 \section attributes Attributes
 
 pinocchIO would not be complete without a proper way of attaching named metadata to 
 a timeline, a dataset or the whole pinocchIO file: that is what attributes are for.
 
 It is possible to attach metadata to any pioncchIO object (timeline, dataset, or file) 
 as char, int, float, double or even string.
 
 See \ref attribute "attributes API" for more information on how to read and write attributes.
 
 */

/**
 \defgroup file File API
 \ingroup api
 
 @brief This section describes the pinocchIO file structure and the functions
 performing basic operations on pinocchIO files.
 
 @{
 */


#ifndef _PINOCCHIO_FILE_H
#define _PINOCCHIO_FILE_H

#include "pIOTypes.h"

/**
 @brief Check file invalidity
 
 Check whether the pinocchIO file handle is invalid.
 
 @param pioFile pinocchIO file handle
 @returns
    - TRUE if the pinocchIO file handle is invalid
    - FALSE otherwise
 */
#define PIOFileIsInvalid PIOObjectIsInvalid

/**
 @brief Check file validity
 
 Check whether the pinocchIO file handle is valid.
 
 @param pioFile pinocchIO file handle
 @returns
    - TRUE if the pinocchIO file handle is valid
    - FALSE otherwise
 */
#define PIOFileIsValid   PIOObjectIsValid

/**
 @brief Name of the HDF5 group meant to contain timelines
 
 pinocchIO files actually are based on the HDF5 format.\n
 pinocchIO timelines are actually HDF5 datasets gathered in a predefined HDF5 group.\n
 The name of this group is defined here. 
 */
#define PIOFile_Structure_Group_Timelines "timeline"

/**
 @brief Name of the HDF5 group meant to contain datasets

 pinocchIO files actually are based on the HDF5 format.\n
 pinocchIO datasets are actually HDF5 groups gathered in a predefined HDF5 group.\n
 The name of this group is defined here.  
 */
#define PIOFile_Structure_Group_Datasets  "dataset"

/**
 @brief Name of the HDF5 dataset meant to contain the actual data
 
 pinocchIO dataset actually are HDF5 groups containing two HDF5 datasets.\n
 One of them is meant to store the actual data.\n
 The name of this HDF5 dataset is defined here.
 */
#define PIOFile_Structure_Datasets_Data  "data"

/**
 @brief Name of the HDF5 dataset meant to contain the number of vectors per time range.
 
 pinocchIO dataset actually are HDF5 groups containing two HDF5 datasets.\n
 One of them is meant to store the number of vectors per time range.\n
 The name of this HDF5 dataset is defined here.
 */
#define PIOFile_Structure_Datasets_Link "link"

/**
	@brief Create new pinocchIO file

    Create a new pinocchIO file at location \a path 
    meant to describe the medium stored at location \a medium.  

    - The directory meant to store the pinocchIO file must exist.
    - The user must be granted writing rights inside this directory.
    - There must not be any existing file at location \a path.
 
	@param[in] path Path to the new pinocchIO file
	@param[in] medium Path to the medium described by this new file
	@returns 
        - a writable pinocchIO file handle when successful
        - \ref PIOFileInvalid otherwise
 */
PIOFile pioNewFile  ( const char* path, const char* medium );

/**
	@brief Open pinocchIO file
 
    Open the pinocchIO file at location \a path 
    with required access \a rights.
 
    - A pinocchIO file must exist at location \a path.
    - The user must be granted the requested rights on this file.
 
	@param[in] path Path to the existing pinocchIO file
	@param[in] rights Requested rights on this file (see \ref PIOFileRights)
	@returns
        - a readable pinocchIO file handle when successful and \a rights is \ref PINOCCHIO_READONLY
        - a readable and writable pinocchIO file handle when successful \a rights is \ref PINOCCHIO_READNWRITE
        - \ref PIOFileInvalid otherwise
 */
PIOFile pioOpenFile( const char* path, PIOFileRights rights );

/**
	@brief Close pinocchIO file
 
    Close a previously opened pinocchIO file.
    Upon success, the pinocchIO file handle will be set to \ref PIOFileInvalid.
 
	@param[in,out] file pinocchIO file
	@returns
        - 1 when file is successfully closed
        - 0 otherwise
 */
int pioCloseFile( PIOFile* file );

/**
	@}
 */

#endif
