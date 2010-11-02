/*
 *  piols.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 21/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

/*!
 @header pinocchIO ls
 @abstract   
 @discussion 
 */

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pinocchIO.h"


#define PIOLS_PATH_MAX_LENGTH 40

static int verbose_flag;
static int timeline_flag = 1;
static int dataset_flag = 1;

void usage(const char * path2tool)
{
	fprintf(stdout, 
			"USAGE: %s [options] FILE\n", path2tool);
	fprintf(stdout, 
			"       --no-timeline\n"
			"           Do not list timelines\n"
			"       --no-dataset\n"
			"           Do not list datasets\n");
	fflush(stdout);
}

void pretty_print(const char * path2dataset, const char * description)
{
	int pathLength = strlen(path2dataset);
	//int descLength = strlen(description);
	int dotsLength = PIOLS_PATH_MAX_LENGTH - pathLength;
	int d;
	
	fprintf(stdout, "%s", path2dataset);
	if (dotsLength > 2)
	{
		fprintf(stdout, " ");
		for (d=0; d<dotsLength; d++) fprintf(stdout, ".");
		fprintf(stdout, " ");
	}
	else
	{
		fprintf(stdout, "\n");
		for (d=0; d<PIOLS_PATH_MAX_LENGTH-2; d++) fprintf(stdout, " ");
		fprintf(stdout, "... ");
	}
	fprintf(stdout, "%s\n", description);
}

int main (int argc, char *const  argv[])
{	
	char* pinocchio_file = NULL;
	
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
			{"no-timeline", no_argument, &timeline_flag, 0},
			{"no-dataset",  no_argument, &dataset_flag,  0},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		
		c = getopt_long (argc, argv, "h",
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
		fprintf(stderr, "Missing path to pinocchIO file.\n");
		fflush(stderr); 
		usage(argv[0]);
		exit(-1);		
	}
	
	pinocchio_file = argv[optind];
	
	
	PIOFile pioFile = pioOpenFile(pinocchio_file, PINOCCHIO_READONLY);
	if (PIOFileIsInvalid(pioFile))
	{
		fprintf(stderr, "Cannot open pinocchIO file %s.\n", pinocchio_file);
		fflush(stderr);
		exit(-1);
	}
	
	fprintf(stdout, "== Medium ==\n");
	fprintf(stdout, "%s\n", pioFile.medium);
	fflush(stdout);
	
	if (timeline_flag)
	{
		char** timelines = NULL;
		int numberOfTimelines = pioGetListOfTimelines(pioFile, &timelines);
		int tl;
		fprintf(stdout, "== %d timeline(s) ==\n", numberOfTimelines);
		for (tl = 0; tl<numberOfTimelines; tl++) 
		{
			PIOTimeline pioTimeline = pioOpenTimeline(PIOMakeObject(pioFile), timelines[tl]);
			if (PIOTimelineIsInvalid(pioTimeline))
			{
				pretty_print(timelines[tl], "ERROR - CANNOT OPEN TIMELINE");
				continue;
			}
			pretty_print(timelines[tl], pioTimeline.description);
			pioCloseTimeline(&pioTimeline);
		}
		for (tl = 0; tl<numberOfTimelines; tl++) { free(timelines[tl]); timelines[tl] = NULL; }
		free(timelines); timelines = NULL;
	}
	
	if (dataset_flag)
	{
		
		char** datasets = NULL;
		int numberOfDatasets = pioGetListOfDatasets(pioFile, &datasets);
		int ds;
		fprintf(stdout, "== %d dataset(s) ==\n", numberOfDatasets);
		
		for (ds = 0; ds<numberOfDatasets; ds++) 
		{
			PIODataset pioDataset = pioOpenDataset(PIOMakeObject(pioFile), datasets[ds]);
			if (PIODatasetIsInvalid(pioDataset))
			{
				pretty_print(datasets[ds], "ERROR - CANNOT OPEN DATASET");
				continue;
			}
			pretty_print(datasets[ds], pioDataset.description);
			pioCloseDataset(&pioDataset);
		}
		for (ds = 0; ds<numberOfDatasets; ds++) { free(datasets[ds]); datasets[ds] = NULL; }
		free(datasets); datasets = NULL;
	}
	
	pioCloseFile(&pioFile);
	
	return 1;
}
