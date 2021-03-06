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
#include "Util-pkg.h"
#include "P3d-pkg.h"
#include "Planner-pkg.h"
#include "Collision-pkg.h"

/**
 * Note: the integer values of the different
 * EXPANSION_DIRECTION_METHODs are defined in the
 * p3d_type.h file
 */
static int EXPANSION_DIRECTION_METHOD = GLOBAL_CS_EXP;

/**
 * Value of the bias toward the goal componant
 * if the expansion process is biased.
 */
static double GOAL_BIAS = 0.1;

/**
 * Flag TRUE if the expansion process
 * is biased toward a  goal configuration
 */
static int IS_GOAL_BIAS = FALSE;


/**
 * Flag to say if a sampling direction must
 * validate the Rlg constraints or not
 */
static int IsDirSampleWithRlg = FALSE;

/**
 * Get the value of the bias toward the goal componant
 * if the expansion process is biased.
 * @return: the value of the bias toward the goal componant
 */
double p3d_GetGoalBiasValue(void) {
  return GOAL_BIAS;
}

/**
 * Set the value of the bias toward the goal componant
 * if the expansion process is biased.
 * @param[In]: the value of the bias toward the goal componant
 */
void p3d_SetGoalBiasValue(double GoalBias) {
  GOAL_BIAS = GoalBias;
}

/**
 * p3d_GetIsGoalBias
 * Get if the expansion is biased toward the goal
 * return: TRUE if there is a bias toward the goal
 */
int p3d_GetIsGoalBias(void) {
  return IS_GOAL_BIAS;
}

/**
 * p3d_SetIsGoalBias
 * Set if the expansion is biased toward the goal
 * @param[In] IsGoalBias: TRUE if there is a bias toward the goal
 */
void p3d_SetIsGoalBias(int IsGoalBias) {
  IS_GOAL_BIAS = IsGoalBias;
}

/**
 * p3d_SetExpansionDirectionMethod
 * Set the current value of the method used to
 * choose the direction of expansion
 * @param[in] the expansion direction method
*/
void p3d_SetExpansionDirectionMethod(int ExpDirMethod) {
  EXPANSION_DIRECTION_METHOD = ExpDirMethod;
}

/**
 * p3d_GetExpansionDirectionMethod
 * Get the  value of the current method used to
 * choose the direction of expansion
 * @return: the current expansion direction method
*/
int p3d_GetExpansionDirectionMethod(void) {
  return EXPANSION_DIRECTION_METHOD;
}




/**
 * p3d_GetIsDirSampleWithRlg
 * Get if the  sampling direction must
 * validate the Rlg constraints or not
 * @return: TRUE if if the  sampling direction
 * must validate the Rlg constraints
 */
int p3d_GetIsDirSampleWithRlg(void) {
  return IsDirSampleWithRlg;
}

/**
 * p3d_GetIsDirSampleWithRlg
 * Set if the  sampling direction must
 * validate the Rlg constraints or not
 * @param[In] isDirSampleWithRlg: TRUE
 * if if the  sampling direction
 * must validate the Rlg constraints
 */
void p3d_SetIsDirSampleWithRlg(int isDirSampleWithRlg) {
  IsDirSampleWithRlg = isDirSampleWithRlg;
}


/**
 * SelectExpansionDirection
 * Main function selecting a direction of expansion
 * for a connect componant to expand
 * @param[In] GraphPt: Pointer to the robot graph
 * @param[In] CompToExpandPt: the connected componant to expand
 * @param[In] GoalCompPt: the goal componant that we want to reach. Can
 * be used to bias th expansion. Can be set to NULL if we don't wawnt any bias.
 * @param[Out]: DirectionConfig: the configuration selected as
 * a direction of expansion
 * @param[In] ArePassiveDofsSampled: say if the Passive parameters are sampled
 * Should be FALSE only during the active expansion of a Manhattan like expansion
 * @return: TRUE if a direction of expansion has been found
 * FALSE  otherwise.
 */
