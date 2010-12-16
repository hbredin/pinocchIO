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

#include "structure_utils.h"

#include <stdlib.h>
#include <string.h>
#include <hdf5_hl.h>

#include "pIOTypes.h"
#include "pIOAttributes.h"


hid_t linkDatatype()
{
	hid_t tLink;
	hid_t tPosition;
	hid_t tNumber;	
	
	tLink = H5Tcreate(H5T_COMPOUND, sizeof(link_t));
	tPosition = H5Tcopy(H5T_NATIVE_INT32);
	tNumber = H5Tcopy(H5T_NATIVE_INT32);
	H5Tinsert(tLink, "position", HOFFSET(link_t, position), tPosition);
	H5Tinsert(tLink, "number", HOFFSET(link_t, number), tNumber);
	H5Tclose(tPosition);
	H5Tclose(tNumber);
	return tLink;
}

int lengthOfList( listOfPaths_t* list)
{
	if (list != NULL) 
		return 1 + lengthOfList(list->next);
	return 0;
}

listOfPaths_t* addCopyToList( listOfPaths_t* list, char* path)
{
	listOfPaths_t* new_element = (listOfPaths_t*) malloc(sizeof(listOfPaths_t));
	new_element->path = (char*) malloc((strlen(path)+1)*sizeof(char));
	sprintf(new_element->path, "%s", path);
	new_element->next = list;
	return new_element;
}

int destroyList( listOfPaths_t* list)
{
	if (list != NULL)
	{
		if (list->next != NULL) destroyList(list->next);
		free(list->path);
		free(list);
	}
	return 1;
}

int getTimesUsed(PIOTimeline pioTimeline)
{
	ERROR_SWITCH_INIT
	int times_used = -1;
	herr_t get_err;
	
	ERROR_SWITCH_OFF
	get_err = H5LTget_attribute_int(pioTimeline.identifier, ".", PIOAttribute_TimesUsed, &times_used);
	ERROR_SWITCH_ON
	
	if (get_err < 0) return -1;
	return times_used;
}

int incrementTimesUsed(PIOTimeline pioTimeline)
{
	ERROR_SWITCH_INIT
	herr_t set_err;
	
	int times_used = getTimesUsed(pioTimeline);
	if (times_used < 0) return -1;
	
	times_used++;
	
	ERROR_SWITCH_OFF
	set_err = H5LTset_attribute_int(pioTimeline.identifier, ".", PIOAttribute_TimesUsed, &times_used, 1);
	ERROR_SWITCH_ON
	
	if (set_err < 0) return -1;
	return times_used;
}

int decrementTimesUsed(PIOTimeline pioTimeline)
{
	ERROR_SWITCH_INIT
	herr_t set_err;
	
	int times_used = getTimesUsed(pioTimeline);
	if (times_used < 0) return -1;
	
	times_used--;
	
	ERROR_SWITCH_OFF
	set_err = H5LTset_attribute_int(pioTimeline.identifier, ".", PIOAttribute_TimesUsed, &times_used, 1);
	ERROR_SWITCH_ON
	
	if (set_err < 0) return -1;
	return times_used;
}

listOfPaths_t* listOfObjectsInGroup( hid_t group, H5G_obj_t filter)
{
	ERROR_SWITCH_INIT
	
	herr_t get_err;
	H5G_info_t groupInfo; 
	H5G_obj_t objType;
	hsize_t i, numberOfLinks;
	size_t size;
	char* name = NULL;
	listOfPaths_t* listOfPaths = NULL;
	
	// get number of links in group
	ERROR_SWITCH_OFF
	get_err = H5Gget_info(group, &groupInfo);
	ERROR_SWITCH_ON
	if (get_err < 0) return listOfPaths;
	numberOfLinks = groupInfo.nlinks;
	
	// loop on all links in group
	for (i=0; i<numberOfLinks; i++)
	{
		// get type of current link
		objType = H5Gget_objtype_by_idx(group, i);
		// only add requested type of objects to the list
		if ( objType == filter || filter == -1) 
		{
			// get length of object name
			ERROR_SWITCH_OFF
			size = H5Lget_name_by_idx(group, ".", H5_INDEX_NAME, H5_ITER_INC, i, NULL, -1, H5P_DEFAULT);
			ERROR_SWITCH_ON
			if (size < 0) return listOfPaths;
			
			// get object name
			// allocate with extra character for '\0'
			name = (char *) malloc((size+1)*sizeof(char));
			ERROR_SWITCH_OFF
			size = H5Lget_name_by_idx(group, ".", H5_INDEX_NAME, H5_ITER_INC, i, name, size+1, H5P_DEFAULT);
			ERROR_SWITCH_ON
			if (size < 0)
			{
				free(name);
				return listOfPaths;
			}
			
			// add it into the list
			listOfPaths = addCopyToList(listOfPaths, name);
			free(name); name = NULL;			
		}
	}
	return listOfPaths;	
}

