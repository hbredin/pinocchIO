// 
// Copyright 2010-2011 Herve BREDIN (bredin@limsi.fr)
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

#ifndef _LIST_UTILS_H
#define _LIST_UTILS_H

#include "gptTypes.h"

int             numberOfLabelsInList (listOfLabels_t* list);

listOfLabels_t* addLabelToList       (listOfLabels_t* list, int label);
int             destroyListOfLabels  (listOfLabels_t* list);

listOfLabels_t* isLabelInList        (listOfLabels_t* list, int label);
listOfLabels_t* updateLabelCount     (listOfLabels_t* list, 
                                      int label,
                                      int countDelta);
int             getLabelCount        (listOfLabels_t* list, int label);

#endif

