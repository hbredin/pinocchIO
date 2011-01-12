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

/**
 \page piocp piocp
 
 \section usage Usage 
 \section example Example
 */


#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pinocchIO.h"

static int verbose_flag = 0;
static int all_flag = 0;

void usage(const char * path2tool)
{
	fprintf(stdout, 
			"USAGE: %s [options] INPUT OUTPUT\n", path2tool);
	fprintf(stdout, 
            "                --all\n"
            "                Copy everything -- BEWARE: fails quietly...\n"
			"       -t PATH, --timeline=PATH\n"
			"                Copy timeline at PATH\n"
			"       -d PATH, --dataset=PATH\n"
			"                Copy dataset at PATH\n");
	fflush(stdout);
}

int main (int argc, char *const  argv[])
{	
	char* input_file = NULL;
	char* output_file = NULL;
	char* timeline_path = NULL;
	char* dataset_path = NULL;
    
    PIOFile pioInputFile = PIOFileInvalid;
    PIOFile pioOutputFile = PIOFileInvalid;
        
    char** pathsToTimelines = NULL;
    int numberOfTimelines = 0;
    int i;
    
    char** pathsToDatasets = NULL;
    int numberOfDatasets = 0;
    
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
            {"all",      no_argument, &all_flag, 1},
			{"timeline", required_argument, 0, 't'},
			{"dataset",  required_argument, 0, 'd'},
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
	
	if (optind+2>argc)
	{
		usage(argv[0]);
		exit(-1);		
	}
    
    if (all_flag && timeline_path && dataset_path)
    {
        fprintf(stderr, "--all, --timeline and --dataset options are mutually exclusive.\n");
        fflush(stderr);
        exit(-1);
    }
    
	input_file = argv[optind];
    optind++;
    output_file = argv[optind];
	
	pioInputFile = pioOpenFile(input_file, PINOCCHIO_READONLY);
	if (PIOFileIsInvalid(pioInputFile))
	{
		fprintf(stderr, "Cannot open input file %s with read-only access.\n", input_file);
		fflush(stderr);
		exit(-1);
	}
    
    pioOutputFile = pioOpenFile(output_file, PINOCCHIO_READNWRITE);
    if (PIOFileIsInvalid(pioOutputFile))
    {
        fprintf(stderr, "Cannot open output file %s with read-n-write access.\n", output_file);
        fflush(stderr);
        pioCloseFile(&pioInputFile);
        exit(-1);
    }
    
    // copy timeline
    if (timeline_path)
    {
        if (!pioCopyTimeline(timeline_path, pioInputFile, pioOutputFile))
        {
            fprintf(stderr, "Cannot copy timeline %s.\n", timeline_path);
            fflush(stderr);
            pioCloseFile(&pioInputFile);
            pioCloseFile(&pioOutputFile);
            exit(-1);
        }        
    }
        
    // copy dataset
    if (dataset_path)
    {
        if (!pioCopyDataset(dataset_path, pioInputFile, pioOutputFile))
        {
            fprintf(stderr, "Cannot copy dataset %s.\n", dataset_path);
            fflush(stderr);
            pioCloseFile(&pioInputFile);
            pioCloseFile(&pioOutputFile);
            exit(-1);            
        }    
    }
    
    if (all_flag)
    {
        numberOfTimelines = pioGetListOfTimelines(pioInputFile, &pathsToTimelines);
        for (i=0; i<numberOfTimelines; i++)
            pioCopyTimeline(pathsToTimelines[i], pioInputFile, pioOutputFile);
        for (i=0; i<numberOfTimelines; i++)
            free(pathsToTimelines[i]);
        free(pathsToTimelines);

        numberOfDatasets = pioGetListOfDatasets(pioInputFile, &pathsToDatasets);
        for (i=0; i<numberOfDatasets; i++)
            pioCopyDataset(pathsToDatasets[i], pioInputFile, pioOutputFile);
        for (i=0; i<numberOfDatasets; i++)
            free(pathsToDatasets[i]);
        free(pathsToDatasets);
    }
    
    pioCloseFile(&pioOutputFile);
    pioCloseFile(&pioInputFile);
    
	return 1;
}
