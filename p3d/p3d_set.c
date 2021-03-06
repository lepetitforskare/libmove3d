/*
 * Copyright (c) 2001-2014 LAAS/CNRS
 * All rights reserved.
 *
 * Redistribution  and  use  in  source  and binary  forms,  with  or  without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice and this list of conditions.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice and  this list of  conditions in the  documentation and/or
 *      other materials provided with the distribution.
 *
 * THE SOFTWARE  IS PROVIDED "AS IS"  AND THE AUTHOR  DISCLAIMS ALL WARRANTIES
 * WITH  REGARD   TO  THIS  SOFTWARE  INCLUDING  ALL   IMPLIED  WARRANTIES  OF
 * MERCHANTABILITY AND  FITNESS.  IN NO EVENT  SHALL THE AUTHOR  BE LIABLE FOR
 * ANY  SPECIAL, DIRECT,  INDIRECT, OR  CONSEQUENTIAL DAMAGES  OR  ANY DAMAGES
 * WHATSOEVER  RESULTING FROM  LOSS OF  USE, DATA  OR PROFITS,  WHETHER  IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR  OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.                                  
 *
 * Siméon, T., Laumond, J. P., & Lamiraux, F. (2001). 
 * Move3d: A generic platform for path planning. In in 4th Int. Symp.
 * on Assembly and Task Planning.
 */
#include "Util-pkg.h"
#include "P3d-pkg.h"

#ifdef P3D_LOCALPATH
#include "Localpath-pkg.h"
#endif

#ifdef P3D_PLANNER
#include "Planner-pkg.h"
#endif

#ifdef LIGHT_PLANNER
#include "lightPlanner/proto/lightPlannerApi.h"
#include "lightPlanner/proto/ManipulationArmData.hpp"
#endif

/*******************************************************************/

/**************************************************/
/* Fonction changeant les coordonnees de la boite */
/* englobante de l'environnement courant          */
/* In : les coordonnees de la boite               */
/* Out :                                          */
/**************************************************/
int p3d_set_env_box(double x1, double x2, double y1, double y2, double z1, double z2)
{ 

 pp3d_env e = (pp3d_env)p3d_get_desc_curid(P3D_ENV);
 pp3d_box b;
  if(!e) {
    PrintWarning(("MP: p3d_set_env_box: no env defined\n"));
    return(FALSE);
  }

  if((x1>=x2)||(y1>=y2)||(z1>=z2)) {
    PrintWarning(("MP: p3d_set_env_box: wrong values\n"));
    return(FALSE);
  }
  b = &e->box;
  b->x1 = x1; b->x2 = x2; b->y1 = y1; b->y2 = y2; b->z1 = z1; b->z2 = z2;

  /* begin modif Carl */
  e->dmax = sqrt(SQR(b->x2-b->x1)+SQR(b->y2-b->y1)+SQR(b->z2-b->z1))/200.;
  e->graphic_dmin = sqrt(SQR(b->x2-b->x1)+SQR(b->y2-b->y1)+SQR(b->z2-b->z1))/200.;
  /* end modif Carl */

  return(TRUE);
}

void p3d_set_env_background_color(double r, double g, double b)
{ 
  pp3d_env e = (pp3d_env)p3d_get_desc_curid(P3D_ENV);

  e->background_color[0]= r;
  e->background_color[1]= g;
  e->background_color[2]= b;
}


/*--------------------------------------------------------------------------*/
/*!
 * \brief Set the bounding box of the robot. The angles in input are in radian
 *
 * Note: This function check the validities of the bounds. It sets both
 *       bounds and random bounds to the same value. When min and max bounds
 *       are equal, it sets the degree of freedom not usable by user, if not,
 *       it sets the dof usable.
 *
 * \param x1: Minimum bound for the translation on x axis.
 * \param x2: Maximum bound for the translation on x axis.
 * \param y1: Minimum bound for the translation on y axis.
 * \param y2: Maximum bound for the translation on y axis.
 * \param z1: Minimum bound for the translation on z axis.
 * \param z2: Maximum bound for the translation on z axis.
 * \param t1: Minimum bound for the rotation on x axis in radian.
 * \param t2: Maximum bound for the rotation on x axis in radian.
 * \param u1: Minimum bound for the rotation on y axis in radian.
 * \param u2: Maximum bound for the rotation on y axis in radian.
 * \param v1: Minimum bound for the rotation on z axis in radian.
 * \param v2: Maximum bound for the rotation on z axis in radian.
 *
 * \return TRUE if success, else FALSE
 *
 * \warning This function must not be used anymore.
 */
