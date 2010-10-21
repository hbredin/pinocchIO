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
#include "pinocchIO/pinocchIO.h"

static int verbose_flag;

void usage(const char * path2tool)
{
	fprintf(stdout, 
			"USAGE: %s -p FILE\n", path2tool);
	fprintf(stdout, 
			"       -p FILE, --pinocchio=FILE\n"
			"           Examine pinocchIO file FILE.\n");
	fflush(stdout);
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
			{"pinocchio",     required_argument, 0, 'p'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		
		c = getopt_long (argc, argv, "hp:",
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
				
			case 'p':
				pinocchio_file = optarg;
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
	
	if (!pinocchio_file)
	{
		fprintf(stderr, "Missing path to pinocchIO file.\n");
		fflush(stderr); 
		usage(argv[0]);
		exit(-1);
	}
	
	PIOFile pioFile = pioOpenFile(pinocchio_file, PINOCCHIO_READONLY);
	if (PIOFileIsInvalid(pioFile))
	{
		fprintf(stderr, "Cannot open pinocchIO file %s.\n", pinocchio_file);
		fflush(stderr);
		exit(-1);
	}
	
	char** timelines = NULL;
	int numberOfTimelines = pioGetListOfTimelines(pioFile, &timelines);
	int tl;
	fprintf(stdout, "Timelines\n");
	for (tl = 0; tl<numberOfTimelines; tl++) 
	{
		fprintf(stdout, "%s", timelines[tl]);
		PIOTimeline pioTimeline = pioOpenTimeline(PIOMakeObject(pioFile), timelines[tl]);
		fprintf(stdout, " %s\n", pioTimeline.description);
		pioCloseTimeline(pioTimeline);
	}
	for (tl = 0; tl<numberOfTimelines; tl++) free(timelines[tl]);
	free(timelines);
	
	
	char** datasets = NULL;
	int numberOfDatasets = pioGetListOfDatasets(pioFile, &datasets);
	int ds;
	fprintf(stdout, "Datasets\n");
	for (ds = 0; ds<numberOfDatasets; ds++) 
	{
		fprintf(stdout, "%s", datasets[ds]);
		PIODataset pioDataset = pioOpenDataset(PIOMakeObject(pioFile), datasets[ds]);
		fprintf(stdout, " %s\n", pioDataset.description);
		pioCloseDataset(pioDataset);
	}
	for (ds = 0; ds<numberOfDatasets; ds++) free(datasets[ds]);
	free(datasets);
	
	
	pioCloseFile(pioFile);
	return 1;
}
	