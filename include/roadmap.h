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
#ifndef _ROADMAP_H
#define _ROADMAP_H

#include "stat.h" // Statistic module; Commit Jim; date: 01/10/2008
#include <vector>

/*! \struct node
\brief Structure de noeud du graphe
\brief (pour la planification globale) */

typedef struct node {
  int         type; /* type du noeud (isole,...) */
  int         num; /* numero du noeud dans le graphe */
  int         numcomp; /* numero de la composante connexe du noeud */
  struct compco *comp;
  configPt q; /* la configuration du robot */
  double* coldeg_q; /*configuration in the space of the collective degrees */ // Modif Juan
  double      dq; /* le rayon de la boule */
  int *iksol;  /* index of the solution generated by the cntrt (modif Juan) */
  double dqmin;
  int nneighb;    /* nombre de voisins dans le graphe */
  struct list_node *neighb; /* voisins du noeud dans le graphe */
  struct list_node *last_neighb;
  int nedge;    /* nombre d'arete partant du noeud dans le graphe */
  struct list_edge *edges; /* aretes reliant le noeud a ses voisins */
  struct list_edge *last_edge; /* aretes reliant le noeud a ses voisins */
  double dist_Nnew; /* distance au noeud courant cree */
  /* for graph exploration Astar*/
  double f;                /*g+h*/
  double g;                /*Real cost from the init node*/
  double h;                /*heuristic cost to goal node*/
  short opened;            /* whether node is open in A* search */
  short closed;            /* whether node is closed in A* search */
  struct node *search_from; /* parent node in A* search */
  struct node *search_to;  /* child node in A* search */
  struct edge *edge_from;  /* edge connecting to node search from */
  int n_fail_extend;       // modif Juan (test)
  int n_extend;            // modif Juan (test)
  double weight;           // modif Juan (test)
  /* for graph search DFS*/
  int discovered;
  int processed;

  /* cost of a node according to the cost function in a space */
  double cost;

  /* total cost from the root node to the current node in the tree
     graph (for diffusion techniques) */
  double sumCost;

  /* Position of the node in the tree starting from the root node. The
     goal nodes have a rank of 1 and all the sons of a node of rank n
     have a rank of n+1 */
  int rankFromRoot;

  /* Flag TRUE if a node is discarded for the selection as nearest
     neighbor*/
  int IsDiscarded;

 /* Used in cost space problems. Current node temperature. */
  double temp;

  /*Used in cost space problems. Number of consecutive times the node
    failed to be extended due to the temperature in the cost test */
  int nbFailedTemp;

  /* Used in cost space problems. Number of consecutive times the node
     create a son with a cost inferior than its own cost.
     Warning: this variable is currently no more used*/
  // int NbDown;

  /* Used in the p3d_OrderingGraphSearch algorithm,
     variant of the astar algorithm that is used if the flag
     GlobalOrdering is set to TRUE*/
  dbl_list* orderCostListEdge;


  double avr_dist;         // modif Noureddine
  double min_dist;         // modif Noureddine
  double E;                // modif Noureddine
  struct node *parent;     // modif Juan (test)
  p3d_matrix4 RelMobFrame;
  double radius; /* modif Leonard */
  int pinpointed;          // flag to indicate pinpointed node (for graph displaying)
  int boundary; /* say if the node is a boundary node or not (Leonard) */

  int is_dist_sc_ligand_checked;
  int* list_closed_flex_sc;   //table of the size of nb flexible sc.
                              // list_closed_flex_sc[i] = 1 if the ligand and sc are closed,
                              // 0 otherwise
  int isSingularity;
  //start path deform
  int only_for_cycle; /*flag for nodes used in cycling graphs */
  int visible;
  //end path deform

  p3d_vector3 g3d_position; // 3d position of the node when drawing the graph
  bool g3d_position_flag;
#ifdef MULTIGRAPH
  int mergeState; //The merge state of the node: 0 None, 1 trajectory, 2 All
  int needMgCycle;
#endif

} p3d_node, *pp3d_node;

typedef struct list_node{
  p3d_node *N;
  struct list_node *next;
  struct list_node *prev;
} p3d_list_node;


/* Structure d'arete du graphe */
typedef struct edge {
  p3d_node  *Ni, *Nf;
  p3d_localpath *path;
  p3d_localpath_type planner;
  double     longueur;
  double cost;
  int sens_edge;
  //start path deform
  int visible;
  int unvalid;
  int for_cycle;
  //end path deform
} p3d_edge;

typedef struct list_edge{
  p3d_edge *E;
  struct list_edge *next;
  struct list_edge *prev;
} p3d_list_edge;

