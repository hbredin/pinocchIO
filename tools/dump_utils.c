
#include <stdlib.h>
#include <string.h>
#include "pinocchIO.h"

int text_dump(FILE* file, 
              void* buffer,
              int numberOfEntries)
{
    char* string = NULL;
    char* buffer_char = NULL;
    
    buffer_char = (char*)buffer;
    
    string = (char*) malloc((numberOfEntries+1)*sizeof(char*));
    memcpy(string, buffer_char, numberOfEntries);
    string[numberOfEntries] = '\0';
    
    fprintf(file, "%s", string);
    free(string);
    
    return numberOfEntries;
}

int timestamp_dump(FILE* file, PIOTimeRange timerange)
{
    fprintf(file, "%lf %lf ", 
            (double)(1.*timerange.time)/timerange.scale,
            (double)(1.*timerange.time+timerange.duration)/timerange.scale);									                    
    return 1;
}

int one_array_dump(FILE* file, 
                   void* buffer, PIODatatype datatype, int entryIndex,
                   int svmlight)
{
    int* buffer_int;
    float* buffer_float;
    double* buffer_double;
    char* buffer_char;
    int dimension;
    int d;
    
    buffer_int = (int*)buffer;
    buffer_float = (float*)buffer;
    buffer_double = (double*)buffer;
    buffer_char = (char*)buffer;		
    dimension = datatype.dimension;
    
    
    for (d=0; d<dimension; d++)
    {					
        switch (datatype.type)
        {
            case PINOCCHIO_TYPE_INT:
                if (svmlight && (buffer_int[entryIndex*dimension+d]))
                    fprintf(file, "%d:%d ", d, buffer_int[entryIndex*dimension+d]);
                else
                    if (!svmlight) fprintf(file, "%d ", buffer_int[entryIndex*dimension+d]);
                break;
            case PINOCCHIO_TYPE_FLOAT:
                if (svmlight && (buffer_float[entryIndex*dimension+d]))
                    fprintf(file, "%d:%f ", d, buffer_float[entryIndex*dimension+d]);
                else
                    if (!svmlight) fprintf(file, "%f ", buffer_float[entryIndex*dimension+d]);
                break;
            case PINOCCHIO_TYPE_DOUBLE:
                if (svmlight && (buffer_double[entryIndex*dimension+d]))
                    fprintf(file, "%d:%lf ", d, buffer_double[entryIndex*dimension+d]);
                else
                    if (!svmlight) fprintf(file, "%lf ", buffer_double[entryIndex*dimension+d]);
                break;
            case PINOCCHIO_TYPE_CHAR:
                if (svmlight && (buffer_char[entryIndex*dimension+d]))
                    fprintf(file, "%d:%c ", d, buffer_char[entryIndex*dimension+d]);
                else
                    if (!svmlight) fprintf(file, "%c ", buffer_char[entryIndex*dimension+d]);                            
                break;
            default:
                break;                                
        }
    }
    
    return 1;
}
                

int ez_dump(FILE* file,
            void* buffer, PIODatatype datatype, int numberOfEntries,
            int* labels, int numberOfLabels,
            PIOTimeRange timerange,
            int multiple,
            int label,
            int timestamp,
            int svmlight)
{
    int n;
    
    if ((datatype.type == PINOCCHIO_TYPE_CHAR) && (datatype.dimension == 1))
    {
        if (timestamp) timestamp_dump(file, timerange);
        
        text_dump(file, buffer, numberOfEntries);
    }
    else 
    {
        if (multiple)
        {
            if (timestamp) timestamp_dump(file, timerange);
            for (n=0; n<numberOfEntries; n++) 
                one_array_dump(file, buffer, datatype, n, svmlight);
            fprintf(file, "\n");
        }
        else 
        {
            for (n=0; n<numberOfEntries; n++) 
            {
                if (timestamp) timestamp_dump(file, timerange);
                if (svmlight) 
                {
                    if ((label) && (numberOfLabels > 0))
                        fprintf(file, "%d ", labels[0]);
                    else
                        fprintf(file, "%d ", -1);
                }

                one_array_dump(file, buffer, datatype, n, svmlight);
                fprintf(file, "\n");
            }
        }
    }
    
    return numberOfEntries;
}