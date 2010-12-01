/*
 *  gpt2svmlight.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 22/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gepetto.h"

static int verbose_flag = 0;
static int string_flag = 0;
static int label_flag = 0;
static int svmlight_flag = 0;

void usage(const char * path2tool)
{
	fprintf(stdout, 
			"USAGE: %s [options] FILE\n", path2tool);
    fprintf(stdout, 
            "           --svmlight\n"
            "           Use SVMlight format\n"
            "       -L, --label\n"
            "           Show label\n"
			"           --text\n"
			"           Display char as text\n");
	fflush(stdout);
}

int main (int argc, char *const  argv[])
{	
	char* gepetto_configuration_file = NULL;

	void* buffer = NULL; // data buffer
	
	int* buffer_int;
	float* buffer_float;
	double* buffer_double;
	char* buffer_char;
	char* string = NULL;

    int numberOfVectors; // number of vector for each timerange
	int n; // vector counter
    int dimension;
	int d; // dimension counter
    int label;
    
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
            {"svmlight", no_argument, &svmlight_flag, 1},
            {"label",    no_argument, 0,              'L'},
			{"text",     no_argument, &string_flag,   1},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		
//		c = getopt_long (argc, argv, "hs",
//						 long_options, &option_index);
		c = getopt_long (argc, argv, "hL",
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
				
//			case 's':
//				timestamp_flag = 1;
//				break;
                
            case 'L':
                label_flag = 1;
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
	
    if (svmlight_flag) label_flag = 1;
    if (svmlight_flag && string_flag)
    {
        fprintf(stderr, "Incompatible options (svmlight and text)\n");
        fflush(stderr);
        usage(argv[0]);
        exit(-1);
    }
    
	if (optind+1>argc)
	{
		fprintf(stderr, "Missing path to gepetto configuration file.\n");
		fflush(stderr); 
		usage(argv[0]);
		exit(-1);		
	}
	gepetto_configuration_file = argv[optind];

    GPTServer gptServer = gptNewServerFromConfigurationFile(gepetto_configuration_file);
    if (GPTServerIsInvalid(gptServer))
    {
        fprintf(stderr, "Could not initialize Gepetto server from configuration file %s.\n",
                gepetto_configuration_file);
        fflush(stderr);
        exit(-1);
    }
    
    dimension = gptGetServerDimension(gptServer);
    
    numberOfVectors = gptReadNextData(&gptServer, gptServer.datatype, &buffer, &label);
    while (gptServer.eof == 0) 
    {
        if (numberOfVectors > 0)
        {
            buffer_int = (int*)buffer;
            buffer_float = (float*)buffer;
            buffer_double = (double*)buffer;
            buffer_char = (char*)buffer;			            
            
            if (label_flag)
                fprintf(stdout, "%d ", label);
            
            if (string_flag & 
                (gptServer.datatype.type==PINOCCHIO_TYPE_CHAR) & 
                (dimension == 1))
            {            
                string = (char*) malloc((numberOfVectors+1)*sizeof(char*));
                memcpy(string, buffer_char, numberOfVectors);
                string[numberOfVectors] = '\0';
                fprintf(stdout, "%s\n", string);
                free(string);
            }
            else
            {
                for (n=0; n<numberOfVectors; n++)
                {				                
                    for (d=0; d<dimension; d++)
                    {					
                        switch (gptServer.datatype.type) {
                            case PINOCCHIO_TYPE_INT:
                                if (svmlight_flag && (buffer_int[n*dimension+d]))
                                    fprintf(stdout, "%d:%d ", d, buffer_int[n*dimension+d]);
                                else
                                    if (!svmlight_flag) fprintf(stdout, "%d ", buffer_int[n*dimension+d]);
                                break;
                            case PINOCCHIO_TYPE_FLOAT:
                                if (svmlight_flag && (buffer_float[n*dimension+d]))
                                    fprintf(stdout, "%d:%f ", d, buffer_float[n*dimension+d]);
                                else
                                    if (!svmlight_flag) fprintf(stdout, "%f ", buffer_float[n*dimension+d]);
                                break;
                            case PINOCCHIO_TYPE_DOUBLE:
                                if (svmlight_flag && (buffer_double[n*dimension+d]))
                                    fprintf(stdout, "%d:%lf ", d, buffer_double[n*dimension+d]);
                                else
                                    if (!svmlight_flag) fprintf(stdout, "%lf ", buffer_double[n*dimension+d]);
                                break;
                            case PINOCCHIO_TYPE_CHAR:
                                if (svmlight_flag && (buffer_char[n*dimension+d]))
                                    fprintf(stdout, "%d:%c ", d, buffer_char[n*dimension+d]);
                                else
                                    if (!svmlight_flag) fprintf(stdout, "%c ", buffer_char[n*dimension+d]);                            
                                break;
                            default:
                                break;
                        }
                    }
                    fprintf(stdout, "\n");
                }
            }
        }
        numberOfVectors = gptReadNextData(&gptServer, gptServer.datatype, &buffer, &label);        
    }
    
    fflush(stdout);
    
    gptCloseServer(&gptServer);

	return 1;
}
