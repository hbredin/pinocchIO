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


#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "pinocchIO.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b)) 
#define MIN(a,b) ((a) < (b) ? (a) : (b)) 

static int verbose_flag = 0;
static int maximum_flag = 0;
static int minimum_flag = 0;
//static int count_flag   = 0;
//static int average_flag = 0;

void prepareBufferForMaximum( void* buffer, PIODatatype pioDatatype)
{
    int dimension = pioDatatype.dimension;
    int d;
    
    char* buffer_char = buffer;
    int*  buffer_int  = buffer;
    float* buffer_float = buffer;
    double* buffer_double = buffer;
    
    switch (pioDatatype.type) 
    {
        case PINOCCHIO_TYPE_CHAR:
            for (d=0; d<dimension; d++) buffer_char[d] = -CHAR_MAX;
            break;
        case PINOCCHIO_TYPE_INT:
            for (d=0; d<dimension; d++) buffer_int[d] = -INT_MAX;
            break;
        case PINOCCHIO_TYPE_FLOAT:
            for (d=0; d<dimension; d++) buffer_float[d] = -FLT_MAX;
            break;
        case PINOCCHIO_TYPE_DOUBLE:
            for (d=0; d<dimension; d++) buffer_double[d] = -DBL_MAX;
            break;
        default:
            break;
    }
}

void updateBufferForMaximum( void* aggregated_buffer, void* buffer, int number, PIODatatype pioDatatype)
{
    int dimension = pioDatatype.dimension;
    int d;
    int n;

    char* aggregated_buffer_char = aggregated_buffer;
    int*  aggregated_buffer_int  = aggregated_buffer;
    float* aggregated_buffer_float = aggregated_buffer;
    double* aggregated_buffer_double = aggregated_buffer;
    
    char* buffer_char = buffer;
    int*  buffer_int  = buffer;
    float* buffer_float = buffer;
    double* buffer_double = buffer;

    switch (pioDatatype.type) 
    {
        case PINOCCHIO_TYPE_CHAR:
            for (n=0; n<number; n++) 
                for (d=0; d<dimension; d++) 
                    aggregated_buffer_char[d] = MAX(aggregated_buffer_char[d], buffer_char[n*dimension+d]);
            break;
        case PINOCCHIO_TYPE_INT:
            for (n=0; n<number; n++) 
                for (d=0; d<dimension; d++) 
                    aggregated_buffer_int[d] = MAX(aggregated_buffer_int[d], buffer_int[n*dimension+d]);
            break;
        case PINOCCHIO_TYPE_FLOAT:
            for (n=0; n<number; n++) 
                for (d=0; d<dimension; d++) 
                    aggregated_buffer_float[d] = MAX(aggregated_buffer_float[d], buffer_float[n*dimension+d]);
            break;
        case PINOCCHIO_TYPE_DOUBLE:
            for (n=0; n<number; n++) 
                for (d=0; d<dimension; d++) 
                    aggregated_buffer_double[d] = MAX(aggregated_buffer_double[d], buffer_double[n*dimension+d]);
            break;
        default:
            break;
    }
}

void prepareBufferForMinimum( void* buffer, PIODatatype pioDatatype)
{
    int dimension = pioDatatype.dimension;
    int d;
    
    char* buffer_char = buffer;
    int*  buffer_int  = buffer;
    float* buffer_float = buffer;
    double* buffer_double = buffer;
    
    switch (pioDatatype.type) 
    {
        case PINOCCHIO_TYPE_CHAR:
            for (d=0; d<dimension; d++) buffer_char[d] = CHAR_MAX;
            break;
        case PINOCCHIO_TYPE_INT:
            for (d=0; d<dimension; d++) buffer_int[d] = INT_MAX;
            break;
        case PINOCCHIO_TYPE_FLOAT:
            for (d=0; d<dimension; d++) buffer_float[d] = FLT_MAX;
            break;
        case PINOCCHIO_TYPE_DOUBLE:
            for (d=0; d<dimension; d++) buffer_double[d] = DBL_MAX;
            break;
        default:
            break;
    }
}