/* Structure de composante connexe */
typedef struct compco {
  int num; /* numero de la composante connexe */
  int nnode; /* nombre de noeuds dans la composante connexe */
  p3d_list_node *nodes;  /* noeuds faisant partie de la composante connexe */
  p3d_list_node *dist_nodes;/* noeuds de la composante connexe pour le tri par distance */
  p3d_list_node *last_node; /* dernier noeud de la composante */
  struct compco *suiv;
  struct compco *prec;
  int ncanreach;
  int nbRefinNodes;
  double temperature;
  struct list_compco * canreach;
  struct list_compco * last_canreach;
  configPt box_env_small[2];
  double minCost;   /*Used in cost space problems. Minimal
		      cost actually found in the connect componant*/
  double maxCost;   /*Used in cost space problems. Maximal
		      cost actually found in the connect componant*/
  double maxUrmsonCost; /*Used in cost space problems (Urmson variant). Maximal
		       sum cost actually found in the connect componant*/
  int* AnaSuccessTab;
  int nbTests;
} p3d_compco;

typedef struct list_compco{
  struct compco *comp;
  struct list_compco *next;
  struct list_compco *prev;
} p3d_list_compco;


/* Structure d'exterma pour classer les paves selon les ddl */
typedef struct ddlbox{
  int num; /* numero du noeud auquel ce ddl se rattache */
  char   min_max; /* 0 si c'est un min, 1 si c'est un max */
  double val; /* valeur du min ou du max */
  struct ddlbox *next;
  struct ddlbox *prev;
} p3d_ddlbox;

/* Structure de graphe */
typedef struct graph {
  p3d_env *env; /* environnement auquel le graphe se rattache */
  p3d_rob *rob; /* robot auquel le graphe se rattache */
  char * file; /* nom du fichier de sauvegarde du graphe */
  int oriented;

  int nnode; /* nombre de noeuds dans le graphe */
  p3d_list_node *nodes; /* noeuds du graphe */
  p3d_list_node *last_node; /* dernier noeud du graphe */
  int nedge; /*nombre d'aretes */
  p3d_list_edge *edges; /* aretes */
  p3d_list_edge *last_edge;
  int ncomp; /* nombre de composantes connexes */
  p3d_compco *comp; /*liste des composantes connexes */
  p3d_compco *last_comp;
  p3d_ddlbox **ddlbox; /* liste des intervalles des paves sur les ddl */
  unsigned long int hhCount; /* number of the next point of the Halton Set */
  int nb_standart_nodes; /*Number of nodes built by standart-RRT*/
  int nb_DD_nodes;       /*Number of nodes built by DD-RRT*/
  int nboundary; /* number of boundary nodes */
  int n_call_nearest; /*number  of call to the nearest neighbour function */

  int IsCurrentNodeBias; /* Flag to tell if the current
			    direction of expansion is given by a node
			    (used to avoid the superposition of 2 nodes) */
  p3d_node*  NodeBiasPt;
  int        arc_type;
  int        arc_value;
  p3d_node   *search_start;
  p3d_node   *search_goal;
  p3d_node   *search_goal_sol;
  int        search_done;
  int        search_path;
  int        search_numdev;
  double     search_cost;
  int        search_path_length;

  double time;
#ifdef MULTIGRAPH
  double mgTime;
#endif
#ifdef DPG
  class DpgGrid * dpgGrid;
#endif

	// Planning on graph stat
	double totTime;
	double optTime;
  double rrtTime;
  double rrtCost1;
  double rrtCost2;

  int nb_test_BB;
  int nb_test_coll;
  int nb_test_energy;  //mod noureddine
  int nb_local_call;
  int nb_q;
  int nb_q_free;
  int nb_bkb_q_free;
  int nb_q_closed;

  int ** usedIkSols; //List of iksol in the graph
  int nbUsedIkSols; //The number of item in the list
  //start path deform
  p3d_list_node *dist_nodes ;/* noeuds du graph pour un tri global par distance */
  p3d_node* start_nodePt;
  p3d_node* last_nodePt;
  p3d_node* prev_nodePt;
  p3d_traj* traj1Pt;
  p3d_traj* traj2Pt;
  //end path deform

  double critic_cur_pb_level;
  int n_consec_fail_pb_level;
  int n_consec_pb_level;
  double CurPbLevel;
  p3d_vector3 g3d_position; // 3d position of the node when drawing the graph
  bool g3d_position_flag;
  p3d_jnt* g3d_drawnjnt; // used when drawing the graph

  struct p3d_statistics * stat; /* Statistic data structure of the algorithm
						  associated with the graph; Commit Jim; date: 01/10/2008 */

} p3d_graph, *pp3d_graph;


//start path deform
typedef struct project_point {
  int i;
  int j;
  int in_test;
  int is_valid;
  double cost;
}p3d_project_point;

