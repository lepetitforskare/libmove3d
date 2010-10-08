/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern int p3d_compute_traj_nloc ( p3d_traj *trajPt );
extern double p3d_compute_traj_rangeparam ( p3d_traj *trajPt );
extern double p3d_compute_traj_length ( p3d_traj *trajPt );
extern double p3d_compute_traj_cost(p3d_traj *trajPt);
extern configPt p3d_config_at_param_along_traj ( p3d_traj *trajPt, double parameter );
extern configPt p3d_config_at_distance_along_traj ( p3d_traj *trajPt, double distance );
extern void p3d_traj_stay_within_dist ( p3d_traj *trajPt, int dir, double *distances, double *localparamPt );
extern double p3d_ends_and_length_traj ( p3d_traj* trajPt, configPt *qinitPt, configPt *qgoalPt );
extern p3d_traj *p3d_create_empty_trajectory ( p3d_rob *robotPt );
extern p3d_traj *p3d_create_traj_by_copy ( p3d_traj *source );
extern int p3d_concat_traj ( p3d_traj *traj1Pt, p3d_traj *traj2Pt );
extern int p3d_replace_traj ( p3d_traj *traj1Pt, double upval1, double upval2, p3d_traj *traj2Pt );
extern p3d_traj* p3d_get_sub_traj(p3d_traj *traj1Pt, p3d_localpath* startLp, p3d_localpath* endLp);

//start path deform
extern int p3d_destroy_traj(p3d_rob* robotPt, p3d_traj* traj);
extern void p3d_compute_traj_project(p3d_rob* robotPt, p3d_traj* traj1Pt, p3d_traj* traj2Pt, int nstep);
extern int p3d_is_one_project(p3d_rob* robotPt, p3d_traj* traj1Pt, p3d_traj* traj2Pt, double param1, double param2);
extern int p3d_test_projection(p3d_rob* robotPt , p3d_traj *traj1Pt, p3d_traj* traj2Pt, int nstep);
extern int p3d_is_projectable(p3d_rob* robotPt , p3d_traj* traj1Pt, p3d_traj* traj2Pt, int nstep, double *p1, double *p2);
extern p3d_traj* p3d_build_cycle_from_trajs(p3d_rob* robotPt, p3d_traj* traj1Pt, p3d_traj* traj2Pt);
extern int p3d_is_reductible_cycle(p3d_rob* robotPt, p3d_traj* trajPt, int nstep);
extern int p3d_sort_project_point(void* proj_point1, void* proj_point2);
extern p3d_project_point ** p3d_init_proj_point_table(int nstep);
extern double p3d_compute_proj_point_cost(p3d_project_point* proj_point, int nstep);
extern void p3d_free_proj_point_table(p3d_project_point ** proj_table, int nstep);
extern p3d_traj* p3d_invert_traj(p3d_rob* robotPt, p3d_traj* traj);
extern p3d_traj* p3d_extract_traj_from_traj(p3d_traj *traj1Pt, double upval1, double upval2);
extern int p3d_check_traj_continuity(p3d_rob* robotPt, p3d_traj* trajPt);
extern p3d_traj*  p3d_create_traj_from_list_nodes(p3d_graph* G,dbl_list* list_node);
extern void p3d_ikSol_at_param_along_traj(p3d_traj *trajPt, double parameter, int ** ikSol);
//end path deform






/**
 * p3d_GetRandSuccesConfAlongTraj
 * Get 3 successive configurations 
 * at a random parameter along a given path. 
 * The path distance between these configuration id given by 
 * dMax*extendStepParam (equal to the step in the extend mode 
 * of diffusion.process).
 * param[In] trajPt: the given trajectory
 * param[Out] qPrevPt: 1st config along the path
 * param[Out] qCurrentPt: 2nd config along the path
 * param[Out] qNextPt: 3rd config along the path
 * param[Out] prevDistPt parameter along the path of the 1st config
 * param[Out] randDistPt parameter along the path of the 2nd config
 * param[Out] nextDistPt parameter along the path of the 3rd config
 */
void p3d_GetRandSuccesConfAlongTraj(p3d_traj* trajPt,  configPt* qPrevPt, 
				    configPt*  qCurrentPt, configPt* qNextPt, 
				    double* prevDistPt, double* randDistPt, 
				    double* nextDistPt);


/**
 * p3d_createThreeConfTraj
 * create a trajectory from 3 configurations
 * Creates 2 localpaths with the local method 
 * associated to the robot 
 * param[In] robotPt: the current robot
 * param[In] qPrev: the first configuration (init of the traj)
 * param[In] qNew: the second configuration
 * param[In] qNext: the last configuration (end of the traj)
 * @return: the created trajectory
 */  
p3d_traj* p3d_createThreeConfTraj(p3d_rob* robotPt, configPt qPrev, 
				  configPt qNew, configPt qNext);

#endif /* __CEXTRACT__ */
