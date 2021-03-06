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

extern void kcd_init_AABB_TREE ( void );
extern void kcd_clean_up_AABB_TREE ( void );
extern int kcd_get_the_bb ( int val1, int val2 );
extern int kcd_get_total_height ( void );
extern int kcd_get_nofels_on_level ( int hierheight );
extern int kcd_get_aabb_i_by_level ( int hierheight, int i );
extern int kcd_determine_aabb_around_aabbs ( int *the_aabb_group, int the_aabb_group_nofels, int hierheight );
extern double new_aabbs_square_distance ( int b1_nr, int b2_nr );
extern int aabbs_overlap ( kcd_bb *aabb1, kcd_bb *aabb2 );
extern double aabbs_square_distance ( int b1_nr, int b2_nr );
extern double aabbs_distance ( int b1_nr, int b2_nr );
extern double aabbs_penetration_measure ( int b1_nr, int b2_nr );
extern double aabbs_square_penetration_measure_when_collision ( int b1_nr, int b2_nr );
extern void kcd_sort_aabb_tree ( axis_list_elp *the_list, int the_list_size );
extern void insert_static_aabb ( int aabbtoinsert_nr, int hierheight, int group_nr, int *group_below, int group_below_nr );
extern void create_aabb_hierarchy ( void );
extern int kcd_study_candidate ( int hier_level, int el_at_level, int mobj_id, kcd_bb *robot_bb, int *tot_nof_res, kcd_matrix4 *mat2, int with_report, double *minimal_distance_estimate, int *nearest_obstacle, double the_tolerance );
extern void kcd_init_robot_vs_static_aabbs ( int mobj_id, double x1, double x2, double y1, double y2, double z1, double z2, int *tot_nof_res );
extern int kcd_update_robot_vs_static_aabbs ( int mobj_id, double x1, double x2, double y1, double y2, double z1, double z2, int *tot_nof_res, int with_report, double *min_dist );
extern void kcd_init_movable_stuff_robot ( int i );
extern int check_robot_vs_obbs_of_this_aabb ( int mobj_id, double a[4], double center_a[3], kcd_bb *a_bb, int with_report, double *minimal_distance_estimate, int *nearest_obstacle );
extern int kcd_updated_robot_vs_static_obbs ( int mobj_id, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, int with_report, double *minimal_distance_estimate, int *nearest_obstacle );
extern int kcd_mo_in_collision ( int mobj_id, int with_report, double *kcd_minimal_distance_estimate, int *nearest_obstacle );
extern void kcd_delete_movable_stuff_robot ( int i );
extern int kcd_robot_in_collision ( int robot_id, int with_report, double *min_dist_estimate, int *nearest_obstacle );
extern void kcd_init_movable_stuff ( void );
extern void kcd_clean_up_mov_stuff ( void );
extern void add_to_aabb_hierarchy ( void );

#endif /* __CEXTRACT__ */