typedef struct path_nodes {
  dbl_list* list_node; // liste de noeuds
  /*p3d_node*/void* N; // Le noeud
  double f;
  double g;
  double h;
  int opened;
  int closed;
  int nnode;// nombre de noeuds dans list_node
}p3d_path_nodes;
//end path deform

#ifdef MULTIGRAPH
/** \brief Double chained List of p3d_flatSuperGraphNode*/
typedef struct fsgListNode{
  /** \brief The p3d_flatSuperGraphNode data*/
  struct flatSuperGraphNode * node;
  /** \brief Pointer on the previous node in the list*/
  struct fsgListNode * prev;
  /** \brief Pointer on the next node in the list*/
  struct fsgListNode * next;
}p3d_fsgListNode;

/** \brief Double chained List of p3d_flatSuperGraphEdge*/
typedef struct fsgListEdge{
  /** \brief The p3d_flatSuperGraphEdge data*/
  struct flatSuperGraphEdge * edge;
  /** \brief Pointer on the previous edge in the list*/
  struct fsgListEdge * prev;
  /** \brief Pointer on the next edge in the list*/
  struct fsgListEdge * next;
}p3d_fsgListEdge;

/** \brief The flatSuperGraphNode is a collection of nodes (configurations). Each of these nodes is generated for one robot or one multigraph part (e.g. for a humanoid torso with two arms, each arm is a multigraph part.) and no one block (is in collision) with another*/
typedef struct flatSuperGraphNode{
  /** \brief The node id (unique)*/
  int num;
  /** \brief The number of nodes (equal to p3d_rob->mg->nGraphs)*/
  int nNodes;
  /** \brief the config corresponding to this node's configs*/
  configPt q;
  /** \brief the node corresponding to this node's merging (freed with the global graph)*/
  p3d_node * mergedNode;
  /** \brief The nodes (configurations)*/
  p3d_node ** nodes;
  /** \brief The number of edges connected to this node*/
  int nEdges;
  /** \brief The list of edges connected to this node*/
  p3d_fsgListEdge * fsgEdges;

  /************* Astar Variables****************/
  /** \brief heuristic cost to goal node*/
  double h;
  /** \brief Real cost from the init node*/
  double g;
  /** \brief g+h*/
  double f;
  /** \brief Whether node is open in A* search*/
  int opened;
  /** \brief Whether node is closed in A* search*/
  int closed;
  /** \brief Parent node in A* search*/
  struct flatSuperGraphNode *search_from;
  /** \brief Child node in A* search*/
  struct flatSuperGraphNode *search_to;
  /** \brief Edge connecting to node search from*/
  struct flatSuperGraphEdge *edge_from;
  /** \brief The node edges listed by cost*/
  dbl_list * orderCostListEdge;
}p3d_flatSuperGraphNode;

/** \brief The flatSuperGraphEdge connect two flatSuperGraphNode. For e.g. if the flatSuperGraphNode is composed by two configurations.*/
typedef struct flatSuperGraphEdge{
  /** \brief The first flatSuperGraphNode*/
  struct flatSuperGraphNode * node1;
  /** \brief The second flatSuperGraphNode*/
  struct flatSuperGraphNode * node2;
  /************* Astar Variables****************/
  /** \brief The cost of the edge*/
  double cost;
}p3d_flatSuperGraphEdge;

/** \brief The flatSuperGraph is a set of flatSuperGraphNodes connected by flatSuperGraphEdges */
typedef struct flatSuperGraph{
  /** \brief the number of flatSuperGraphNodes in the flatSuperGraph*/
  int nNodes;
  /** \brief The list of flatSuperGraphNodes*/
  p3d_fsgListNode * nodes;
  /** \brief the number of flatSuperGraphEdges in the flatSuperGraph*/
  int nEdges;
  /** \brief The list of flatSuperGraphEdges*/
  p3d_fsgListEdge * edges;
  /** \brief The List of edges failed in coorination. This edges are not created in the FSG*/
  std::vector<p3d_flatSuperGraphEdge *> autoColNodes;

  /************* Astar Variables****************/
  /** \brief A* start node*/
  struct flatSuperGraphNode * search_start;
  /** \brief A* goal node*/
  struct flatSuperGraphNode   *search_goal;
  /** \brief A* search is done*/
  int search_done;
  /** \brief A* number of devlopement*/
  int search_numdev;
  /** \brief A* path is found by A**/
  int search_path;
  /** \brief The goal solution found by A* */
  struct flatSuperGraphNode *search_goal_sol;
  /** \brief The cost of the solution found by A* */
  double search_cost;
  /** \brief The solution length found by A* */
  int search_path_length;
}p3d_flatSuperGraph;
#endif //MULTIGRAPH

#endif
