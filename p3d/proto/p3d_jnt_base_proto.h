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

extern const char * p3d_jnt_base_get_dof_name ( int i_dof );
extern void p3d_jnt_base_set_dof ( p3d_jnt * jntPt, int i_dof, double val );
extern void p3d_jnt_base_set_dof_bounds ( p3d_jnt * jntPt, int i_dof, double vmin, double vmax );
extern void p3d_jnt_base_stay_within_dist ( p3d_stay_within_dist_data * prev_data, p3d_jnt * jntPt, p3d_stay_within_dist_data * data, double * distance, configPt q_init, configPt q_max_param, double max_param, double * reach_param );
extern void p3d_jnt_base_calc_dof ( p3d_jnt * jntPt );
extern p3d_jnt * p3d_jnt_base_create ( p3d_matrix4 pos, double * v, double * vmin, double * vmax, double * vmin_rand, double * vmax_rand, double *velocity_max, double * acceleration_max, double *jerk_max, double * param );
extern p3d_jnt * p3d_jnt_base_create_deg ( p3d_matrix4 pos, double * v, double * vmin, double * vmax, double * vmin_rand, double * vmax_rand, double *velocity_max, double * acceleration_max, double *jerk_max, double * param );

//start path deform
extern void p3d_jnt_base_stay_within_sphere(p3d_stay_within_dist_data * prev_data,
                                            p3d_jnt * jntPt,
                                            p3d_stay_within_dist_data * data,
                                            double * distance,
                                            double * reach_param);
//end path deform
#endif /* __CEXTRACT__ */
