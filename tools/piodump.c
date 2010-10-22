/*
 *  piodump.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 22/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

/*!
 @header pinocchIO file dump
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
			"USAGE: %s [options] FILE\n", path2tool);
	fprintf(stdout, 
			"       -t PATH, --timeline=PATH\n"
			"           Dump timeline at PATH\n"
			"       -d PATH, --dataset=PATH\n"
			"           Dump dataset at PATH\n"
			"       -s, --timestamp\n"
			"           Show timestamps\n");
	fflush(stdout);
}

int main (int argc, char *const  argv[])
{	
	char* pinocchio_file = NULL;
	char* timeline_path = NULL;
	char* dataset_path = NULL;
	int timestamp_flag = 0;
	
	int tr; // timerange index
	void* buffer = NULL; // data buffer
	
	int* buffer_int;
	float* buffer_float;
	double* buffer_double;
	char* buffer_char;
	
	int numberOfVectors; // number of vector for each timerange
	int n; // vector counter
	int d; // dimension counter
	
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
			{"timestamp", no_argument,      0, 's'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		
		c = getopt_long (argc, argv, "ht:d:s",
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
				
			case 's':
				timestamp_flag = 1;
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
	
	if ((dataset_path && timeline_path) || (!dataset_path && !timeline_path))
	{
		fprintf(stderr, "Choose between --timeline or --dataset.\n");
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
	
	if (dataset_path)
	{
		PIODataset pioDataset = pioOpenDataset(PIOMakeObject(pioFile), dataset_path);
		if (PIODatasetIsInvalid(pioDataset)) 
		{
			fprintf(stderr, "Cannot open dataset %s.\n", dataset_path);
			fflush(stderr);
			pioCloseFile(&pioFile);
			exit(-1);
		}
		
		PIOTimeline pioTimeline = pioGetTimeline(pioDataset);
		if (PIOTimelineIsInvalid(pioTimeline))
		{
			fprintf(stderr, "Cannot get timeline from dataset %s.\n", dataset_path);
			fflush(stderr);
			pioCloseDataset(&pioDataset);
			pioCloseFile(&pioFile);
			exit(-1);
		}
		
		PIODatatype pioDatatype = pioGetDatatype(pioDataset);
		if (PIODatatypeIsInvalid(pioDatatype))
		{
			fprintf(stderr, "Cannot get datatype from dataset %s.\n", dataset_path);
			fflush(stderr);
			pioCloseTimeline(&pioTimeline);
			pioCloseDataset(&pioDataset);
			pioCloseFile(&pioFile);
			exit(-1);
		}
		
		
		buffer = malloc(1);
		
		for (tr=0; tr<pioDataset.ntimeranges; tr++)
		{
			if (pioReallocBuffer(pioDataset, tr, &buffer, pioDatatype) < 0)
			{
				fprintf(stderr, "Error. Cannot realloc buffer for timerange #%d\n", tr);
				fflush(stderr);
				break;
			}

			buffer_int = (int*)buffer;
			buffer_float = (float*)buffer;
			buffer_double = (double*)buffer;
			buffer_char = (char*)buffer;			
			
			numberOfVectors = pioRead(pioDataset, tr, buffer, pioDatatype);
			if (numberOfVectors < 0)
			{
				fprintf(stderr, "Error. Cannot read dataset for timerange #%d\n", tr);
				fflush(stderr);
				break;				
			}
			
						
			for (n=0; n<numberOfVectors; n++)
			{				
				if (timestamp_flag)
				{
					fprintf(stdout, "%lf %lf | ", 
							(double)(1.*pioTimeline.timeranges[tr].time)/pioTimeline.timeranges[tr].scale,
							(double)(1.*(pioTimeline.timeranges[tr].time+pioTimeline.timeranges[tr].duration))/pioTimeline.timeranges[tr].scale);									
				}
				for (d=0; d<pioDatatype.dimension; d++)
				{					
					switch (pioDatatype.type) {
						case PINOCCHIO_TYPE_INT:
							fprintf(stdout, "%d ", buffer_int[n*pioDatatype.dimension+d]);
							break;
						case PINOCCHIO_TYPE_FLOAT:
							fprintf(stdout, "%f ", buffer_float[n*pioDatatype.dimension+d]);
							break;
						case PINOCCHIO_TYPE_DOUBLE:
							fprintf(stdout, "%lf ", buffer_double[n*pioDatatype.dimension+d]);
							break;
						case PINOCCHIO_TYPE_CHAR:
							fprintf(stdout, "%c", buffer_char[n*pioDatatype.dimension+d]);
							break;
						default:
							break;
					}
				}
				fprintf(stdout, "\n");
			}
		}
		
		pioCloseDatatype(&pioDatatype);
		pioCloseTimeline(&pioTimeline);
		pioCloseDataset(&pioDataset);
		free(buffer);
	}
	
	if (timeline_path)
	{
		PIOTimeline pioTimeline = pioOpenTimeline(PIOMakeObject(pioFile), timeline_path);
		if (PIOTimelineIsInvalid(pioTimeline))
		{
			fprintf(stderr, "Cannot open timeline %s.\n", timeline_path);
			fflush(stderr);
			pioCloseFile(&pioFile);
			exit(-1);
		}
		
		for (tr=0; tr<pioTimeline.ntimeranges; tr++) {
			fprintf(stdout, "%lf %lf\n", 
					(double)(1.*pioTimeline.timeranges[tr].time)/pioTimeline.timeranges[tr].scale,
					(double)(1.*(pioTimeline.timeranges[tr].time+pioTimeline.timeranges[tr].duration))/pioTimeline.timeranges[tr].scale);
		}
		
		pioCloseTimeline(&pioTimeline);
	}
	
	
	pioCloseFile(&pioFile);
	return 1;
}
