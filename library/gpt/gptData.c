// 
// Copyright 2011 Herve BREDIN (bredin@limsi.fr)
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

#include "gptData.h"
#include <stdlib.h>
#include <string.h>

// get data dimension
int gptGetServerDimension(GPTServer gptServer)
{
    if (gptServer.servesData)
        return gptServer.datatype.dimension;
    else
        return -1;
}

int gptReadNext(GPTServer* server, PIODatatype datatype, void** buffer, 
                int* nLabels, int** labels)
{
    int number = -1;
    int numberOfLabels = -1;
    int i, r;
    
    if (!DAT_AVAILABLE(*server)) 
    {
        server->eof = 1;
        return -1;
    }
    
    // if last timerange is processed, go to first timerange of next file
    if (server->current_timerange_index == DAT_NTIMERANGES(*server, 
                                                           server->current_file_index))
    {
        if (PIODatasetIsValid(server->current_dataset))
            pioCloseDataset(&(server->current_dataset));
        if (PIOFileIsValid(server->current_file))
            pioCloseFile(&(server->current_file));
        server->current_timerange_index = 0;
        server->current_file_index++;
    }
    
    // if last file is processed, stop
    if (server->current_file_index == DAT_NFILES(*server))
    {
        server->current_file_index = 0;
        server->current_timerange_index = 0;
        server->eof = 1;
        return -1;
    }
    
    // open next file/dataset if necessary
    if (server->current_timerange_index == 0)
    {
        server->current_file = pioOpenFile(DAT_PATH(*server, server->current_file_index),
                                           PINOCCHIO_READONLY);
        server->current_dataset = pioOpenDataset(PIOMakeObject(server->current_file),
                                                 DAT_DATASET(*server));
    }
    
    // if not filtered, read next data
    if (DAT_FILTERED(*server, server->current_file_index, server->current_timerange_index))
    {
        number = pioRead(&(server->current_dataset), 
                         server->current_timerange_index,
                         datatype, 
                         buffer);
        
        // get total number of corresponding labels
        // (as the sum of number of labels for each corresponding timeranges)
        if (nLabels || labels)
        {
            if (LBL_AVAILABLE(*server))
            {
                numberOfLabels = 0;
                for (i=0; 
                     i<server->numberOfCorrespondingLabelTimerange[server->current_file_index][server->current_timerange_index]; 
                     i++) 
                {
                    numberOfLabels += LBL_NLABELS(*server, 
                                                  server->current_file_index, 
                                                  i+server->firstCorrespondingLabelTimerange[server->current_file_index][server->current_timerange_index]);
                }
            }
        }
        
        if (nLabels)
        {
            if (LBL_AVAILABLE(*server)) *nLabels = numberOfLabels;
            else *nLabels = 0;
        }
        
        if (labels)
        {
            if (LBL_AVAILABLE(*server))
            {
                if (numberOfLabels > server->current_data_labels_number)
                {
                    server->current_data_labels_number = numberOfLabels;
                    server->current_data_labels = (int*) realloc(server->current_data_labels,
                                                                 server->current_data_labels_number*sizeof(int));
                }
                
                numberOfLabels = 0;
                for (i=0; 
                     i<server->numberOfCorrespondingLabelTimerange[server->current_file_index][server->current_timerange_index]; 
                     i++) 
                {
                    for (r=0; 
                         r<LBL_NLABELS(*server, 
                                       server->current_file_index, 
                                       i+server->firstCorrespondingLabelTimerange[server->current_file_index][server->current_timerange_index]); 
                         r++) 
                    {
                        server->current_data_labels[numberOfLabels] = LBL_LABEL(*server,
                                                                                server->current_file_index,
                                                                                i+server->firstCorrespondingLabelTimerange[server->current_file_index][server->current_timerange_index],
                                                                                r);
                        numberOfLabels++;
                    }
                }
                
                *labels = server->current_data_labels;
            }
            else *labels = NULL;
        }
        
        server->current_timerange_index++;
    }
    // otherwise, go to next timerange
    else 
    {
        server->current_timerange_index++;
        number = gptReadNext(server, datatype, buffer, nLabels, labels);
    }
    
    // return number of data
    return number;
}


long gptDumpServer(GPTServer* server,
                   PIODatatype datatype,
                   void* buffer)
{
    int f; // file index
    int tr; // time range index
    long totalNumberOfEntries; // total number of served entries
    long numberOfEntries;  // local number of entries
    long oneEntrySize; // memory size of one entry
    void* localBuffer = NULL;
    
    oneEntrySize = (long)pioGetSize(datatype);
    totalNumberOfEntries = 0;
    
    // if buffer == NULL, return expected size of buffer
    if (!buffer)
    {

        for (f=0; f<DAT_NFILES(*server); f++)
        {
            for (tr=0; tr<DAT_NTIMERANGES(*server, f); tr++)
            {
                if (server->filtered[f][tr]) 
                    totalNumberOfEntries += server->numberOfEntriesPerTimerangePerFile[f][tr];
            }
        }
        return  oneEntrySize * totalNumberOfEntries;
    }

    while (server->eof == 0) 
    {   
        numberOfEntries = gptReadNextData(server, datatype, &localBuffer);
        if (numberOfEntries > 0)
        {
            memcpy(buffer + totalNumberOfEntries*oneEntrySize, 
                   localBuffer, 
                   numberOfEntries*oneEntrySize);
            totalNumberOfEntries += numberOfEntries;            
        }
    }    
    return totalNumberOfEntries;
}




