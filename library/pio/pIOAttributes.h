/*
 *  pIOAttributes.h
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 15/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
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

/// Checks attribute protection
///
/// \param[in] attr_name	Name of attribute
///
/// \returns int
///		- 1 if attribute name is reserved for pinocchIO internal use
///		- 0 if attribute name can be used
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

