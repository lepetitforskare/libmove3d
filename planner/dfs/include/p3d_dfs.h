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
#ifndef __P3DDFS_H__
#define __P3DDFS_H__


class Dfs {
  public:
    Dfs();
    virtual ~Dfs();
/**
 * @brief The user have to call this function to use the DFS alogorithm
 * @param graph The graph to search inside
 * @param node The node to search from
 * @return The list of node found by the dfs
 */
    virtual void *p3d_dfs(void* graph, void* node) = 0;
  protected:
/**
 * @brief Delete the node list (private attribute)
 */
    virtual void deleteNodeList(void) = 0;
/**
 * @brief Initialize dfs attributes for all the graph nodes
 * @param g the graph to initialize
 */
    virtual void initGraph(void* graph) = 0;
/**
 * @brief Set the given nodes discovered variable to value
 * @param n The node
 * @param value The value
 */
    virtual void setDiscovered(void* node, bool value) = 0;
/**
 * @brief Get the given nodes discovered variable value
 * @param n The node
 * @return The value of discoverd variable
 */
    virtual bool getDiscovered(void* node) = 0;
/**
 * @brief Set the given nodes processed variable to value
 * @param n The node
 * @param value The value
 */
    virtual void setProceeded(void* node, bool value) = 0;
/**
 * @brief Get the given nodes processed variable value
 * @param n The node
 * @return The value of processed variable
 */
    virtual bool getProceeded(void* node) = 0;
/**
 * @brief This function determines the action to do on the found node.
 * @param n The node
 */
    virtual void processNode(void* node) = 0;
/**
 * @brief This function determines the action to do on the found edge.
 * @param n The node
 * @param id The edge id in the node structure
 */
    virtual void processEdge(void* edge, int id) = 0;
/**
 * @brief Get the number of edges connecting this node his neigbours
 * @param n The node
 * @return 
 */
    virtual int getNodeNbEdges(void* node) = 0;
/**
 * @brief Get the id'th edge on the nodes edges list
 * @param n The node
 * @param id The edge id in the node structure
 * @return 
 */
    virtual void* getEdgeNode(void* node, int id) = 0;
/**
 * @brief Check if the edge is valid or not
 * @param n The node
 * @param id The edge id in the node structure
 * @return True if the node is valid, False otherwise
 */
    virtual bool validEdge(void* edge, int id) = 0;
    void deepFirstSearch(void* graph, void* node);
    void* _listNode;
};

#endif

