/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern void kcd_get_prim_abs_pos ( void *primPt, int can_move, void *placement );
extern void kcd_get_prim_rel_pos ( void *primPt, void *placement );
extern void kcd_get_obj_abs_pos ( int kcd_ext_o, void *placement );
extern void kcd_get_pt ( void *polyPt, int i, double *x, double *y, double *z );
extern void kcd_get_pt_arr ( void *polyPt, void *the_points );
extern unsigned int kcd_get_nb_pts ( void *polyPt );
extern unsigned int kcd_get_nb_fs ( void *polyPt );
extern unsigned int kcd_get_nb_pts_in_f ( void *polyPt, int i );
extern int kcd_get_i_pt_in_f ( void *polyPt, int fid, int pid );
extern void kcd_get_pt_in_f_arr ( void *polyPt, int fid, void *the_points );
extern int kcd_get_poly_entity_type ( void *polyPt );
extern int kcd_poly_is_convex ( void *polyPt );
extern int kcd_facet_is_convex ( void *polyPt, int fid );
extern void kcd_get_solid_r1 ( void *polyPt, double *the_datum );
extern void kcd_get_solid_r2 ( void *polyPt, double *the_datum );
extern void kcd_get_solid_h ( void *polyPt, double *the_datum );
extern void kcd_get_solid_s ( void *polyPt, double *the_datum );
extern void kcd_get_solid_x ( void *polyPt, double *x );
extern void kcd_get_solid_y ( void *polyPt, double *y );
extern void kcd_get_solid_z ( void *polyPt, double *z );
extern void kcd_get_solid_xyz ( void *polyPt, double *x, double *y, double *z );
extern int kcd_get_aabb_on_mo ( int kcd_ext_o, double *x1, double *x2, double *y1, double *y2, double *z1, double *z2 );
extern void kcd_get_scene_size ( double *x, double *y, double *z );

#endif /* __CEXTRACT__ */
