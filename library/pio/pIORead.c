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

#include "pIORead.h"

#include <stdlib.h>
#include <string.h>

#include "pIODataset.h"
#include "pIODatatype.h"
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

int pioReadData(PIODataset* pioDataset, int timerangeIndex, 
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
    new_buffer_size = link.number*pioGetSize(pioDatatype);
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

int pioDumpDataset(PIODataset* pioDataset, 
                   PIODatatype pioDatatype, 
                   void* already_allocated_buffer)
{
    int tr = 0;
    
    int tmp_number = -1;
    int number = 0;

    size_t tmp_size = -1;
    size_t size = 0;
    void* tmp_buffer = NULL;
    
    if (already_allocated_buffer)
    {
        for (tr=0; tr<pioDataset->ntimeranges; tr++)
        {
            // read data for timerange tr
            tmp_number = pioReadData(pioDataset, tr, pioDatatype, &tmp_buffer);
            if (tmp_number < 0) return -1; // stop if something bad happened
            
            // deduce buffer size from number of read data
            tmp_size = tmp_number*pioGetSize(pioDatatype);
            
            // copy buffer at correct position
            memcpy(already_allocated_buffer+size, tmp_buffer, tmp_size);
            
            // update position in output buffer
            size = size + tmp_size;
            
            // update total number of read data
            number = number + tmp_number;
        }
    }
    else 
    {
        for (tr=0; tr<pioDataset->ntimeranges; tr++)
        {
            // read number of data for timerange tr
            tmp_number = pioReadNumber(*pioDataset, tr);
            if (tmp_number < 0) return -1; // stop if something bad happened
                        
            // update total number of data
            number = number + tmp_number;
        }
        
        // deduce buffer size from number of data
        size = number*pioGetSize(pioDatatype);
        
        return size;
    }
    
    return number;
}


