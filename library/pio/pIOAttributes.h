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
 \defgroup attribute Attribute API 
 \ingroup api
 @{
 */


#ifndef _PINOCCHIO_ATTRIBUTES_H
#define _PINOCCHIO_ATTRIBUTES_H

#include "pIOTypes.h"

#define PIOAttribute_Version      "version"
#define PIOAttribute_File_Medium  "medium"
#define PIOAttribute_Description  "description"
#define PIOAttribute_TimesUsed    "times_used"
#define PIOAttribute_Timeline     "timeline"

#define PIOAttribute_NumberProtected 5

/**
	Check if attribute name is protected 
	@param attr_name Attribute name
	@returns 
        - 1 if attribute name is reserved for pinocchIO internal use
        - 0 otherwise
 */
int pioAttributeIsProtected( const char* attr_name);


/// Checks attribute existence. 
///
/// \param[in] dataset	Any of file, group or dataset handle
/// \param[in] attr_name	Name of checked attribute
///
/// \returns	int
///		- positive value if attribute exists
///		- negative value otherwise
int pioHasAttribute(PIOObject object,
					const char* attr_name );
#define pioFileHasAttribute     pioHasAttribute
#define pioDatasetHasAttribute  pioHasAttribute
#define pioTimelineHasAttribute pioHasAttribute


/// Adds/modifies string attribute to dataset
///
/// \param[in] dataset	pinocchIO dataset handle
/// \param[in] attr_name	name of attribute
/// \param[in] attr_value	value of attribute
///
/// \returns	int
///		- positive value when successful
///		- negative value otherwise.
int pioAddAttributeString (PIOObject pioObject, 
						   const char* attr_name, const char* attr_value);

/// Adds/modifies integer attribute to dataset
///
/// \param[in] dataset	pinocchIO dataset handle
/// \param[in] attr_name	name of attribute
/// \param[in] attr_value	value of attribute
///
/// \returns	int
///		- positive value when successful
///		- negative value otherwise.
int pioAddAttributeInteger(PIOObject pioObject,
						   const char* attr_name, int attr_value);

/// Adds/modifies float attribute to dataset
///
/// \param[in] dataset	pinocchIO dataset handle
/// \param[in] attr_name	name of attribute
/// \param[in] attr_value	value of attribute
///
/// \returns	int
///		- positive value when successful
///		- negative value otherwise.
int pioAddAttributeFloat  (PIOObject pioObject,
						   const char* attr_name, float attr_value);

/// Adds/modifies double attribute to dataset
///
/// \param[in] dataset	pinocchIO dataset handle
/// \param[in] attr_name	name of attribute
/// \param[in] attr_value	value of attribute
///
/// \returns	int
///		- positive value when successful
///		- negative value otherwise.
int pioAddAttributeDouble(PIOObject pioObject,
						  const char* attr_name, double attr_value);

/// Reads string attribute
///
/// \param[in] dataset	Dataset handle
/// \param[in] attr_name	Name of attribute
/// \param[out] attr_value	Value of attribute
///
/// \returns int
///		- positive value if successful
///		- negative value otherwise
///
/// \note attr_value is allocated from within pioReadAttributeString: do not forget to free it when done with it to avoid memory leak.
int pioReadAttributeString (PIOObject pioObject,
							const char* attr_name, char** attr_value);

/// Reads integer attribute
///
/// \param[in] dataset	Dataset handle
/// \param[in] attr_name	Name of attribute
/// \param[out] attr_value	Value of attribute
///
/// \returns int
///		- positive value if successful
///		- negative value otherwise
int pioReadAttributeInteger(PIOObject pioObject,
							const char* attr_name, int* attr_value);

/// Reads float attribute
///
/// \param[in] dataset	Dataset handle
/// \param[in] attr_name	Name of attribute
/// \param[out] attr_value	Value of attribute
///
/// \returns int
///		- positive value if successful
///		- negative value otherwise
int pioReadAttributeFloat  (PIOObject pioObject,
							const char* attr_name, float* attr_value);

/// Reads double attribute
///
/// \param[in] dataset	Dataset handle
/// \param[in] attr_name	Name of attribute
/// \param[out] attr_value	Value of attribute
///
/// \returns int
///		- positive value if successful
///		- negative value otherwise
int pioReadAttributeDouble (PIOObject pioObject,
							const char* attr_name, double* attr_value);

#endif

/**
	@}
 */


