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
	<#Description#>
	@param[in] server <#server description#>
	@param[in] labelValue <#labelValue description#>
	@param[out] timeranges <#timeranges description#>
	@returns <#return value description#>
 */
int gptGetTimerangesForLabel(GPTServer server,
                             int labelValue,
                             GPTTimeRange* timeranges);
                             


#endif

