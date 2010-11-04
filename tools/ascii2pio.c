/*
 *  ascii2pio.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 02/11/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

/*!
 @header ascii to pinocchIO conversion tool
 @abstract   
 @discussion 
 */

#include <pinocchIO/pinocchIO.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

#define DEBUG_LINE fprintf(stderr, "Line %d\n", __LINE__); fflush(stderr);

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

int readTimeline( const char* ascii_file, int nLines, PIOTimeRange* timeline, int32_t scale)
{
	char* line = NULL;
	FILE* file = NULL;
	int lineId = -1;
	size_t length = -1;
	char* separator = " \t\r\n";
	double start_sec, stop_sec;
	
	file = fopen(ascii_file, "r");
	if (!file) return -1;
	
	for (lineId = 0; lineId < nLines; lineId++) 
	{
		line = fgetln(file, &length);
		
		start_sec = atof(strtok(line, separator));
		stop_sec = atof(strtok(NULL, separator));
		
		timeline[lineId].scale    = scale;
		timeline[lineId].time     = start_sec*scale;
		timeline[lineId].duration = (stop_sec-start_sec)*scale;	
	}
	fclose(file);
	return lineId;
}

int readArrayDataset(const char* ascii_file, int nLines, int dimension, double* buffer)
{
	char* line = NULL;
	FILE* file = NULL;
	int lineId = -1;
	size_t length = -1;
	char* separator = " \t\r\n";
	int d = -1;
	double start_sec, stop_sec;
	
	file = fopen(ascii_file, "r");
	if (!file) return -1;
	
	for (lineId = 0; lineId < nLines; lineId++) 
	{
		line = fgetln(file, &length);
		
		start_sec = atof(strtok(line, separator));
		stop_sec = atof(strtok(NULL, separator));
		
		for (d=0; d<dimension; d++) 
			buffer[dimension*lineId+d] = atof(strtok(NULL, separator));
	}
	fclose(file);
	return lineId;
}

int readStringDataset(const char* ascii_file, int nLines, char** *strings)
{
	char* line = NULL;
	FILE* file = NULL;
	int lineId = -1;
	size_t length = -1;
	char* separator = " \t\r\n";
	char* newline = "\r\n";
	char* string = NULL;
	int dimension = -1;
	double start_sec, stop_sec;

	
	file = fopen(ascii_file, "r");
	if (!file) return -1;
	
	for (lineId = 0; lineId < nLines; lineId++) 
	{
		line = fgetln(file, &length);
		
		start_sec = atof(strtok(line, separator));
		stop_sec = atof(strtok(NULL, separator));
		
		string = strtok(NULL, newline);
		dimension = strlen(string)+1;		
		
		(*strings)[lineId] = (char*)malloc(dimension*sizeof(char));
		 memcpy((*strings)[lineId], string, dimension);
	}
	fclose(file);
	return lineId;	
}



int usage(int argc, char *const argv[])
{
	fprintf(stderr,
			"Usage: %s INPUT OUTPUT --timeline=PATH --description=DESCR\n" \
			"                                                                   \n" \
			"   -d, --dataset=PATH                                              \n" \
			"                   Read dataset from INPUT text file and add it    \n" \
			"                   into OUTPUT pinocchIO file at internal PATH.    \n" \
			"                   Timeline provided by option --timeline is used. \n" \
			"                                                                   \n" \
			"       --char      Data is stored as char array.                   \n" \
			"       --int       Data is stored as int array.                    \n" \
			"       --float     Data is stored as float array.                  \n" \
			"       --double    Data is stored as double array.                 \n" \
			"   -n, --dimension=D                                               \n" \
			"                   Set array dimension. Default is 1               \n" \
			"                                                                   \n" \
			"       --string    Data is store as string.                        \n" \
			"                                                                   \n" \
			"   -t, --timeline=PATH                                             \n" \
			"                   If timeline at PATH does not exist:             \n" \
			"						Read timeline from INPUT text file and add  \n" \
			"						it into OUTPUT pinocchIO file at PATH.      \n" \
			"					If it exists:                                   \n" \
			"						Timeline in OUTPUT pinocchIO file at PATH is\n" \
			"                       used as timeline for new dataset.           \n" \
			"                                                                   \n" \
			"   -p, --precision=SCALE                                           \n" \
			"                   Set precision used for new timeline.            \n" \
			"                   Default is 1000 (1 ms precision).               \n" \
			"                                                                   \n" \
			"   -D, --description=\"DESCRIPTION\"                               \n" \
			"                   Dataset/timeline description                    \n" \
			"                                                                   \n" \
			"                                                                   \n" \
			" Timeline format - one segment per line: TIMERANGE                 \n" \
			" Data format - one descriptor per line: TIMERANGE DATA             \n" \
			" TIMERANGE equals 'start stop' (with option --time-read)           \n" \
			"           is empty            (with option --time-copy)           \n" \
			" DATA equals 'x1 x2 ... xD' (with option --char/int/float/double)  \n" \
			"      equals 'string'       (with option --string)                 \n",
			argv[0]);
	return 1;	
}

