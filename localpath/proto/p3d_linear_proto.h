/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern p3d_lin_data * p3d_alloc_spec_lin_localpath ( configPt q_i, configPt q_f );
extern p3d_localpath * p3d_alloc_lin_localpath ( p3d_rob *robotPt, configPt q_i, configPt q_f, int lp_id, int is_valid );
extern double p3d_lin_dist ( p3d_rob *robotPt, p3d_localpath *localpathPt );
extern void p3d_destroy_lin_data ( p3d_rob* robotPt, p3d_lin_data* lin_dataPt );
extern void p3d_lin_destroy ( p3d_rob* robotPt, p3d_localpath* localpathPt );
extern configPt p3d_lin_config_at_distance ( p3d_rob *robotPt, p3d_localpath *localpathPt, double distance );
extern double p3d_lin_stay_within_dist ( p3d_rob* robotPt, p3d_localpath* localpathPt, double parameter, int dir, double *distances );
extern p3d_localpath *p3d_copy_lin_localpath ( p3d_rob* robotPt, p3d_localpath* localpathPt );
extern p3d_localpath *p3d_extract_lin ( p3d_rob *robotPt, p3d_localpath *localpathPt, double l1, double l2 );
extern double p3d_lin_cost ( p3d_rob *robotPt, p3d_localpath *localpathPt );
extern p3d_localpath *p3d_simplify_lin ( p3d_rob *robotPt, p3d_localpath *localpathPt, int *need_colcheck );
extern int p3d_write_lin ( FILE *file, p3d_rob* robotPt, p3d_localpath* localpathPt );
extern void print_LIN ( p3d_rob *robotPt, p3d_localpath *localpathPt );
extern p3d_localpath *p3d_linear_localplanner ( p3d_rob *robotPt, configPt qi, configPt qf, int* ikSol );
extern void lm_destroy_linear_params ( p3d_rob *robotPt, void *paramPt );
extern p3d_localpath *p3d_read_linear_localpath ( p3d_rob *robotPt, FILE *file, double version );

#endif /* __CEXTRACT__ */