int p3d_set_robot_box(double x1, double x2, double y1, double y2, 
          double z1, double z2, double t1, double t2,
          double u1, double u2, double v1, double v2)
{
  double ex1,ex2,ey1,ey2,ez1,ez2;
  double vmin, vmax;
  pp3d_rob robotPt = (pp3d_rob)p3d_get_desc_curid(P3D_ROBOT);
  pp3d_jnt jntPt = robotPt->joints[0];

  if(!robotPt) {
    PrintWarning(("MP: p3d_set_robot_box: no env defined\n"));
    return(FALSE);
  }

  if((x1>x2)||(y1>y2)||(z1>z2)||(t1>t2)) {
    PrintWarning(("MP: p3d_set_robot_box: wrong values\n"));
    return(FALSE);
  }

  p3d_get_env_box(&ex1,&ex2,&ey1,&ey2,&ez1,&ez2);
  if((x1 < ex1)||(x2 > ex2)||(y1<ey1)||(y2>ey2)||
     (z1<ez1)||(z2>ez2)||(t1<-M_PI)||(t2>M_PI)){ 
    PrintError(("MP: p3d_set_robot_box: change values to remain inside env\n"));
  }

  vmin = MAX(x1, ex1);
  vmax = MAX(vmin, MIN(x2, ex2));
  p3d_jnt_set_dof_bounds(jntPt, 0, vmin, vmax); 
  p3d_jnt_set_dof_rand_bounds(jntPt, 0, vmin, vmax);
  if(LEQ(vmax, vmin))
    { p3d_jnt_set_dof_is_user(jntPt, 0, FALSE); }
  else
    { p3d_jnt_set_dof_is_user(jntPt, 0, TRUE); }
  vmin = MAX(y1, ey1);
  vmax = MAX(vmin, MIN(y2, ey2));
  p3d_jnt_set_dof_bounds(jntPt, 1, vmin, vmax); 
  p3d_jnt_set_dof_rand_bounds(jntPt, 1, vmin, vmax);
  if(LEQ(vmax, vmin))
    { p3d_jnt_set_dof_is_user(jntPt, 1, FALSE); }
  else
    { p3d_jnt_set_dof_is_user(jntPt, 1, TRUE); }
  vmin = MAX(z1, ez1);
  vmax = MAX(vmin, MIN(z2, ez2));
  p3d_jnt_set_dof_bounds(jntPt, 2, vmin, vmax); 
  p3d_jnt_set_dof_rand_bounds(jntPt, 2, vmin, vmax);
  if(LEQ(vmax, vmin))
    { p3d_jnt_set_dof_is_user(jntPt, 2, FALSE); }
  else
    { p3d_jnt_set_dof_is_user(jntPt, 2, TRUE); }
  vmin = t1;
  vmax = MAX(vmin, t2);
  p3d_jnt_set_dof_bounds(jntPt, 3, vmin, vmax); 
  p3d_jnt_set_dof_rand_bounds(jntPt, 3, vmin, vmax);
  if(LEQ(vmax, vmin))
    { p3d_jnt_set_dof_is_user(jntPt, 3, FALSE); }
  else
    { p3d_jnt_set_dof_is_user(jntPt, 3, TRUE); }
  vmin = u1;
  vmax = MAX(vmin, u2);
  p3d_jnt_set_dof_bounds(jntPt, 4, vmin, vmax); 
  p3d_jnt_set_dof_rand_bounds(jntPt, 4, vmin, vmax);
  if(LEQ(vmax, vmin))
    { p3d_jnt_set_dof_is_user(jntPt, 4, FALSE); }
  else
    { p3d_jnt_set_dof_is_user(jntPt, 4, TRUE); }
  vmin = v1;
  vmax = MAX(vmin, v2);
  p3d_jnt_set_dof_bounds(jntPt, 5, vmin, vmax); 
  p3d_jnt_set_dof_rand_bounds(jntPt, 5, vmin, vmax);
  if(LEQ(vmax, vmin))
    { p3d_jnt_set_dof_is_user(jntPt, 5, FALSE); }
  else
    { p3d_jnt_set_dof_is_user(jntPt, 5, TRUE); }

  return(TRUE);
}


