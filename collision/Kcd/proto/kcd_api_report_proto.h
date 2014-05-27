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

extern void kcd_init_pairInCollision ( void );
extern void kcd_get_pairInCollision ( void **primId1, void **primId2 );
extern void kcd_get_pairObjInCollision ( int *objId1, int *objId2 );
extern void kcd_set_pairInCollision ( void *primId1, int ext_p_id1, void *primId2, int ext_p_id2 );
extern void kcd_clean_up_pairInCollision ( void );
extern void kcd_init_dist_mo_report_table ( int nof_mos );
extern void kcd_get_dist_mo_so ( int mo_id, double *d );
extern void kcd_set_dist_mo_so ( int mo_id, double d );
extern void kcd_clean_up_dist_mo_report_table ( int nof_mos );
extern void kcd_init_nearest_obstacle ( void );
extern void kcd_modify_nearest_obstacle ( int rob_i, int static_object_bb_id );
extern int kcd_get_nearest_obstacle_bb_id ( int rob_nr );
extern int kcd_get_nearest_obstacle_information ( int group_id, kcd_matrix4 SOwrtW );
extern void kcd_init_report ( void );
extern void kcd_clean_up_report ( void );
extern void kcd_set_report_to_zero ( void );
extern void kcd_add_report ( int rob_i, int static_object_bb_id );
extern int kcd_check_report ( int rob_nr );
extern void kcd_get_dist_grp_mo_so ( int robot_id, int body_id, double *d );
extern void kcd_set_distance_body_obst ( int robot_id, int body_id, double d );
extern void kcd_set_points_closest_pair ( int robot_id, int body_id, kcd_vector3 point_mo, kcd_vector3 point_so );
extern void kcd_init_kcd_distance_report_table ( int nof_robots );
extern void kcd_init_kcd_distance_report_table_entry ( int robot_id, int nof_bodies );
extern void kcd_init_distance_report_table ( void );
extern void kcd_clean_up_kcd_distance_report_table_entry ( int robot_id, int nof_bodies );
extern void kcd_clean_up_kcd_distance_report_table ( int nof_robots );
extern void kcd_clean_up_distance_report_table ( void );
extern void kcd_get_dist_mo_mo ( int mo_id, double *d );
extern void kcd_set_dist_mo_mo ( int mo_id, double d );
extern void kcd_init_kcd_distance_report_table_mos ( int nof_mos );
extern void kcd_init_kcd_distance_report_table_mos_entry ( int nof_mos );
extern void kcd_reset_kcd_distance_report_table_mos_entry ( int nof_mos );
extern void kcd_init_distance_report_table_mos ( void );
extern void kcd_clean_up_kcd_distance_report_mos_table ( int nof_mos );
extern void kcd_set_points_closest_pair_mos ( int mo1, int mo2, kcd_vector3 point_mo1, kcd_vector3 point_mo2 );
extern void kcd_get_points_closest_pair ( int robot_id, int body_id, kcd_vector3 point_mo, kcd_vector3 point_so );
extern void kcd_get_points_closest_pair_mo_so ( int mo1, kcd_vector3 point_mo, kcd_vector3 point_so );
extern void kcd_set_points_closest_pair_mo_so ( int mo1, kcd_vector3 point_mo, kcd_vector3 point_so );
extern void kcd_get_points_closest_pair_mos ( int mo1, int *mo2, kcd_vector3 point_mo1, kcd_vector3 point_mo2 );
extern int kcd_get_points_closest_pair_other_mo ( int mo1 );
extern void kcd_reset_dist_mo_report_table ( int nof_mos );
extern void kcd_reset_dist_grp_so_report_table ( int group_id );
extern void kcd_reset_distance_report_table_grp_so ( void );

#endif /* __CEXTRACT__ */
