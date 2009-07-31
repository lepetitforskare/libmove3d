/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

/**
 * p3d_SetCurrentNearNode
 * Set which is the near/best node found in
 * the roadmap.
 * @param[In]: the near/best node found in
 * the roadmap.
 */
void p3d_SetCurrentNearNode(p3d_node *NearNodePt);

/**
 * p3d_GetCurrentNearNode
 * Get  which is the near/best node found in
 * the roadmap.
 * @return: the near/best node found in
 * the roadmap.
 */
  p3d_node* p3d_GetCurrentNearNode(void);


extern void p3d_set_DMAX ( double D );
extern double p3d_get_DMAX ( void );
extern int p3d_GetStopValue(void);
extern void  p3d_SetStopValue(int);
extern double p3d_calc_DMAX ( p3d_rob *robotPt );
extern void p3d_set_COMP_NODES ( int N );
extern int p3d_get_COMP_NODES ( void );
extern void p3d_set_NB_NODES ( int N );
extern int p3d_get_NB_NODES ( void );
extern void p3d_set_EXPAND_CHOICE ( int N );
extern int p3d_get_EXPAND_CHOICE ( void );
extern void p3d_set_RANDOM_CHOICE ( int N );
extern int p3d_get_RANDOM_CHOICE ( void );
extern void p3d_set_SAMPLING_CHOICE ( int N );
extern int p3d_get_SAMPLING_CHOICE ( void );
extern void p3d_set_EXPAND_NB_NODES ( int N );
extern int p3d_get_EXPAND_NB_NODES ( void );
extern void p3d_set_EXPAND_NB_WALK ( int N );
extern int p3d_get_EXPAND_NB_WALK ( void );
extern void p3d_set_EXPAND_OPTION ( int N );
extern int p3d_get_EXPAND_OPTION ( void );
extern void p3d_set_EXPAND_DQ ( double *dq );
extern double *p3d_get_EXPAND_DQ ( void );
extern void p3d_set_EXPAND_DELTA ( double *dq );
extern double *p3d_get_EXPAND_DELTA ( void );
extern void p3d_set_PLANNER_CHOICE ( int N );
extern int p3d_get_PLANNER_CHOICE ( void );
extern void p3d_set_MOTION_PLANNER ( int N );
extern int p3d_get_MOTION_PLANNER ( void );
extern void p3d_set_SORTING ( int N );
extern int p3d_get_SORTING ( void );
extern void p3d_set_rrt_pb_mode ( int N );
extern int p3d_get_rrt_pb_mode ( void );
extern void p3d_set_flag_passive_parameters_for_planner( int val );
extern int p3d_get_flag_passive_parameters_for_planner( void );
extern void p3d_set_ORIENTED ( int N );
extern int p3d_get_ORIENTED ( void );
extern void p3d_set_nbtry_DD(int N);
extern int p3d_get_nbtry_DD(void);
extern void p3d_set_NB_OPTIM ( int N );
extern int p3d_get_NB_OPTIM ( void );
extern void p3d_set_NB_specific(int N);
extern int p3d_get_NB_specific(void);
extern void p3d_set_frames_for_metric(int irf, int imf);
extern double p3d_get_tmax(void);
extern void p3d_set_tmax(double tmax);
extern double p3d_get_optimization_time(void);
extern void p3d_set_optimization_time(double optTime);
extern int p3d_get_ik_choice(void);
extern void p3d_set_ik_choice(int ik_choice);
extern void p3d_init_ik_draw_current(void);
extern void p3d_set_ik_draw(int ik_draw, int value);
extern int* p3d_get_ik_draw(void);
extern int p3d_get_planning_type(void);
extern void p3d_set_planning_type(int planning_type);
extern int p3d_GetIsWeightedRotations(void);
extern void p3d_SetIsWeightedRotations(int IsWeightedRota);//start path defom
extern void p3d_set_dof1(int dof1);
extern int p3d_get_dof1(void);
extern void p3d_set_dof2(int dof2);
extern int p3d_get_dof2(void);
extern void p3d_set_cycles(int cycles);
extern int p3d_get_cycles(void);
extern int p3d_set_test_reductib(int test_reduc);
extern int p3d_get_test_reductib(void);
extern void p3d_set_is_visibility_discreet(int vis);
extern int p3d_get_is_visibility_discreet(void);
extern void p3d_set_nretract(int nretract);
extern int p3d_get_nretract(void);
extern void p3d_set_num_draw_proj(int num);
extern int p3d_get_num_draw_proj(void);
extern void p3d_set_Nstep(int nStep);
extern int p3d_get_Nstep(void);
extern void p3d_set_show_cycle_only(int cycle);
extern int p3d_get_show_cycle_only(void);
extern void p3d_set_retract_search_type(int type);
extern int p3d_get_retract_search_type(void);
extern void p3d_set_nretract_max(int nretract);
extern int p3d_get_nretract_max(void);
extern void p3d_set_is_general_proj(int gen_proj);
extern int p3d_get_is_general_proj(void);
extern void p3d_set_max_connect(int n);
extern int p3d_get_max_connect(void);
extern void p3d_set_gvis(int n);
extern int p3d_get_gvis(void);
extern void p3d_set_saveInfoInFile(int saveInfoInFile);
extern int p3d_get_saveInfoInFile(void);
#ifdef MULTIGRAPH
extern void p3d_set_multiGraph(int multiGraph);
extern int p3d_get_multiGraph(void);
#endif
extern void p3d_set_costComputation(int cost);
extern int p3d_get_costComputation(void);

extern int p3d_get_frames_for_metric(p3d_rob *robotPt, p3d_matrix4 **ref_frame, p3d_matrix4 **mob_frame);

#endif /* __CEXTRACT__ */