/*--------------------------------------------------------------------------*/
/*!
 * \brief Set the bounding box of the robot. The angles in input are in degree
 *
 * Note: This function check the validities of the bounds. It sets both
 *       bounds and random bounds to the same value. When min and max bounds
 *       are equal, it sets the degree of freedom not usable by user, if not,
 *       it sets the dof usable.
 *
 * \param x1: Minimum bound for the translation on x axis.
 * \param x2: Maximum bound for the translation on x axis.
 * \param y1: Minimum bound for the translation on y axis.
 * \param y2: Maximum bound for the translation on y axis.
 * \param z1: Minimum bound for the translation on z axis.
 * \param z2: Maximum bound for the translation on z axis.
 * \param t1: Minimum bound for the rotation on x axis in degree.
 * \param t2: Maximum bound for the rotation on x axis in degree.
 * \param u1: Minimum bound for the rotation on y axis in degree.
 * \param u2: Maximum bound for the rotation on y axis in degree.
 * \param v1: Minimum bound for the rotation on z axis in degree.
 * \param v2: Maximum bound for the rotation on z axis in degree.
 *
 * \return TRUE if success, else FALSE
 *
 * \warning This function must not be used anymore.
 */
int p3d_set_robot_box_deg(double x1, double x2, double y1, double y2, 
        double z1, double z2, double t1, double t2,
        double u1, double u2, double v1, double v2 )
{
  return p3d_set_robot_box(x1, x2, y1, y2, z1, z2, DTOR(t1), DTOR(t2),
         DTOR(u1), DTOR(u2), DTOR(v1), DTOR(v2));
}


/*--------------------------------------------------------------------------*/
/*!
 * \brief change the minimum value of user bounds.
 *
 * Note:
 *   - The user bounds are the bounds of degree of freedom which are
 *     controled by user (p3d_dof_data::is_user == ::TRUE)
 *   - If all the user bounds are not defined (\a n < rob::nb_user_dof)
 *     keep the previous bounds
 *   - Angles are in degree
 *   - Keeps the validity of max bounds and random bounds.
 *
 * \param robotPt: the robot.
 * \param dtab:    the array of minimum values for user bounds
 * \param n:       the number of bounds defined in \a dtab
 */
void p3d_set_user_config_min_bounds_deg(p3d_rob * robotPt,
          double * dtab, int n)
{
  int i, j, k;
  p3d_jnt * jntPt;
  double vmin, vmax;

  for(i=0; i<=robotPt->njoints; i++) {
    jntPt = robotPt->joints[i];
    k = jntPt->index_user_dof;
    for(j=0; j<jntPt->dof_equiv_nbr; j++) {
      if (k>=n)
  { break; }
      if (p3d_jnt_get_dof_is_user(jntPt, j)) {
  p3d_jnt_get_dof_bounds_deg(jntPt, j, &vmin, &vmax);
  vmin = dtab[k];
  vmax = MAX(vmin, vmax);
  p3d_jnt_set_dof_bounds_deg(jntPt, j, vmin, vmax);
  p3d_jnt_get_dof_rand_bounds_deg(jntPt, j, &vmin, &vmax);
  vmin = MAX(vmin, dtab[k]);
  vmax = MAX(vmin, vmax);
  p3d_jnt_set_dof_rand_bounds_deg(jntPt, j, vmin, vmax);
  k ++;
      }
    }
  }
}


/*--------------------------------------------------------------------------*/
/*!
 * \brief change the maximum value of user bounds.
 *
 * Note:
 *   - The user bounds are the bounds of degree of freedom which are
 *     controled by user (p3d_dof_data::is_user == ::TRUE)
 *   - If all the user bounds are not defined (\a n < rob::nb_user_dof)
 *     keep the previous bounds
 *   - Angles are in degree
 *   - Checks the validity of the min bounds.
 *
 * \param robotPt: the robot.
 * \param dtab:    the array of maximum values for user bounds
 * \param n:       the number of bounds defined in \a dtab
 */
void p3d_set_user_config_max_bounds_deg(p3d_rob * robotPt,
          double * dtab, int n)
{
  int i, j, k;
  p3d_jnt * jntPt;
  double vmin, vmax;

  for(i=0; i<=robotPt->njoints; i++) {
    jntPt = robotPt->joints[i];
    k = jntPt->index_user_dof;
    for(j=0; j<jntPt->dof_equiv_nbr; j++) {
      if (k>=n)
  { break; }
      if (p3d_jnt_get_dof_is_user(jntPt, j)) {
  p3d_jnt_get_dof_bounds_deg(jntPt, j, &vmin, &vmax);
  vmax = dtab[k];
  vmin = MIN(vmin, vmax);
  p3d_jnt_set_dof_bounds_deg(jntPt, j, vmin, vmax);
  p3d_jnt_get_dof_rand_bounds_deg(jntPt, j, &vmin, &vmax);
  vmax = MIN(vmax, dtab[k]);
  vmin = MIN(vmin, vmax);
  p3d_jnt_set_dof_rand_bounds_deg(jntPt, j, vmin, vmax);
  k ++;
      }
    }
  }
}


