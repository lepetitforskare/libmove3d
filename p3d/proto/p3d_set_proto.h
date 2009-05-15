/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern int p3d_set_env_box ( double x1, double x2, double y1, double y2, double z1, double z2 );
extern int p3d_set_robot_box ( double x1, double x2, double y1, double y2, double z1, double z2, double t1, double t2, double u1, double u2, double v1, double v2 );
extern int p3d_set_robot_box_deg ( double x1, double x2, double y1, double y2, double z1, double z2, double t1, double t2, double u1, double u2, double v1, double v2 );
extern void p3d_set_user_config_min_bounds_deg ( p3d_rob * robotPt, double * dtab, int n );
extern void p3d_set_user_config_max_bounds_deg ( p3d_rob * robotPt, double * dtab, int n );
extern void p3d_set_this_robot_radius ( p3d_rob *robotPt, double radius );
extern void p3d_set_robot_radius ( double radius );
extern void p3d_set_env_graphic_dmax ( double val );
extern void p3d_set_env_object_tolerance ( double obj_tol );
extern void p3d_set_env_dmax ( double dmax );
extern void p3d_set_ROBOT_GOTO ( configPt q );
extern void p3d_set_ROBOT_START ( configPt q );
extern void p3d_set_ROBOT_GOTO_deg_to_rad ( configPt q_deg );
extern void p3d_set_ROBOT_START_deg_to_rad ( configPt q_deg );
extern int p3d_set_new_robot_config ( const char * name, const configPt q, int * ikSol, config_namePt config );
extern void p3d_set_robot_config_deg_to_rad ( const char * name, configPt q_deg );
extern void p3d_set_robot_steering_method ( const char * name );
extern void p3d_set_robot_singularty(p3d_rob *robotPt);
extern void p3d_pushRobotTransitionsDegToRad(configPt q_deg, int position);
#endif /* __CEXTRACT__ */
