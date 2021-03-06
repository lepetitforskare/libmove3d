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
#ifndef PA10CONST_H
#define PA10CONST_H

/*
 * Here are defined arm constant for pa10 Jido
 */

#define PA10_ARM_A2  0.450
#define PA10_ARM_R4  0.480
#define PA10_ARM_OF1 M_PI
#define PA10_ARM_OF2 (-(M_PI)/2)
#define PA10_ARM_OF3 (-(M_PI)/2)
#define PA10_ARM_OF4 0.0
#define PA10_ARM_OF5 0.0
#define PA10_ARM_OF6 0.0
#define PA10_ARM_EPSILON 0.01
#define PA10_ARM_NB_JNT 6
#define XARM_BASE_HEIGHT         0.317
// #define XARM_BASE_HEIGHT         0.000
 #define PA10_6ARM_LENGTH         0.070
 #define PA10_TOOL_LENGTH         0.21


#define PA10_Q1_INIT 34.808258
#define PA10_Q2_INIT 30.000000
#define PA10_Q3_INIT 120.608154
#define PA10_Q4_INIT  35.103245
#define PA10_Q5_INIT -66.519173
#define PA10_Q6_INIT -22.566366


//! The symbolic names of the joints that play a particular role in some computations.
//! The .macro file of the robot must contain joints with these names.
//! In the case of GP_FINGERJOINT, it is the prefix of all the finger joints (e.g.: fingerJoint1, fingerJoint2,
//! etc. or fingerJoint_thumbBase, fingerJoint_forefingerDist, etc.).
#define GP_PLATFORMJOINT    "platformJoint"
#define GP_ARMBASEJOINT      "armBaseJoint"
#define GP_ARMJOINT1            "armJoint1"
#define GP_ARMJOINT2            "armJoint2"
#define GP_ARMJOINT3            "armJoint3"
#define GP_ARMJOINT4            "armJoint4"
#define GP_ARMJOINT5            "armJoint5"
#define GP_ARMJOINT6            "armJoint6"
#define GP_ARMJOINT7            "armJoint7"
#define GP_WRISTJOINT          "wristJoint"
#define GP_FINGERJOINT        "fingerJoint"
#define GP_FREEFLYERJOINT  "freeflyerJoint"
#define GP_VIRTUAL_OBJECT   "virtual_object"

//! Name of the gripper's joint:
#define GP_GRIPPERJOINT      "fingerJointGripper"

#define MLP_GROUP_BASE_RS_NAME "base_rs"
#define MLP_GROUP_BASE_SM_NAME "base_sm"
#define MLP_GROUP_UPBODY_LIN_NAME "upbody_lin"
#define MLP_GROUP_UPBODY_SM_NAME  "upbody_sm"
#define MLP_GROUP_HEAD_LIN_NAME "head_lin"
#define MLP_GROUP_HEAD_SM_NAME "head_sm"
#define MLP_GROUP_HAND_LEFT_LIN_NAME "hand_left_lin"
#define MLP_GROUP_HAND_LEFT_SM_NAME "hand_left_sm"
#define MLP_GROUP_HAND_RIGHT_LIN_NAME "hand_right_lin"
#define MLP_GROUP_HAND_RIGHT_SM_NAME "hand_right_sm"

#endif
