/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern void g3d_set_color_vect ( int color, GLdouble color_vect[4] );
extern void g3d_get_color_vect(int color, GLdouble color_vect[4]);
extern void g3d_set_color ( int color, GLdouble color_vect[4] );
extern void g3d_drawDisc ( double x, double y, double z, float r, int color, double color_vect[4] );
extern void g3d_drawSphere ( double x, double y, double z, float r);
extern void g3d_drawColorSphere ( double x, double y, double z, float r, int color, double color_vect[4] );
extern void g3d_drawCircle ( double x, double y, double r, int color, double *color_vect, double width);
extern void g3d_drawOneLine ( double x1, double y1, double z1, double x2, double y2, double z2, int color, double *color_vect );
extern void g3d_draw_rep_obj ( p3d_jnt *jnt, double a, int num );
extern void g3d_draw_simple_box(double x1,double x2,double y1,double y2,double z1,double z2, int color, int fill, double width);
extern void g3d_draw_a_box ( double x1, double x2, double y1, double y2, double z1, double z2, int c, int fill );
extern int BoxInFrustum_obj ( p3d_obj *o, G3D_Window *win );
extern int SphereInFrustum ( G3D_Window *win, float x, float y, float z, float radius );
extern void g3d_extract_frustum ( G3D_Window *win );
extern int g3d_calcule_resolution ( G3D_Window *win, p3d_poly *p );
extern void g3d_draw_primitive ( G3D_Window *win, p3d_poly *p, int fill );
extern void g3d_draw_poly ( p3d_poly *p, G3D_Window *win, int coll, int fill );
extern void g3d_draw_poly_with_color(p3d_poly *p,G3D_Window *win,int coll,int fill,double color);
extern void g3d_draw_poly_special ( p3d_poly *p, G3D_Window *win, int color );
extern void g3d_init_all_poly_gouraud ( void );
extern void g3d_init_all_poly ( void );
extern void g3d_delete_all_poly ( int mode );
extern void GLUerrorCallback ( GLenum errorCode );
extern void g3d_init_box2 ( p3d_poly *p, int fill );
extern void g3d_init_box ( p3d_poly* p, int fill );
extern void g3d_init_cylindreGLU2 ( p3d_poly* p, int fill, float base_radius, float top_radius );
extern void g3d_init_cylindreGLU ( p3d_poly* p, int fill, float base_radius, float top_radius );
extern void g3d_init_sphereGLU ( p3d_poly* p, int fill );
extern void g3d_init_polyquelconque ( p3d_poly *p, int fill );
extern void g3d_init_poly ( p3d_poly *p, int fill );
extern void g3d_delete_poly ( p3d_poly *p, int mode );
extern void g3d_draw_solid_cone(double radius, double height, int nbSegments);
extern void g3d_draw_arrow(p3d_vector3 p1, p3d_vector3 p2, double red, double green, double blue);
extern void g3d_drawRepMoveObj(p3d_matrix4 frame ,double length, int axis);
extern void g3d_drawSphMoveObj(p3d_matrix4 mat ,double length);
extern int g3d_lineLineIntersect( p3d_vector3 p1, p3d_vector3 p2, p3d_vector3 p3, p3d_vector3 p4, p3d_vector3 *pa, p3d_vector3 *pb, double *mua, double *mub);
extern void g3d_draw_frame(p3d_matrix4 frame, double length);
extern int g3d_draw_cylinder(p3d_vector3 p1, p3d_vector3 p2, double radius, unsigned int nbSegments);
extern void g3d_rgb_from_int(int i, double color[4]);
extern void g3d_rgb_from_hue(double x, double color[4]);
extern int g3d_draw_robot_joints(p3d_rob* robot, double size);
extern int g3d_draw_p3d_polyhedre ( p3d_polyhedre *polyhedron );
extern int g3d_circle_table(double **sint, double **cost, const int n);
extern void g3d_draw_solid_sphere(double radius, int nbSegments);
extern void g3d_draw_solid_sphere(double x, double y, double z, double radius, int nbSegments);
extern void g3d_draw_solid_cylinder(double radius, double length, int nbSegments);
extern int g3d_draw_body_normals(p3d_obj *obj, double length);
extern int g3d_draw_robot_normals(p3d_rob *robot, double length);
extern int g3d_draw_body_vertex_normals(p3d_obj *obj, double length);
extern int g3d_draw_robot_vertex_normals(p3d_rob *robot, double length);
#endif /* __CEXTRACT__ */