/******************************************************/
/* Fonction changeant le rayon du robot               */
/* In : le rayon                                      */
/* Out :                                              */
/******************************************************/
void p3d_set_this_robot_radius(p3d_rob *robotPt, double radius)
{
#ifdef P3D_LOCALPATH
  lm_reeds_shepp_str *lm_reeds_shepp_paramPt = 
    lm_get_reeds_shepp_lm_param(robotPt);
  
  lm_reeds_shepp_paramPt->radius = radius;
#endif
}

/******************************************************/
/* Fonction changeant le rayon du robot courant       */
/* In : le rayon                                      */
/* Out :                                              */
/******************************************************/
void p3d_set_robot_radius(double radius)
{
  p3d_rob *robotPt = (p3d_rob*)p3d_get_desc_curid(P3D_ROBOT);

#ifdef P3D_LOCALPATH
  lm_reeds_shepp_str *lm_reeds_shepp_paramPt = 
    lm_get_reeds_shepp_lm_param(robotPt);
	
  if (lm_reeds_shepp_paramPt == NULL) {
    PrintWarning(("p3d_set_robot_radius: Set radius without lm_reeds_shepp_str !!!\n"));
  } else
    { lm_reeds_shepp_paramPt->radius = radius; }
#endif
}

/* begin modif Carl */
/*********************************************************/
/* Fonction changeant le pas de discretisation graphique */
/* des trajectoires de l'environnement courant           */
/* In : le rayon                                         */
/* Out :                                                 */
/*********************************************************/
void p3d_set_env_graphic_dmax(double val)
{pp3d_env e = (pp3d_env)p3d_get_desc_curid(P3D_ENV);

  e->graphic_dmin = val;
  /* PrintInfo(("p3d_set_env_graphic_dmax set %f\n",e->graphic_dmin)); */
}
/* end modif Carl */


/* modif Pepijn */
/******************************************************/
/* Function who changes the attribute object_tolerance*/
/* of the current environment                         */
/* In :object_tolerance                               */
/* Out :                                              */
/******************************************************/
void p3d_set_env_object_tolerance(double obj_tol)
{
  pp3d_env e = (pp3d_env)p3d_get_desc_curid(P3D_ENV);
  e->object_tolerance = obj_tol;
  /* PrintInfo(("p3d_set_env_dmax set %f\n",e->dmax)); */
}
/************************************************/






/******************************************************/
/* Fonction changeant le pas de discretisation des    */
/* trajectoires de l'environnement courant            */
/* In : le rayon                                      */
/* Out :                                              */
/******************************************************/
void p3d_set_env_dmax(double dmax)
{
  pp3d_env e = (pp3d_env)p3d_get_desc_curid(P3D_ENV);

  e->dmax = dmax;
  /* PrintInfo(("p3d_set_env_dmax set %f\n",e->dmax)); */
}
/************************************************/


/*
 *  Set the goal configuration of the current robot
 *
 *  Input:  the goal configuration
 *
 */

void p3d_set_ROBOT_GOTO(configPt q)
{
  pp3d_rob robotPt = (pp3d_rob)p3d_get_desc_curid(P3D_ROBOT);
  p3d_copy_config_into(robotPt,q,&(robotPt->ROBOT_GOTO) );
}

/*
 *  Set the initial configuration of the current robot
 *
 *  Input:  the initial configuration. 
 */

void p3d_set_ROBOT_START(configPt q)
{
  pp3d_rob r = (pp3d_rob)p3d_get_desc_curid(P3D_ROBOT);
  p3d_copy_config_into(r,q,&(r->ROBOT_POS) );
}

/*
 *  Set the goal configuration of the current robot after converting
 *  angles from degree to radian.
 *
 *  Input:  the goal configuration
 *
 */

void p3d_set_ROBOT_GOTO_deg_to_rad(configPt q_deg)
{
  pp3d_rob robotPt = (pp3d_rob)p3d_get_desc_curid(P3D_ROBOT);
  configPt q=NULL;

  q = p3d_copy_config_deg_to_rad(robotPt, q_deg);
  p3d_copy_config_into(robotPt,q,&(robotPt->ROBOT_GOTO) );
  p3d_destroy_config(robotPt, q);
}

