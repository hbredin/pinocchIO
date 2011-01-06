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
 \defgroup config Server configuration
 \ingroup gepetto
 
 @{
 */

#ifndef _GEPETTO_CONFIG_H
#define _GEPETTO_CONFIG_H

/**
	@brief Creates new gepetto server from configuration file
	@param[in] filename path to configuration file
	@returns
        - gepetto server if successful
        - invalid gepetto server otherwise 
 */
GPTServer gptNewServerFromConfigurationFile(const char* filename);

#endif

/**
	@}
 */

