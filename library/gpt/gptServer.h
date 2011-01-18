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
 \ingroup gepetto
 
 @brief Not documented yet.
 
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
#define GPTServerIsInvalid(o)   ((o).numberOfDataFiles < 1)

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
 @param[in] numberOfLabelFiles Number of label pinocchIO files
 @param[in] pathToLabelFile List of paths to label pinocchIO files
 @param[in] pathToLabelDataset Path to pinocchIO dataset containing server labels
 
 @returns 
    - valid Gepetto server when successful
    - invalid Gepetto server (@ref GPTServerInvalid) otherwise
 */
GPTServer gptNewServer(int numberOfDataFiles, char** pathToDataFile, const char* pathToDataDataset,
                       GPTLabelFilterType labelFilterType, int labelFilterReference,
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

/**
 @brief Get dimension of server data

 Get the dimension of the entries provided by the server.
 
 @param[in] server Gepetto server
 @returns
    - the dimension of data if server serves data
    - -1 otherwise
 */
int gptGetServerDimension(GPTServer server);


/**
 @brief Read next data and its label from server
 
 Update @a buffer (and @a label) so that it contains next data entries (and their label) 
 available from the @a server.
 
 @note
 Gepetto uses an internal buffer to store the requested data. <b>Do not free it!</b>\n
 This buffer is modified (and possibly moved) by each call to gptReadNext().\n
 Typically, one would call gptReadNext() and then copy the buffer content 
 into another variable before calling gptReadNext() again.\n
 See pioRead() documentation for more information on how to use @a buffer.
 
 @param[in,out] server Gepetto server 
 @param[in] datatype Buffer datatype
 @param[out] buffer Data buffer
 @param[in, out] label Data label. Use NULL if you are not interested in the label. 
 
 @returns
 - @a number of entries when successful
 - -1 otherwise
 
 */
int gptReadNext(GPTServer* server,
                PIODatatype datatype,
                void** buffer,
                int* label);

/**
 @brief Read next data from server
 
 Update @a buffer so that it contains next data entries available from the @a server.
 
 @note
 Gepetto uses an internal buffer to store the requested data. <b>Do not free it!</b>\n
 This buffer is modified (and possibly moved) by each call to gptReadNext().\n
 Typically, one would call gptReadNext() and then copy the buffer content 
 into another variable before calling gptReadNext() again.\n
 See pioRead() documentation for more information on how to use @a buffer.
 
 @param[in,out] server Gepetto server 
 @param[in] datatype Buffer datatype
 @param[out] buffer Data buffer
 
 @returns
 - @a number of entries when successful
 - -1 otherwise
 */
#define gptReadNextData(server, datatype, buffer) gptReadNext((server), (datatype), (buffer), NULL)

int gptPrintStatistics(GPTServer server, FILE* file);


// 2 steps usage:
//   - already_allocated_buffer = malloc(pioDumpServer(pioServer, pioDatatype, NULL))
//   - number = pioDumpServer(pioServer, pioDatatype, already_allocated_buffer)
//int gptDumpServer(GPTServer* server, 
//                  PIODatatype pioDatatype, 
//                  void* already_allocated_buffer);

#endif

/**
	@}
 */