/*
 *  Set the initial configuration of the current robot after converting
 *  angles from degree to radian.
 *
 *  Input:  the initial configuration. 
 */

void p3d_set_ROBOT_START_deg_to_rad(configPt q_deg)
{
  pp3d_rob robotPt = (pp3d_rob)p3d_get_desc_curid(P3D_ROBOT);
  configPt q=NULL;
  
  q = p3d_copy_config_deg_to_rad(robotPt, q_deg);
  p3d_copy_config_into(robotPt,q,&(robotPt->ROBOT_POS) );
  p3d_destroy_config(robotPt, q);
}

/*
 *  Set the initial configuration of the current robot after converting
 *  angles from degree to radian.
 *
 *  Input:  the initial configuration. 
 */

void p3d_pushRobotTransitionsDegToRad(configPt q_deg, int position)
{
  pp3d_rob robotPt = (pp3d_rob)p3d_get_desc_curid(P3D_ROBOT);
  configPt q=NULL;

  q = p3d_copy_config_deg_to_rad(robotPt, q_deg);
  if(position < MAX_TRANSITION){
    p3d_copy_config_into(robotPt,q,&(robotPt->transitionConfigs[position]));
  }
  p3d_destroy_config(robotPt, q);
}

/* D�but modif Fabien */
/*
 * Add a new configuration to the current robot after config or 
 * at the begining if config == NULL
 */
int p3d_set_new_robot_config(p3d_rob* r, const char * name, const configPt q, int* ikSol, config_namePt config)
{
//  pp3d_rob r = (pp3d_rob)p3d_get_desc_curid(P3D_ROBOT);
  int i, num;  
  config_namePt * new_conf;
  num = -1;

  if ((name == NULL) || (q == NULL)) {
    fprintf(stderr, "Mauvais parametres dans p3d_set_new_robot_config !\n");
    return FALSE;
  }

  /* V�rifie l'unicit� du nom */
  for(i=0; i<r->nconf; i++) {
    if (r->conf[i] == config)
      { num = i+1; }
    if (strcmp(name, r->conf[i]->name) == 0) {
      p3d_copy_config_into(r, q, &(r->conf[i]->q));
      return FALSE;
    }
  }
  if (num == -1)
    { num = 0; }

  /* Mise � jour de la liste des configurations */
  new_conf = MY_ALLOC(config_namePt, r->nconf+1);
  if (new_conf == NULL) {
    fprintf(stderr, "Erreur d'allocation memoire dans p3d_set_new_robot_config !\n");
    return FALSE;
  }
  for(i=0; i<num; i ++)
    { new_conf[i] = r->conf[i]; }
  for(i=num+1; i<=r->nconf; i ++)
    { new_conf[i] = r->conf[i-1]; }
  new_conf[num] = MY_ALLOC(config_name, 1);
  if (new_conf[num] == NULL) {
    fprintf(stderr, "Erreur d'allocation memoire dans p3d_set_new_robot_config !\n");
    MY_FREE(new_conf, config_namePt, r->nconf+1);
    return FALSE;
  }
  new_conf[num]->q = p3d_copy_config(r, q);
  if (new_conf[num]->q == NULL) {
    fprintf(stderr, "Erreur d'allocation memoire dans p3d_set_new_robot_config !\n");
    MY_FREE(new_conf[num], config_name, 1);
    MY_FREE(new_conf, config_namePt, r->nconf+1);
    return FALSE;
  }
  if(p3d_get_ik_choice() != IK_NORMAL){
#ifdef P3D_CONSTRAINTS
    p3d_copy_iksol(r->cntrt_manager, ikSol, &(new_conf[num]->ikSol));
#endif
    if (new_conf[num]->ikSol == NULL) {
      fprintf(stderr, "Erreur d'allocation memoire dans p3d_set_new_robot_config !\n");
      p3d_destroy_config(r, new_conf[num]->q);
      MY_FREE(new_conf[num], config_name, 1);
      MY_FREE(new_conf, config_namePt, r->nconf+1);
      return FALSE;
    }
  }else{
    new_conf[num]->ikSol = NULL;
  }

  new_conf[num]->name = strdup(name);
  if (new_conf[num]->name == NULL) {
	  fprintf(stderr, "Erreur d'allocation memoire dans p3d_set_new_robot_config !\n");
    if(p3d_get_ik_choice() != IK_NORMAL){
#ifdef P3D_CONSTRAINTS
      p3d_destroy_specific_iksol(r->cntrt_manager, new_conf[num]->ikSol);
#endif
    }
    p3d_destroy_config(r, new_conf[num]->q);
    MY_FREE(new_conf[num], config_name, 1);
    MY_FREE(new_conf, config_namePt, r->nconf+1);
    return FALSE;
  }
  MY_FREE(r->conf, config_namePt, r->nconf);
  r->conf = new_conf;
  r->nconf ++;
  r->confcur = r->conf[num];

  return TRUE;
}

