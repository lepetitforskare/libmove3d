/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.
 *
 *   Created: Wed Jun 23 14:30:05 2004
 */
#ifndef __CEXTRACT__

extern void p3d_set_TEST_PHASE ( int val );
extern void p3d_constraint_get_nb_param ( const char *namecntrt, int *nb_Dofpasiv, int *nb_Dofactiv, int *nb_Dval, int *nb_Ival );
extern int p3d_create_constraint ( p3d_cntrt_management * cntrt_manager, const char *namecntrt, int nb_pas, p3d_jnt **pas_jntPt, int *pas_jnt_dof, int *Dofpassiv, int nb_act, p3d_jnt **act_jntPt, int *act_jnt_dof, int *Dofactiv, int nb_Dval, double *Dval, int nb_Ival, int *Ival, int ct_num, int state );
extern int p3d_update_constraint ( p3d_cntrt * ct, int state );
extern int p3d_constraint_dof ( const char *namecntrt, int nb_pas, int *pas_jnt_num, int *pas_jnt_dof, int nb_act, int *act_jnt_num, int *act_jnt_dof, int nb_Dval, double *Dval, int nb_Ival, int *Ival, int ct_num, int state );
extern int p3d_constraint_dof_r ( p3d_rob *robotPt, const char *namecntrt, int nb_pas, int *pas_jnt_num, int *pas_jnt_dof, int nb_act, int *act_jnt_num, int *act_jnt_dof, int nb_dval, double *Dval, int nb_ival, int *Ival, int ct_num, int state );
extern int p3d_constraint ( const char *namecntrt, int nb_pas, int *Jpasiv, int nb_act, int *Jactiv, int nb_dval, double *Dval, int nb_ival, int *Ival, int ct_num, int state );
extern int p3d_constraint_r ( p3d_rob *robotPt, const char *namecntrt, int nb_pas, int *Jpasiv, int nb_act, int *Jactiv, int nb_dval, double *Dval, int nb_ival, int *Ival, int ct_num, int state );
extern p3d_cntrt_management * p3d_create_cntrt_manager ( int nb_dof );
extern void p3d_destroy_cntrt_manager ( p3d_cntrt_management * cntrt_manager );
extern int p3d_clear_cntrt_manager ( p3d_cntrt_management * cntrt_manager );
extern int p3d_copy_cntrt_manager_into ( p3d_cntrt_management * cntrt_manager_srcPt, p3d_cntrt_management * cntrt_manager_destPt );
extern p3d_cntrt *p3d_get_current_cntrt ( void );
extern int p3d_set_current_cntrt ( p3d_cntrt *ct );
extern int p3d_set_cntrt_Tatt ( int ct_num, double *matelem );
extern int p3d_set_cntrt_Tatt_r ( p3d_rob *r, int ct_num, double *matelem );
extern int p3d_set_cntrt_Tatt2(int ct_num, double *matelem);
extern int p3d_set_cntrt_Tatt2_r(p3d_rob *r, int ct_num, double *matelem);
extern void p3d_set_singularity(int constraint, int nJnt, int *jnts, double *values);
extern void p3d_set_singular_rel(int constraint, int singularity, int nPaires, int *classes);
extern int p3d_update_all_jnts_state ( int mode );
extern int p3d_update_jnts_state(p3d_cntrt_management * cntrt_manager, p3d_cntrt *ct, int cntrt_state);
extern void p3d_col_deactivate_cntrt_manager_pairs ( p3d_cntrt_management * cntrt_manager );
extern void p3d_col_deactivate_cntrt_manager_pairs_into ( p3d_cntrt_management * cntrt_manager, p3d_collision_pair * col_pairPt );
extern void p3d_col_deactivate_cntrt_pairs ( void );
extern void p3d_col_deactivate_one_cntrt_pairs_into ( p3d_cntrt *ct, p3d_collision_pair * col_pairPt );
extern void p3d_col_deactivate_one_cntrt_pairs ( p3d_cntrt *ct );
extern void p3d_col_activate_one_cntrt_pairs ( p3d_cntrt *ct );
extern void p3d_col_deactivate_enchained_cntrts_pairs_into ( p3d_cntrt *ct, p3d_cntrt *ect, int rob_dof, p3d_collision_pair * col_pairPt );
extern void p3d_col_deactivate_enchained_cntrts_pairs ( p3d_cntrt *ct, p3d_cntrt *ect, int rob_dof );
extern void p3d_col_activate_enchained_cntrts_pairs ( p3d_cntrt *ct, p3d_cntrt *ect, int rob_dof );
extern void p3d_enchain_cntrt ( p3d_cntrt *ect, int rob_dof, int arg );
extern void p3d_unchain_cntrts ( p3d_cntrt *ct );
extern void p3d_reenchain_cntrts ( p3d_cntrt *ct );
extern p3d_cntrt * p3d_create_generic_cntrts ( p3d_cntrt_management * cntrt_manager, const char *namecntrt, int nb_passif, p3d_jnt ** pas_jntPt, int * pas_jnt_dof, int * pas_rob_dof, int nb_actif, p3d_jnt ** act_jntPt, int * act_jnt_dof, int * act_rob_dof );
extern int p3d_actived_cntrts ( p3d_cntrt_management *cntrt_manager );
extern int p3d_check_cntrts_at_conf ( p3d_rob *robotPt, configPt q );
extern int p3d_check_cntrts_at_conf_multisol ( p3d_rob *robotPt, configPt q, configPt qp, double dl );
extern int p3d_cntrt_localpath_classic_test ( p3d_rob *robotPt, p3d_localpath *localpathPt, double *Kpath );

