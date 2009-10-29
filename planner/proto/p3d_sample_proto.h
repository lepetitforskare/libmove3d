/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern void p3d_init_random_seed ( int seed );
extern void p3d_init_random ( void );
extern double p3d_random ( double a, double b );
extern double NormalRand ( double Sigma_d_a );
extern void p3d_bridge_shoot(p3d_rob *r, configPt q, int sample_passive);//mokhtar
extern void p3d_obprm_shoot(p3d_rob *r, configPt q, int sample_passive);//mokhtar
extern int p3d_shoot ( p3d_rob *robotPt, configPt q , int sample_passive);
extern int p3d_standard_shoot(p3d_rob *robotPt, configPt q, int sample_passive);
extern int p3d_shoot_inside_box(p3d_rob *robotPt, configPt q, configPt box_env_small[2], int sample_passive);
/**
 * p3d_RandDirShoot
 * Shoot a pseudo-random direction
 * (still better than space sampling)
 * this direction is not normalized !
 * @param[In] robotPt: the current robot
 * @param[out] q: the configuration as random direction
 * @param[In] sample_passive: TRUE if the passive dofs have to
 * be taken into consideration
 * @return: Currently, it returns always TRUE has a new 
 * direction can always success, but it could change.
 */

int p3d_RandDirShoot(p3d_rob* robotPt, configPt q, int sample_passive);
int p3d_RandNShpereDirShoot(p3d_rob* robotPt, configPt q, int sample_passive);
bool p3d_isOutOfBands(p3d_rob* robotPt, configPt q, int sample_passive);

#endif /* __CEXTRACT__ */
