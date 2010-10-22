/*
 *  pIOWrite.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 22/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

#include "pIOWrite.h"

#include "pIODataset.h"
#include "structure_utils.h"

int pioWrite(PIODataset* pioDataset, int timerangeIndex, 
			 void* dataBuffer, int dataNumber, PIODatatype dataType)
{
	ERROR_SWITCH_INIT
	herr_t extend_err;
	herr_t write_err;
	
	hsize_t newExtent[1] = {-1};
	
	hsize_t position[1] = {-1};
	hsize_t number[1] = {-1};
	
	hid_t dataspaceForData = -1;
	hid_t bufferDataspaceForData = -1;

	link_t link = {-1, -1};
	hid_t dataspaceForLink = -1;
	hid_t bufferDataspaceForLink = -1;
	hid_t link_datatype = -1;
	
	if (!(timerangeIndex<pioDataset->ntimeranges)) return -1;
	
	// extend dataset
	newExtent[0] = (hsize_t)(pioDataset->stored + dataNumber);
	ERROR_SWITCH_OFF
	extend_err = H5Dextend(pioDataset->identifier, newExtent);
	ERROR_SWITCH_ON
	if (extend_err < 0) return -1;	
	
	// append data to dataset
	position[0] = (hsize_t)pioDataset->stored;
	number[0] = (hsize_t)dataNumber;
	dataspaceForData = H5Dget_space(pioDataset->identifier);
	H5Sselect_hyperslab(dataspaceForData, H5S_SELECT_SET, position, NULL, number, NULL);
	bufferDataspaceForData = H5Screate_simple(1, number, NULL);
	ERROR_SWITCH_OFF
	write_err = H5Dwrite(pioDataset->identifier, dataType.identifier, bufferDataspaceForData, dataspaceForData, H5P_DEFAULT, dataBuffer);
	ERROR_SWITCH_ON
	H5Sclose(bufferDataspaceForData);
	H5Sclose(dataspaceForData);
	if (write_err < 0) return -1;
	
	// update link dataset
	link.number = dataNumber;
	link.position = pioDataset->stored;
	position[0] = timerangeIndex;
	number[0] = 1;
	dataspaceForLink = H5Dget_space(pioDataset->link_identifier);
	H5Sselect_hyperslab(dataspaceForLink, H5S_SELECT_SET, position, NULL, number, NULL);
	bufferDataspaceForLink = H5Screate_simple(1, number, NULL);
	link_datatype = linkDatatype();
	ERROR_SWITCH_OFF
	write_err = H5Dwrite(pioDataset->link_identifier, link_datatype, bufferDataspaceForLink, dataspaceForLink, H5P_DEFAULT, &link);
	ERROR_SWITCH_ON
	H5Tclose(link_datatype);
	H5Sclose(bufferDataspaceForLink);
	H5Sclose(dataspaceForLink);
	if (write_err < 0) return -1;
	
	// update dataset
	pioDataset->stored = pioDataset->stored + dataNumber;
	
	return dataNumber;
}



