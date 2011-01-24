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
 \page gptdump gptdump
 
 \section usage Usage 
 \section example Example
 */


#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gepetto.h"

extern int ez_dump(FILE* file,
                   void* buffer, PIODatatype bufferDatatype, 
                   int numberOfEntriesInBuffer, int dumpMultipleEntriesIntoOneLine,
                   int dumpLabel, int* labels, int numberOfLabels,
                   int dumpTimerange, PIOTimeRange timerange,
                   int useSVMLightFormat);



static int verbose_flag = 0;
static int multiple_flag = 0;
static int svmlight_flag = 0;
static int label_flag = 0;

int checkArguments()
{
    if (svmlight_flag && multiple_flag)
    {
        fprintf(stderr, 
                "SVMlight format does not support multiple entries per line.\n"); fflush(stderr);
        return 0;
    }
    
    if (svmlight_flag) label_flag = 1;
    
    return 1;
}


void usage(const char * path2tool)
{
	fprintf(stdout, 
			"USAGE: %s [options] FILE\n", path2tool);
	fprintf(stdout, 
            "       --multiple                                                      \n"
            "                Multiple entries per line                              \n"
            "       --label                                                         \n"
            "                Prepend each line with the label when available        \n"
            "       --svmlight                                                      \n"
            "                Use SVMlight format (implies --label)                  \n"
            );
	fflush(stdout);
}



int main (int argc, char *const  argv[])
{	
	char* gepetto_configuration_file = NULL;

	void* buffer = NULL; // data buffer
	
    int numberOfVectors; // number of vector for each timerange

    int nLabels = -1; 
    int* labels = NULL;
    
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
            {"multiple", no_argument, &multiple_flag, 1},
            {"svmlight", no_argument, &svmlight_flag, 1},
            {"label",    no_argument, &label_flag,    1},
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
				break;

			case 'h':
				usage(argv[0]);
				exit(-1);
				break;
				
			case '?':
				usage(argv[0]);
				break;
				
			default:
				abort ();
		}
	}
	
	if (optind+1>argc)
	{
		fprintf(stderr, "Missing path to gepetto configuration file.\n");
		fflush(stderr); 
		usage(argv[0]);
		exit(-1);		
	}
	gepetto_configuration_file = argv[optind];
    
    if (!checkArguments())
    {
        usage(argv[0]);
        exit(-1);
    }
        
    GPTServer gptServer = gptNewServerFromConfigurationFile(gepetto_configuration_file);
    if (GPTServerIsInvalid(gptServer))
    {
        fprintf(stderr, "Could not initialize Gepetto server from configuration file %s.\n",
                gepetto_configuration_file);
        fflush(stderr);
        exit(-1);
    }
            
    while (gptServer.eof == 0) 
    {   
        numberOfVectors = gptReadNext(&gptServer, gptServer.datatype, &buffer, &nLabels, &labels);
        
        if (numberOfVectors >= 0)
        {
            if (label_flag)
            {
                ez_dump(stdout, 
                        buffer, gptServer.datatype, 
                        numberOfVectors, multiple_flag, 
                        label_flag, labels, nLabels, 
                        0, PIOTimeRangeInvalid, 
                        svmlight_flag);
            }
            else 
            {
                ez_dump(stdout, 
                        buffer, gptServer.datatype, 
                        numberOfVectors, multiple_flag, 
                        label_flag, NULL, -1, 
                        0, PIOTimeRangeInvalid,
                        svmlight_flag);            
            }
        }
    }
    
    fflush(stdout);
    
    gptCloseServer(&gptServer);

	return 1;
}
