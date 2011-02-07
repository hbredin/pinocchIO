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
 \defgroup gptconfig Configuration API
 \ingroup gptapi
 
 @brief Functions dealing with Gepetto server configuration

 \verbatim
## Gepetto server
## Sample configuration file
data = {
	dataset = "ColorHistogram/B10G10R10"; 
	files = {
		list = "/people/bredin/lists/videos/TRECVid2007_devel.lst";
		prefix = "/vol/raid220/bredin/Features/";
		suffix = ".pio";
	};
};

label = {
	dataset = "TRECVid/Annotation/2007/1001"; // Person
	files = {
		list = "/people/bredin/lists/videos/TRECVid2007_devel.lst";
		prefix = "/vol/raid220/bredin/Annotations/";
		suffix = ".pio";
	};
};

filter = {
	# none = 1;
	equalsTo = 1;
	# differsFrom = -1;
	# greaterThan = 1;
	# smallerThan = 1;
 
    maximumNumberOfSamples = 2000;
};
\endverbatim

 
 */

#ifndef _GEPETTO_CONFIG_H
#define _GEPETTO_CONFIG_H

/**
 @brief Create new Gepetto server from configuration file

 Create a new Gepetto server based on a configuration file.
 
 @param[in] filename Path to configurtion file
 @returns 
    - Gepetto server if successful
    - invalid Gepetto server otherwise
 
 @note
 Use gptCloseServer() to close the server when no longer needed. 
 
 \ingroup gptconfig
 */
GPTServer gptNewServerFromConfigurationFile(const char* filename);


#endif

