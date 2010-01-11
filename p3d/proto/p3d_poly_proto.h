/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern void p3d_get_poly_pos_of_polyh ( p3d_poly *polyPt, p3d_matrix4 **pos );
extern void p3d_get_poly_pos0 ( p3d_poly *polyPt, p3d_matrix4 **pos0 );
extern void p3d_get_poly_pos_rel_jnt ( p3d_poly *polyPt, p3d_matrix4 **pos );
extern int p3d_get_poly_id ( p3d_poly *polyPt );
extern void p3d_get_poly_point_array ( p3d_poly *polyPt, p3d_vector3 **the_points );
extern unsigned int p3d_get_nof_points_of_polyh ( p3d_poly *polyPt );
extern unsigned int p3d_get_nof_points_in_face_of_polyh ( p3d_poly *polyPt, int i );
extern void p3d_get_points_in_face_of_polyh ( p3d_poly *polyPt, int fid, poly_index_p *the_points );
extern int p3d_get_index_point_in_face_of_polyh ( p3d_poly *polyPt, int fid, int pid );
extern unsigned int p3d_get_nof_faces_of_polyh ( p3d_poly *polyPt );
extern int p3d_get_poly_is_convex ( p3d_poly *polyPt );
extern int p3d_get_facet_is_convex ( p3d_poly *polyPt, int facet_nr );
extern void p3d_get_prim_radius ( p3d_poly *polyPt, double *the_datum );
extern void p3d_get_prim_other_radius ( p3d_poly *polyPt, double *the_datum );
extern void p3d_get_prim_sin_slope ( p3d_poly *polyPt, double *the_datum );
extern void p3d_get_prim_height ( p3d_poly *polyPt, double *height );
extern void p3d_get_prim_x_length ( p3d_poly *polyPt, double *x );
extern void p3d_get_prim_y_length ( p3d_poly *polyPt, double *y );
extern void p3d_get_prim_z_length ( p3d_poly *polyPt, double *z );
extern void p3d_get_prim_lengths ( p3d_poly *polyPt, double *x, double *y, double *z );
extern p3d_poly *p3d_poly_beg_poly ( char name[20], int type );
extern void p3d_poly_add_vert ( p3d_poly *poly, float x, float y, float z );
extern void p3d_poly_add_face ( p3d_poly *poly, int *listeV, int nb_Vert );
extern void p3d_poly_end_poly ( p3d_poly *poly );
extern void p3d_poly_set_color ( p3d_poly *p, int color, double *color_vect );
extern void p3d_poly_set_entity_type ( p3d_poly *p, int entity_tjip );
extern int p3d_poly_get_entity_type ( p3d_poly *p );
extern void p3d_get_box ( double *x1, double *x2, double *y1, double *y2, double *z1, double *z2 );
extern int p3d_poly_get_nb ( void );
extern p3d_poly *p3d_poly_get_poly ( int i );
extern p3d_poly *p3d_poly_get_poly_by_name ( char *name );
extern p3d_poly *p3d_poly_get_first ( void );
extern p3d_poly *p3d_poly_get_next ( void );
extern void p3d_poly_destroy_primitive ( p3d_primitive *primaat );
extern int p3d_poly_del_poly ( p3d_poly *p );
extern p3d_vector3 *sample_triangle_surface(p3d_vector3 p1, p3d_vector3 p2, p3d_vector3 p3, double step, unsigned int *nb_samples);

#endif /* __CEXTRACT__ */
