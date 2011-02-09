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
 \page piodump piodump
 
 \section usage Usage 
 \section example Example
 */


#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pinocchIO/pinocchIO.h"

char* pinocchio_path = NULL;
char* timeline_path = NULL;
char* dataset_path = NULL;
char* output_path = NULL;

int ascii = 0;
char* format = "%g";
int timestamp = 0;
int multiple = 0;

int binary = 0;
int bin_int = 0;
int bin_float = 0;
int bin_double = 0;

int libsvm = 0;
int fvec = 0;
int ivec = 0;
int bvec = 0;
int string = 0;

void usage(const char * path2tool)
{
	fprintf(stdout, 
			"USAGE: %s [options] FILE\n", path2tool);
	fprintf(stdout, 
			"       -t PATH, --timeline=PATH                                        \n"
			"                Dump timeline at PATH                                  \n"
            "                                                                       \n"
			"       -d PATH, --dataset=PATH                                         \n"
			"                Dump dataset at PATH                                   \n"
            "                                                                       \n"
            "       -o PATH, --output=PATH                                          \n"
            "                Dump into output file                                  \n"
            "                Default behavior is to output to stdout                \n"
            "                                                                       \n"
            " * User-defined formats                                                \n"
            "                                                                       \n"
            "       --ascii                                                         \n"
            "         -f FMT, --format=FMT                                          \n"
            "                Format used by printf (%%d, %%f, %%02d, etc.)          \n"
            "                Default is %s.                                         \n"
            "         --timestamp                                                   \n"
            "                Prepend each line with timestamp                       \n"
            "         --multiple                                                    \n"
            "                In case of multiple vectors per timerange,             \n"
            "                concatenate them all into one single line.             \n"
            "                Default behavior is to output one line per vector.     \n"
            "                                                                       \n"
            "       --binary                                                        \n"
            "         --integer                                                     \n"
            "         --float                                                       \n"
            "         --double                                                      \n"
            "                                                                       \n"
            " * Format presets                                                      \n"
            "                                                                       \n"
            "       --libsvm[=anything]                                             \n"
            "                Use libsvm format                                      \n"
            "                When any value is provided, even empty time ranges are \n"
            "                dumped as one vector made of zeros.                    \n"
            "       --fvec                                                          \n"
            "       --ivec                                                          \n"
            "       --bvec                                                          \n"
            "                Use yael file format (float, integer or bytes)         \n"
            "       --string                                                        \n",
            format
            );
	fflush(stdout);
}