/*
 *  Set the initial configuration of the current robot after converting
 *  angles from degree to radian.
 *
 *  Input:  the initial configuration. 
 */

void p3d_set_robot_config_deg_to_rad(const char * name, configPt q_deg)
{
  pp3d_rob r = (pp3d_rob)p3d_get_desc_curid(P3D_ROBOT);
  configPt q=NULL;
  
  q = p3d_copy_config_deg_to_rad(r, q_deg);
  p3d_set_new_robot_config(r, name, q, r->ikSol, r->confcur);
  p3d_destroy_config(r, q);
}

/*
 *  Set the steering method of the current robot after 
 *
 *  Input:  name of steering method. 
 */
void p3d_set_robot_steering_method(const char * name)
{
  p3d_localpath_type lpl_type;
#ifdef P3D_LOCALPATH
  lpl_type = p3d_local_getid_planner(name);

  if (lpl_type == P3D_NULL_OBJ) {
    PrintInfo(("p3d_set_robot_steering_methode: warning: "
     "unknown steering method: %s !\n", name));
  } else
    { p3d_local_set_planner(lpl_type); }
#endif
}
/* fin modif fabien */

//! Changes the way an object will be displayed.
//! \param o pointer to the object
//! \param new_mode the mode to apply to the object
//! \param color the color (double[4]) to use to display the object (only used for some of the possible modes),
//!  can be left to NULL if unused
//! \return 0 in case of success, 1 otherwise
int p3d_set_obj_display_mode(p3d_obj *o, p3d_obj_display_mode new_mode, double *color)
{
  if(o==NULL)
  {
    printf("%s: %d: p3d_set_obj_display_mode(): input p3d_obj* is NULL.\n",__FILE__,__LINE__);
    return 1;
  }

  int i;
  poly_display_mode new_poly_mode;

  o->display_mode= new_mode;

  switch(new_mode) {
    case P3D_OBJ_DEFAULT_DISPLAY:
       new_poly_mode= POLY_DEFAULT_DISPLAY;
    break;
    case P3D_OBJ_NO_DISPLAY:
       new_poly_mode= POLY_NO_DISPLAY;
    break;
    case P3D_OBJ_UNLIT_BLACK_DISPLAY:
       new_poly_mode= POLY_UNLIT_BLACK_DISPLAY;
    break;
    case P3D_OBJ_UNLIT_RED_DISPLAY:
       new_poly_mode= POLY_UNLIT_RED_DISPLAY;
    break;
    case P3D_OBJ_RED_DISPLAY:
       new_poly_mode= POLY_RED_DISPLAY;
    break;
    case P3D_OBJ_UNLIT_GREEN_DISPLAY:
       new_poly_mode= POLY_UNLIT_GREEN_DISPLAY;
    break;
    case P3D_OBJ_GREEN_DISPLAY:
       new_poly_mode= POLY_GREEN_DISPLAY;
    break;
    case P3D_OBJ_UNLIT_BLUE_DISPLAY:
       new_poly_mode= POLY_UNLIT_BLUE_DISPLAY;
    break;
    case P3D_OBJ_BLUE_DISPLAY:
       new_poly_mode= POLY_BLUE_DISPLAY;
    break;
    case P3D_OBJ_CUSTOM_COLOR_DISPLAY:
       new_poly_mode= POLY_CUSTOM_COLOR_DISPLAY;
    break;
    case P3D_OBJ_UNLIT_CUSTOM_COLOR_DISPLAY:
       new_poly_mode= POLY_UNLIT_CUSTOM_COLOR_DISPLAY;
    break;
  }

  if(new_poly_mode==POLY_CUSTOM_COLOR_DISPLAY || new_poly_mode==POLY_UNLIT_CUSTOM_COLOR_DISPLAY)  {
    if(color==NULL)    {
      printf("%s: %d: p3d_set_obj_display_mode(): color should not be NULL.\n",__FILE__,__LINE__);
      return 1;
    }
  }

  for(i=0; i<o->np; ++i) {
    p3d_set_poly_display_mode(o->pol[i], new_poly_mode, color);
  }

  return 0;
}

