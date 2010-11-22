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
    config_t config;
    const config_setting_t *data_or_label_section = NULL;
    const config_setting_t *files_section = NULL;
    
    const char* cur_dataset = NULL;
    const char* cur_file = NULL;
    int f, ff; // file counter
    
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
    
    if (config_setting_is_list(files_section) != CONFIG_TRUE)
    {
        fprintf(stderr, "Files must be provided as a list of files.\n");
        fprintf(stderr, "Example: files = ( \"file1\", \"file2\" );");
        config_destroy(&config);
        return 0;          
    }
    
    // alloc/copy path to dataset
    *pathToDataset = (char*)malloc((strlen(cur_dataset)+1)*sizeof(char));
    sprintf(*pathToDataset, "%s", cur_dataset);
    
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
