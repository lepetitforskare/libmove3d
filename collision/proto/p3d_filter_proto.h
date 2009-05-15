/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern int p3d_filter_mechanism_requested ( void );
extern void p3d_filter_switch_filter_mechanism ( int waarde );
extern void p3d_filter_set_current_robot_nr ( int waarde );
extern void p3d_filter_setoff_linkfilter ( void );
extern void p3d_filter_seton_linkfilter ( void );
extern void p3d_filter_set_draw_robot_box ( int i );
extern int must_draw_robot_box ( void );
extern void p3d_filter_switch_draw_robot_box ( void );
extern int p3d_get_robotboxlist ( void );
extern void p3d_set_robotboxlist ( int newvalue );
extern void p3d_reset_robotboxlist ( void );
extern void p3d_set_filterbox ( double x1, double x2, double y1, double y2, double z1, double z2 );
extern void p3d_get_filter_box ( double *x1, double *x2, double *y1, double *y2, double *z1, double *z2 );
extern void p3d_filter_init_filter ( void );
extern void p3d_filter_init_robot_box ( void );
extern void p3d_filter_set_current_active_link ( int linknb );
extern int p3d_filter_relevant_pair ( p3d_poly *objn );
extern int p3d_filter_relevant_facet ( p3d_poly *current_polyhedron, int facet_nr, int type );
extern int p3d_filter_relevant_poly ( p3d_poly *objn );
extern int p3d_filter_needs_init ( void );
extern void p3d_filter_cleanup ( void );

#endif /* __CEXTRACT__ */
