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
/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern p3d_node* p3d_APInode_shoot ( p3d_graph *graphPt );
extern p3d_node** p3d_APInode_shoot_multisol(p3d_graph *graphPt, int* nbNodes);
extern p3d_node* p3d_APInode_shoot_nocolltest ( p3d_graph *graphPt );
extern p3d_node* p3d_APInode_make ( p3d_graph *graphPt, configPt q );
extern p3d_node* p3d_APInode_make_multisol ( p3d_graph *graphPt, configPt q, int *iksol );
extern int p3d_APInode_shoot_singularity(p3d_rob *rob, configPt* q, int *singNum, int *cntrtNum, configPt rootConfig, int * rootIkSol);
extern void p3d_APInode_shoot_normal(p3d_graph *graphPt, configPt* q, int shootPassive);
extern void p3d_APInode_desalloc ( p3d_graph *graphPt, p3d_node *nodePt );
extern int p3d_APInode_linked ( p3d_graph *graphPt, p3d_node *N1, p3d_node *N2, double *dist );
extern int p3d_APInode_linked_multisol ( p3d_graph *graphPt, p3d_node *N1, p3d_node *N2, double *dist );
extern double p3d_APInode_dist ( p3d_graph *graphPt, p3d_node *N1, p3d_node *N2 );
extern double p3d_APInode_dist_multisol ( p3d_graph *graphPt, p3d_node *N1, p3d_node *N2 );
extern int p3d_APInode_expand ( p3d_graph *graphPt, p3d_node *N, int (*fct_stop)(void), void (*fct_draw)(void) );
extern p3d_traj *p3d_graph_to_traj ( p3d_rob *robotPt );
extern void p3dAddTrajToGraph(p3d_rob* robot, p3d_graph* graph, p3d_traj* traj);
#endif /* __CEXTRACT__ */