listOfPaths_t* listOfDatasetsInGroup(hid_t group)
{
	return listOfObjectsInGroup(group, H5G_DATASET);
}

listOfPaths_t* listOfGroupsInGroup(hid_t group)
{
	return listOfObjectsInGroup(group, H5G_GROUP);
}

int recursiveListOfObjectsInGroup(hid_t group, H5G_obj_t filter, listOfPaths_t** paths, int counter, char* current_path)
{
	listOfPaths_t* datasets_names = NULL;
	listOfPaths_t* dataset_name = NULL;
	listOfPaths_t* groups_names = NULL;
	listOfPaths_t* group_name = NULL;

	hid_t subgroup;
	
	ERROR_SWITCH_INIT
	
	// initialize running_counter
	int running_counter = counter;
	
	// get list of datasets if requested
	if (filter == H5G_DATASET || filter == -1) 
	{
		datasets_names = listOfDatasetsInGroup(group);
		// loop on datasets
		dataset_name = datasets_names;
		while (dataset_name != NULL)
		{				
			// add dataset name at the end of current_path
			char* path = (char*) malloc(sizeof(char)*(strlen(current_path)+strlen(dataset_name->path)+1));
			sprintf(path, "%s%s", current_path, dataset_name->path);
			// add resulting path to the list
			*paths = addCopyToList(*paths, path);
			free(path); path = NULL;
			// go to next dataset
			dataset_name = dataset_name->next;
			// update running counter
			running_counter++;			
		}			
		destroyList(datasets_names); datasets_names = NULL;	
	}
	
	// get list of groups
	groups_names = listOfGroupsInGroup(group);
	// loop on groups
	group_name = groups_names;
	while (group_name != NULL) 
	{
		// add group name at the end of current_path
		char* path = (char*) malloc(sizeof(char)*(strlen(current_path)+strlen(group_name->path)+2));
		sprintf(path, "%s%s/", current_path, group_name->path);		
		
		if (filter == H5G_GROUP || filter == -1)
		{
			// add resulting path to the list
			*paths = addCopyToList(*paths, path);
			// update running counter
			running_counter++;			
		}
		
		// try to recursively dive into group
		ERROR_SWITCH_OFF
		subgroup = H5Gopen2(group, group_name->path, H5P_DEFAULT);
		ERROR_SWITCH_ON
		if (subgroup < 0) continue;
		
		// recurse
		running_counter = recursiveListOfObjectsInGroup(subgroup, filter, paths, running_counter, path);
		H5Gclose(subgroup);

		// free running path
		free(path); path = NULL;
		
		// go to next group
		group_name = group_name->next;
	}
	destroyList(groups_names); groups_names = NULL;
	
	return running_counter;
}

listOfPaths_t* allDatasetsInGroup(hid_t file, const char* path2group)
{
	
	hid_t group;
	int numberOfDatasets = -1;
	listOfPaths_t* paths = NULL;
	ERROR_SWITCH_INIT
	
	ERROR_SWITCH_OFF
	group = H5Gopen2(file, path2group, H5P_DEFAULT);
	ERROR_SWITCH_ON
	if (group < 0) return NULL;
	
	numberOfDatasets = recursiveListOfObjectsInGroup(group, H5G_DATASET, &paths, 0, "");
	H5Gclose(group);
	
	return paths;
}


//int allMatchingDatasetsInGroup(hid_t file,
//							   const char* path2group,
//							   const char* filter,
//							   char*** path)
//{
//	char** tofilter_path = NULL;
//	int numberOfDatasets = -1;
//	int numberOfMatchingDatasets = 0;
//	int ds;
//	
//	numberOfDatasets = allDatasetsInGroup(file, path2group, &tofilter_path);
//	if (numberOfDatasets < 0) return -1;
//	
//	int filter_length = strlen(filter);
//	int path_length;
//	
//	for (ds = 0; ds < numberOfDatasets; ds++)
//	{
//		path_length = strlen(tofilter_path[ds]);
//		if (strcmp(filter, tofilter_path[ds]+path_length-filter_length) == 0) numberOfMatchingDatasets++;
//	}
//	
//	*path = (char**) malloc(numberOfMatchingDatasets*sizeof(char*));
//	numberOfMatchingDatasets = 0;
//	for (ds = 0; ds < numberOfDatasets; ds++) 
//	{
//		path_length = strlen(tofilter_path[ds]);
//		if (strcmp(filter, tofilter_path[ds]+path_length-filter_length) == 0) 
//		{
//			(*path)[numberOfMatchingDatasets] = (char*) malloc((path_length+1)*sizeof(char));			
//			strncpy((*path)[numberOfMatchingDatasets], tofilter_path[ds], path_length); 
//			(*path)[numberOfMatchingDatasets][path_length] = '\0';
//			numberOfMatchingDatasets++;
//		}
//	}
//	
//	for (ds=0; ds<numberOfDatasets; ds++) free(tofilter_path[ds]);
//	free(tofilter_path);
//	
//	return numberOfMatchingDatasets;
//}