int SelectExpansionDirection(p3d_graph* GraphPt, p3d_compco* CompToExpandPt,
                             p3d_compco* GoalCompPt,
                             configPt DirectionConfig, int ArePassiveDofsSampled) {
  int  IsExpandDirectionFound = FALSE;
  double GoalBias = -1.;
  double RandomValue;
  p3d_node* SelectedNodePt = NULL;
  int IsGoalBias = 0;
  int savedRlg;

  if ((GraphPt == NULL) || (CompToExpandPt == NULL)) {
    PrintInfo(("Warning :  Try to expand a comp with wrong graph structures \n"));
    return FALSE;
  }
  IsGoalBias = p3d_GetIsGoalBias();
  if (p3d_GetIsDirSampleWithRlg() == FALSE) {
    //Save the previous Rlg setting to shoot without Rlg
    savedRlg = p3d_get_RLG();
    p3d_set_RLG(FALSE);
  }
  switch (p3d_GetExpansionDirectionMethod()) {
    case GLOBAL_CS_EXP:
      /* Selection in the entire CSpace and
         biased to the Comp of the goal configuration */

      /* note: Currently the p3d_shoot function always
         return TRUE, but it could change */
      if (IsGoalBias == TRUE) {
        if (GoalCompPt == NULL) {
          PrintInfo(("No bias possible as the goal Componant is not specified\n"));
          return FALSE;
        }
        GoalBias = p3d_GetGoalBiasValue();
        RandomValue = p3d_random(0., 1.);
        if (RandomValue > GoalBias) {
          /* Classical random expansion*/
          GraphPt->IsCurrentNodeBias = FALSE;
        } else {
          GraphPt->IsCurrentNodeBias = TRUE;
          /* select randomly a node in the goal componant as direction of expansion */
          SelectedNodePt = p3d_RandomNodeFromComp(GoalCompPt);
          GraphPt->IsCurrentNodeBias = TRUE;
          GraphPt->NodeBiasPt = SelectedNodePt;
          if (SelectedNodePt == NULL) {
            PrintInfo(("Warning: The biased expansion failed because \
                       a target node can't be choosen\n"));
            if (p3d_GetIsDirSampleWithRlg() == FALSE) {
              //Restore the previous Rlg setting
              p3d_set_RLG(savedRlg);
            }
            return FALSE;
          }
          p3d_copy_config_into(GraphPt->rob, SelectedNodePt->q, & DirectionConfig);
          IsExpandDirectionFound = TRUE;
        }
      } else {/* Selection in the entire CSpace */
        IsExpandDirectionFound = p3d_shoot(GraphPt->rob, DirectionConfig, ArePassiveDofsSampled);
      }
      break;
    case SUBREGION_CS_EXP:
      if (IsGoalBias == TRUE) {
        /* Selection in a subregion of the CSpace
           (typically close to the current tree)
           and  biased to the goal configuration */
        if (GoalCompPt == NULL) {
          PrintInfo(("No bias possible as the goal Componant is not specified\n"));
          if (p3d_GetIsDirSampleWithRlg() == FALSE) {
            //Restore the previous Rlg setting
            p3d_set_RLG(savedRlg);
          }
          return FALSE;
        }
        GoalBias = p3d_GetGoalBiasValue();
        RandomValue = p3d_random(0., 1.);
        if (RandomValue > GoalBias) {
          /* Classical random expansion*/
          GraphPt->IsCurrentNodeBias = FALSE;
          IsExpandDirectionFound = p3d_shoot_inside_box(GraphPt->rob,
                                   DirectionConfig,
                                   CompToExpandPt->box_env_small,
                                   ArePassiveDofsSampled);
        } else {
          /* select randomly a node in the goal componant as direction of expansion */

          SelectedNodePt = p3d_RandomNodeFromComp(GoalCompPt);
          GraphPt->IsCurrentNodeBias = TRUE;
          GraphPt->NodeBiasPt = SelectedNodePt;
          if (SelectedNodePt == NULL) {
            PrintInfo(("Warning: The biased expansion failed because \
                       a target node can't be choosen\n"));
            if (p3d_GetIsDirSampleWithRlg() == FALSE) {
              //Restore the previous Rlg setting
              p3d_set_RLG(savedRlg);
            }
            return FALSE;
          }
          p3d_copy_config_into(GraphPt->rob, SelectedNodePt->q, & DirectionConfig);

          IsExpandDirectionFound = TRUE;
        }

      } else {
        /* Selection in a subregion of the CSpace
           (typically close to the current tree/comp) */
        IsExpandDirectionFound = p3d_shoot_inside_box(GraphPt->rob,
                                 DirectionConfig, CompToExpandPt->box_env_small,
                                 ArePassiveDofsSampled);
      }
      break;

    default:
      /* The default method is based select randomly
         in the all space a direction of expansion  */
      IsExpandDirectionFound = p3d_shoot(GraphPt->rob, DirectionConfig,
                                         ArePassiveDofsSampled);
  }
  if (p3d_GetIsDirSampleWithRlg() == FALSE) {
    //Restore the previous Rlg setting
    p3d_set_RLG(savedRlg);
  }
  return IsExpandDirectionFound;
}
