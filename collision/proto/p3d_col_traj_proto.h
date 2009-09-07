/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

#include <vector>
extern std::vector<double> aveBBDist;

extern void p3d_col_set_microcollision ( int value );
extern int p3d_col_get_microcollision ( void );
extern int change_position_robot_without_obj ( p3d_rob *robotPt, p3d_localpath *localpathPt, double l );
extern int change_position_robot_without_obj_multisol ( p3d_rob *robotPt, p3d_localpath *localpathPt, double l, double dl, configPt qp );
extern int change_position_robot_multisol( p3d_rob *robotPt, p3d_localpath *localpathPt, double l, double dl, configPt qp );

extern void p3d_set_current_q_inv(p3d_rob *robotPt, p3d_localpath *localpathPt, configPt q_inv); 
extern int p3d_get_current_q_inv(p3d_rob *robotPt, configPt *q_invPt);

extern int p3d_col_test_localpath_classic ( p3d_rob *robotPt, p3d_localpath *localpathPt, int *ntest, double *Kpath, configPt *q_atKpath );
extern int p3d_col_test_localpath_classic_multisol ( p3d_rob *robotPt, p3d_localpath *localpathPt, int *ntest, double *Kpath, configPt *q_atKpath );
extern int p3d_col_and_cntrts_test_localpath_classic ( p3d_rob *robotPt, p3d_localpath *localpathPt, int *ntest, double *Kpath, configPt *q_atKpath );
extern int p3d_col_and_cntrts_test_localpath_classic_multisol ( p3d_rob *robotPt, p3d_localpath *localpathPt, int *ntest, double *Kpath, configPt *q_atKpath );
extern int p3d_onlycol_test_localpath_classic ( p3d_rob *robotPt, p3d_localpath *localpathPt, int *ntest, double *Kpath, configPt *q_atKpath );
extern void p3d_col_env_free_memory_traj_col_tab ( void );
extern void p3d_col_env_set_traj_method ( p3d_traj_test_type type );
extern p3d_traj_test_type p3d_col_env_get_traj_method ( void );
extern int p3d_col_test_localpath ( p3d_rob *robotPt, p3d_localpath *localpathPt, int *ntest );
extern int p3d_col_test_traj ( p3d_rob *robotPt, p3d_localpath *localpathPt, int *ntest );

//start path deform
extern int p3d_test_visibility_edge(p3d_rob *robotPt, configPt q0, configPt q_edge1,configPt q_edge2);
//end path deform
#endif /* __CEXTRACT__ */