//! Changes the way a robot will be displayed.
///! \param robotPt pointer to the robot
//! \param new_mode the mode to apply to the robot
//! \param color the color (double[4]) to use to display the robot (only used for some of the possible modes),
//!  can be left to NULL if unused
//! \return 0 in case of success, 1 otherwise
int p3d_set_robot_display_mode(p3d_rob *robotPt, p3d_rob_display_mode new_mode, double *color)
{
  if(robotPt==NULL)
  {
    printf("%s: %d: p3d_set_robot_display_mode(): input p3d_rob* is NULL.\n",__FILE__,__LINE__);
    return 1;
  }

  int i;
  p3d_obj_display_mode new_obj_mode;

  robotPt->display_mode= new_mode;

  switch(new_mode) {
    case P3D_ROB_DEFAULT_DISPLAY:
       new_obj_mode= P3D_OBJ_DEFAULT_DISPLAY;
    break;
    case P3D_ROB_NO_DISPLAY:
       new_obj_mode= P3D_OBJ_NO_DISPLAY;
    break;
    case P3D_ROB_UNLIT_BLACK_DISPLAY:
       new_obj_mode= P3D_OBJ_UNLIT_BLACK_DISPLAY;
    break;
    case P3D_ROB_UNLIT_RED_DISPLAY:
       new_obj_mode= P3D_OBJ_UNLIT_RED_DISPLAY;
    break;
    case P3D_ROB_RED_DISPLAY:
       new_obj_mode= P3D_OBJ_RED_DISPLAY;
    break;
    case P3D_ROB_UNLIT_GREEN_DISPLAY:
       new_obj_mode= P3D_OBJ_UNLIT_GREEN_DISPLAY;
    break;
    case P3D_ROB_GREEN_DISPLAY:
       new_obj_mode= P3D_OBJ_GREEN_DISPLAY;
    break;
    case P3D_ROB_UNLIT_BLUE_DISPLAY:
       new_obj_mode= P3D_OBJ_UNLIT_BLUE_DISPLAY;
    break;
    case P3D_ROB_BLUE_DISPLAY:
       new_obj_mode= P3D_OBJ_BLUE_DISPLAY;
    break;
    case P3D_ROB_CUSTOM_COLOR_DISPLAY:
       new_obj_mode= P3D_OBJ_CUSTOM_COLOR_DISPLAY;
    break;
    case P3D_ROB_UNLIT_CUSTOM_COLOR_DISPLAY:
       new_obj_mode= P3D_OBJ_UNLIT_CUSTOM_COLOR_DISPLAY;
    break;
  }

  if(new_mode==P3D_ROB_CUSTOM_COLOR_DISPLAY || new_mode==P3D_ROB_UNLIT_CUSTOM_COLOR_DISPLAY)  {
    if(color==NULL)    {
      printf("%s: %d: p3d_set_robot_display_mode(): color should not be NULL.\n",__FILE__,__LINE__);
      return 1;
    }
  }

  for(i=0; i<robotPt->no; ++i) {
    p3d_set_obj_display_mode(robotPt->o[i], new_obj_mode, color);
  }

  return 0;
}

#if defined(LIGHT_PLANNER)
#include "Collision-pkg.h"
//! Sets the object that will possibly carried by the robot.
//! This object is a freeflyer robot.
int p3d_set_object_to_carry_to_arm(p3d_rob *MyRobot, int arm_id,const char *object_name){
  if(MyRobot==NULL)
  {
    printf("%s: %d: p3d_set_object_to_carry(): input p3d_rob* is NULL.\n",__FILE__,__LINE__);
    return 1;
  }
  p3d_rob *carriedObject= NULL;
	
	// Set carried object
    (*MyRobot->armManipulationData)[arm_id].setCarriedObject( object_name );
  if(object_name){
    carriedObject = (*MyRobot->armManipulationData)[arm_id].getCarriedObject();
  }
  if(carriedObject == NULL) {
    printf("%s: %d: p3d_set_object_to_carry(): There is no robot with name \"%s\".\n", __FILE__, __LINE__, object_name);
    return 1;
  }
  
	// Set manipulation joint to be 
	// the size of the manipulated object (See if this should not take the biggest or a compound)
	(*MyRobot->armManipulationData)[arm_id].getManipulationJnt()->dist = carriedObject->joints[1]->dist;
  MyRobot->isCarryingObject = TRUE;
  return 0;
}
#endif

