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

int pioRead(PIODataset* pioDataset, int timerangeIndex, 
			PIODatatype pioDatatype, void** buffer)
{
	ERROR_SWITCH_INIT
	herr_t read_err;
	
	link_t link = {-1, -1};
	
	hsize_t position[1] = {-1};
	hsize_t number[1] = {-1};
	
	hid_t dataspaceForData = -1;
	hid_t bufferDataspaceForData = -1;
    size_t new_buffer_size = -1;

	if (getLink(*pioDataset, timerangeIndex, &link)<0) return -1;
    
    // 
    if (link.number == 0) return 0; 
    
    // realloc internal buffer if necessary
    new_buffer_size = link.number*H5Tget_size(pioDatatype.identifier);
    if (new_buffer_size > pioDataset->buffer_size)
    {
        pioDataset->buffer = realloc(pioDataset->buffer, new_buffer_size);
        if (pioDataset->buffer == NULL) 
        {
            pioDataset->buffer_size = 0;
            return -1;
        }
        pioDataset->buffer_size = new_buffer_size;
    }
	
	// read dataset
	position[0] = (hsize_t)(link.position);
	number[0] = (hsize_t)(link.number);
	dataspaceForData = H5Dget_space(pioDataset->identifier);
	H5Sselect_hyperslab(dataspaceForData, H5S_SELECT_SET, position, NULL, number, NULL);
	bufferDataspaceForData = H5Screate_simple(1, number, NULL);
	ERROR_SWITCH_OFF
	read_err = H5Dread(pioDataset->identifier, pioDatatype.identifier, 
                       bufferDataspaceForData, dataspaceForData, H5P_DEFAULT, pioDataset->buffer);
	ERROR_SWITCH_ON
	H5Sclose(bufferDataspaceForData);
	H5Sclose(dataspaceForData);
	if (read_err < 0) return -1;
	
    *buffer = pioDataset->buffer;
    
	return link.number;
}

int pioReadNumber(PIODataset pioDataset, int timerangeIndex)
{
	link_t link = {-1, -1};	
	if (getLink(pioDataset, timerangeIndex, &link)<0) return -1;
    return link.number;    
}