int main (int argc, char *const  argv[])
{	
    
    PIOFile pioFile = PIOFileInvalid;
    PIODataset pioDataset = PIODatasetInvalid;
    PIODatatype pioDatatype = PIODatatypeInvalid;
    PIOTimeline pioTimeline = PIOTimelineInvalid;
    
    int dimension;
    PIOBaseType basetype; 
    FILE* output = stdout;
	
	int c;
	while (1)
	{
		static struct option long_options[] =
		{
			{"timeline",  required_argument, 0, 't'},
			{"dataset",   required_argument, 0, 'd'},
            {"output",    required_argument, 0, 'o'},
            
            {"ascii",     no_argument,       &ascii,     1 },
            {"format",    required_argument, 0,         'f'},
			{"timestamp", no_argument,       &timestamp, 1 },
            {"multiple",  no_argument,       &multiple,  1 },
            
            {"binary",    no_argument, &binary,     1},
            {"integer",   no_argument, &bin_int,    1},
            {"float",     no_argument, &bin_float,  1},
            {"double",    no_argument, &bin_double, 1},
            
            {"libsvm", optional_argument, 0,      'L'},
            {"fvec",   no_argument,       &fvec,   1},
            {"ivec",   no_argument,       &ivec,   1},
            {"bvec",   no_argument,       &bvec,   1},
			{"string", no_argument,       &string, 1},
            
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		
		c = getopt_long (argc, argv, "ht:d:o:f:L::",
						 long_options, &option_index);
		
		/* Detect the end of the options. */
		if (c == -1)
			break;
		
		switch (c)
		{
			case 0:
				if (long_options[option_index].flag != 0)
					break;
				break;
				
			case 't':
				timeline_path = optarg;
				break;
				
			case 'd':
				dataset_path = optarg;
				break;
					
            case 'o':
                output_path = optarg;
                break;
                
            case 'f':
                format = optarg;
                break;
                
            case 'L':
                libsvm = 1;
                if (optarg) libsvm = 2;
                break;
                
			case 'h':
				usage(argv[0]);
				exit(-1);
				break;
				
			default:
				abort ();
		}
	}
	
    if (optind+1<= argc)
        pinocchio_path = argv[optind];
	
    if (!pinocchio_path)
    {
		fprintf(stderr, "Missing path to pinocchIO file.\n"); 
        fflush(stderr); 
		usage(argv[0]);
		exit(-1);		
	} 
    
    if ((!timeline_path && !dataset_path) || (timeline_path && dataset_path))
    {
        fprintf(stderr, "Incompatible --dataset/--timeline options.\n");
        fflush(stderr);
        usage(argv[0]);
        exit(-1);
    }
    
	
	if (dataset_path)
	{
        // default is ascii
        if (!ascii && !binary && !libsvm && !string && !fvec && !ivec && !bvec)
            ascii = 1;
        
        // Prepare output file
        if (output_path)
        {
            if (ascii || libsvm || string)
                output = fopen(output_path, "w");
            
            if (binary || fvec || ivec || bvec)
                output = fopen(output_path, "wb");
            
            if (!output)
            {
                fprintf(stderr, "Cannot open output file %s.\n", output_path);
                fflush(stderr);
                exit(-1);                
            }
        }        
        
        // Get info about dataset

        pioFile = pioOpenFile(pinocchio_path, PINOCCHIO_READONLY);
        if (PIOFileIsInvalid(pioFile))
        {
            fprintf(stderr, "Cannot open pinocchIO file %s.\n", pinocchio_path);
            fflush(stderr);
            fclose(output);
            exit(-1);
        }
        
		pioDataset = pioOpenDataset(PIOMakeObject(pioFile), dataset_path);
		if (PIODatasetIsInvalid(pioDataset)) 
		{
			fprintf(stderr, "Cannot open dataset %s in file %s.\n", dataset_path, pinocchio_path);
			fflush(stderr);
			pioCloseFile(&pioFile);
            fclose(output);
			exit(-1);
		}
		
		pioDatatype = pioGetDatatype(pioDataset);
		if (PIODatatypeIsInvalid(pioDatatype))
		{
			fprintf(stderr, "Cannot get datatype from dataset %s in file %s.\n", dataset_path, pinocchio_path);
			fflush(stderr);
			pioCloseDataset(&pioDataset);
			pioCloseFile(&pioFile);
            fclose(output);
			exit(-1);
		}
		
        dimension = pioDatatype.dimension;
        basetype = pioDatatype.type;
        
        pioCloseDatatype(&pioDatatype);
        
        if (ascii)
        {
            size_t size;
            int nVectors;
            int tr, n, d, N;
            double* buffer;
            PIODatatype outDatatype = PIODatatypeInvalid;
            pioTimeline = PIOTimelineInvalid;
            int* number;
            
            outDatatype = pioNewDatatype(PINOCCHIO_TYPE_DOUBLE, dimension);
            
            size = pioDumpDataset(&pioDataset, outDatatype, NULL, NULL);
            buffer = (double*)malloc(size);
            number = (int*)malloc(pioDataset.ntimeranges*sizeof(int));
            nVectors = pioDumpDataset(&pioDataset, outDatatype, buffer, number);
            
            if (timestamp)
                pioTimeline = pioGetTimeline(pioDataset);
            
            N = 0;
            for (tr=0; tr<pioDataset.ntimeranges; tr++) 
            {
                if (multiple)
                {
                    if (timestamp)
                    {
                        fprintf(output, 
                                "%f %f", 
                                1.0 *  pioTimeline.timeranges[tr].time                                        / pioTimeline.timeranges[tr].scale,
                                1.0 * (pioTimeline.timeranges[tr].time + pioTimeline.timeranges[tr].duration) / pioTimeline.timeranges[tr].scale);
                    }
                    for (n=0; n<number[tr]; n++)
                    {
                        for (d=0; d<dimension; d++)
                        {
                            fprintf(output, " ");
                            fprintf(output, format, buffer[N*dimension+d]);                           
                        }
                        N++;
                    } 
                    fprintf(output, "\n");
                }
                else 
                {
                   for (n=0; n<number[tr]; n++)
                   {
                       if (timestamp)
                       {
                           fprintf(output, 
                                   "%f %f", 
                                   1.0 *  pioTimeline.timeranges[tr].time                                        / pioTimeline.timeranges[tr].scale,
                                   1.0 * (pioTimeline.timeranges[tr].time + pioTimeline.timeranges[tr].duration) / pioTimeline.timeranges[tr].scale);                           
                       }
                       
                       for (d=0; d<dimension; d++)
                       {
                           fprintf(output, " ");
                           fprintf(output, format, buffer[N*dimension+d]);                           
                       }
                       N++;
                       fprintf(output, "\n");
                   } 
                }                
            }
            
            free(buffer);
            free(number);
            if (timestamp)
                pioCloseTimeline(&pioTimeline);
        }
        
        if (string)
        {
            if ((dimension != 1) || (basetype != PINOCCHIO_TYPE_CHAR))
            {
                fprintf(stderr, "Cannot dump dataset as string.\n");
                fflush(stderr);
                pioCloseDataset(&pioDataset);
                pioCloseFile(&pioFile);
                fclose(output);
                exit(-1);
            }

            size_t size;
            int nVectors;
            int tr, n, N;
            char* buffer;
            PIODatatype outDatatype = PIODatatypeInvalid;
            PIOTimeline pioTimeline = PIOTimelineInvalid;
            int* number;
            char onecharacter[2]; onecharacter[1] = '\0';
            
            outDatatype = pioNewDatatype(PINOCCHIO_TYPE_CHAR, dimension);
            
            size = pioDumpDataset(&pioDataset, outDatatype, NULL, NULL);
            buffer = (char*)malloc(size);
            number = (int*)malloc(pioDataset.ntimeranges*sizeof(int));
            nVectors = pioDumpDataset(&pioDataset, outDatatype, buffer, number);
            
            if (timestamp)
                pioTimeline = pioGetTimeline(pioDataset);
            
            N = 0;
            for (tr=0; tr<pioDataset.ntimeranges; tr++) 
            {
                if (timestamp)
                {
                    fprintf(output, 
                            "%f %f ", 
                            1.0 *  pioTimeline.timeranges[tr].time                                        / pioTimeline.timeranges[tr].scale,
                            1.0 * (pioTimeline.timeranges[tr].time + pioTimeline.timeranges[tr].duration) / pioTimeline.timeranges[tr].scale);
                }
                for (n=0; n<number[tr]; n++)
                {
                    memcpy(onecharacter, buffer+N, 1);
                    fprintf(output, "%s", onecharacter);
                    N++;
                } 
                fprintf(output, "\n");
            }
            
            free(buffer);
            free(number);
            if (timestamp)
                pioCloseTimeline(&pioTimeline);
        }
        
        if (libsvm)
        {
            size_t size;
            int nVectors;
            int tr, n, d, N;
            double* buffer;
            int* number;
            PIODatatype outDatatype = PIODatatypeInvalid;
            
            outDatatype = pioNewDatatype(PINOCCHIO_TYPE_DOUBLE, dimension);
            
            size = pioDumpDataset(&pioDataset, outDatatype, NULL, NULL);
            buffer = (double*)malloc(size);
            number = (int*)malloc(pioDataset.ntimeranges*sizeof(int));
            nVectors = pioDumpDataset(&pioDataset, outDatatype, buffer, number);
            
            N = 0;
            for (tr=0; tr<pioDataset.ntimeranges; tr++) 
            {
                if (number[tr] > 0)
                {
                    for (n=0; n<number[tr]; n++)
                    {
                        // label
                        fprintf(output, "-1");
                        
                        // id:value (only when value does not equal 0)
                        for (d=0; d<dimension; d++)
                        {
                            if (buffer[N*dimension+d] != 0)
                            {
                                fprintf(output, " %d:", d);
                                fprintf(output, format, buffer[N*dimension+d]);
                            }
                        }
                        N++;
                        fprintf(output, "\n");
                    } 
                }
                else 
                {
                    // add dummy vector if requested
                    // label 0:0 1:0 2:0 3:0 ... dimension:0
                    if (libsvm == 2) fprintf(output, "-1\n");
                }
            }
                        
            free(number);
            free(buffer);
        }
        
        if (binary)
        {
            size_t size;
            size_t itemSize;
            int nVectors;
            void* buffer;
            PIODatatype outDatatype = PIODatatypeInvalid;
            
            // default is float
            if (!bin_float && !bin_int && !bin_double)
                bin_float = 1;
            
            if (bin_float)
            {
                outDatatype = pioNewDatatype(PINOCCHIO_TYPE_FLOAT, dimension);
                itemSize = sizeof(float);
            }
            if (bin_int)
            {
                outDatatype = pioNewDatatype(PINOCCHIO_TYPE_INT, dimension);
                itemSize = sizeof(int);
            }
            if (bin_double)
            {
                outDatatype = pioNewDatatype(PINOCCHIO_TYPE_DOUBLE, dimension);
                itemSize = sizeof(double);
            }
            
            size = pioDumpDataset(&pioDataset, outDatatype, NULL, NULL);
            buffer = malloc(size);
            nVectors = pioDumpDataset(&pioDataset, outDatatype, buffer, NULL);
            fwrite(buffer, itemSize, nVectors*dimension, output);
            free(buffer);
        }
        
        if (fvec)
        {
            size_t size;
            size_t itemSize;
            size_t dimensionSize;
            int nVectors;
            int n;
            float* buffer;
            PIODatatype outDatatype = PIODatatypeInvalid;
            
            outDatatype = pioNewDatatype(PINOCCHIO_TYPE_FLOAT, dimension);
            itemSize = sizeof(float);
            
            size = pioDumpDataset(&pioDataset, outDatatype, NULL, NULL);
            buffer = (float*)malloc(size);
            nVectors = pioDumpDataset(&pioDataset, outDatatype, buffer, NULL);
            
            dimensionSize = sizeof(int);
            for (n=0; n<nVectors; n++) 
            {
                fwrite(&dimension, dimensionSize, 1, output);
                fwrite(buffer+n*dimension, itemSize, dimension, output);
            }
            free(buffer);
        }
        
        if (ivec)
        {
            size_t size;
            size_t itemSize;
            size_t dimensionSize;
            int nVectors;
            int n;
            int* buffer;
            PIODatatype outDatatype = PIODatatypeInvalid;
            
            outDatatype = pioNewDatatype(PINOCCHIO_TYPE_INT, dimension);
            itemSize = sizeof(int);
            
            size = pioDumpDataset(&pioDataset, outDatatype, NULL, NULL);
            buffer = (int*)malloc(size);
            nVectors = pioDumpDataset(&pioDataset, outDatatype, buffer, NULL);
            
            dimensionSize = sizeof(int);
            for (n=0; n<nVectors; n++) 
            {
                fwrite(&dimension, dimensionSize, 1, output);
                fwrite(buffer+n*dimension, itemSize, dimension, output);
            }
            free(buffer);
        }
        
        if (bvec)
        {
            size_t size;
            size_t itemSize;
            size_t dimensionSize;
            int nVectors;
            int n;
            char* buffer;
            PIODatatype outDatatype = PIODatatypeInvalid;
            
            outDatatype = pioNewDatatype(PINOCCHIO_TYPE_CHAR, dimension);
            itemSize = sizeof(char);
            
            size = pioDumpDataset(&pioDataset, outDatatype, NULL, NULL);
            buffer = (char*)malloc(size);
            nVectors = pioDumpDataset(&pioDataset, outDatatype, buffer, NULL);
            
            dimensionSize = sizeof(int);
            for (n=0; n<nVectors; n++) 
            {
                fwrite(&dimension, dimensionSize, 1, output);
                fwrite(buffer+n*dimension, itemSize, dimension, output);
            }
            free(buffer);
        }
    
		fclose(output);
        pioCloseDataset(&pioDataset);
        pioCloseFile(&pioFile);
	}
	
	if (timeline_path)
	{
        int tr;
        
        // Prepare output file
        
        if (output_path)
        {
            output = fopen(output_path, "w");
            if (!output)
            {
                fprintf(stderr, "Cannot open output file %s.\n", output_path);
                fflush(stderr);
                exit(-1);                
            }
        }        
        
        // Get info about timeline
        
        pioFile = pioOpenFile(pinocchio_path, PINOCCHIO_READONLY);
        if (PIOFileIsInvalid(pioFile))
        {
            fprintf(stderr, "Cannot open pinocchIO file %s.\n", pinocchio_path);
            fflush(stderr);
            fclose(output);
            exit(-1);
        }
        
		pioTimeline = pioOpenTimeline(PIOMakeObject(pioFile), timeline_path);
		if (PIOTimelineIsInvalid(pioTimeline))
		{
			fprintf(stderr, "Cannot open timeline %s in file %s.\n", timeline_path, pinocchio_path);
			fflush(stderr);
			pioCloseFile(&pioFile);
            fclose(output);
			exit(-1);
		}
		
		for (tr=0; tr<pioTimeline.ntimeranges; tr++)
        {
            fprintf(output, 
                    "%f %f\n", 
                    1.0 *  pioTimeline.timeranges[tr].time                                        / pioTimeline.timeranges[tr].scale,
                    1.0 * (pioTimeline.timeranges[tr].time + pioTimeline.timeranges[tr].duration) / pioTimeline.timeranges[tr].scale);
		}
		
        fclose(output);
		pioCloseTimeline(&pioTimeline);
        pioCloseFile(&pioFile);
	}
	
	return 1;
}
