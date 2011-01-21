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

#ifndef _GEPETTO_LABEL_H
#define _GEPETTO_LABEL_H

#include "gptTypes.h"

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
 
 */
int gptGetTimerangesForLabel(GPTServer server,
                             int labelValue,
                             GPTTimeRange* timeranges);
                             


#endif

