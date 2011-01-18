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

#include "list_utils.h"
#include <stdlib.h>

int numberOfLabelsInList (listOfLabels_t* list)
{
    if (list)
        return 1+numberOfLabelsInList(list->next);
    return 0;
}

listOfLabels_t* isLabelInList (listOfLabels_t* list, int label)
{
    if (list)
    {
        if (list->value == label) 
            return list;
        else 
            return isLabelInList(list->next, label);
    }
    return NULL;
}

listOfLabels_t* addLabelToList (listOfLabels_t* list, int label)
{
    listOfLabels_t* new_element = (listOfLabels_t*) malloc(sizeof(listOfLabels_t));
    new_element->value = label;
    new_element->count = 0;
    new_element->next = list;
    return new_element;
}

int destroyListOfLabels (listOfLabels_t* list)
{
    if (list)
    {
        if (list->next != NULL) 
            destroyListOfLabels(list->next);
        free(list);
    }
    return 1;
}

listOfLabels_t* updateLabelCount (listOfLabels_t* list, int label, int countDelta)
{
    listOfLabels_t* element = isLabelInList(list, label);
    
    if (element)
    {
        element->count += countDelta;
        return list;
    }
    else {
        element = addLabelToList(list, label);
        element->count += countDelta;
        return element;
    }
}

int getLabelCount (listOfLabels_t* list, int label)
{
    listOfLabels_t* element = isLabelInList(list, label);
    if (element) return element->count;
    else return 0;
}

