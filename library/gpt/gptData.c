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
#include "stdlib.h"

// get data dimension
int gptGetServerDimension(GPTServer gptServer)
{
    if (gptServer.servesData)
        return gptServer.datatype.dimension;
    else
        return -1;
}

int gptReadNext(GPTServer* gptServer, PIODatatype pioDatatype, void** buffer, int** labels, int* nLabels)
{
    int number = -1;
    
    if (!DAT_AVAILABLE(*gptServer)) 
    {
        gptServer->eof = 1;
        return -1;
    }
    
    // if last timerange is processed, go to first timerange of next file
    if (gptServer->current_timerange_index == DAT_NTIMERANGES(*gptServer, 
                                                              gptServer->current_file_index))
    {
        if (PIODatasetIsValid(gptServer->current_dataset))
            pioCloseDataset(&(gptServer->current_dataset));
        if (PIOFileIsValid(gptServer->current_file))
            pioCloseFile(&(gptServer->current_file));
        gptServer->current_timerange_index = 0;
        gptServer->current_file_index++;
    }
    
    // if last file is processed, stop
    if (gptServer->current_file_index == DAT_NFILES(*gptServer))
    {
        gptServer->current_file_index = 0;
        gptServer->current_timerange_index = 0;
        gptServer->eof = 1;
        return -1;
    }
    
    // open next file/dataset if necessary
    if (gptServer->current_timerange_index == 0)
    {
        gptServer->current_file = pioOpenFile(DAT_PATH(*gptServer, gptServer->current_file_index),
                                              PINOCCHIO_READONLY);
        gptServer->current_dataset = pioOpenDataset(PIOMakeObject(gptServer->current_file),
                                                    DAT_DATASET(*gptServer));
    }
    
    // if not filtered, read next data
    if (DAT_FILTERED(*gptServer, gptServer->current_file_index, gptServer->current_timerange_index))
    {
        number = pioRead(&(gptServer->current_dataset), 
                         gptServer->current_timerange_index,
                         pioDatatype, 
                         buffer);
        
        if (labels)
        {
            if (LBL_AVAILABLE(*gptServer))
                *labels = DAT_LABELS(*gptServer, 
                                     gptServer->current_file_index, 
                                     gptServer->current_timerange_index);
            else *labels = NULL;
        }
        
        if (nLabels)
        {
            if (LBL_AVAILABLE(*gptServer))
                *nLabels = DAT_NLABELS(*gptServer, 
                                       gptServer->current_file_index, 
                                       gptServer->current_timerange_index);
            else *nLabels = 0;
        }
        
        gptServer->current_timerange_index++;
    }
    // otherwise, go to next timerange
    else 
    {
        gptServer->current_timerange_index++;
        number = gptReadNext(gptServer, pioDatatype, buffer, labels, nLabels);
    }
    
    // return number of data
    return number;
}



