/*
 *  pIODatatype.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 20/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

#include "pIODatatype.h"

PIODatatype pioNewDatatype( const PIOBaseType type, int dimension )
{
	hsize_t dim[1]; 
	PIODatatype datatype;

	ERROR_SWITCH_INIT
	
	dim[0] = (hsize_t) dimension;
	
	datatype.dimension = dimension;
	datatype.type = type;
	
	ERROR_SWITCH_OFF
	switch (type) {
		case PINOCCHIO_TYPE_INT:
			datatype.identifier = H5Tarray_create2(H5T_NATIVE_INT, 
												   1, dim); 
			break;
		case PINOCCHIO_TYPE_FLOAT:
			datatype.identifier = H5Tarray_create2(H5T_NATIVE_FLOAT, 
												   1, dim); 
			break;
		case PINOCCHIO_TYPE_DOUBLE:
			datatype.identifier = H5Tarray_create2(H5T_NATIVE_DOUBLE, 
												   1, dim); 
			break;
		case PINOCCHIO_TYPE_CHAR:
			datatype.identifier = H5Tarray_create2(H5T_NATIVE_CHAR, 
												   1, dim);
			break;
		default:
			datatype.identifier = -1;
			break;
	}
	ERROR_SWITCH_ON
	
	if (PIODatatypeIsInvalid(datatype)) return PIODatatypeInvalid;
	return datatype;
}

int pioCloseDatatype( PIODatatype pioDatatype )
{
	pioDatatype.dimension = -1;
	pioDatatype.type = -1;
	
	if (H5Tclose(pioDatatype.identifier) < 0) return 0;
	pioDatatype.identifier = -1;
	
	return 1;
}

PIODatatype pioGetDatatype(PIODataset pioDataset)
{
	ERROR_SWITCH_INIT
	PIODatatype pioDatatype = PIODatatypeInvalid;

	hid_t datatype = -1;
	H5T_class_t class = -1;
	hid_t basetype = H5T_NO_CLASS;
	hid_t basetype_native = H5T_NO_CLASS;
	int ndims;
	hsize_t dims[1];
	
	int dimension;
	PIOBaseType type = -1;
			
	ERROR_SWITCH_OFF
	datatype = H5Dget_type(pioDataset.identifier);
	ERROR_SWITCH_ON
	if (datatype<0) return PIODatatypeInvalid;
	
	ERROR_SWITCH_OFF
	class = H5Tget_class(datatype);
	ERROR_SWITCH_ON
	
	if (class != H5T_ARRAY)
	{
		H5Tclose(datatype);
		return PIODatatypeInvalid;
	}
	
	ndims = H5Tget_array_ndims( datatype );
	if (ndims != 1 )
	{
		H5Tclose(datatype);
		return PIODatatypeInvalid;
	}
	
	ERROR_SWITCH_OFF
	H5Tget_array_dims2(datatype, dims);
	ERROR_SWITCH_ON
	dimension = dims[0];

	ERROR_SWITCH_OFF
	basetype = H5Tget_super(datatype);
	ERROR_SWITCH_ON
	if (basetype == H5T_NO_CLASS) 
	{
		H5Tclose(basetype);
		H5Tclose(datatype);
		return PIODatatypeInvalid;
	}
	
	ERROR_SWITCH_OFF
	basetype_native = H5Tget_native_type(basetype, H5T_DIR_ASCEND);
	H5Tclose(basetype);
	ERROR_SWITCH_ON
	if (basetype_native < 0)
	{
		H5Tclose(datatype);
		return PIODatatypeInvalid;
	}
	
	if (H5Tequal(H5T_NATIVE_INT, basetype_native))
	{
		type = PINOCCHIO_TYPE_INT;
	}
	if (H5Tequal(H5T_NATIVE_FLOAT, basetype_native))
	{
		type = PINOCCHIO_TYPE_FLOAT;
	}
	if (H5Tequal(H5T_NATIVE_DOUBLE, basetype_native))
	{
		type = PINOCCHIO_TYPE_DOUBLE;
	}
	if (H5Tequal(H5T_NATIVE_CHAR, basetype_native))
	{
		type = PINOCCHIO_TYPE_CHAR;
	}
	H5Tclose(basetype_native);
	H5Tclose(datatype);
	
	if (type < 0) return PIODatatypeInvalid;
	
	pioDatatype = pioNewDatatype(type, dimension);
	return pioDatatype;
}
