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

void usage(const char * path2tool)
{
	fprintf(stdout, 
			"USAGE: %s [options] INPUT OUTPUT\n", path2tool);
	fprintf(stdout, 
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
    
    PIODataset pioInputDataset = PIODatasetInvalid;
    PIODataset pioOutputDataset = PIODatasetInvalid;
    
    PIOTimeline pioInputTimeline = PIOTimelineInvalid;
    PIOTimeline pioOutputTimeline = PIOTimelineInvalid;
    
    PIODatatype pioDatatype = PIODatatypeInvalid;
    
    int t; // timerange counter
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
    
    if (timeline_path && dataset_path)
    {
        fprintf(stderr, "--timeline and --dataset options are mutually exclusive.\n");
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
    
    // make sure input dataset timeline is copied before input dataset is copied 
    if (dataset_path)
    {
        pioInputDataset = pioOpenDataset(PIOMakeObject(pioInputFile), dataset_path);
        if (PIODatasetIsInvalid(pioInputDataset))
        {
            fprintf(stderr, "Cannot open input dataset %s.\n", dataset_path);
            fflush(stderr);
            pioCloseFile(&pioInputFile);
            pioCloseFile(&pioOutputFile);
            exit(-1);                
        }
        
        pioInputTimeline = pioGetTimeline(pioInputDataset);
        if (PIOTimelineIsInvalid(pioInputTimeline))
        {
            fprintf(stderr, "Cannot get timeline of input dataset %s.\n", dataset_path);
            fflush(stderr);
            pioCloseDataset(&pioInputDataset);
            pioCloseFile(&pioInputFile);
            pioCloseFile(&pioOutputFile);
            exit(-1);
        }
        
        timeline_path = (char*) malloc((strlen(pioInputTimeline.path)+1)*sizeof(char));
        sprintf(timeline_path, "%s", pioInputTimeline.path);
        
        pioCloseTimeline(&pioInputTimeline);
        pioCloseDataset(&pioInputDataset);
    }
      
    // copy input timeline
    if (timeline_path)
    {
        // open input timeline
        pioInputTimeline = pioOpenTimeline(PIOMakeObject(pioInputFile), timeline_path);
        if (PIOTimelineIsInvalid(pioInputTimeline))
        {
            fprintf(stderr, "Cannot open input timeline %s.\n", timeline_path);
            fflush(stderr);
            pioCloseFile(&pioInputFile);
            pioCloseFile(&pioOutputFile);
            exit(-1);
        }
        
        // check if a timeline with same path already exists
        pioOutputTimeline = pioOpenTimeline(PIOMakeObject(pioOutputFile), pioInputTimeline.path);
        if (PIOTimelineIsValid(pioOutputTimeline))
        {
            // if so, compare existing timeline with to-be-copied timeline
            if (pioCompareTimeLines(pioInputTimeline.timeranges, pioInputTimeline.ntimeranges,
                                pioOutputTimeline.timeranges, pioOutputTimeline.ntimeranges) != PINOCCHIO_TIMELINE_COMPARISON_SAME)
            {
                fprintf(stderr, 
                        "Different timeline already exists at path %s.\n",
                        pioInputTimeline.path);
                fflush(stderr);
                pioCloseTimeline(&pioOutputTimeline);
                pioCloseTimeline(&pioInputTimeline);
                pioCloseFile(&pioInputFile);
                pioCloseFile(&pioOutputFile);
                exit(-1);            
            }
        }
        else 
        {
            // otherwise, try and create new timeline
            pioOutputTimeline = pioNewTimeline(pioOutputFile, pioInputTimeline.path, pioInputTimeline.description,
                                               pioInputTimeline.ntimeranges, pioInputTimeline.timeranges);
            if (PIOTimelineIsInvalid(pioOutputTimeline))
            {
                fprintf(stderr, "Cannot create output timeline %s.\n", pioInputTimeline.path);
                fflush(stderr);
                pioCloseTimeline(&pioInputTimeline);
                pioCloseFile(&pioInputFile);
                pioCloseFile(&pioOutputFile);
                exit(-1);            
            }            
        }
        
        pioCloseTimeline(&pioOutputTimeline);
        pioCloseTimeline(&pioInputTimeline);
    }
        
    
    if (dataset_path)
    {
        // open input dataset
        pioInputDataset = pioOpenDataset(PIOMakeObject(pioInputFile), dataset_path);
        if (PIODatasetIsInvalid(pioInputDataset))
        {
            fprintf(stderr, "Cannot open input dataset %s.\n", dataset_path);
            fflush(stderr);
            pioCloseFile(&pioInputFile);
            pioCloseFile(&pioOutputFile);
            exit(-1);                
        }
        
        // open output timeline
        pioOutputTimeline = pioOpenTimeline(PIOMakeObject(pioOutputFile), timeline_path);
        if (PIOTimelineIsInvalid(pioOutputTimeline))
        {
            fprintf(stderr, "Cannot open output timeline %s.\n", timeline_path);
            fflush(stderr);
            pioCloseDataset(&pioInputDataset);
            pioCloseFile(&pioInputFile);
            pioCloseFile(&pioOutputFile);
            exit(-1);
        }
        
        // open input datatype
        pioDatatype = pioGetDatatype(pioInputDataset);
        if (PIODatatypeIsInvalid(pioDatatype))
        {
            fprintf(stderr, "Cannot get input datatype.\n");
            fflush(stderr);
            pioCloseTimeline(&pioOutputTimeline);
            pioCloseDataset(&pioInputDataset);
            pioCloseFile(&pioInputFile);
            pioCloseFile(&pioOutputFile);
            exit(-1);
        }

        // create output dataset
        pioOutputDataset = pioNewDataset(pioOutputFile, 
                                         pioInputDataset.path, pioInputDataset.description, 
                                         pioOutputTimeline,
                                         pioDatatype);
        if (PIODatasetIsInvalid(pioOutputDataset))
        {
            fprintf(stderr, "Cannot create output dataset %s.\n", pioInputDataset.path);
            fflush(stderr);
            pioCloseDatatype(&pioDatatype);
            pioCloseTimeline(&pioOutputTimeline);
            pioCloseDataset(&pioInputDataset);
            pioCloseFile(&pioInputFile);
            pioCloseFile(&pioOutputFile);
            exit(-1);
        }
        
        // read input dataset and write it to output dataset
        
        for (t=0; t<pioOutputTimeline.ntimeranges; t++) 
        {
            number = pioReadData(&pioInputDataset, t, pioDatatype, &buffer);
            pioWrite(&pioOutputDataset, t, buffer, number, pioDatatype);
        }

        pioCloseDatatype(&pioDatatype);

        pioCloseTimeline(&pioOutputTimeline);
        
        pioCloseDataset(&pioOutputDataset);
        pioCloseDataset(&pioInputDataset);
        
        
    }
    
    
    pioCloseFile(&pioOutputFile);
    pioCloseFile(&pioInputFile);
    
	return 1;
}
