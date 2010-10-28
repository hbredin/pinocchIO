/*
 *  structure_utils.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 20/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

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

int listOfObjectsInGroup( hid_t group, H5G_obj_t filter, char*** objects_names)
{
	ERROR_SWITCH_INIT
	
	herr_t get_err;
	H5G_info_t groupInfo; 
	H5G_obj_t objType;
	hsize_t i, numberOfLinks;
	int counter = 0; 
	size_t size;
	char* name = NULL;
	
	// get number of links in group
	ERROR_SWITCH_OFF
	get_err = H5Gget_info(group, &groupInfo);
	ERROR_SWITCH_ON
	if (get_err < 0) return -1;
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
			if (size < 0) return counter;
			
			// get object name
			// allocate with extra character for '\0'
			name = (char *) malloc((size+1)*sizeof(char));
			ERROR_SWITCH_OFF
			size = H5Lget_name_by_idx(group, ".", H5_INDEX_NAME, H5_ITER_INC, i, name, size+1, H5P_DEFAULT);
			ERROR_SWITCH_ON
			if (size < 0)
			{
				free(name);
				return counter;
			}
			// set final character '\0'
			name[size+1] = '\0';
			
			// add it at the end of the list
			(*objects_names) = (char **)realloc(*objects_names, (counter+1)*sizeof(char*));
			(*objects_names)[counter] = (char *) malloc((size+2)* sizeof(char));
			strcpy((*objects_names)[counter], name);
			(*objects_names)[counter][size+1] = '\0';
			free(name); 
			
			// update number of objects
			counter++;
		}
	}
	return counter;	
}

int listOfDatasetsInGroup( hid_t group, char*** datasets_names)
{
	return listOfObjectsInGroup(group, H5G_DATASET, datasets_names);
}

int listOfGroupsInGroup( hid_t group, char*** groups_names)
{
	return listOfObjectsInGroup(group, H5G_GROUP, groups_names);
}

int recursiveListOfObjectsInGroup(hid_t group, H5G_obj_t filter, char*** paths, H5G_obj_t** type, int counter, char* current_path)
{
	char** datasets_names = NULL;
	char** groups_names = NULL;
	int datasets_count;
	int groups_count;
	int i;
	hid_t subgroup;
	char* running_path;
	
	ERROR_SWITCH_INIT
	
	// initialize running_counter
	int running_counter = counter;
	
	// get list of datasets if requested
	if (filter == H5G_DATASET || filter == -1) 
	{
		datasets_count = listOfDatasetsInGroup(group, &datasets_names);
		if (datasets_count > 0)
		{
			// extend list of paths
			*paths = (char **) realloc(*paths, (running_counter+datasets_count)*sizeof(char*));
			// extend list of types
			*type = (H5G_obj_t *) realloc(*type, (running_counter+datasets_count)*sizeof(int));
			
			// loop on datasets
			for (i=0; i<datasets_count; i++)
			{
				// save full path to dataset 
				(*paths)[running_counter] = 
				(char *) malloc((strlen(datasets_names[i])+strlen(current_path)+1) * sizeof(char));
				sprintf((*paths)[running_counter], 
						"%s%s", current_path, datasets_names[i]);
				
				// save type of path
				(*type)[running_counter] = H5G_DATASET;
				
				// update running_counter
				running_counter++;			
			}
		}
		
		// free memory
		if ( datasets_names ) 
		{
			for (i=0; i<datasets_count; i++) free(datasets_names[i]);
			free(datasets_names);
		}
	}
	
	
	// get list of groups
	groups_count = listOfGroupsInGroup(group, &groups_names);
	if (groups_count > 0)
	{
		// loop on groups
		for (i=0; i<groups_count; i++)
		{
			// add group in list of objects if requested
			if (filter == H5G_GROUP || filter == -1)
			{
				// extend list of paths
				*paths = (char **) realloc(*paths, (running_counter+1)*sizeof(char*));
				// extend list of types
				*type = (H5G_obj_t *) realloc(*type, (running_counter+1)*sizeof(int));
				
				// save full path to group
				(*paths)[running_counter] = 
				(char *) malloc((strlen(groups_names[i])+strlen(current_path)+2)* sizeof(char));
				sprintf((*paths)[running_counter], 
						"%s%s/", current_path, groups_names[i]);
				
				// save type of path
				(*type)[running_counter] = H5G_GROUP;
				
				// update running_counter
				running_counter++;
			}
			
			// try to recursively dive into group
			ERROR_SWITCH_OFF
			subgroup = H5Gopen2(group, groups_names[i], H5P_DEFAULT);
			ERROR_SWITCH_ON
			if (subgroup < 0) continue;
			
			// set running_path
			running_path = (char *) malloc((strlen(groups_names[i])+strlen(current_path)+2)* sizeof(char));
			sprintf(running_path, 
					"%s%s/", current_path, groups_names[i]);
			
			// recurse
			running_counter = recursiveListOfObjectsInGroup(subgroup, filter, paths, type, running_counter, running_path);
			// free running_path
			free(running_path);
			H5Gclose(subgroup);
		}
	}
	
	// free memory
	if ( groups_names ) 
	{
		for (i=0; i<groups_count; i++) free(groups_names[i]);
		free(groups_names);
	}
	
	return running_counter;
}


int allDatasetsInGroup(hid_t file,
					   const char* path2group,
					   char*** path)
{
	
	hid_t group;
	H5G_obj_t *type = NULL;
	int numberOfDatasets = -1;
	ERROR_SWITCH_INIT
	
	ERROR_SWITCH_OFF
	group = H5Gopen2(file, path2group, H5P_DEFAULT);
	ERROR_SWITCH_ON
	if (group < 0) return -1;
	
	numberOfDatasets = recursiveListOfObjectsInGroup(group, H5G_DATASET, path, &type, 0, "");
	if (type) free(type); type = NULL;
	H5Gclose(group);
	
	return numberOfDatasets;
}


int allMatchingDatasetsInGroup(hid_t file,
							   const char* path2group,
							   const char* filter,
							   char*** path)
{
	char** tofilter_path = NULL;
	int numberOfDatasets = -1;
	int numberOfMatchingDatasets = 0;
	int ds;
	
	numberOfDatasets = allDatasetsInGroup(file, path2group, &tofilter_path);
	if (numberOfDatasets < 0) return -1;
	
	int filter_length = strlen(filter);
	int path_length;
	
	for (ds = 0; ds < numberOfDatasets; ds++)
	{
		path_length = strlen(tofilter_path[ds]);
		if (strcmp(filter, tofilter_path[ds]+path_length-filter_length) == 0) numberOfMatchingDatasets++;
	}
	
	*path = (char**) malloc(numberOfMatchingDatasets*sizeof(char*));
	numberOfMatchingDatasets = 0;
	for (ds = 0; ds < numberOfDatasets; ds++) 
	{
		path_length = strlen(tofilter_path[ds]);
		if (strcmp(filter, tofilter_path[ds]+path_length-filter_length) == 0) 
		{
			(*path)[numberOfMatchingDatasets] = (char*) malloc((path_length+1)*sizeof(char));			
			strncpy((*path)[numberOfMatchingDatasets], tofilter_path[ds], path_length); 
			(*path)[numberOfMatchingDatasets][path_length] = '\0';
			numberOfMatchingDatasets++;
		}
	}
	
	for (ds=0; ds<numberOfDatasets; ds++) free(tofilter_path[ds]);
	free(tofilter_path);
	
	return numberOfMatchingDatasets;
}
