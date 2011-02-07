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
 
 @brief Functions dealing with pinocchIO attributes

 @{
 */

#ifndef _PINOCCHIO_ATTRIBUTES_H
#define _PINOCCHIO_ATTRIBUTES_H

#include "pIOTypes.h"

/**
 @brief Name of the HDF5 attributes meant to store pinocchIO version
 
 Each time a file, a dataset or a timeline is created, pinocchIO version used
 to create it is attached, as an HDF5 attribute.
 The name of this HDF5 attribute is defined here.
 
 @note
 This is a @ref PIOAttribute_ListProtected "protected" attribute.
 */
#define PIOAttribute_Version      "version"

/**
 @brief Name of the HDF5 attributes meant to store path to medium
 
 Each time a file is created, pinocchIO stores, as an HDF5 attribute, the path
 to the medium the file describes.
 The name of this HDF5 attribute is defined here.

 @note
 This is a @ref PIOAttribute_ListProtected "protected" attribute.
 */
#define PIOAttribute_File_Medium  "medium"

/**
 @brief Name of the HDF5 attributes meant to store descriptions
 
 Each time a dataset or a timeline is created, a textual description is attached
 to it as an HDF5 attribute.
 The name of this HDF5 attribute is defined here.
 
 @note
 This is a @ref PIOAttribute_ListProtected "protected" attribute.
 */
#define PIOAttribute_Description  "description"


/**
 @brief Name of the HDF5 attributes meant to store timeline usage counter
 
 Each time a timeline is used by a dataset, its usage counter is updated and
 stored as an HDF5 attribute.
 The name of this HDF5 attribute is defined here.
 
 @note
 This is a @ref PIOAttribute_ListProtected "protected" attribute.
 */
#define PIOAttribute_TimesUsed    "times_used"

/**
 @brief Name of the HDF5 attributes meant to store path to timeline
 
 Each time a dataset is created, the path to its timeline is stored as an HDF5
 attribute.
 The name of this HDF5 attribute is defined here.
 
 @note
 This is a @ref PIOAttribute_ListProtected "protected" attribute.
 */
#define PIOAttribute_Timeline     "timeline"

/**
 @brief Number of protected attributes
 
 Some attributes are used by pinocchIO internally and are therefore protected:
 pinocchIO users cannot use their names.\n
 The list of protected attributes is stored in @ref PIOAttribute_ListProtected,
 the length of which is defined here.
 */
#define PIOAttribute_NumberProtected 5

/**
 @brief Check protection of attribute
 
 Check if attribute name @a attr_name is used internally by pinocchIO and
 therefore cannot be used by pioAddAttributeXXX functions.
 
 @param[in] attr_name Attribute name
 @returns
    - TRUE if attribute is @ref PIOAttribute_ListProtected "protected"
    - FALSE otherwise
 */
int pioAttributeIsProtected( const char* attr_name);

/**
	@brief Check attribute existence
 
    Check whether @a object has an attribute with name @a attr_name.
    @a object can be a pinocchIO file, dataset or timeline.\n
    Simply use @ref PIOMakeObject to make it a pinocchIO generic object first.
    
	@param[in] object pinocchIO object
	@param[in] attr_name Attribute name
	@returns 
        - a positive value if successful and attribute exists
        - 0 if successful and attribute does not exist.
        - a negative value if unsuccessful.
 */
int pioHasAttribute(PIOObject object,
					const char* attr_name );

/**
 @brief Check attribute

 Check whether file @a f has an attribute with name @a attr_name.

 @param[in] f pinocchIO file
 @param[in] a Attribute name
 @returns 
    - a positive value if successful and attribute exists
    - 0 if successful and attribute does not exist.
    - a negative value if unsuccessful.
 
 @ingroup file
 */
#define pioFileHasAttribute(f, a) (pioHasAttribute(PIOMakeObject(f), a))

/**
 @brief Check attribute existence
 
 Check whether dataset @a d has an attribute with name @a attr_name.
 
 @param[in] d pinocchIO dataset
 @param[in] a Attribute name
 @returns 
 - a positive value if successful and attribute exists
 - 0 if successful and attribute does not exist.
 - a negative value if unsuccessful.
 
 @ingroup dataset
 */
#define pioDatasetHasAttribute(d, a) (pioHasAttribute(PIOMakeObject(d), a))

/**
 @brief Check attribute existence
 
 Check whether timeline @a t has an attribute with name @a attr_name.
 
 @param[in] t pinocchIO timeline
 @param[in] a Attribute name
 @returns 
 - a positive value if successful and attribute exists
 - 0 if successful and attribute does not exist.
 - a negative value if unsuccessful.
 
 @ingroup timeline
 */
#define pioTimelineHasAttribute(t, a) (pioHasAttribute(PIOMakeObject(t), a))

/**
 @brief Add string attribute

 Add (or replace, if it already exists) an attribute called @a attr_name 
 with string value @a attr_value to pinocchIO object @a pioObject.\n
 Use @ref PIOMakeObject to apply this function to pinocchIO files, datasets 
 or timelines.
 
 @param[in] pioObject pinocchIO object
 @param[in] attr_name Attribute name
 @param[in] attr_value Attribute value
 @returns 
    - non-negative value when successful
    - negative value otherwise
 
 @note
 If @a attr_name is @ref PIOAttribute_ListProtected "protected", 
 pinocchIO prints a warning to standard output.
 */
int pioAddAttributeString (PIOObject pioObject,
						   const char* attr_name, const char* attr_value);

/**
 @brief Add integer attribute
 
 Add (or replace, if it already exists) an attribute called @a attr_name 
 with int value @a attr_value to pinocchIO object @a pioObject.\n
 Use @ref PIOMakeObject to apply this function to pinocchIO files, datasets 
 or timelines.
 
 @param[in] pioObject pinocchIO object
 @param[in] attr_name Attribute name
 @param[in] attr_value Attribute value
 @returns 
 - non-negative value when successful
 - negative value otherwise
 
 @note
 If @a attr_name is @ref PIOAttribute_ListProtected "protected", 
 pinocchIO prints a warning to standard output.
 */
int pioAddAttributeInteger(PIOObject pioObject,
						   const char* attr_name, int attr_value);

/**
 @brief Add float attribute
 
 Add (or replace, if it already exists) an attribute called @a attr_name 
 with float value @a attr_value to pinocchIO object @a pioObject.\n
 Use @ref PIOMakeObject to apply this function to pinocchIO files, datasets 
 or timelines.
 
 @param[in] pioObject pinocchIO object
 @param[in] attr_name Attribute name
 @param[in] attr_value Attribute value
 @returns 
 - non-negative value when successful
 - negative value otherwise
 
 @note
 If @a attr_name is @ref PIOAttribute_ListProtected "protected", 
 pinocchIO prints a warning to standard output.
 */
int pioAddAttributeFloat  (PIOObject pioObject,
						   const char* attr_name, float attr_value);

/**
 @brief Add double attribute
 
 Add (or replace, if it already exists) an attribute called @a attr_name 
 with double value @a attr_value to pinocchIO object @a pioObject.\n
 Use @ref PIOMakeObject to apply this function to pinocchIO files, datasets 
 or timelines.
 
 @param[in] pioObject pinocchIO object
 @param[in] attr_name Attribute name
 @param[in] attr_value Attribute value
 @returns 
 - non-negative value when successful
 - negative value otherwise
 
 @note
 If @a attr_name is @ref PIOAttribute_ListProtected "protected", 
 pinocchIO prints a warning to standard output.
 */
int pioAddAttributeDouble(PIOObject pioObject,
						  const char* attr_name, double attr_value);

/**
 @brief Read string attribute
 
 Read the string attribute called @a attr_name from pinocchIO object @a pioObject and store
 its value into @a attr_value.\n
 Use @ref PIOMakeObject to apply this function to pinocchIO files, datasets 
 or timelines.
 
 @param[in] pioObject pinocchIO object
 @param[in] attr_name Attribute name
 @param[out] attr_value Attribute value
 @returns
    - positive value when successful
    - negative value otherwise
 
 @note
 pioReadAttributeString() allocates @a attr_value internally.
 Do not forget to free it when no longer needed to avoid memory leaks.
 */
int pioReadAttributeString (PIOObject pioObject,
							const char* attr_name, char** attr_value);

/**
 @brief Read integer attribute
 
 Read the integer attribute called @a attr_name from pinocchIO object @a pioObject and store
 its value into @a attr_value.\n
 Use @ref PIOMakeObject to apply this function to pinocchIO files, datasets 
 or timelines.
 
 @param[in] pioObject pinocchIO object
 @param[in] attr_name Attribute name
 @param[out] attr_value Attribute value
 @returns
 - positive value when successful
 - negative value otherwise 
 */
int pioReadAttributeInteger(PIOObject pioObject,
							const char* attr_name, int* attr_value);

/**
 @brief Read float attribute
 
 Read the float attribute called @a attr_name from pinocchIO object @a pioObject and store
 its value into @a attr_value.\n
 Use @ref PIOMakeObject to apply this function to pinocchIO files, datasets 
 or timelines.
 
 @param[in] pioObject pinocchIO object
 @param[in] attr_name Attribute name
 @param[out] attr_value Attribute value
 @returns
 - positive value when successful
 - negative value otherwise 
 */
int pioReadAttributeFloat  (PIOObject pioObject,
							const char* attr_name, float* attr_value);

/**
 @brief Read double attribute
 
 Read the double attribute called @a attr_name from pinocchIO object @a pioObject and store
 its value into @a attr_value.\n
 Use @ref PIOMakeObject to apply this function to pinocchIO files, datasets 
 or timelines.
 
 @param[in] pioObject pinocchIO object
 @param[in] attr_name Attribute name
 @param[out] attr_value Attribute value
 @returns
 - positive value when successful
 - negative value otherwise 
 */
int pioReadAttributeDouble (PIOObject pioObject,
							const char* attr_name, double* attr_value);

#endif

/**
	@}
 */


