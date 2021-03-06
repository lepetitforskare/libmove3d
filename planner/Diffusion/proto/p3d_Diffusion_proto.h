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
#ifndef __CEXTRACT__

/**
 * p3d_ExpandCompWithoutGoal 
 * Expand a connect componant without any goal to reach
 * stop when a given amount of nodes has been created or
 * when the user push the stop button 
 * @param[In] GraphPt: Pointer to the robot graph
 * @param[In] CompPt: Pointer to the componant to expand
 * @param[In] StopFunction: a pointer to the function used to 
 * stop the application (through the stop button)
 * @param[In] DrawFunction: a pointer to the function used to 
 * draw the graph
 * @return: The total number of created nodes
*/
int p3d_ExpandCompWithoutGoal(p3d_graph* GraphPt, 
			      p3d_compco* CompPt, 
			      int (*StopFunction)(void), 
			      void (*DrawFunction)(void));

/**
 * p3d_ExpandCompToGoal 
 * Expand a connect componant with a goal to reach 
 * @param[In] GraphPt: Pointer to the robot graph
 * @param[In] CompPt: Pointer to the componant to expand
 * @param[In] StopFunction: a pointer to the function used to 
 * stop the application (through the stop button)
 * @param[In] DrawFunction: a pointer to the function used to 
 * draw the graph
 * @return: the number of created nodes
*/
int  p3d_ExpandCompToGoal(p3d_graph* GraphPt, p3d_compco* CompPt, 
			  p3d_node* GoalNodePt, 
			  int (*StopFunction)(void), 
			  void (*DrawFunction)(void));

/**
 * p3d_ExpandCompToGoal 
 * Expand the initial componant (ie. including 
 * the search start node) and try to connect it to the goal 
 * configuration (search_goal) 
 * @param[In] GraphPt: Pointer to the robot graph
 * @param[In] StopFunction: a pointer to the function used to 
 * stop the application (through the stop button)
 * @param[In] DrawFunction: a pointer to the function used to 
 * draw the graph
 * @return: the number of created nodes
*/
int p3d_ExpandInitCompToGoal(p3d_graph* GraphPt, 
			     int (*StopFunction)(void),
			     void (*DrawFunction)(void));

/**
 * p3d_BiExpandInitGoalComp 
 * Expand the initial and Goal componants and try to connect 
 * them. 
 * @param[In] GraphPt: Pointer to the robot graph
 * @param[In] StopFunction: a pointer to the function used to 
 * stop the application (through the stop button)
 * @param[In] DrawFunction: a pointer to the function used to 
 * draw the graph
 * @return: if the extremal configurations are linked
*/
int p3d_BiExpandInitGoalComp(p3d_graph* GraphPt, 
			     int (*StopFunction)(void),
			     void (*DrawFunction)(void));

/**
 * p3d_RunDiffusion
 * Main function to run a diffusion process based 
 * on the current value of the setting parameter
 * @param[In] GraphPt: Pointeer on the Robot Graph
 * @param[In] fct_stop: stop function used to stop the process.
 * Used in articular when the user push the stop button
 * @param[In] fct_draw: function used to draw the current graph.
 * @return: TRUE if the extremal positions are linked.
 */
int p3d_RunDiffusion(p3d_graph* GraphPt, int (*fct_stop)(void),
		     void (*fct_draw)(void), p3d_node* Ns, p3d_node* Ng);

/**
 * p3d_DiffuseOneConf
 * Create a new configuration diffused from a first one
 * by sampling a rand configuration in the space.
 * The diffusion step id equal to dMax*extendStepParam
 * (similarly to the one used in the expend version of the 
 * diffusion methods)
 * @param[In] robotPt: the current robot.
 * @param[In] qcurrent: the configuration from which the 
 * diffusion occure 
 * @return: the diffused new configuration 
 */
configPt p3d_DiffuseOneConf(p3d_rob* robotPt, configPt qCurrent);
#endif