static int string_flag = 0;    	
static PIOBaseType basetype = PINOCCHIO_TYPE_DOUBLE; 

int main (int argc, char *const  argv[])
{	
	int c;
	
	// Command line arguments
	char* in_ascii = NULL;
	char* out_pIO = NULL;
	
	char* path2dataset = NULL;
	char* path2timeline = NULL;
	int precision = 1000;
	char* dataset_description = NULL;
	char* timeline_description = NULL;
	int dimension = 1;
	//PIOBaseType basetype = PINOCCHIO_TYPE_DOUBLE; 
	
	PIOFile pioFile = PIOFileInvalid;
	PIOTimeRange* timeline = NULL;
	int ntimeranges = -1;
	int lineId = -1;
	PIOTimeline pioTimeline = PIOTimelineInvalid;
	
	PIODatatype pioDatatype = PIODatatypeInvalid;
	PIODataset pioDataset = PIODatasetInvalid;
	
	double* arrayBuffer = NULL;
	char** strings = NULL;
    
	while (1)
	{
		static struct option long_options[] =
		{
			// path to dataset
			{"dataset",     required_argument, 0, 'd'},
			// datatype
			{"char",        no_argument,       (int*) &basetype, PINOCCHIO_TYPE_CHAR},
			{"int",         no_argument,       (int*) &basetype, PINOCCHIO_TYPE_INT},
			{"float",       no_argument,       (int*) &basetype, PINOCCHIO_TYPE_FLOAT},
			{"double",      no_argument,       (int*) &basetype, PINOCCHIO_TYPE_DOUBLE},			
			{"string",      no_argument,       &string_flag, 1},
			{"dimension",   required_argument, 0, 'n'},

			// path to timeline
            {"timeline",    required_argument, 0, 't'},
			// precision of timeline
			{"precision",   required_argument, 0, 'p'},
			
			// description of timeline/dataset
			{"description", required_argument, 0, 'D'},
			
			{"format",      required_argument, 0, 'f'},
			
			{0, 0, 0, 0}
		};
		
		/* getopt_long stores the option index here. */
		int option_index = 0;
		
		c = getopt_long (argc, argv, "d:n:t:p:D:", long_options, &option_index);
		
		/* Detect the end of the options. */
		if (c == -1) break;
		
		switch (c)
		{
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0)
					break;
				/* This is an option with only a long name */
				if (optarg)
				{
				} else {
				}
				break;
			case 'D':
				dataset_description = optarg;
				timeline_description = optarg;
				break;
			case 't':
				path2timeline = optarg;
				break;
			case 'p':
				precision = atoi(optarg);
				break;
			case 'd':
				path2dataset = optarg;
				break;
			case 'n':
				dimension = atoi(optarg);
				break;
			case '?':
				/* getopt_long already printed an error message. */
				usage(argc, argv);
				exit(-1);
				break;
			default:
				abort ();
		}
	}
	
	if (optind+2>argc)
	{
		usage(argc, argv);
		exit(-1);
	}
    
	in_ascii = argv[optind];
	optind++;
	out_pIO = argv[optind];	
	
	if (!path2timeline) 
	{
		fprintf(stderr, "Missing path to timeline.\n");
		fflush(stderr);
		exit(-1);
	}
	
	if (!dataset_description)
	{
		fprintf(stderr, "Missing dataset/timeline description.\n");
		
		exit(-1);
	}
	
	// count number of lines in ascii file
	ntimeranges = getNumberOfLines(in_ascii);
	if (ntimeranges < 0) 
	{
		fprintf(stderr, "Cannot read file %s.\n", in_ascii);
		fflush(stderr);
		exit(-1);
	}
	if (ntimeranges == 0)
	{
		fprintf(stderr, "Input file %s is empty.\n", in_ascii);
		fflush(stderr);
		exit(-1);
	}
	
	// open pinocchIO file
	pioFile = pioOpenFile(out_pIO, PINOCCHIO_READNWRITE);
	if (PIOFileIsInvalid(pioFile))
	{
		fprintf(stderr, "Cannot open file %s.\n", out_pIO);
		fflush(stderr);
		exit(-1);
	}
	
	// tries to load timeline
	pioTimeline = pioOpenTimeline(PIOMakeObject(pioFile), path2timeline);
	if (PIOTimelineIsInvalid(pioTimeline))
	{
		// timeline probably does not exist --> create it
		timeline = (PIOTimeRange*) malloc(ntimeranges*sizeof(PIOTimeRange));
		readTimeline( in_ascii, ntimeranges, timeline, precision);
		pioTimeline = pioNewTimeline(pioFile, path2timeline, timeline_description, ntimeranges, timeline);
		free(timeline);
		if (PIOTimelineIsInvalid(pioTimeline))
		{
			fprintf(stderr, "Cannot create timeline %s from file %s.\n", path2timeline, in_ascii);
			fflush(stderr);
			pioCloseFile(&pioFile);
			exit(-1);
		}
	}

	// check length of timeline
	if (pioTimeline.ntimeranges != ntimeranges)
	{
		fprintf(stderr,
				"Number of timeranges in timeline %s do not match number of lines in file %s.\n", 
				pioTimeline.path, in_ascii);
		fflush(stderr);
		pioCloseTimeline(&pioTimeline);
		pioCloseFile(&pioFile);
		exit(-1);
	}
	
	
	if (path2dataset)
	{
		if (string_flag) 
		{
			basetype = PINOCCHIO_TYPE_CHAR;
			dimension = 1;
		}
		
		pioDatatype = pioNewDatatype(basetype, dimension);
		if (PIODatatypeIsInvalid(pioDatatype))
		{
			pioCloseTimeline(&pioTimeline);
			pioCloseFile(&pioFile);
			fprintf(stderr, "Cannot create datatype.\n");
			fflush(stderr);
			exit(-1);
		}
		
		pioDataset = pioNewDataset(pioFile, path2dataset, dataset_description, pioTimeline, pioDatatype);
		if (PIODatasetIsInvalid(pioDataset))
		{
			pioCloseDatatype(&pioDatatype);
			pioCloseTimeline(&pioTimeline);
			pioCloseFile(&pioFile);
			fprintf(stderr, "Cannot create dataset %s.\n", path2dataset);
			fflush(stderr);
			exit(-1);
		}
	
		pioCloseDatatype(&pioDatatype);
		pioCloseTimeline(&pioTimeline);
		
		// load dataset
		if (string_flag)
		{
			strings = (char**) malloc(ntimeranges*sizeof(char*));
			readStringDataset(in_ascii, ntimeranges, &strings);
			
			for (lineId=0; lineId<ntimeranges; lineId++) {
				fprintf(stdout, "%s\n", strings[lineId]);
			}
			
		}
		else
		{
			arrayBuffer = (double*) malloc(dimension*ntimeranges*sizeof(double));
			readArrayDataset(in_ascii, ntimeranges, dimension, arrayBuffer);
		}
		
		if (string_flag)
			pioDatatype = pioNewDatatype(PINOCCHIO_TYPE_CHAR, 1);
		else 
			pioDatatype = pioNewDatatype(PINOCCHIO_TYPE_DOUBLE, dimension);

		if (string_flag)
			for (lineId=0; lineId<ntimeranges; lineId++)
				pioWrite(&pioDataset, lineId, strings[lineId], strlen(strings[lineId]), pioDatatype);
		else 
			for (lineId=0; lineId<ntimeranges; lineId++)
				pioWrite(&pioDataset, lineId, arrayBuffer+dimension*lineId, 1, pioDatatype);

		if (string_flag)
		{
			for (lineId=0; lineId<ntimeranges; lineId++) { free(strings[lineId]); strings[lineId] = NULL; }
			free(strings); strings = NULL;
		}
		else
			free(arrayBuffer); arrayBuffer = NULL;

		pioCloseDatatype(&pioDatatype);
		pioCloseDataset(&pioDataset);
				
	}
	
	pioCloseFile(&pioFile);
	return 1;
}
