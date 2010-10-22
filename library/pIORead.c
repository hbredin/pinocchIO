/*
 *  pIORead.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 22/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

#include "pIORead.h"

#include <stdlib.h>

#include "pIODataset.h"
#include "structure_utils.h"

int getLink(PIODataset pioDataset, int timerangeIndex, link_t* link)
{
	ERROR_SWITCH_INIT
	herr_t read_err;

	hsize_t position[1] = {-1};
	hsize_t number[1] = {-1};
	hid_t dataspaceForLink = -1;
	hid_t bufferDataspaceForLink = -1;
	hid_t link_datatype = -1;
	
	if (!(timerangeIndex<pioDataset.ntimeranges)) return -1;
	
	// read link dataset
	position[0] = timerangeIndex;
	number[0] = 1;
	dataspaceForLink = H5Dget_space(pioDataset.link_identifier);
	H5Sselect_hyperslab(dataspaceForLink, H5S_SELECT_SET, position, NULL, number, NULL);
	bufferDataspaceForLink = H5Screate_simple(1, number, NULL);
	link_datatype = linkDatatype();
	ERROR_SWITCH_OFF
	read_err = H5Dread(pioDataset.link_identifier, link_datatype, bufferDataspaceForLink, dataspaceForLink, H5P_DEFAULT, link);
	ERROR_SWITCH_ON
	H5Tclose(link_datatype);
	H5Sclose(bufferDataspaceForLink);
	H5Sclose(dataspaceForLink);
	
	if (read_err < 0) return -1;
	return 1;
}

int pioReallocBuffer(PIODataset pioDataset, int timerangeIndex,
					 void** buffer, PIODatatype datatype)
{
	link_t link = {-1, -1};
	
	if (getLink(pioDataset, timerangeIndex, &link)<0) return -1;
	(*buffer) = realloc(*buffer, link.number*H5Tget_size(datatype.identifier));
	if ((*buffer) == NULL) return -1;
	
	return 1;
}

int pioRead(PIODataset pioDataset, int timerangeIndex, 
			void* buffer, PIODatatype dataType)
{
	ERROR_SWITCH_INIT
	herr_t read_err;
	
	link_t link = {-1, -1};
	
	hsize_t position[1] = {-1};
	hsize_t number[1] = {-1};
	
	hid_t dataspaceForData = -1;
	hid_t bufferDataspaceForData = -1;
	
	if (getLink(pioDataset, timerangeIndex, &link)<0) return -1;
	
	// read dataset
	position[0] = (hsize_t)(link.position);
	number[0] = (hsize_t)(link.number);
	dataspaceForData = H5Dget_space(pioDataset.identifier);
	H5Sselect_hyperslab(dataspaceForData, H5S_SELECT_SET, position, NULL, number, NULL);
	bufferDataspaceForData = H5Screate_simple(1, number, NULL);
	ERROR_SWITCH_OFF
	read_err = H5Dread(pioDataset.identifier, dataType.identifier, bufferDataspaceForData, dataspaceForData, H5P_DEFAULT, buffer);
	ERROR_SWITCH_ON
	H5Sclose(bufferDataspaceForData);
	H5Sclose(dataspaceForData);
	if (read_err < 0) return -1;
	
	return link.number;
}

