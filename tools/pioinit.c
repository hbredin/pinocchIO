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
 \page pioinit pioinit
 
 \a pioinit creates an empty pinocchIO file for a given medium.
 
 \section usage Usage 
\verbatim
 $ pioinit /path/to/medium /path/to/pinocchIO/file
\endverbatim 
 \section example Example
\verbatim
 $ pioinit /my/favorite/movie.avi /data/movie.pio
 $ piols /data/movie.pio
 == Medium ==
 /my/favorite/movie.avi
 == 0 timeline(s) ==
 == 0 dataset(s) ==
\endverbatim
 */

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pinocchIO/pinocchIO.h"

static int verbose_flag = 0;

void usage(const char * path2tool)
{
	fprintf(stdout, 
			"USAGE: %s MEDIUM OUTPUT\n", path2tool);
	fflush(stdout);
}

int main (int argc, char *const  argv[])
{	
	char* medium_file = NULL;
	char* output_file = NULL;
    
    PIOFile pioOutputFile = PIOFileInvalid;
    
	int c;
	while (1)
	{
		static struct option long_options[] =
		{
			/* These options set a flag. */
			{"verbose", no_argument, &verbose_flag, 1},
			{"brief",   no_argument, &verbose_flag, 0},
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
				                
			case '?':
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
        
	medium_file = argv[optind];
    optind++;
    output_file = argv[optind];
	

	pioOutputFile = pioNewFile(output_file, medium_file);
	if (PIOFileIsInvalid(pioOutputFile))
	{
		fprintf(stderr, "Cannot create file %s.\n", output_file);
		fflush(stderr);
		exit(-1);
	}
    
    pioCloseFile(&pioOutputFile);
        
	return 1;
}
