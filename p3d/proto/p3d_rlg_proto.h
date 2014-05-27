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

extern void p3d_generate_rlg ( p3d_cntrt *ctPt );
extern int p3d_set_random_loop_generator ( char *rlg_type, int ctnum, int faj, int laj, double rmax, double rmin );
extern int p3d_random_loop_generator_ok ( void );
extern void p3d_active_RLG_flags ( void );
extern int p3d_get_RLG ( void );
extern void p3d_set_RLG ( int val );
extern int p3d_random_loop_generator ( p3d_rob *robotPt, configPt q );
extern int p3d_random_loop_generator_without_parallel ( p3d_rob *robotPt, configPt q );
extern int p3d_reshoot_and_set_config_from_ct ( p3d_rob *robotPt, p3d_cntrt *ct );
extern int p3d_inter_ang_regions ( double ar1m, double ar1M, double ar2m, double ar2M, p3d_matrix2 sol );
extern int p3d_inter_ang_regions_II ( double ar1m, double ar1M, double ar2m, double ar2M, p3d_matrix2 sol );
extern int p3d_merge_ang_intervals(int nint1, int nint2, double *intmin1, double *intmax1, double *intmin2, double *intmax2);
extern double p3d_random_in_several_intervals ( int iv1, int iv2, p3d_matrix2 interv1, p3d_matrix2 interv2, double refval );
extern double p3d_random_in_two_intervals ( double minlim1, double maxlim1, double minlim2, double maxlim2 );
extern double p3d_random_in_three_intervals ( double minlim1, double maxlim1, double minlim2, double maxlim2, double minlim3, double maxlim3 );
extern int p3d_rel_pos_lines ( p3d_vector3 u1, p3d_vector3 u2, p3d_vector3 P1P2 );
extern void p3d_solve_eqsys_2 ( p3d_matrix2 A, p3d_vector2 B, p3d_vector2 unk );
extern int p3d_mat3Rank ( p3d_matrix3 mat, double tolerance );
extern double p3d_lineDist ( p3d_vector3 P0, p3d_vector3 v, p3d_vector3 point );
extern int p3d_lineInt ( p3d_vector3 P1, p3d_vector3 v1, p3d_vector3 P2, p3d_vector3 v2, p3d_vector3 res );
extern int p3d_line_planeInt ( p3d_vector3 Pl1, p3d_vector3 Pl2, p3d_vector3 Pp, p3d_vector3 vp, p3d_vector3 res );
extern int p3d_line_planeInt_II ( p3d_vector3 Pl1, p3d_vector3 Pl2, p3d_vector3 Pp1, p3d_vector3 Pp2, p3d_vector3 Pp3, p3d_vector3 res );
extern int p3d_same_sign_vect ( p3d_vector3 v1, p3d_vector3 v2 );
extern void p3d_destroy_rlg_chain ( p3d_rlg_chain *rlgchPt );
extern void p3d_destroy_rlg_base ( p3d_rlg_base *rlgbsPt );
extern void p3d_destroy_rlg ( p3d_rlg *rlgPt );
extern double p3d_get_max_extension_manipulator ( p3d_cntrt *manip_cntrt );
extern int p3d_compute_T_base_robot_manip ( p3d_cntrt *ctPt, p3d_jnt *robotbasejntPt, p3d_jnt *manipbasejntPt, p3d_matrix4 Tbm );
extern double p3d_get_zmin_manipulator ( p3d_cntrt *manip_cntrt );
extern double p3d_get_zmax_manipulator ( p3d_cntrt *manip_cntrt );

#endif /* __CEXTRACT__ */
