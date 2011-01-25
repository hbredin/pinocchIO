// 
// Copyright 2011 Herve BREDIN (bredin@limsi.fr)
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
 \defgroup gptdata Data API
 \ingroup gptapi
 
 @brief Functions dealing with access to data served by Gepetto servers.

 */


#ifndef _GEPETTO_DATA_H
#define _GEPETTO_DATA_H

#include "gptTypes.h"

/**
 @brief Get dimension of server data
 
 Get the dimension of the entries provided by the server.
 
 @param[in] server Gepetto server
 @returns
 - the dimension of data if server serves data
 - -1 otherwise
 
 @ingroup gptdata
 */
int gptGetServerDimension(GPTServer server);


/**
 @brief Read next data and its label from server
 
 Update @a buffer (and @a labels) so that it contains next data entries (and their labels) 
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
 @param[out] labels Data labels. Use NULL if you are not interested in the labels.
 @param[out] nLabels Number of data labels. Use NULL if you are not interested in the labels.
 
 @returns
 - @a number of entries when successful
 - -1 otherwise
 
 @ingroup gptdata
 */
int gptReadNext(GPTServer* gptServer,
                PIODatatype pioDatatype, 
                void** buffer, 
                int* nLabels, 
                int** labels);

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
 
 @ingroup gptdata
 */
#define gptReadNextData(server, datatype, buffer) gptReadNext((server), (datatype), (buffer), NULL, NULL)


#endif