//! Modifies the robot part the body belongs to.
//! \param robotPt pointer to the robot the body belongs to
//! \param name name of the object
//! \param part type of the part
//! \return 0 in case of success, 1 otherwise
int p3d_mark_body(p3d_rob *robotPt, char *name, p3d_robot_part part)
{
  if(robotPt==NULL)
  {
    printf("%s: %d: p3d_mark_body(): input p3d_rob* is NULL.\n",__FILE__,__LINE__);
    return 1;
  }

  p3d_obj *obj= NULL;

  obj= p3d_get_robot_body_by_name(robotPt, name);

  if(obj!=NULL)
  {  obj->robot_part= part;  }
  else
  { 
    printf("%s: %d: p3d_mark_body(): robot \"%s\" has no body name \"%s\".\n",__FILE__,__LINE__,robotPt->name,name);
    return 1;
  }

  return 0;
}

//! Sets the weight of a robot body that will be used in distance computation.
//! \param robotPt pointer to the robot the body belongs to
//! \param name name of the object
//! \param weight value of the weight
//! \return 0 in case of success, 1 otherwise
int p3d_set_distance_weight(p3d_rob *robotPt, char *name, double weight)
{
  if(robotPt==NULL)
  {
    printf("%s: %d: p3d_set_distance_weight(): input p3d_rob* is NULL.\n",__FILE__,__LINE__);
    return 1;
  }

  p3d_obj *obj= NULL;

  obj= p3d_get_robot_body_by_name(robotPt, name);

  if(obj!=NULL)
  {  obj->distance_weight= weight;  }
  else
  { 
    printf("%s: %d: p3d_set_distance_weight(): robot \"%s\" has no body name \"%s\".\n",__FILE__,__LINE__,robotPt->name,name);
    return 1;
  }

  return 0;
}

//! Sets the flag used to authorize collision test with a distance threshold or not for the given robot.
//! \param robotPt pointer to the robot the body belongs to
//! \param value TRUE or FALSE
//! \return 0 in case of success, 1 otherwise
int p3d_set_collision_tolerance_inhibition(p3d_rob *robotPt, int value)
{
  if(robotPt==NULL)
  {
    printf("%s: %d: p3d_inhibit_collision_tolerance(): input p3d_rob* is NULL.\n",__FILE__,__LINE__);
    return 1;
  }

  robotPt->inhibitCollisionTolerance= value;

  return 0;
}

/*  p3d_saveConfigTofile
 *
 *  Input:  the robot,
 *          the configuration,
 *          the filename
 *
 *  Output: 0 OK
 *
 *
 *  Description:
 */
int p3d_saveConfigTofile(p3d_rob* robotPt, configPt q,char* filename)
{


    //Creating the file Variable version 1.0
    xmlDocPtr doc = xmlNewDoc(xmlCharStrdup("1.0"));


    //Writing the root node
    xmlNodePtr root = xmlNewNode (NULL, xmlCharStrdup(robotPt->name));


    writeXmlRobotConfig(root, robotPt, q);

    xmlDocSetRootElement(doc, root);


    //Writing the file on HD
    xmlSaveFormatFile (filename, doc, 1);
    xmlFreeDoc(doc);
    return 0;
}


/*  p3d_loadConfigFromfile
 *
 *  Input:  the robot,
 *          the configuration,
 *          the filename
 *
 *  Output: 0 OK
 *
 *
 *  Description:
 */
configPt p3d_loadConfigFromfile(p3d_rob* robotPt, char* filename)
{

    configPt q_hum;

    xmlDocPtr doc;
    xmlNodePtr root;

    doc = xmlParseFile(filename);

    if(doc==NULL)
    {
        printf("Document not parsed successfully (doc==NULL)");
        return q_hum;
    }

    root = xmlDocGetRootElement(doc);

    if (root == NULL)
    {
        printf("Document not parsed successfully");
        xmlFreeDoc(doc);
        return q_hum;
    }

    if (xmlStrcmp(root->name, xmlCharStrdup(robotPt->name)))
    {
        printf("The XML conf is not for the robot given as input.");
        xmlFreeDoc(doc);
        return q_hum;
    }


    xmlNodePtr ptrTmp = root;
    q_hum = readXmlConfig(robotPt,ptrTmp->xmlChildrenNode->next);

    return q_hum;


}
