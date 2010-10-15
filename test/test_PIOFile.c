/*
 *  test_PIOFile.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 14/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

#include <stdlib.h>
#include "pinocchIO/pinocchIO.h"

int main (int argc, char *const  argv[])
{	
	PIOFile pioFile = pioNewFile("/tmp/test.pio", "/path/to/medium");
	if (PIOObjectIsInvalid(pioFile))
	{
		fprintf(stderr, "Could not create test pinocchIO file.\n");
		exit(-1);
	}
	pioCloseFile(pioFile);

	pioFile = pioOpenFile("/tmp/test.pio", PINOCCHIO_READONLY);
	if (PIOObjectIsInvalid(pioFile))
	{
		fprintf(stderr, "Could not reopen just created test pinocchIO file.\n");
		exit(-1);
	}
	fprintf(stdout, "%s\n", pioFile.medium);
	pioCloseFile(pioFile);
	
	
	
	return 1;
}