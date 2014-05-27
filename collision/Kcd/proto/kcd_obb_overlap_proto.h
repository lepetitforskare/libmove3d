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

extern void set_DO_KCD_GJK ( int val );
extern void kcd_assign_tolerance ( double val );
extern void kcd_get_tolerance ( double *tol );
extern int kcd_tolerance_is_zero ( void );
extern int obb_disjoint_and_distance ( kcd_matrix4 Place, double a[3], double b[3], double *kcd_obb_distance );
extern int obb_disjoint ( kcd_matrix4 Place, double a[3], double b[3] );
extern int obb_disjoint_trees ( int mobj_id, kcd_matrix4 a_box_pos, kcd_matrix4 inv_a_pos, double a[4], kcd_bb *second_box, int with_report, double *minimal_distance_estimate, int *nearest_obstacle );
extern int collision_exists_between_2_robot_obbtrees ( kcd_bb *rob_box, kcd_matrix4 invM2, kcd_matrix4 mat2, kcd_matrix4 mat1, kcd_matrix4 thisM1, kcd_bb *second_box, kcd_matrix4 *THISmat8, kcd_matrix4 *THISmat9, int with_report, double *distance_estimate, int *nearest_obstacle );

#endif /* __CEXTRACT__ */
