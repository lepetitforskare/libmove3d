/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern const char * p3d_jnt_rotate_get_dof_name ( int i_dof );
extern void p3d_jnt_rotate_stay_within_dist ( p3d_stay_within_dist_data * prev_data, p3d_jnt * jntPt, p3d_stay_within_dist_data * data, double * distance, configPt q_init, configPt q_max_param, double max_param, double * reach_param );
extern void p3d_jnt_rotate_calc_jnt_mat ( p3d_jnt * jntPt );
extern void p3d_jnt_rotate_calc_mat_jac ( p3d_jnt * jntPt );
extern p3d_jnt * p3d_jnt_rotate_create ( p3d_matrix4 pos, double * v, double * vmin, double * vmax, double * vmin_rand, double * vmax_rand, double * velocity_max, double *acceleration_max, double *jerk_max, double * torque_max, double * param );
extern p3d_jnt * p3d_jnt_rotate_create_deg ( p3d_matrix4 pos, double * v, double * vmin, double * vmax, double * vmin_rand, double * vmax_rand, double * velocity_max, double *acceleration_max, double *jerk_max, double * torque_max, double * param );

//start path deform
extern void p3d_jnt_rotate_stay_within_sphere(p3d_stay_within_dist_data * prev_data,
                                       p3d_jnt * jntPt,
                                       p3d_stay_within_dist_data * data,
                                       double * distance,
                                       double * reach_param);
//end path deform
#endif /* __CEXTRACT__ */
