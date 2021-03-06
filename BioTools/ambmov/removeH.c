/*
 * Copyright (c) 2001-2014 LAAS/CNRS
 * All rights reserved.
 *
 * Redistribution  and  use  in  source  and binary  forms,  with  or  without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice and this list of conditions.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice and  this list of  conditions in the  documentation and/or
 *      other materials provided with the distribution.
 *
 * THE SOFTWARE  IS PROVIDED "AS IS"  AND THE AUTHOR  DISCLAIMS ALL WARRANTIES
 * WITH  REGARD   TO  THIS  SOFTWARE  INCLUDING  ALL   IMPLIED  WARRANTIES  OF
 * MERCHANTABILITY AND  FITNESS.  IN NO EVENT  SHALL THE AUTHOR  BE LIABLE FOR
 * ANY  SPECIAL, DIRECT,  INDIRECT, OR  CONSEQUENTIAL DAMAGES  OR  ANY DAMAGES
 * WHATSOEVER  RESULTING FROM  LOSS OF  USE, DATA  OR PROFITS,  WHETHER  IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR  OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.                                  
 *
 * Siméon, T., Laumond, J. P., & Lamiraux, F. (2001). 
 * Move3d: A generic platform for path planning. In in 4th Int. Symp.
 * on Assembly and Task Planning.
 */
/**********************************************************************/
// INCLUDES

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "atoms.h"
#include "mathfcts.h"
#include "protein.h"
#include "atomhandling.h"
#include "pdbFormat.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/**********************************************************************/
// EXTERN FUNCTIONS

extern int fill_protein_struct(FILE *pdbfile, char *pdbfilename, protein *protPt);
extern void free_protein(protein *protPt);

extern void write_pdb_without_H_atoms(FILE *pdboutfile, protein *protPt, formatTypes format);

/**********************************************************************/
// GLOBAL VARIABLES

// call arguments
//  - files
static char pdbfilename[255],pdboutfilename[255];
static FILE *pdbfile;

/**********************************************************************/
// GENERAL FUNCTIONS

static int read_call_arguments(int argc, char **argv) {
  
  if(argc < 3) {
    printf("Not enought arguments\n");
    return -1;
  }
  
  if((!strcpy(pdbfilename,argv[1])) ||
     (!strcpy(pdboutfilename,argv[2])))
    return -1;
  
  return 1;
}


/**********************************************************************/
/**********************************************************************/
// MAIN
/**********************************************************************/
/**********************************************************************/

int
removeH (char *pdbfilename)
{
  protein *protPt;
  char pdboutfilename[200];
  char *name;
  FILE * pdboutfile;

  // open files
  pdbfile = fopen(pdbfilename, "r");
  if (pdbfile == NULL) {
    printf("pdbin file cannot be open\n");
    return -1;
  }
  name = strtok(pdbfilename,"." );
  strcpy(pdboutfilename, name);
  strcat(pdboutfilename, "_delH.pdb");
  pdboutfile = fopen(pdboutfilename, "w");
  if (pdboutfile == NULL) {
    printf("pdbout file cannot be open\n");
    fclose(pdbfile);
    return -1;
  }
  
  // alloc protein
  protPt = (protein *) malloc(sizeof(protein));
  
  // read PDB and write protein data structure
  if(fill_protein_struct(pdbfile,pdbfilename,protPt) < 0) {
    printf("ERROR while writing protein data structure from PDB\n");
    free_protein(protPt);  
    fclose(pdbfile);
    fclose(pdboutfile);
    return -1;
  }

  // write pdbout file from protein data structure
  write_pdb_without_H_atoms(pdboutfile, protPt, INSIGHT);

  free_protein(protPt);  
  fclose(pdbfile);
  fclose(pdboutfile);  
  return 1;
}
