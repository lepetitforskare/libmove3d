/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern p3d_graph * p3d_allocinit_graph ( void );
extern p3d_compco * p3d_create_void_compco ( p3d_graph * G );
extern p3d_node * p3d_allocinit_node ( void );
extern void p3d_create_compco ( p3d_graph *G, p3d_node *N );
extern int p3d_del_graph ( p3d_graph *G );
extern void p3d_remove_compco ( p3d_graph * G, p3d_compco * CDel );
extern void p3d_insert_node_in_graph ( p3d_graph *G, p3d_node *nodePt );
extern p3d_list_node * p3d_add_node_to_list ( p3d_node * N, p3d_list_node * TargetList );
extern void p3d_add_node_compco ( p3d_node * N, p3d_compco * C );
extern int p3d_compco_linked_to_compco ( p3d_compco * Source, p3d_compco * Target );
extern p3d_list_compco * p3d_add_list_compco ( p3d_list_compco * CLTarget, p3d_compco * CAdd );
extern void p3d_add_compco_to_reachable_list ( p3d_compco * Target, p3d_compco * Add );
extern void p3d_add_compco_to_reachable_list_and_update_predecessors ( p3d_graph * G, p3d_compco * CTarget, p3d_compco * CAdd );
extern void p3d_del_compco_from_reachable_list ( p3d_compco * CTarget, p3d_compco * CDel );
extern p3d_list_node * p3d_add_node_to_list_node_nof_connex_order ( p3d_node * NAdd, p3d_list_node * NLTarget );
extern void p3d_add_node_neighbour ( p3d_graph *G, p3d_node *N1, p3d_node *N2 );
extern void p3d_create_one_edge ( p3d_graph *G, p3d_node *Ni, p3d_node *Nf, double dist );
extern void p3d_create_edges ( p3d_graph *G, p3d_node *N1, p3d_node *N2, double dist );
extern void p3d_order_node_list ( p3d_list_node * NodeList );
/**
TODO
*/
extern double p3d_getEdgeCost(p3d_edge* edge);
/**
 * p3d_TestConfInGraph
 * try to find in the graph if there 
 * is a node with the same configuration
 * return the node if it exists
 * @param[In]: GraphPt: pointer to the robot graph.
 * @param[In]: Config: configuration for which we try to 
 * find a corresponding node
 * @return: The corresponding node if it exists, NULL otherwise.
 */
p3d_node* p3d_TestConfInGraph(p3d_graph* GraphPt, configPt Config);

/**
 * p3d_InitRun
 * Init some variables for a motion planning problem
 * (diffusion and PRM technics)
 * @param[In] GraphPt: Pointer on th robot graph
 * @param[In] Ns: The starting node of the problem
 * @param[In] Ng: The final node of the problem
 */
void p3d_InitRun(p3d_graph* GraphPt, p3d_node* Ns, p3d_node* Ng);




void p3d_reinit_array_exhausted_nodes(void);
void  p3d_identify_exhausted_nodes(void);
void p3d_reinit_array_farther_nodes(void);
void  p3d_identify_farther_nodes(void);
//path deformation functions
extern void p3d_clear_comp(p3d_graph *G, p3d_compco* comp);
extern void p3d_compute_2D_Cspace_proj(p3d_rob* robotPt, int dof1, int dof2, int nstep);
extern int p3d_is_node_useful_for_cycle(p3d_node* nodePt, p3d_node** node_to_connect1Pt, p3d_node** node_to_connect2Pt, p3d_graph *graphPt);
extern int p3d_test_reductibility(p3d_graph *G, p3d_node *N, p3d_node * node1Pt, p3d_node * node2Pt);
extern int p3d_is_node_in_list(p3d_list_node* list_nodes, p3d_node* nodePt);
/**
 * p3d_CreateExtremalNodeFromConf
 * Create an extremal node from an extremal configuration
 * Function used to initialize a motion planning problem.
 * This function is only called when a node with the same conf 
 * than the extremal conf has not been found in the graph.
 * Otherwise there is no nead to create a new node.
 * @param[In] GraphPt: Pointer on the Robot graph
 * @param[In] Config: The configuration from wich we want to 
 * create a node 
 * @return: the new created node
 */
extern p3d_node* p3d_CreateExtremalNodeFromConf(p3d_graph* GraphPt, configPt Config);
extern dbl_list* p3d_list_nodes_visible_from_conf(p3d_graph* graphPt, p3d_list_node* nodesPt, configPt q);
extern int p3d_is_visible_connectivity(p3d_graph* graphPt,  dbl_list* list_nodes, p3d_node* nodePt, dbl_list* list_connect_nodes);
extern p3d_node* find_nearest_node(p3d_graph* graphPt, p3d_node* nodePt, dbl_list* list_nodes);
extern int p3d_test_visible_connectivity(p3d_graph* graphPt,  dbl_list* list_nodes, 
                                         p3d_node* nodePt,  dbl_list* list_connect_nodes, int test_faces);
extern int p3d_test_discreet_visibility_edge(p3d_graph* G, configPt q0,
                                             configPt q_edge1,configPt q_edge2);
//end path deform

/**
 * p3d_EvaluateExpandDiffic
 * Function attempting to evaluate the difficulty 
 * to success one process (for exemple node expansion) 
 * during a graph construction
 * @param[In]: CompPt: connect componant for which the 
 * difficulty is considered
 * @param[In]: IsSuccess: The new result of the test.
 * Note: write the information in a plotable file. Start 
 * after nbTests have been done and make an average over 
 * the last nbTests results
 */
void p3d_EvaluateExpandDiffic(p3d_compco* CompPt,  int IsSuccess);

/** 
 * p3d_CreateDenseRoadmap
 * Create a (2^D)*(2^D) grid based graph.
 * It is the possible to find an optimal path
 * inside this grid graph.
 * @param[In] robotPt: the robot
 * @return: the created graph
 * Note that if a graph already exists
 * it does not return anything.
 */
p3d_graph*  p3d_CreateDenseRoadmap(p3d_rob *robotPt);

/** 
 * p3d_ExtractBestTraj
 * Extract the best trajectory from the graph
 * that link the query nodes. Creates a trajectory
 * and print its main cost characteristics
 * @param[In] graphPt: the robot graph
 */
void p3d_ExtractBestTraj(p3d_graph *graphPt);


/*
 * IsSmallDistNodeInGraph
 * TODO
 * @return: TRUE if the there is a commun parent in the graph between 
 * N1 and N2 at a maximal distance of maxLevel for both nodes
 */

int p3d_IsSmallDistInGraph(p3d_graph* G, p3d_node* N1, p3d_node* 
			   N2, int maxLevel, double Step);

#endif /* __CEXTRACT__ */
