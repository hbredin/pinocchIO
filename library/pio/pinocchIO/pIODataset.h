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
 \defgroup dataset Dataset API
 \ingroup api
 
 @brief Functions dealing with pinocchIO datasets

 A dataset is a collection of sets of D-dimensional vectors aligned with a timeline.
 
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
 
 @{
 */

#ifndef _PINOCCHIO_DATASET_H
#define _PINOCCHIO_DATASET_H

#include "pIOTypes.h"

/**
 @brief Check dataset invalidity
 
 Check whether the pinocchIO dataset handle is invalid.
 
 @param[in] d pinocchIO dataset handle
 @returns
 - TRUE if the pinocchIO dataset handle is invalid
 - FALSE otherwise
 */
#define PIODatasetIsInvalid(d) (!PIODatasetIsValid(d))

/**
 @brief Check dataset validity
 
 Check whether the pinocchIO dataset handle is valid.
 
 @param[in] d pinocchIO dataset handle
 @returns
 - TRUE if the pinocchIO dataset handle is valid
 - FALSE otherwise
 */
#define PIODatasetIsValid(d)   (((d).identifier > 0) && ((d).link_identifier > 0))

/**
 @brief Create new pinocchIO dataset
 
 Create a new pinocchIO dataset in pinocchIO file \a pioFile at internal location \a path.
 
 - There must not be any existing dataset at internal location \a path.
 
 @param[in] pioFile pinocchIO file handle
 @param[in] path Internal path to the new dataset
 @param[in] description Textual description of the new dataset
 @param[in] pioTimeline Dataset timeline
 @param[in] pioDatatype Dataset datatype
 @returns 
 - a pinocchIO dataset handle when successful
 - \ref PIODatasetInvalid otherwise
 
 @note
 Use pioCloseDataset() to close the timeline when no longer needed. 
 */
PIODataset pioNewDataset(PIOFile pioFile,
						 const char* path, const char* description,
						 PIOTimeline pioTimeline,
						 PIODatatype pioDatatype);

/**
 @brief Open pinocchIO dataset
 
 Open the pinocchIO dataset at internal location \a path in file containing \a pioObject.\n
 \a pioObject can be the file itself, another dataset stored in the same file, or a
 timeline stored in the same file. Just use \ref PIOMakeObject beforehand to convert it to a PIOObject.
 
 @param[in] pioObject PIOObject stored in the same file as requested timeline 
 @param[in] path Path to the existing pinocchIO dataset
 @returns
 - a pinocchIO dataset handle when successful
 - \ref PIODatasetInvalid otherwise
 
 @note
 Use pioCloseDataset() to close the dataset when no longer needed. 
 */
PIODataset pioOpenDataset(PIOObject pioObject, const char* path);

/**
 @brief Close pinocchIO dataset
 
 Close a previously opened pinocchIO dataset.
 Upon success, the pinocchIO dataset handle will be set to \ref PIODatasetInvalid.
 
 @param[in,out] pioDataset pinocchIO dataset handle
 @returns
 - 1 when file is successfully closed
 - 0 otherwise
 */
int pioCloseDataset(PIODataset* pioDataset);

/**
 @}
 */

/**
 @brief Remove pinocchIO dataset
 
 Remove the pinocchIO dataset at internal location \a path from file containing \a pioObject.\n
 \a pioObject can be the file itself, another dataset stored in the same file, or a
 timeline stored in the same file. Just use \ref PIOMakeObject beforehand to convert it to a PIOObject.
 
 @param[in] pioObject PIOObject stored in the same file as requested dataset 
 @param[in] path Path to the pinocchIO dataset to be removed
 @returns
 - 1 if dataset was successfully removed
 - 0 otherwise
 
 @ingroup file
 */    
int pioRemoveDataset(PIOObject pioObject, const char* path);

/**
 @brief Get list of pinocchIO datasets
 
 Get the list of internal paths to all pinocchIO datasets stored in \a pioFile.
 
 @param[in] pioFile pinocchIO file handle
 @param[out] pathsToDatasets Array of paths to datasets
 @returns number of datasets
 
 \note
 \a pathsToDatasets is allocated by \ref pioGetListOfDatasets. It must be freed
 when no longer useful to avoid memory leaks:
 \verbatim
 char** pathsToDatasets = NULL;
 int numberOfDatasets;
 
 // obtain list of path to datasets
 numberOfDatasets = pioGetListOfDatasets(pioFile, &pathsToDatasets);
 // pathsToDatasets[i] contains paths to ith dataset
 // ... do what needs to be done ...
 
 // free memory
 for( int i=0; i<numberOfDatasets; i++) free(pathsToDatasets[i]);
 free(pathsToDatasets); 
 \endverbatim
 
 @ingroup file
 */
int pioGetListOfDatasets(PIOFile pioFile, char*** pathsToDatasets);

/**
 @brief Copy a dataset from one file to another one
 
 Copy dataset at location @a path from @a input file into @a output file.
 If it is missing in the output file, the input dataset timeline is also copied.
 
 @param[in] path Path to dataset
 @param[in] input Input file
 @param[in] output Output file
 @returns 
 - TRUE when successful
 - FALSE otherwise
 
 @note
 In case a dataset already exists at the same @a path in the @a output file,
 pioCopyDataset() will return FALSE, even if input and output datasets are
 identical.
 
 @ingroup file
 */
int pioCopyDataset(const char* path, PIOFile input, PIOFile output);


#endif


