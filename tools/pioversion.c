/*
 *  pioversion.c
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 22/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

/*!
 @header pinocchIO version
 @abstract   
 @discussion 
 */

#include <stdlib.h>
#include <stdio.h>
#include "pIOVersion.h"

int main (int argc, char *const  argv[])
{	
  fprintf(stdout, "pinocchIO %s (%s)\n", PINOCCHIO_VERSION, PINOCCHIO_RELEASE_DATE);
  fflush(stdout);
  return 1;
}
