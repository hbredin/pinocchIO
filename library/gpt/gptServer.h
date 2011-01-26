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
 \defgroup server Server API
 \ingroup gptapi
 
 @brief Functions dealing with Gepetto servers.
 
 @{
 */


#ifndef _GEPETTO_SERVER_H
#define _GEPETTO_SERVER_H

#include <libconfig.h>
#include "gptTypes.h"

/**
 @brief Check server invalidity
 
 Check whether the Gepetto server is invalid.
 
 @param[in] o Gepetto server handle
 @returns
 - TRUE if the Gepetto server is invalid
 - FALSE otherwise
 */
#define GPTServerIsInvalid(o)   (!LBL_AVAILABLE(o) && !DAT_AVAILABLE(o))

/**
 @brief Check server validity
 
 Check whether the Gepetto server is valid.
 
 @param[in] o Gepetto server handle
 @returns
 - TRUE if the Gepetto server is valid
 - FALSE otherwise
 */
#define GPTServerIsValid(o)     (!GPTServerIsInvalid(o))

/**
 @brief Create new Gepetto server
 
 @param[in] numberOfDataFiles Number of data pinocchIO files
 @param[in] pathToDataFile List of paths to data pinocchIO files
 @param[in] pathToDataDataset Path to pinocchIO dataset containing server data
 @param[in] labelFilterType Type of label filter
 @param[in] labelFilterReference Reference of label filter
 @param[in] maximumNumberOfSamplesPerLabel Maximum number of samples per label served by the server
 @param[in] numberOfLabelFiles Number of label pinocchIO files
 @param[in] pathToLabelFile List of paths to label pinocchIO files
 @param[in] pathToLabelDataset Path to pinocchIO dataset containing server labels
 
 @returns 
    - valid Gepetto server when successful
    - invalid Gepetto server (@ref GPTServerInvalid) otherwise
 */
GPTServer gptNewServer(int numberOfDataFiles, char** pathToDataFile, const char* pathToDataDataset,
                       GPTLabelFilterType labelFilterType, int labelFilterReference, int maximumNumberOfSamplesPerLabel,
                       int numberOfLabelFiles, char** pathToLabelFile, const char* pathToLabelDataset);

/**
 @brief Close Gepetto server
 
 Close a previously opened Gepetto server.
 Upon success, the Gepetto server handle will be set to @ref GPTServerInvalid.
 
 @param[in,out] server Gepetto server
 @returns
 - 1 when server is successfully closed
 - 0 otherwise
 */
int gptCloseServer(GPTServer* server);


#endif

/**
	@}
 */

