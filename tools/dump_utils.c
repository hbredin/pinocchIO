
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
            void* buffer, PIODatatype bufferDatatype, 
            int numberOfEntriesInBuffer, int dumpMultipleEntriesIntoOneLine,
            int dumpLabel,     int* labels, int numberOfLabels,
            int dumpTimerange, PIOTimeRange timerange,
            int useSVMLightFormat)
{
    int n;
    int i;
    
    if ((bufferDatatype.type == PINOCCHIO_TYPE_CHAR) && (bufferDatatype.dimension == 1))
    {
        if (dumpTimerange) timestamp_dump(file, timerange);
        
        text_dump(file, buffer, numberOfEntriesInBuffer);
    }
    else 
    {
        if (dumpMultipleEntriesIntoOneLine)
        {
            if (dumpTimerange) timestamp_dump(file, timerange);
            if (dumpLabel)
            {
                if (numberOfLabels > 0)
                {
                    fprintf(file, "%d", labels[0]);
                    for (i=1; i<numberOfLabels; i++) fprintf(file, "/%d", labels[i]);
                }
                else fprintf(file, "-1");
                fprintf(file, " ");
            }
            
            for (n=0; n<numberOfEntriesInBuffer; n++) 
                one_array_dump(file, buffer, bufferDatatype, n, useSVMLightFormat);
            fprintf(file, "\n");
        }
        else 
        {
            for (n=0; n<numberOfEntriesInBuffer; n++) 
            {
                if (dumpTimerange) timestamp_dump(file, timerange);
                
                if (dumpLabel)
                {
                    if (useSVMLightFormat)
                    {
                        if (numberOfLabels > 0)
                            fprintf(file, "%d", labels[0]);
                        else fprintf(file, "-1");
                    }
                    else 
                    {
                        if (numberOfLabels > 0)
                        {
                            fprintf(file, "%d", labels[0]);
                            for (i=1; i<numberOfLabels; i++) fprintf(file, "/%d", labels[i]);
                        }
                        
                        else fprintf(file, "-1");
                    }
                    fprintf(file, " ");
                }

                one_array_dump(file, buffer, bufferDatatype, n, useSVMLightFormat);
                fprintf(file, "\n");
            }
        }
    }
    
    return numberOfEntriesInBuffer;
}

