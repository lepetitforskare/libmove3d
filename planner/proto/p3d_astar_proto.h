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

extern int p3d_graph_search ( p3d_graph *graph, double (*fct_heurist)(p3d_node *, p3d_node *, p3d_graph *), int (*fct_valid)(p3d_node *, p3d_edge *, p3d_graph *), int (*fct_end)(p3d_node *, p3d_node *) );
extern int p3d_OrderingGraphSearch(
  p3d_graph *graph, 
  double (*fct_heurist)(p3d_node *, p3d_node *, p3d_graph *),
  int (*fct_valid)(p3d_node *, p3d_edge *, p3d_graph *),
  int (*fct_end)(p3d_node *, p3d_node *));
extern int p3d_valid ( p3d_node * nodePt, p3d_edge * edgePt, p3d_graph * graphPt );
extern int p3d_valid_multisol(p3d_node * nodePt, p3d_edge * edgePt, p3d_graph * graphPt);
extern int p3d_end ( p3d_node *n1, p3d_node *n2 );
extern double p3d_heurist ( p3d_node *n1, p3d_node *n2, p3d_graph *G );

//start path deform
extern int p3d_graph_many_search(p3d_graph* graph, 
				 double(*fct_heurist)(p3d_node*, p3d_node*, p3d_graph*),
				 int(*fct_valid)(p3d_node*, p3d_edge*, p3d_graph*),
				 int (*fct_end)(p3d_node *, p3d_node *), 
				 p3d_traj* traj);

extern int p3d_graph_new_search(p3d_graph *graph, 
				double (*fct_heurist)(p3d_node*, p3d_node*, p3d_graph*),
				int (*fct_valid)(p3d_node *, p3d_edge *, p3d_graph *), 
				int (*fct_end)(p3d_node *, p3d_node *));
extern p3d_path_nodes*  p3d_allocinit_path_nodes(void);
extern void p3d_destroy_path_nodes(p3d_path_nodes* path_nodes);
extern int IsNodeInPath(p3d_node* nestNodePt, p3d_node* N);

//end path deform
#endif /* __CEXTRACT__ */
