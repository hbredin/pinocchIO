/*
 *  gptConfig.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 22/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include <string.h>
#include <stdlib.h>
#include <libconfig.h>

#include "gptTypes.h"
#include "gptConfig.h"
#include "gptServer.h"

#define GEPETTO_CONFIGURATION_FILE_SECTION_TITLE_FILTER "filter"

#define GEPETTO_CONFIGURATION_FILE_FILTER_TYPE_NONE "none"
#define GEPETTO_CONFIGURATION_FILE_FILTER_TYPE_EQUALS_TO "equalsTo"
#define GEPETTO_CONFIGURATION_FILE_FILTER_TYPE_DIFFERS_FROM "differsFrom"
#define GEPETTO_CONFIGURATION_FILE_FILTER_TYPE_GREATER_THAN "greaterThan"
#define GEPETTO_CONFIGURATION_FILE_FILTER_TYPE_SMALLER_THAN "smallerThan"

#define GEPETTO_CONFIGURATION_FILE_SECTION_TITLE_DATA   "data"
#define GEPETTO_CONFIGURATION_FILE_SECTION_TITLE_LABEL  "label"

#define GEPETTO_CONFIGURATION_FILE_PATH_TO_DATASET "dataset"
#define GEPETTO_CONFIGURATION_FILE_LIST_OF_FILES "files"


int getNumberOfLines( const char* ascii_file )
{
	char* line = NULL;
	FILE* file = NULL;
	int numberOfLines = -1;
	size_t length = -1;
	
	file = fopen(ascii_file, "r");
	if (!file) return -1;
	
	numberOfLines = 0;
	while (!feof(file)) 
	{
		line = fgetln(file, &length);
		if (!line) break;
		numberOfLines++;
	}
	
	fclose(file);
	return numberOfLines;
}

int readLines(const char* ascii_file, int nLines, char** *strings)
{
	char* line = NULL;
	FILE* file = NULL;
	int lineId = -1;
	size_t length = -1;
	char* newline = "\r\n";
	char* string = NULL;
	int dimension = -1;
	
	file = fopen(ascii_file, "r");
	if (!file) return -1;
	
	for (lineId = 0; lineId < nLines; lineId++) 
	{
		line = fgetln(file, &length);
		string = strtok(line, newline);
		dimension = strlen(string)+1;		
		
		(*strings)[lineId] = (char*)malloc(dimension*sizeof(char));
        memcpy((*strings)[lineId], string, dimension);
	}
	fclose(file);
	return lineId;	
}

int getFilterFromConfigurationFile(const char* filename, GPTLabelFilterType* labelFilterType, int* labelFilterReference)
{
    config_t config;
    const config_setting_t *filter_section = NULL;

    config_init(&config);
    // raed configuration file
    if (config_read_file(&config, filename) == CONFIG_FALSE)
    {
        fprintf(stderr,
                "Could not parse configuration file %s.\n",
                filename);
        fprintf(stderr, "%s at line %d\n", config_error_text(&config), config_error_line(&config));
        fflush(stderr);
        config_destroy(&config);
        return 0;
    }    
    
    *labelFilterType = GEPETTO_LABEL_FILTER_TYPE_UNDEFINED;
    *labelFilterReference = -1;    
    
    filter_section = config_lookup(&config, GEPETTO_CONFIGURATION_FILE_SECTION_TITLE_FILTER);
    if (filter_section)
    {
        if (config_setting_lookup_int(filter_section,
                                      GEPETTO_CONFIGURATION_FILE_FILTER_TYPE_NONE,
                                      labelFilterReference) == CONFIG_TRUE)
            *labelFilterType = GEPETTO_LABEL_FILTER_TYPE_NONE;
        
        if (config_setting_lookup_int(filter_section, 
                                      GEPETTO_CONFIGURATION_FILE_FILTER_TYPE_EQUALS_TO, 
                                      labelFilterReference) == CONFIG_TRUE)
            *labelFilterType = GEPETTO_LABEL_FILTER_TYPE_EQUALS_TO;
        
        if (config_setting_lookup_int(filter_section, 
                                      GEPETTO_CONFIGURATION_FILE_FILTER_TYPE_DIFFERS_FROM, 
                                      labelFilterReference) == CONFIG_TRUE)
            *labelFilterType = GEPETTO_LABEL_FILTER_TYPE_DIFFERS_FROM;
        
        if (config_setting_lookup_int(filter_section, 
                                      GEPETTO_CONFIGURATION_FILE_FILTER_TYPE_GREATER_THAN, 
                                      labelFilterReference) == CONFIG_TRUE)
            *labelFilterType = GEPETTO_LABEL_FILTER_TYPE_GREATER_THAN;
        
        if (config_setting_lookup_int(filter_section, 
                                      GEPETTO_CONFIGURATION_FILE_FILTER_TYPE_SMALLER_THAN, 
                                      labelFilterReference) == CONFIG_TRUE)
            *labelFilterType = GEPETTO_LABEL_FILTER_TYPE_SMALLER_THAN;        
    }
    
    config_destroy(&config);
    
    return 1;
}

int getPathsFromConfigurationFile(const char* filename, const char* data_or_label,
                                  int* numberOfFiles, char*** pathToFile, char** pathToDataset)
{
    int f, ff; // file counter
    
    config_t config;
    const config_setting_t *data_or_label_section = NULL;
    const config_setting_t *files_section = NULL;
    
    const char* cur_dataset = NULL;

    int is_list = CONFIG_FALSE;
    const char* cur_file = NULL;
    
    int is_group = CONFIG_FALSE;
    const char* prefix = NULL;
    const char* suffix = NULL;
    const char* base_list = NULL;
    char** base_path = NULL;
    
    // initialize configuration file parser
    config_init(&config);
    
    // raed configuration file
    if (config_read_file(&config, filename) == CONFIG_FALSE)
    {
        fprintf(stderr,
                "Could not parse configuration file %s.\n",
                filename);
        fflush(stderr);
        fprintf(stderr, "%s at line %d\n", config_error_text(&config), config_error_line(&config));
        config_destroy(&config);
        return 0;
    }
    
    // data_or_label is "data" or "label"
    // get corresponding section in configuration file
    data_or_label_section = config_lookup(&config, data_or_label);
    if (!data_or_label_section)
    {
        fprintf(stdout,
                "Did not find any \"%s\" section in configuration file %s.\n",
                data_or_label, filename);
        fflush(stdout);
        *numberOfFiles = 0;
        config_destroy(&config);
        return 0;        
    }
    
    // get path to dataset
    if (config_setting_lookup_string(data_or_label_section, "dataset", &cur_dataset) == CONFIG_FALSE)
    {
        fprintf(stderr,
                "Could not find %s/dataset in configuration file %s.\n",
                data_or_label, filename);
        fflush(stderr);
        config_destroy(&config);
        return 0;                
    }
    
    // get files section
    files_section = config_setting_get_member(data_or_label_section, "files");
    if (!files_section)
    {
        fprintf(stderr, 
                "Could not find %s/files section in configuration file %s.\n",
                data_or_label, filename);
        fflush(stderr);
        config_destroy(&config);
        return 0;
    }
    
    is_list = config_setting_is_list(files_section);
    is_group = config_setting_is_group(files_section);
    
    if (!(is_list || is_group))
    {
        fprintf(stderr, "Files must be provided as a list of files.\n");
        fprintf(stderr, "Example: files = ( \"file1\", \"file2\" );");
        config_destroy(&config);
        return 0;          
    }
    
    // alloc/copy path to dataset
    *pathToDataset = (char*)malloc((strlen(cur_dataset)+1)*sizeof(char));
    sprintf(*pathToDataset, "%s", cur_dataset);
    
    if (is_list)
    {
        // get number of files
        *numberOfFiles = config_setting_length(files_section);
        
        // get paths to files
        *pathToFile = (char**) malloc((*numberOfFiles) * sizeof(char*));
        for (f=0; f<*numberOfFiles; f++)
        {
            cur_file = config_setting_get_string_elem(files_section, f);
            
            if (!cur_file)
            {
                fprintf(stderr, 
                        "There must be something wrong with %dth file in list.\n", 
                        f+1);
                for (ff=0; ff<f; ff++) free((*pathToFile)[ff]); free((*pathToFile));
                free(*pathToDataset);
                config_destroy(&config);
                return 0;                            
            }
            
            (*pathToFile)[f] = (char*) malloc((strlen(cur_file)+1)*sizeof(char));
            sprintf((*pathToFile)[f], "%s", cur_file);        
        }
    }
    
    if (is_group)
    {
        if (config_setting_lookup_string(files_section, "prefix", &prefix) == CONFIG_FALSE)
            prefix = "";
        
        if (config_setting_lookup_string(files_section, "suffix", &suffix) == CONFIG_FALSE)
            suffix = "";
        
        if (config_setting_lookup_string(files_section, "list", &base_list) == CONFIG_FALSE)
        {
            fprintf(stderr, "Missing %s>%s> list  section in file %s.\n", 
                    data_or_label, 
                    GEPETTO_CONFIGURATION_FILE_LIST_OF_FILES, 
                    filename);
            fflush(stderr);
            free(*pathToDataset);
            config_destroy(&config);
            return 0;
        }

        // get number of files
        *numberOfFiles = getNumberOfLines(base_list);
        if (*numberOfFiles < 0)
        {
            fprintf(stderr, "Problem with list %s.\n", base_list);
            fflush(stderr);
            free(*pathToDataset);
            config_destroy(&config);
            return 0;
        }
        
        // get base paths to files
        base_path = (char**) malloc((*numberOfFiles)*sizeof(char*));
        readLines(base_list, *numberOfFiles, &base_path);
        
        // get paths to files
        *pathToFile = (char**) malloc((*numberOfFiles)*sizeof(char*));
        for (f=0; f<*numberOfFiles; f++) 
        {
            (*pathToFile)[f] = (char*) malloc(strlen(prefix)+strlen(suffix)+strlen(base_path[f])+1);
            sprintf((*pathToFile)[f], "%s%s%s", prefix, base_path[f], suffix);
        }
        
        for (f=0; f<*numberOfFiles; f++) free(base_path[f]); free(base_path);
    }
    
    config_destroy(&config);
    
    return 1;
}


GPTServer gptNewServerFromConfigurationFile(const char* filename)
{
    GPTServer gptServer = GPTServerInvalid;
    
    int numberOfDataFiles = -1;
    char** pathToDataFile = NULL;
    char* pathToDataDataset = NULL;

    int numberOfLabelFiles = -1;
    char** pathToLabelFile = NULL;
    char* pathToLabelDataset = NULL;
    
    GPTLabelFilterType labelFilterType = GEPETTO_LABEL_FILTER_TYPE_UNDEFINED;
    int labelFilterReference = -1;
    
    int f;
    
    // parse "data" section
    if (!getPathsFromConfigurationFile(filename, 
                                       GEPETTO_CONFIGURATION_FILE_SECTION_TITLE_DATA, 
                                       &numberOfDataFiles, &pathToDataFile, &pathToDataDataset))
        return GPTServerInvalid;
    
    // try and parse "filter" section
    if (!getFilterFromConfigurationFile(filename, &labelFilterType, &labelFilterReference))
    {
        for (f=0; f<numberOfDataFiles; f++) free(pathToDataFile[f]); free(pathToDataFile);
        free(pathToDataDataset);
        return GPTServerInvalid;
    }
    
    // if filter section parsing proved to provided a filter (even NONE filter), parse "label" section
    if (labelFilterType != GEPETTO_LABEL_FILTER_TYPE_UNDEFINED)
    {
        if (!getPathsFromConfigurationFile(filename, 
                                           GEPETTO_CONFIGURATION_FILE_SECTION_TITLE_LABEL,
                                           &numberOfLabelFiles, &pathToLabelFile, &pathToLabelDataset))
        {
            for (f=0; f<numberOfDataFiles; f++) free(pathToDataFile[f]); free(pathToDataFile);
            free(pathToDataDataset);
            return GPTServerInvalid;
        }        
    }
    
    // initialize server
    gptServer = gptNewServer(numberOfDataFiles, pathToDataFile, pathToDataDataset,
                             labelFilterType, labelFilterReference, 
                             numberOfLabelFiles, pathToLabelFile, pathToLabelDataset);
    
    // free what needs to be freed
    for (f=0; f<numberOfDataFiles; f++) free(pathToDataFile[f]); free(pathToDataFile);
    free(pathToDataDataset);

    if (labelFilterType != GEPETTO_LABEL_FILTER_TYPE_UNDEFINED)
    {
        for (f=0; f<numberOfLabelFiles; f++) free(pathToLabelFile[f]); free(pathToLabelFile);
        free(pathToLabelDataset);
    }
    
    return gptServer;
}
