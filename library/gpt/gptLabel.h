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
 \defgroup gptlabel Label API
 \ingroup gptapi
 
 @brief Functions dealing with access to labels served by Gepetto servers.
 
 */

#ifndef _GEPETTO_LABEL_H
#define _GEPETTO_LABEL_H

#include "gptTypes.h"


/**
 @brief Get list of of distinct labels
 
 @param[in] server Gepetto server
 @param[out] list Sorted list of distinct label values
 @returns 
    - 0 if server does not serve labels
    - number of distinct labels otherwise
 
 @note
 @a list has to be allocated with enough memory space to store all label values.
 A first call to gptGetListOfDistinctLabels() with NULL list will help you:
\verbatim
 numberOfDistinctLabels = gptGetListOfDistinctLabels(server, NULL);
 listOfDistinctLabels = (int*) malloc(numberOfDistinctLabels*sizeof(int));
 gptGetListOfDistinctLabels(server, listOfDistinctLabels);
\endverbatim
 
 @ingroup gptlabel
 
 */
int gptGetListOfDistinctLabels(GPTServer server, int* list);

/**
 @brief Get list of timeranges with a specified label
 
 @param[in] server Gepetto server
 @param[in] labelValue Label value
 @param[out] timeranges Array of timeranges with requested label
 @returns total number of timeranges with matching label when successful
    
 @note
 @a timeranges has to be allocated first. A first call to gptGetTimerangesForLabel()
 with @a timeranges = NULL can be used to get the expected number of timeranges.
\verbatim
 numberOfTimeranges = gptGetTimerangesForLabel(server, labelValue, NULL);
 timeranges = (GPTTimeRange*) malloc(numberOfTimeranges*sizeof(GPTTimeRange));
 numberOfTimeranges = gptGetTimerangesForLabel(server, labelValue, timeranges);
\endverbatim
 
 @ingroup gptlabel
 
 */
int gptGetTimerangesForLabel(GPTServer server,
                             int labelValue,
                             GPTTimeRange* timeranges);
                             
#endif

