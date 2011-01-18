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
 \page gptstat gptstat
 
 \section usage Usage 
 \section example Example
 */


#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gepetto.h"

void usage(const char * path2tool)
{
	fprintf(stdout, 
			"USAGE: %s FILE\n", path2tool);
	fflush(stdout);
}

int main (int argc, char *const  argv[])
{	
    int c;
	char* gepetto_configuration_file = NULL;
        
	while (1)
	{
		static struct option long_options[] =
		{
			/* These options set a flag. */
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		
		c = getopt_long (argc, argv, "",
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
		fprintf(stderr, 
                "Missing path to gepetto configuration file.\n");
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

    gptPrintStatistics(gptServer, stdout);
    fflush(stdout);
    
    gptCloseServer(&gptServer);
    
	return 1;
}
