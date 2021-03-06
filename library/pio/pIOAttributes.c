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

#include "pIOAttributes.h"

#include <hdf5_hl.h>
#include <string.h>
#include <stdlib.h>

/**
 @brief List of protected attributes
 
 Some attributes are used by pinocchIO internally and are therefore protected:
 pinocchIO users cannot use their names.\n
 The list of protected attributes is defined here.

 @ingroup attribute
 */
char* PIOAttribute_ListProtected[PIOAttribute_NumberProtected] = {
	PIOAttribute_Version,
	PIOAttribute_File_Medium,
	PIOAttribute_Description,
	PIOAttribute_TimesUsed,
	PIOAttribute_Timeline,
};

int pioAttributeIsProtected( const char* attr_name)
{
	int i;
	for (i=0; i<PIOAttribute_NumberProtected; i++)
		if (strcmp(attr_name, PIOAttribute_ListProtected[i])==0)
			return 1;
	return 0;
}

int pioHasAttribute(PIOObject pioObject, const char* attr_name )
{
	return H5Aexists(pioObject.identifier, attr_name);	
}

int pioAddAttributeString(PIOObject pioObject,
						  const char* attr_name, const char* attr_value)
{	
	if (pioAttributeIsProtected(attr_name)) 
	{
		fprintf(stdout, 
				"WARNING: Attribute name %s is a reserved for pinocchIO internal use.\n",
				attr_name);		
		return -1;
	}
	return H5LTset_attribute_string(pioObject.identifier, ".", 
									attr_name, attr_value);
}

int pioAddAttributeInteger(PIOObject pioObject,
						   const char* attr_name, int attr_value)
{
	if (pioAttributeIsProtected(attr_name)) 
	{
		fprintf(stdout, 
				"WARNING: Attribute name %s is a reserved for pinocchIO internal use.\n",
				attr_name);		
		return -1;
	}
	return H5LTset_attribute_int(pioObject.identifier, ".", 
								 attr_name, &attr_value, 1);
}

int pioAddAttributeFloat(PIOObject pioObject,
						 const char* attr_name, float attr_value)
{
	if (pioAttributeIsProtected(attr_name)) 
	{
		fprintf(stdout, 
				"WARNING: Attribute name %s is a reserved for pinocchIO internal use.\n",
				attr_name);		
		return -1;
	}
	return H5LTset_attribute_float(pioObject.identifier, ".",
								   attr_name, &attr_value, 1);
}

int pioAddAttributeDouble(PIOObject pioObject,
						  const char* attr_name, double attr_value)
{
	if (pioAttributeIsProtected(attr_name)) 
	{
		fprintf(stdout, 
				"WARNING: Attribute name %s is a reserved for pinocchIO internal use.\n",
				attr_name);		
		return -1;
	}
	return H5LTset_attribute_double(pioObject.identifier, ".",
									attr_name, &attr_value, 1);
}

int pioReadAttributeString(PIOObject pioObject,
						   const char* attr_name, char** attr_value)
{
	hid_t attr;
	size_t storage;
	
	if (pioHasAttribute(pioObject, attr_name))
	{
		attr = H5Aopen_name(pioObject.identifier, attr_name);
		storage = H5Aget_storage_size(attr);
		H5Aclose(attr);
		
		*attr_value = (char*)malloc( storage + sizeof(char));
		H5LTget_attribute_string(pioObject.identifier, ".",
								 attr_name, *attr_value);
		return 1;
	}
	else {
		return -1;
	}
}

int pioReadAttributeInteger(PIOObject pioObject,
							const char* attr_name, int* attr_value)
{
	if (pioHasAttribute(pioObject, attr_name))
	{
		H5LTget_attribute_int(pioObject.identifier, ".",
							  attr_name, attr_value);	
		return 1;
	} 
	else {
		return -1;
	}
}

int pioReadAttributeFloat  (PIOObject pioObject,
							const char* attr_name, float* attr_value)
{
	if (pioHasAttribute(pioObject, attr_name))
	{
		H5LTget_attribute_float(pioObject.identifier, ".",
								attr_name, attr_value);	
		return 1;
	} 
	else {
		return -1;
	}
}

int pioReadAttributeDouble (PIOObject pioObject,
							const char* attr_name, double* attr_value)
{
	if (pioHasAttribute(pioObject, attr_name))
	{
		H5LTget_attribute_double(pioObject.identifier, ".",
								 attr_name, attr_value);	
		return 1;
	} 
	else {
		return -1;
	}
}
