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
 *
 *   Created: Thu Apr 24 14:01:25 2008
 */
#ifdef MULTILOCALPATH
#include "../../lightPlanner/proto/ManipulationStruct.hpp"

#ifndef __CEXTRACT__
extern void lm_convert_p3dMatrix_To_GbTh(const p3d_matrix4 M ,Gb_th* th);
extern void lm_convert_GbTh_To_p3dMatrix(const Gb_th* th, p3d_matrix4 M);

extern p3d_group_type p3d_group_getid_group(const char * name);
extern int p3d_group_planner(p3d_rob* robotPt, int nbGraph, p3d_group_type gpType, p3d_softMotion_data* softMotion_data, int *ikSol);
extern p3d_group_type p3d_group_getType_group(int nbGraph);


extern p3d_softMotion_data * p3d_create_softMotion_data_multilocalpath(p3d_rob* robotPt, int mlpId);
extern p3d_softMotion_data * p3d_alloc_spec_softMotion_localpath (p3d_rob *robotPt, p3d_softMotion_data * sm_data);
//, double J_max[], double A_max[], double V_max[], double V_init[], double X_init[], double V_end[], double X_end[]);
extern p3d_localpath * p3d_alloc_softMotion_localpath ( p3d_rob *robotPt, p3d_softMotion_data * sm_data, int lp_id, int is_valid );
extern p3d_localpath *p3d_softMotion_localplanner ( p3d_rob *robotPt, int multiGraphID, p3d_softMotion_data* smDataIn, configPt qi, configPt qf, configPt qfp1, int* ikSol);
extern void lm_destroy_softMotion_params ( p3d_rob * robotPt, void *paramPt);
extern void p3d_destroy_softMotion_data(p3d_rob* robotPt, p3d_softMotion_data* softMotion_dataPt);
extern void p3d_softMotion_destroy(p3d_rob* robotPt, p3d_localpath* localpathPt);
extern p3d_localpath *p3d_copy_softMotion_localpath(p3d_rob* robotPt, p3d_localpath* localpathPt);
extern psoftMotion_str lm_create_softMotion(p3d_rob *robotPt, int mlpId);
extern psoftMotion_str lm_get_softMotion_lm_param(p3d_rob *robotPt);
extern psoftMotion_str lm_get_softMotion_lm_param_multilocalpath(p3d_rob *robotPt, int nbGraphs);

extern int p3d_set_softMotion_subrobot_model(char name[], double *dtab);
extern int p3d_create_softMotion_local_method_for_freeflyer(double *dtab);
extern Gb_q6 lm_get_q6_from_configPt(configPt conf, int index);
extern void lm_set_Gb_v3_for_translation_and_rotation( const Gb_th* th, Gb_v3* pose, Gb_v3* rot);
extern void lm_softMotion_compute_tangent(const Gb_v3 *p1, const Gb_v3 *p2, const Gb_v3 *p3, Gb_v3 *tangent);
extern void lm_set_cond_softMotion_data_FREEFLYER(Gb_v3 poseLinInit, Gb_v3 poseLinEnd, Gb_v3 poseAngInit,
														Gb_v3 poseAngEnd, Gb_v3 velLinInit, Gb_v3 velAngInit,
														Gb_v3 velLinEnd, Gb_v3 velAngEnd, p3d_softMotion_data* softMotion_data);
extern void lm_set_cond_softMotion_data(int index_dof, int nbJoints, configPt qi, configPt qf, double *velInit, double *velEnd, double *accInit, double *accEnd, p3d_softMotion_data* softMotion_data);

extern void lm_set_and_get_motionTimes(p3d_softMotion_data* softMotion_data, double* timeMotionMax, int* axisMotionMax);
extern void lm_get_softMotion_segment_params_FREEFLYER(p3d_softMotion_data* softMotion_data, double param, SM_SEGMENT * segment, int * segId, int index);
extern void lm_get_softMotion_segment_params(p3d_softMotion_data* softMotion_data, double param, SM_SEGMENT * segment, int * segId, int index);

extern void lm_get_paramDiff_for_param(p3d_softMotion_data* softMotion_data, SM_SEGMENT* seg, int segId, int index, double param, double* paramDiff);

extern void lm_compute_softMotion_for_r6Arm( p3d_softMotion_data* softMotion_data);
extern configPt p3d_softMotion_config_at_param(p3d_rob *robotPt, p3d_localpath *localpathPt, double param);
extern double p3d_softMotion_stay_within_dist(p3d_rob* robotPt, p3d_localpath* localpathPt,
																				double parameter, whichway dir, double *distances);
extern double p3d_softMotion_cost(p3d_rob *robotPt, p3d_localpath *localpathPt);
extern p3d_localpath *p3d_extract_softMotion(p3d_rob *robotPt, p3d_localpath *localpathPt, double l1, double l2);
extern p3d_localpath *p3d_simplify_softMotion(p3d_rob *robotPt, p3d_localpath *localpathPt, int *need_colcheck);

extern void p3d_softMotion_export_traj(p3d_rob* robotPt, p3d_traj* traj, int trajType,  char *fileName, char *fileNameSeg, bool flagSaveFile, bool flagPlot,double SAMPLING_TIME,
					    std::vector <int> &lp, std::vector < std::vector <double> > &positions, SM_TRAJ &smTraj);
extern void softMotion_data_copy_into(p3d_rob *robotPt, const p3d_softMotion_data * sm_data, p3d_softMotion_data * softMotion_data);
extern p3d_localpath *p3d_extract_softMotion_with_velocities(p3d_rob *robotPt, p3d_localpath *localpathPt,	double l1, double l2);

/* Function specific to multigraph group */
extern int p3d_softMotion_localplanner_FREEFLYER(p3d_rob* robotPt, int graphId, p3d_group_type gpType, p3d_softMotion_data* softMotion_data, int* ikSol);
extern int p3d_softMotion_localplanner_KUKA_ARM(p3d_rob* robotPt, int graphId, p3d_group_type gpType, p3d_softMotion_data* softMotion_data, int* ikSol);
extern int p3d_softMotion_localplanner_JOINT(p3d_rob* robotPt, int graphId, p3d_group_type gpType, p3d_softMotion_data* softMotion_data, int* ikSol);
extern int p3d_softMotion_localplanner_UPBODY_CART(p3d_rob* robotPt, int graphId, p3d_group_type gpType, p3d_softMotion_data* softMotion_data, int* ikSol);
extern void p3d_softMotion_set_stay_within_dist(int value);

int lm_compute_softMotion(p3d_rob* robotPt, int mlpID, p3d_softMotion_data* softMotion_data);


/////////////////////
// COPY FUNCTIONS  //
/////////////////////
extern p3d_softMotion_data* p3d_copy_softMotion_data(p3d_rob* robotPt, int mlpID, p3d_softMotion_data *sm_data);
void lm_set_and_get_motionTimes_FREEFLYER(p3d_softMotion_data* softMotion_data, double* timeMotionMax, int* axisMotionMax);
void 	lm_set_motion_softMotion_data_FREEFLYER(SM_TIMES localtimes[], double jerk[], int DirTransition_a[],
																							int DirTransition_b[], p3d_softMotion_data* softMotion_data);

// Function to externaly set the 
// Cost of a localpath
extern double (*ext_compute_localpath_kin_cost)(p3d_rob* robotPt,p3d_localpath* path);


#endif /* __CEXTRACT__ */
#endif