void updateBufferForMinimum( void* aggregated_buffer, void* buffer, int number, PIODatatype pioDatatype)
{
    int dimension = pioDatatype.dimension;
    int d;
    int n;
    
    char* aggregated_buffer_char = aggregated_buffer;
    int*  aggregated_buffer_int  = aggregated_buffer;
    float* aggregated_buffer_float = aggregated_buffer;
    double* aggregated_buffer_double = aggregated_buffer;
    
    char* buffer_char = buffer;
    int*  buffer_int  = buffer;
    float* buffer_float = buffer;
    double* buffer_double = buffer;
    
    switch (pioDatatype.type) 
    {
        case PINOCCHIO_TYPE_CHAR:
            for (n=0; n<number; n++) 
                for (d=0; d<dimension; d++) 
                    aggregated_buffer_char[d] = MIN(aggregated_buffer_char[d], buffer_char[n*dimension+d]);
            break;
        case PINOCCHIO_TYPE_INT:
            for (n=0; n<number; n++) 
                for (d=0; d<dimension; d++) 
                    aggregated_buffer_int[d] = MIN(aggregated_buffer_int[d], buffer_int[n*dimension+d]);
            break;
        case PINOCCHIO_TYPE_FLOAT:
            for (n=0; n<number; n++) 
                for (d=0; d<dimension; d++) 
                    aggregated_buffer_float[d] = MIN(aggregated_buffer_float[d], buffer_float[n*dimension+d]);
            break;
        case PINOCCHIO_TYPE_DOUBLE:
            for (n=0; n<number; n++) 
                for (d=0; d<dimension; d++) 
                    aggregated_buffer_double[d] = MIN(aggregated_buffer_double[d], buffer_double[n*dimension+d]);
            break;
        default:
            break;
    }
}


//void prepareBufferForAverage( void* buffer, PIODatatype pioDatatype)
//{
//    int dimension = pioDatatype.dimension;
//    int d;
//    
//    char* buffer_char = buffer;
//    int*  buffer_int  = buffer;
//    float* buffer_float = buffer;
//    double* buffer_double = buffer;
//    
//    switch (pioDatatype.type) 
//    {
//        case PINOCCHIO_TYPE_CHAR:
//            for (d=0; d<dimension; d++) buffer_char[d] = 0;
//            break;
//        case PINOCCHIO_TYPE_INT:
//            for (d=0; d<dimension; d++) buffer_int[d] = 0;
//            break;
//        case PINOCCHIO_TYPE_FLOAT:
//            for (d=0; d<dimension; d++) buffer_float[d] = 0;
//            break;
//        case PINOCCHIO_TYPE_DOUBLE:
//            for (d=0; d<dimension; d++) buffer_double[d] = 0;
//            break;
//        default:
//            break;
//    }
//}


void usage(const char * path2tool)
{
	fprintf(stdout, 
			"USAGE: %s [options] INPUT\n", path2tool);
	fprintf(stdout, 
			"       -t PATH, --timeline=PATH\n"
			"                Target timeline\n"
			"       -d PATH, --dataset=PATH\n"
			"                Original dataset\n"
            "                --maximum\n"
            "                --minimum\n"
//            "                --count\n"
//            "                --average\n");
            );
	fflush(stdout);
}