extern void p3d_init_iksol(p3d_cntrt_management *cntrt_manager);
extern void p3d_realloc_iksol(p3d_cntrt_management *cntrt_manager);
extern void p3d_reset_iksol(p3d_cntrt_management *cntrt_manager);
extern void p3d_get_iksol_vector ( p3d_cntrt_management *cntrt_manager, int ***iksol );
extern double*** p3d_get_ikSolConfig_vector(p3d_cntrt_management *cntrt_manager);
extern int p3d_get_ikSpecific_solution (p3d_cntrt_management *cntrt_manager, int ctNum, int sol);
extern double* p3d_get_ikSpecific_config(p3d_cntrt_management *cntrt_manager, int ctNum, int sol);
extern int p3d_is_multisol ( p3d_cntrt_management *cntrt_manager );
extern int p3d_compare_iksol(p3d_cntrt_management *cntrt_manager, int *iksol1, int *iksol2);
extern void p3d_get_iksol_vector_for_solution(p3d_cntrt_management *cntrt_manager, int **iksol_dst, int sol);
extern void p3d_print_iksol(p3d_cntrt_management *cntrt_manager, int* iksol);
extern int* p3d_get_niksol_vector ( p3d_cntrt_management *cntrt_manager);
extern void p3d_set_iksol_elem ( int ctNum, int val );
extern int p3d_get_random_ikSol(p3d_cntrt_management *cntrt_manager, int ctNum);
extern void p3d_set_robot_iksol(p3d_rob * r, int *ikSol);
extern void p3d_get_non_sing_iksol(p3d_cntrt_management *cntrt_manager, int *iksol_src1, int *iksol_src2, int **iksol_dst);
extern void p3d_copy_iksol ( p3d_cntrt_management *cntrt_manager, int *iksol_src, int **iksol_dst );
extern void p3d_destroy_iksol ( p3d_cntrt_management *cntrt_manager );
extern void p3d_destroy_specific_iksol(p3d_cntrt_management *cntrt_manager, int *iksol);
extern void p3d_destroy_specific_niksol ( p3d_cntrt_management *cntrt_manager, int *niksol );
extern void p3d_mark_for_singularity(p3d_cntrt_management *cntrt_manager, int ctNum);
extern void p3d_unmark_for_singularity(p3d_cntrt_management *cntrt_manager, int ctNum);
extern int p3d_local_conf_correction(p3d_rob *robotPt, configPt q);
extern int p3d_test_singularity_connexion(p3d_cntrt_management *cntrt_manager, p3d_node *N1, p3d_node *N2);
extern int p3d_get_nb_ikSol(p3d_cntrt_management *cntrt_manager);
extern int p3d_get_random_singularity(p3d_cntrt *ct);
extern p3d_cntrt** p3d_getJointCntrts(p3d_cntrt_management * cntrt_manager, int joint, int *nbCntrts);
extern int p3d_compare_iksol_2(p3d_cntrt_management *cntrt_manager, int *iksol1, int *iksol2);
extern p3d_cntrt * getJntFixedCntrt(p3d_cntrt_management * cntrt_manager, int jntNum);
#endif /* __CEXTRACT__ */
