// 
// Copyright 2010-2011 Herve BREDIN (bredin@limsi.fr)
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
 \page piorm piorm
 
 \a piorm deletes a timeline and/or a dataset from a pinocchIO file.
 
 @note
 In order to delete a timeline, it must not be used by any dataset in the
 pinocchIO file.

 \section usage Usage 
\verbatim
$ piorm [options] /path/to/pinocchIO/file
         -t PATH, --timeline=PATH
                  Remove timeline at PATH
         -d PATH, --dataset=PATH
                  Remove dataset at PATH
\endverbatim
 \section example Example
 - Remove timeline /path/to/timeline
\verbatim
$ piorm -t /path/to/timeline file.pio
\endverbatim
 - Remove dataset /path/to/dataset
\verbatim
$ piorm -d /path/to/dataset file.pio
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
			"USAGE: %s [options] FILE\n", path2tool);
	fprintf(stdout, 
			"       -t PATH, --timeline=PATH\n"
			"           Remove timeline at PATH\n"
			"       -d PATH, --dataset=PATH\n"
			"           Remove dataset at PATH\n");
	fflush(stdout);
}

int main (int argc, char *const  argv[])
{	
	char* pinocchio_file = NULL;
	char* timeline_path = NULL;
	char* dataset_path = NULL;
		
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
	
	if (optind+1>argc)
	{
		fprintf(stderr, "Missing path to pinocchIO file.\n");
		fflush(stderr); 
		usage(argv[0]);
		exit(-1);		
	}
		
	pinocchio_file = argv[optind];
	
	PIOFile pioFile = pioOpenFile(pinocchio_file, PINOCCHIO_READNWRITE);
	if (PIOFileIsInvalid(pioFile))
	{
		fprintf(stderr, "Cannot open pinocchIO file %s with read-n-write access.\n", pinocchio_file);
		fflush(stderr);
		exit(-1);
	}

    if (timeline_path)
    {
        int success = pioRemoveTimeline(PIOMakeObject(pioFile), timeline_path);
        if (!success)
        {
            fprintf(stderr, "Cannot remove timeline %s from file %s.\n", timeline_path, pinocchio_file);
            fflush(stderr);
        }
    }    
    
	if (dataset_path)
	{
        int success = pioRemoveDataset(PIOMakeObject(pioFile), dataset_path);
        if (!success)
        {
            fprintf(stderr, "Cannot remove dataset %s from file %s.\n", dataset_path, pinocchio_file);
            fflush(stderr);
        }
    }
     
    
    pioCloseFile(&pioFile);
    
	return 1;
}