int main (int argc, char *const  argv[])
{	
	char* input_file = NULL;
	char* timeline_path = NULL;
	char* dataset_path = NULL;
    
    char* aggregated_dataset_path = NULL;
    char* aggregated_dataset_description = NULL;
    
    PIOFile pioInputFile = PIOFileInvalid;
    
    PIODataset pioInputDataset = PIODatasetInvalid;
    PIODataset pioOutputDataset = PIODatasetInvalid;
    
    PIOTimeline pioOriginalTimeline = PIOTimelineInvalid;
    PIOTimeline pioTargetTimeline = PIOTimelineInvalid;
    
    PIODatatype pioDatatype = PIODatatypeInvalid;
    
    void* buffer = NULL; // data buffer
    int number; // data number
    
	int c;
	while (1)
	{
		static struct option long_options[] =
		{
			/* These options set a flag. */
			{"verbose", no_argument, &verbose_flag, 1},
			{"brief",   no_argument, &verbose_flag, 0},
			/* These options don't set a flag.
			 We distinguish them by their indices. */
			{"timeline", required_argument, 0, 't'},
			{"dataset",  required_argument, 0, 'd'},
            {"maximum", no_argument, &maximum_flag, 1},
            {"minimum", no_argument, &minimum_flag, 1},
//            {"count",   no_argument, &count_flag,   1},
//            {"average", no_argument, &average_flag, 1},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		
		c = getopt_long (argc, argv, "ht:d:",
						 long_options, &option_index);
		
		/* Detect the end of the options. */
		if (c == -1)
			break;
		
		switch (c)
		{
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0)
					break;
				//				printf ("option %s", long_options[option_index].name);
				//				if (optarg)
				//					printf (" with arg %s", optarg);
				//				printf ("\n");
				break;
				
			case 't':
				timeline_path = optarg;
				break;
				
			case 'd':
				dataset_path = optarg;
				break;
                
			case 'h':
				usage(argv[0]);
				exit(-1);
				break;
				
			case '?':
				/* getopt_long already printed an error message. */
				usage(argv[0]);
				break;
				
			default:
				abort ();
		}
	}
	
	if (optind+1>argc)
	{
		usage(argv[0]);
		exit(-1);		
	}

	input_file = argv[optind];
    optind++;
    
    
    if (minimum_flag && maximum_flag)
    {
        fprintf(stderr, "You must choose one aggregation method.\n");
        fflush(stderr);
        exit(-1);
    }
	
    // open file with read and write access rights
	pioInputFile = pioOpenFile(input_file, PINOCCHIO_READNWRITE);
	if (PIOFileIsInvalid(pioInputFile))
	{
		fprintf(stderr, "Cannot open input file %s with read-n-write access.\n", input_file);
		fflush(stderr);
		exit(-1);
	}
    
    // load target timeline
    pioTargetTimeline = pioOpenTimeline(PIOMakeObject(pioInputFile), timeline_path);
    if (PIOTimelineIsInvalid(pioTargetTimeline))
    {
        fprintf(stderr, "Cannot open target timeline %s in file %s.\n", timeline_path, input_file);
        fflush(stderr);
        pioCloseFile(&pioInputFile);
        exit(-1);
    }
    
    // load original dataset
    pioInputDataset = pioOpenDataset(PIOMakeObject(pioInputFile), dataset_path);
    if (PIODatasetIsInvalid(pioInputDataset))
    {
        fprintf(stderr, "Cannot open original dataset %s in file %s.\n", dataset_path, input_file);
        fflush(stderr);
        pioCloseTimeline(&pioTargetTimeline);
        pioCloseFile(&pioInputFile);
        exit(-1);
    }
    
    // load original timeline
    pioOriginalTimeline = pioGetTimeline(pioInputDataset);
    if (PIOTimelineIsInvalid(pioOriginalTimeline))
    {
        fprintf(stderr, "Cannot open timeline of original dataset %s in file %s.\n", dataset_path, input_file);
        fflush(stderr);
        pioCloseDataset(&pioInputDataset);
        pioCloseTimeline(&pioTargetTimeline);
        pioCloseFile(&pioInputFile);
        exit(-1);
    }
    
    // mapping between original and target timeline
    
    int target_t = 0;
    int original_t;
    int* mapping = (int*) malloc(pioOriginalTimeline.ntimeranges*sizeof(int));
    for (original_t=0; original_t<pioOriginalTimeline.ntimeranges; original_t++) 
    {
        // look for first target timerange intersecting original timerange
        while (
               // not yet reached the end of target timeline
               (target_t < pioTargetTimeline.ntimeranges)
               &&
               // target timerange is before original timerange
               (pioCompareTimeRanges(pioTargetTimeline.timeranges[target_t], 
                                     pioOriginalTimeline.timeranges[original_t]) != PINOCCHIO_TIMERANGE_COMPARISON_DESCENDING) 
               &&
               // target timerange do not intersect original timerange
               (!pioTimeRangeIntersectsTimeRange(pioTargetTimeline.timeranges[target_t], 
                                                 pioOriginalTimeline.timeranges[original_t]))
               )
        {
            // try next target timerange
            target_t++;
        }
        
        // if found, update mask based on 
        if (pioTimeRangeIntersectsTimeRange(pioTargetTimeline.timeranges[target_t], 
                                            pioOriginalTimeline.timeranges[original_t]))
            mapping[original_t] = target_t;
        else 
            mapping[original_t] = -1;
    }
    
    // get datatype
    pioDatatype = pioGetDatatype(pioInputDataset);
    
    if (maximum_flag)
    {
        aggregated_dataset_path = (char*) malloc((strlen(pioInputDataset.path)+strlen("/maximum")+1)*sizeof(char));
        sprintf(aggregated_dataset_path, "%s/maximum", pioInputDataset.path);

        aggregated_dataset_description = (char*) malloc((strlen(pioInputDataset.description)+strlen(" (maximum)")+1)*sizeof(char));
        sprintf(aggregated_dataset_description, "%s (maximum)", pioInputDataset.description);
    }
    
    if (minimum_flag)
    {
        aggregated_dataset_path = (char*) malloc((strlen(pioInputDataset.path)+strlen("/minimum")+1)*sizeof(char));
        sprintf(aggregated_dataset_path, "%s/minimum", pioInputDataset.path);
        
        aggregated_dataset_description = (char*) malloc((strlen(pioInputDataset.description)+strlen(" (minimum)")+1)*sizeof(char));
        sprintf(aggregated_dataset_description, "%s (minimum)", pioInputDataset.description);
    }
    
//    if (count_flag)
//    {
//        aggregated_dataset_path = (char*) malloc((strlen(pioInputDataset.path)+strlen("/number")+1)*sizeof(char));
//        sprintf(aggregated_dataset_path, "%s/number", pioInputDataset.path);
//        
//        aggregated_dataset_description = (char*) malloc((strlen(pioInputDataset.description)+strlen(" (number)")+1)*sizeof(char));
//        sprintf(aggregated_dataset_description, "%s (number)", pioInputDataset.description);        
//    }
//    
//    if (average_flag)
//    {
//        aggregated_dataset_path = (char*) malloc((strlen(pioInputDataset.path)+strlen("/average")+1)*sizeof(char));
//        sprintf(aggregated_dataset_path, "%s/average", pioInputDataset.path);
//        
//        aggregated_dataset_description = (char*) malloc((strlen(pioInputDataset.description)+strlen(" (average)")+1)*sizeof(char));
//        sprintf(aggregated_dataset_description, "%s (average)", pioInputDataset.description);
//    }
    
    pioOutputDataset = pioNewDataset(pioInputFile, aggregated_dataset_path, aggregated_dataset_description, pioTargetTimeline, pioDatatype);
    if (PIODatasetIsInvalid(pioOutputDataset))
    {
        fprintf(stderr, "Cannot create aggregated dataset %s in file %s.\n", aggregated_dataset_path, input_file);
        fflush(stderr);
        free(aggregated_dataset_path);
        free(aggregated_dataset_description);
        free(mapping);
        pioCloseDatatype(&pioDatatype);
        pioCloseTimeline(&pioOriginalTimeline);
        pioCloseDataset(&pioInputDataset);
        pioCloseTimeline(&pioTargetTimeline);
        pioCloseFile(&pioInputFile);
        exit(-1);
    }

    void* aggregated_buffer = malloc(pioGetSize(pioDatatype));   
    int previous_original_t = 0;
    
    for (target_t = 0; target_t < pioTargetTimeline.ntimeranges; target_t++) 
    {
        // initialize aggregated_buffer 
        if (maximum_flag)
            prepareBufferForMaximum(aggregated_buffer, pioDatatype);

        if (minimum_flag)
            prepareBufferForMinimum(aggregated_buffer, pioDatatype);

//        if (average_flag)
//            prepareBufferForAverage(aggregated_buffer, pioDatatype);
        
        // compute aggregated_buffer
        for (original_t = previous_original_t; original_t < pioOriginalTimeline.ntimeranges; original_t++) 
        {
            if (mapping[original_t] < target_t) continue;
            
            if (mapping[original_t] > target_t) 
            {
                previous_original_t = original_t;
                break;
            }
            
            if (mapping[original_t] == target_t)
            {
                number = pioReadData(&pioInputDataset, original_t, pioDatatype, &buffer);
                
                if (maximum_flag)
                    updateBufferForMaximum(aggregated_buffer, buffer, number, pioDatatype);
                
                if (minimum_flag)
                    updateBufferForMinimum(aggregated_buffer, buffer, number, pioDatatype);
                
//                if (average_flag)
//                    updateBufferForAverage(aggregated_buffer, buffer, number, pioDatatype);                                
            }
        }
        
        // write aggregated_buffer
        pioWrite(&pioOutputDataset, target_t, aggregated_buffer, 1, pioDatatype);
    }
    
    free(aggregated_dataset_path);
    free(aggregated_dataset_description);
    free(mapping);
    pioCloseDatatype(&pioDatatype);
    pioCloseTimeline(&pioOriginalTimeline);
    pioCloseDataset(&pioInputDataset);
    pioCloseTimeline(&pioTargetTimeline);
    pioCloseFile(&pioInputFile);
    
	return 1;
}
