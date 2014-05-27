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

extern void p3d_v_collide_set_null ( void );
extern void p3d_v_collide_init_array ( int sz );
extern int p3d_v_collide_is_non_active ( p3d_poly *obj );
extern int p3d_v_collide_object_is_non_active ( p3d_obj *obj );
extern void p3d_v_collide_add_id_to_array ( p3d_poly *p, int v_collide_id );
extern void p3d_v_collide_object_add_id_to_array ( p3d_obj *p, int v_collide_id );
extern int p3d_v_collide_get_id ( p3d_poly *p );
extern int p3d_v_collide_object_get_id ( p3d_obj *obj );
extern p3d_poly* p3d_v_collide_get_poly_by_id ( int id );
extern p3d_obj* p3d_v_collide_get_object_by_id ( int id );
extern void p3d_v_collide_set_pos_of_object ( p3d_obj *p, p3d_matrix4 mat );
extern void p3d_v_collide_set_pos ( p3d_poly *p, p3d_matrix4 mat );
extern int p3d_v_collide_get_num_vc_objects ( void );
extern void p3d_v_collide_start ( void );
extern void p3d_v_collide_del_poly ( p3d_poly *p );
extern void p3d_v_collide_stop ( void );

#endif /* __CEXTRACT__ */
