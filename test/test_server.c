/*
 *  test_server.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 15/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include <stdlib.h>
#include "pinocchIO.h"

int main (int argc, char *const  argv[])
{	
    int d, dimension;
    PIODatatype pioDatatype = PIODatatypeInvalid;
    void* buffer = NULL;
    double* buffer_double = NULL;
    size_t allocation;
    int number;
    PIOServer pioServer;
    
//    PIOServer pioServer = pioNewServerFromConfigurationFile(argv[1]);
//    if (pioServer.nfiles < 1)
//    {
//        fprintf(stderr, "Could not create pinocchIO server from configuration file.\n");
//        exit(-1);
//    }
//
//    dimension = pioGetServerDimension(pioServer);
//    pioDatatype = pioNewDatatype(PINOCCHIO_TYPE_DOUBLE, dimension);
//    
//    while (pioReadNextData(&pioServer, pioDatatype, &buffer) >= 0)
//    {
//        buffer_double = buffer;
////        for (d=0; d<dimension; d++) fprintf(stdout, "%f ", buffer_double[d]);
////        fprintf(stdout, "\n");
//    }
//    
//    pioCloseDatatype(&pioDatatype);
//    pioCloseServer(&pioServer);
    
    pioServer = pioNewServerFromConfigurationFile(argv[1]);
    dimension = pioGetServerDimension(pioServer);
    pioDatatype = pioNewDatatype(PINOCCHIO_TYPE_DOUBLE, dimension);

    allocation = pioDumpServer(&pioServer, pioDatatype, NULL);
    buffer = malloc(allocation);
    number = pioDumpServer(&pioServer, pioDatatype, buffer);
    buffer_double = buffer;
    
    
    fprintf(stdout, "Found %d vectors in %d files\n", number, pioServer.nfiles);
    pioCloseServer(&pioServer);
    
    free(buffer);
    
    
	return 1;
}