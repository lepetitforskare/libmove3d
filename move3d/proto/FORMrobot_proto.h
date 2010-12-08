/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern void position_robot_at_beginning ( int ir, p3d_traj *trajPt );
extern int calc_real_dof ( void );
extern void g3d_create_robot_form ( int ir );
extern double g3d_get_qi ( int i );
extern double g3d_get_qf ( int i );
extern void g3d_add_traj ( char *name, int i );
extern void g3d_add_config_to_ui(char* name,p3d_rob* rob,double* q);
extern int draw_localpath ( p3d_rob *robotPt, p3d_localpath *localpathPt, double dmax );
extern int lpc_writepath_fct ( p3d_rob* robot, p3d_localpath* curLp );
extern void p3d_read_path ( const char *file );
extern void g3d_delete_robot_form ( int ir );
extern void FORMrobot_update ( int ir );
extern void CB_showtraj_obj(FL_OBJECT *ob, long arg);
extern void MovieDrawGraph(void);
#endif /* __CEXTRACT__ */
