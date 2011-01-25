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

#include "gptLabel.h"
#include <stdlib.h>

/**
 @internal

 @brief Get list of label timeranges matching a specific label value
 @param[in] server Gepetto server
 @param[in] labelValue Label value looked for
 @param[in] f File index
 @param[out] timeranges Array of label timeranges
 @returns number of label timeranges
 
 @note
 Calling getTimerangesForLabelAndFile with NULL @a timeranges will
 return the number of label timeranges. It can be uses to preallocate
 @a timeranges with enough memory space to store the whole list of timeranges.
 
 @ingroup gptlabel
 
 */
static int getTimerangesForLabelAndFile(GPTServer server,
                                        int labelValue,
                                        int f,
                                        PIOTimeRange* timeranges)
{
    int t, i;
    
    // Number of timeranges with matching label
    int numberOfTimeranges = 0;
    
    for (t=0; t<LBL_NTIMERANGES(server, f); t++) 
    {
        for (i=0; i<LBL_NLABELS(server, f, t); i++) 
        {
            if (LBL_LABEL(server, f, t, i) == labelValue)
            {
                if (timeranges)
                    timeranges[numberOfTimeranges] = LBL_TIMERANGE(server, f, t);
                numberOfTimeranges++;
                break; // in case of two identical labels in the same timerange
            }
        }
    }
    
    return numberOfTimeranges;
}


int gptGetTimerangesForLabel(GPTServer server,
                             int labelValue,
                             GPTTimeRange* timeranges)
{
    int f, i;
    int localNumberOfTimeranges = -1;
    PIOTimeRange* localTimeranges = NULL;
    int totalNumberOfTimeranges = -1;
    
    if (timeranges)
    {
        totalNumberOfTimeranges = 0;
        for (f=0; f<LBL_NFILES(server); f++) 
        {
            localNumberOfTimeranges = getTimerangesForLabelAndFile(server, labelValue, f, NULL);
            localTimeranges = (PIOTimeRange*) malloc(localNumberOfTimeranges*sizeof(PIOTimeRange));
            localNumberOfTimeranges = getTimerangesForLabelAndFile(server, labelValue, f, localTimeranges);
            for (i=0; i<localNumberOfTimeranges; i++) 
            {
                timeranges[totalNumberOfTimeranges+i].fileIndex = f;
                timeranges[totalNumberOfTimeranges+i].timerange = localTimeranges[i];
            }
            totalNumberOfTimeranges += localNumberOfTimeranges;
            free(localTimeranges);
        }
    }
    else 
    {
        totalNumberOfTimeranges = 0;
        for (f=0; f<LBL_NFILES(server); f++) 
            totalNumberOfTimeranges += getTimerangesForLabelAndFile(server, 
                                                                    labelValue,
                                                                    f, NULL);
    }
    
    return totalNumberOfTimeranges;
}
