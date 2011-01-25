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
 \page cli Command line tools
 
 A set of command line tools is available for basic operations on pinocchIO files
 
 <table>
 <tr>
 <td>\subpage pioversion</td>
 <td>Display installed pinocchIO version</td>
 </tr>    
 <tr>
 <td>\subpage pioinit</td>
 <td>Create an empty pinocchIO file</td>
 </tr>
 <tr>
 <td>\subpage piols</td>
 <td>Get information about a pinocchIO file and its content</td>
 </tr>
 <tr>
 <td>\subpage ascii2pio</td>
 <td>Add a timeline or a dataset to a pinocchIO file, from a text file</td>
 </tr>
 <tr>
 <td>\subpage piodump</td>
 <td>Dump a timeline or a dataset from a pinocchIO file into a file</td>
 </tr>
 <tr>
 <td>\subpage piocp</td>
 <td>Copy a timeline or a dataset from a pinocchIO file to another one</td>
 </tr>
 <tr>
 <td>\subpage piorm</td>
 <td>Remove a timeline or a dataset from a pinocchIO file</td>
 </tr>
 <tr>
 <td>\subpage gptdump</td>
 <td>Dump a Gepetto server into a file</td>
 </tr>
 <tr>
 <td>\subpage gptstat</td>
 <td>Get statistics on a Gepetto server</td>
 </tr>
 </table> 
 */

/**
 \page pioversion pioversion 
 
 \a pioversion displays pinocchIO version and release date to standard output.
 
 \section usage Usage 
\verbatim
 $ pioversion
\endverbatim 
 \section example Example
\verbatim
 $ pioversion
 pinocchIO 0.2.2 (2010-12-01)
\endverbatim 

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
