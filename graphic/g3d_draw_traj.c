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
#include "Graphic-pkg.h"

#include "Util-pkg.h"
#include "P3d-pkg.h"

#include "env.hpp"

#include "move3d-headless.h"

#ifdef P3D_PLANNER
#include "Planner-pkg.h"
#endif

#ifdef P3D_COLLISION_CHECKING
#include "Collision-pkg.h"
#endif

#ifdef LIGHT_PLANNER
#include "ManipulationUtils.hpp"
#include "ManipulationArmData.hpp"
#endif

#include "GroundHeight-pkg.h"
#include <iostream>
#include <string>

#if defined( CXX_PLANNER )
#include "API/project.hpp"
#include "API/Trajectory/trajectory.hpp"
#endif

#if defined( CXX_PLANNER )
#include "cost_space.hpp"
#endif

#include <vector>

static void draw_trace(void);
static void draw_trace_2(int opengl_context);
static int NB_KEY_FRAME = 500;
extern double ZminEnv;
extern double ZmaxEnv;
extern void* GroundCostObj;

void (*ext_compute_config_cost_along_traj)(p3d_rob* r,configPt q) = (void (*)(p3d_rob* r,configPt q))(dummy_void);
void (*ext_g3d_traj_debug)() = NULL;

// Warning Jim
#ifndef P3D_PLANNER
void* GroundCostObj;
#endif

/*************************************************************************************/
int g3d_traj_get_NB_KEY_FRAME(void) {
  return NB_KEY_FRAME;
}

void g3d_traj_set_NB_KEY_FRAME(int param) {
  NB_KEY_FRAME = param;
}

/*************************************************************************************/
/*
 *  Draw the current path of the current robot
 *
 *  Description: sample the path in such a way that no point of the
 *  robot moves more than dmax. For each sample configuration,
 *  collision is checked.
 */


/**********************************************/
/* Fonction reglant les lumieres et affichant */
/* une trace                                  */
/**********************************************/
void g3d_draw_trace(int opengl_context) {
  g3d_set_light(g3d_get_cur_win()->vs);
  g3d_set_default_material();
  draw_trace_2(opengl_context);
}

static void draw_trace(int opengl_context) {
  double u = 0, du, umax, dmax; /* parameters along the local path */
  configPt q;
  pp3d_rob robotPt = (pp3d_rob) p3d_get_desc_curid(P3D_ROBOT);
  int njnt = robotPt->njoints;
  double *distances;
  int i, end_localpath = 0;
  pp3d_env e;
  pp3d_localpath localpathPt;
  G3D_Window *win;

  if (robotPt->tcur == NULL) {
    PrintInfo(("draw_trace : no current trajectory\n"));
    return;
  }
  
  robotPt->draw_transparent = true;

  win = g3d_get_cur_win();
  e = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
  if (e->INIT) {
    g3d_init_all_poly(opengl_context);
    boxlist = -1;
#ifdef P3D_COLLISION_CHECKING
    p3d_reset_robotboxlist();
#endif
    e->INIT = 0;
  }

//   if ((win->vs.list == -1) && (win->vs.GOURAUD)) {
//     p = p3d_poly_get_first();
//     while (p != NULL) {
//       g3d_init_poly(p, 2);
//       p = p3d_poly_get_next();
//     }
//   }


  win->vs.transparency_mode= G3D_TRANSPARENT_AND_OPAQUE;
//g3d_draw_env_box();
  g3d_draw_obstacles(win,opengl_context);

#ifdef P3D_PLANNER
  if(XYZ_GRAPH && ENV.getBool(Env::drawGraph)){g3d_draw_graph();}
#endif
  /* begin modif Carl */
  /* dmax = p3d_get_env_dmax();  */
  dmax = p3d_get_env_graphic_dmax();
  /* end modif Carl */
  localpathPt = robotPt->tcur->courbePt;
  distances = MY_ALLOC(double, njnt + 1);

  while (localpathPt != NULL) {
    umax = localpathPt->range_param;

    while (end_localpath < 2) {

      /* position of the robot corresponding to parameter u */
      q = localpathPt->config_at_param(robotPt, localpathPt, u);
      p3d_set_and_update_robot_conf(q);
      /* collision checking */
//#ifdef P3D_COLLISION_CHECKING
//      p3d_numcoll = p3d_col_test_all();
//#endif
      win->vs.transparency_mode= G3D_TRANSPARENT_AND_OPAQUE;
      g3d_draw_robot(robotPt->num, win, opengl_context);
// 			int i;
// 			for(i=0; i<=robotPt->njoints; i++)
// 			{
// 				g3d_draw_frame(robotPt->joints[i]->abs_pos, 15);
// 			}

      p3d_destroy_config(robotPt, q);


      for (i = 0; i <= njnt; i++) {
        distances[i] = dmax;
      }
#ifdef MULTILOCALPATH
			if (localpathPt->type_lp == MULTI_LOCALPATH){
				//du = p3d_get_env_graphic_dmax()*10;//du = localpathPt->stay_within_dist(robotPt, localpathPt,u, FORWARD, distances);
				int softMotion = FALSE;
				for(int i = 0; i < robotPt->mlp->nblpGp; i++){
					if(localpathPt->mlpLocalpath[i] != NULL) {
						if(localpathPt->mlpLocalpath[i]->type_lp == SOFT_MOTION){
							softMotion = TRUE;
							break;
						}
					}
				}
				if(softMotion){
					du = 10*ENV.getDouble(Env::showTrajFPS)*p3d_get_env_graphic_dmax();  //0.05;
				}else{
					du = ENV.getDouble(Env::showTrajFPS)*p3d_get_env_graphic_dmax()/3;/* localpathPt->stay_within_dist(robotPt, localpathPt,*/
				}
		} else if (localpathPt->type_lp == SOFT_MOTION){
			du = ENV.getDouble(Env::showTrajFPS)*p3d_get_env_graphic_dmax()*3;  //0.05;
		} else {
#endif
				du = ENV.getDouble(Env::showTrajFPS)*p3d_get_env_graphic_dmax();/* localpathPt->stay_within_dist(robotPt, localpathPt,*/
#ifdef MULTILOCALPATH
			}
#endif

      u += du;
      if (u > umax - EPS6) {
        u = umax;
        end_localpath++;
      }
    }
#ifdef P3D_PLANNER
    for (i = 0;i < p3d_get_NB_specific();i++) {
      if (localpathPt != NULL) {
        localpathPt = localpathPt->next_lp;
      }
    }
#endif
    end_localpath = 0;
    u = 0;
  }
  MY_FREE(distances, double, njnt + 1);
  robotPt->draw_transparent = false;
}

static void draw_trace_2(int opengl_context)
{
  p3d_rob* robotPt = (pp3d_rob) p3d_get_desc_curid(P3D_ROBOT);

  if (robotPt->tcur == NULL) 
  {
    PrintInfo(("draw_trace : no current trajectory\n"));
    return;
  }
  
  G3D_Window *win;
  configPt q;
  pp3d_localpath localpathPt;
  int end = FALSE;
  int njnt = robotPt->njoints;
  double u = 0, du, umax; /* parameters along the local path */
  
  robotPt->draw_transparent = false;
  
  win = g3d_get_cur_win();
  win->vs.transparency_mode= G3D_TRANSPARENT_AND_OPAQUE;
  //g3d_draw_env_box();
  g3d_draw_obstacles(win,opengl_context);
#ifdef P3D_PLANNER
  if(XYZ_GRAPH && ENV.getBool(Env::drawGraph)){g3d_draw_graph();}
#endif
  
  umax = p3d_compute_traj_rangeparam(robotPt->tcur);
  localpathPt = robotPt->tcur->courbePt;
  
  while (u < umax - EPS6) 
  {
    /* position of the robot corresponding to parameter u */
    q = p3d_config_at_param_along_traj(robotPt->tcur,u);
    p3d_set_and_update_robot_conf(q);
    
    /* collision checking */
//#ifdef P3D_COLLISION_CHECKING
//    p3d_numcoll = p3d_col_test_all();
//#endif
//    if(u!=0.)
//    {
//      robotPt->draw_transparent = true;
//    }
    win->vs.transparency_mode= G3D_TRANSPARENT_AND_OPAQUE;
    g3d_draw_robot(robotPt->num, win, opengl_context);
    p3d_destroy_config(robotPt, q);
    
#if  defined(LIGHT_PLANNER)
    for( int i=0; i<(*robotPt->armManipulationData).size();i++ ) 
    {
      p3d_rob* object = (*robotPt->armManipulationData)[i].getCarriedObject();
      if( object != NULL ) {
        p3d_sel_desc_num(P3D_ROBOT,object->num);
        g3d_draw_robot( object->num, win, opengl_context);
        p3d_sel_desc_num(P3D_ROBOT,robotPt->num);
      }
    }
#endif
      
#ifdef MULTILOCALPATH
			if (localpathPt->type_lp == MULTI_LOCALPATH){
				//du = p3d_get_env_graphic_dmax()*10;//du = localpathPt->stay_within_dist(robotPt, localpathPt,u, FORWARD, distances);
				int softMotion = FALSE;
				for(int i = 0; i < robotPt->mlp->nblpGp; i++){
					if(localpathPt->mlpLocalpath[i] != NULL) {
						if(localpathPt->mlpLocalpath[i]->type_lp == SOFT_MOTION){
							softMotion = TRUE;
							break;
						}
					}
				}
				if(softMotion){
					du = 10*ENV.getDouble(Env::showTrajFPS)*p3d_get_env_graphic_dmax();  //0.05;
				}else{
					du = ENV.getDouble(Env::showTrajFPS)*p3d_get_env_graphic_dmax()/3;/* localpathPt->stay_within_dist(robotPt, localpathPt,*/
				}
      } else if (localpathPt->type_lp == SOFT_MOTION){
        du = ENV.getDouble(Env::showTrajFPS)*p3d_get_env_graphic_dmax()*3;  //0.05;
      } else {
#endif
				du = ENV.getDouble(Env::showTrajFPS)*p3d_get_env_graphic_dmax();/* localpathPt->stay_within_dist(robotPt, localpathPt,*/
#ifdef MULTILOCALPATH
			}
#endif
      
      u += du;
      if (u > umax - EPS6) {
        u = umax;
        end = TRUE;
      }
    }
  robotPt->draw_transparent = false;
}


/*
 *  play the current trajectories of each robot of the environment
 */

void g3d_show_search(void) {
  int r, nr, ir, ij, njnt, max_njnt;
  pp3d_localpath *localpathPt_tab, localpathPt;
  double *u, *umax, *du, *distances;
  p3d_rob *robotPt;
  double dmax;
  int fini = 0;
  configPt q;

  r = p3d_get_desc_curnum(P3D_ROBOT);
  nr = p3d_get_desc_number(P3D_ROBOT);

  /* arrays of all the trajectories and parameters */
  localpathPt_tab = MY_ALLOC(pp3d_localpath, nr);
  u = MY_ALLOC(double, nr);
  umax = MY_ALLOC(double, nr);
  du = MY_ALLOC(double, nr);

  max_njnt = 0;
  for (ir = 0;ir < nr;ir++) {
    p3d_sel_desc_num(P3D_ROBOT, ir);
    robotPt = (p3d_rob *) p3d_get_desc_curid(P3D_ROBOT);
    max_njnt = MAX(max_njnt, robotPt->njoints);
    if (robotPt->tcur != NULL) {
      localpathPt_tab[ir] = robotPt->tcur->courbePt;
      u[ir] = 0.;
      du[ir] = 0.;
      localpathPt = localpathPt_tab[ir];
      umax[ir] = localpathPt->range_param;
    } else {
      localpathPt_tab[ir] = NULL;
    }
  }
  distances = MY_ALLOC(double, max_njnt + 1);

  /* while all the trajectories have not been shown */
  while (!fini) {
    for (ir = 0;ir < nr;ir++) {
      if (localpathPt_tab[ir] != NULL) {
        /* PrintInfo(("robot : %d u : %f umax : %f fini : %d\n",ir,u[ir],umax[ir],fini)); */
        p3d_sel_desc_num(P3D_ROBOT, ir);
        robotPt = (p3d_rob *) p3d_get_desc_curid(P3D_ROBOT);
        njnt = robotPt->njoints;
        localpathPt = localpathPt_tab[ir];

        if (u[ir] > umax[ir] - EPS6) {
          /* PrintInfo(("On passe u[%d] a umax[%d]\n",ir,ir)); */
          u[ir] = umax[ir];
        }
        /* position of the robot corresponding to parameter u */
        q = localpathPt->config_at_param(robotPt, localpathPt, u[ir]);
        /*  PrintInfo(("q : ")); */
        /*  for(ij=0; ij<=njnt; ij++){ */
        /*    PrintInfo((" %f ",q[ij])); */
        /*  } */
        /*  PrintInfo(("\n")); */
        p3d_set_and_update_robot_conf(q);
        /* collision checking */
        p3d_destroy_config(robotPt, q);

        /* we compute the next step */
        /* begin modif Carl */
        /* dmax = p3d_get_env_dmax();  */
        dmax = p3d_get_env_graphic_dmax();
        /* end modif Carl */
        /* PrintInfo(("dmax avant du : %f\n",dmax)); */
        for (ij = 0; ij <= njnt; ij++) {
          distances[ij] = dmax;
        }
        du[ir] = p3d_get_env_graphic_dmax()/10;/* localpathPt->stay_within_dist(robotPt, localpathPt,
                                                       u[ir], FORWARD, distances); */
        u[ir] += du[ir];
        /* if we are at the end of an elementary curve */
      }
    }

    g3d_draw_allwin_active();

    fini = 1;
    for (ir = 0;ir < nr;ir++) {
      if (localpathPt_tab[ir] != NULL && u[ir] == umax[ir]) {
        /* PrintInfo(("localpathPt_tab[%d] != NULL && u[%d] = 1.0\n",ir,ir)); */
        fini = 0;
        localpathPt_tab[ir] = localpathPt_tab[ir]->next_lp;
        u[ir] = 0.;
        du[ir] = 0.;
      } else if (localpathPt_tab[ir] != NULL && u[ir] <= umax[ir]) {
        fini = 0;
        /* PrintInfo(("localpathPt_tab[%d] != NULL && u[%d] = %f\n",ir,ir,u[ir])); */
      }
    }
  }
  MY_FREE(distances, double, max_njnt + 1);
  MY_FREE(localpathPt_tab, pp3d_localpath, nr);
  MY_FREE(u, double, nr);
  MY_FREE(umax, double, nr);
  MY_FREE(du, double, nr);

  p3d_sel_desc_num(P3D_ROBOT, r);
}


/*
 * Play the current trajectory of a robot
 *
 * Input:  the robot,
 *
 */
int g3d_show_tcur_rob(p3d_rob *robotPt, int (*fct)(p3d_rob* robot, p3d_localpath* curLp)) {
  double u = 0.0;
  double du, umax, dmax; /* parameters along the local path */
  configPt q;
  int njnt = robotPt->njoints;
  double *distances;
  int end_localpath = 0, count = 0, *ikSol = NULL;
  pp3d_localpath localpathPt;
  p3d_traj* traj = robotPt->tcur;
  int stopShowTraj = TRUE;
	
  if (robotPt->tcur == NULL) {
    PrintInfo(("g3d_show_tcur_rob: no current trajectory\n"));
    return 0;
  }

  localpathPt = robotPt->tcur->courbePt;
  distances = MY_ALLOC(double, njnt + 1);

	u = 0.0;
  while ( (localpathPt != NULL) && stopShowTraj ) {
    umax = localpathPt->range_param;
    //activate the constraint for the local path
#ifdef P3D_CONSTRAINTS
    p3d_desactivateAllCntrts(robotPt);
    for(int i = 0; i < localpathPt->nbActiveCntrts; i++){
      p3d_activateCntrt(robotPt, robotPt->cntrt_manager->cntrts[localpathPt->activeCntrts[i]]);
    }
#endif

#if defined(LIGHT_PLANNER)
		robotPt->isCarryingObject = localpathPt->isCarryingObject;
    for(unsigned int i = 0; i < robotPt->armManipulationData->size(); i++){
      (*robotPt->armManipulationData)[i].setCarriedObject(localpathPt->carriedObject[i]); /*!< pointer to the carried object (obstacle environment or robot body) */
    }
		//p3d_mat4Copy(localpathPt->Tgrasp, robotPt->Tgrasp);
#endif

//deb modif xav
		if (u > umax - EPS6) {
			u -= umax;
			end_localpath = 0;
			localpathPt = localpathPt->next_lp;
			continue;
		}
    //while (end_localpath < 2) {
//fin modif xav
	  while (( end_localpath < 1) && stopShowTraj ) {

//	  std::cout << "-------------------------------------------" << std::endl;
//	  std::cout << "Robot = " << robotPt->name << std::endl;

      /* begin modif Carl */
      /* dmax = p3d_get_env_dmax(); */
      dmax = p3d_get_env_graphic_dmax();
      /* end modif Carl */
      /* position of the robot corresponding to parameter u */
      q = localpathPt->config_at_param(robotPt, localpathPt, u);
			
#if defined( P3D_CONSTRAINTS ) && defined( P3D_PLANNER )
      if (!ikSol || !p3d_compare_iksol(robotPt->cntrt_manager, localpathPt->ikSol, ikSol)) {
        p3d_copy_iksol(robotPt->cntrt_manager, localpathPt->ikSol, &ikSol);
        if (p3d_get_ik_choice() != IK_NORMAL) {
          p3d_print_iksol(robotPt->cntrt_manager, localpathPt->ikSol);
        }
      }
#endif
			
      p3d_set_and_update_this_robot_conf_multisol(robotPt, q, NULL, 0, localpathPt->ikSol);

      // The callback function
      // is defined outside libmove3d
      ext_compute_config_cost_along_traj(robotPt,q);
  
#ifndef WITH_XFORMS
	  stopShowTraj = (*fct_stop)();
#endif
			
	  p3d_destroy_config(robotPt, q);
			
//      std::cout << "Print Image" << std::endl;

      /* collision checking */
//#ifdef P3D_COLLISION_CHECKING
//      p3d_numcoll = p3d_col_test_all();
//#endif
      count++;
	  
//       g3d_draw_allwin_active();
      if (fct) if (((*fct)(robotPt, localpathPt)) == FALSE) return(count);
      if(robotPt->tcur != traj){
        localpathPt = robotPt->tcur->courbePt;
        traj = robotPt->tcur;
      }

      for (int i = 0; i <= njnt; i++) {
        distances[i] = dmax;
      }
#ifdef MULTILOCALPATH
			if (localpathPt->type_lp == MULTI_LOCALPATH){
				//du = p3d_get_env_graphic_dmax()*10;//du = localpathPt->stay_within_dist(robotPt, localpathPt,u, FORWARD, distances);
        int softMotion = FALSE;
        for(int i = 0; i < robotPt->mlp->nblpGp; i++){
					if(localpathPt->mlpLocalpath[i] != NULL) {
						if(localpathPt->mlpLocalpath[i]->type_lp == SOFT_MOTION){
							softMotion = TRUE;
							break;
						}
					}
        }
        if(softMotion){
					du = 10*ENV.getDouble(Env::showTrajFPS)*p3d_get_env_graphic_dmax();  //0.05;
				}else{
					du = ENV.getDouble(Env::showTrajFPS)*p3d_get_env_graphic_dmax()/3;/* localpathPt->stay_within_dist(robotPt, localpathPt,*/
				}
      } else if (localpathPt->type_lp == SOFT_MOTION){
        du = ENV.getDouble(Env::showTrajFPS)*p3d_get_env_graphic_dmax()*3;  //0.05;
      } else {
#endif
        du = ENV.getDouble(Env::showTrajFPS)*p3d_get_env_graphic_dmax()/10;/* localpathPt->stay_within_dist(robotPt, localpathPt,*/
#ifdef MULTILOCALPATH
			}
#endif
      u += du;
      
//deb modif xav
//    if (u > umax - EPS6) {
//      u = umax;
//      end_localpath++;
//    }
// 		}
// 		localpathPt = localpathPt->next_lp;
// 		end_localpath = 0;
// 		u = 0;
//		}
     if (u > umax - EPS6) {
			 u -= umax;
       end_localpath++;
     }
    }
    localpathPt = localpathPt->next_lp;
    end_localpath = 0;
//fin modif xav
  }
	
	localpathPt = robotPt->tcur->courbePt;
	
	// Go to last lp
	for (int i=0; i<(robotPt->tcur->nlp-1); i++) {
		localpathPt = localpathPt->next_lp;
	}
	
	// Show last configuration
	q = localpathPt->config_at_param(robotPt, localpathPt, localpathPt->range_param );
	
#if defined( P3D_CONSTRAINTS ) && defined( P3D_PLANNER )
	if (!ikSol || !p3d_compare_iksol(robotPt->cntrt_manager, localpathPt->ikSol, ikSol)) {
		p3d_copy_iksol(robotPt->cntrt_manager, localpathPt->ikSol, &ikSol);
		if (p3d_get_ik_choice() != IK_NORMAL) {
			p3d_print_iksol(robotPt->cntrt_manager, localpathPt->ikSol);
		}
	}
#endif
	
	p3d_set_and_update_this_robot_conf_multisol(robotPt, q, NULL, 0, localpathPt->ikSol);
	
	if (fct) if (((*fct)(robotPt, localpathPt)) == FALSE) return(count);

  MY_FREE(distances, double, njnt + 1);
  return count;
}


/*--------------------------------------------------------------------------*/
/*!
 *  \brief Draw the current trajectory of a given device
 *
 *  \param robotPt     the robot
 *  \param NumBody     Number of the end-effector body
 *  \param NbKeyFrames Number of lines composing the path
 *
 *  \return <NONE>
 *
 */

void g3d_draw_tcur(p3d_rob *robotPt, int indexjnt, int NbKeyFrames) {
  int modulo;
  p3d_localpath *localpathPt;
  double umax, u = 0, du;
  configPt q, qsave;
  int color;
  p3d_jnt *drawnjnt;
  p3d_obj *body = robotPt->o[robotPt->no-1];
  p3d_vector3 pi, pf;
  int val1, val2;
  double Cost1, Cost2;

  if (!robotPt->tcur)
    return;
  
  indexjnt = p3d_get_user_drawnjnt();
  if (indexjnt != -1 && indexjnt <= robotPt->njoints ) {
    drawnjnt = robotPt->joints[indexjnt];
  } else {
#ifdef P3D_COLLISION_CHECKING
    if (p3d_col_get_mode() == p3d_col_mode_bio) {
      drawnjnt = robotPt->joints[0]->next_jnt[robotPt->joints[0]->n_next_jnt - 1];
    } else {
      drawnjnt = body->jnt;
    }
#endif
  }

  qsave = p3d_get_robot_config(robotPt);
  color = Black;
  modulo = 0;
  
  du = robotPt->tcur->range_param / NbKeyFrames;
  localpathPt = robotPt->tcur->courbePt;

  if (!localpathPt)
    return;

  q = localpathPt->config_at_param(robotPt, localpathPt, 0);
  p3d_set_and_update_this_robot_conf(robotPt, q);
  
  p3d_jnt_get_cur_vect_point(drawnjnt, pi);
  p3d_destroy_config(robotPt, q);
  u += du;

  while (localpathPt != NULL) {
    umax = localpathPt->range_param;
    /* color of localpath */
    switch (modulo) {
      case 0:
        color = Violet;
        break;
      case 1:
        color = Blue;
        break;
      case 2:
        color = Red;
        break;
      case 3:
        color = Green;
        break;
      default:
        color = Black;
    }
    while (u < umax) {
      /* position of the robot corresponding to parameter u */
      q = localpathPt->config_at_param(robotPt, localpathPt, u);
      p3d_set_and_update_this_robot_conf(robotPt, q);
      
// draw frame xav
      
//  			for(int i=0; i<=robotPt->njoints; i++)
//  			{
//  				g3d_draw_frame(robotPt->joints[i]->abs_pos, 15);
//  			}

      p3d_jnt_get_cur_vect_point(drawnjnt, pf);
      p3d_destroy_config(robotPt, q);
      
      if ((!ENV.getBool(Env::isCostSpace)) || (GroundCostObj == NULL)) {
        glLineWidth(3.);
        
        if( ENV.getBool(Env::drawMultiColorLocalpath) )
        {
          g3d_drawOneLine(pi[0],pi[1],pi[2],pf[0],pf[1],pf[2],color,NULL);
          
          if (u == 0.0 ) {
            g3d_drawSphere(pf[0],pf[1],pf[2],0.015);
          }
        }
        else {
          g3d_drawOneLine(pi[0], pi[1], pi[2], pf[0], pf[1], pf[2], Black, NULL);
        }

        glLineWidth(1.);
      } else {
        val1 = GHintersectionVerticalLineWithGround(GroundCostObj, pi[0], pi[1], &Cost1);
        val2 = GHintersectionVerticalLineWithGround(GroundCostObj, pf[0], pf[1], &Cost2);
        glLineWidth(3.);
#ifdef P3D_PLANNER
        g3d_drawOneLine(pi[0], pi[1], Cost1 + (ZmaxEnv - ZminEnv)*0.02, pf[0], pf[1], Cost2 + (ZmaxEnv - ZminEnv)*0.02, Red, NULL);
#endif
        glLineWidth(1.);
        
        if (u == 0.0 ) {
          g3d_drawSphere(pf[0],pf[1], Cost2 + (ZmaxEnv - ZminEnv)*0.02, 1.);
        }
      }
      p3d_vectCopy(pf, pi);

      u += du;
    }
    u = 0.0;
    //u -= umax;
    localpathPt = localpathPt->next_lp;
    modulo++;
    if (modulo > 3)
      modulo = 0;
  }

  p3d_set_and_update_this_robot_conf(robotPt, qsave);
  p3d_destroy_config(robotPt, qsave);
}

/*--------------------------------------------------------------------------*/
/*!
 *  \brief Draw the current trajectory of all the device
 *
 *  \return <NONE>
 *
 */

void g3d_draw_all_tcur(void) {

  if(!ENV.getBool(Env::debugCostOptim))
  {
    p3d_rob *robotPt;
    int r, nr, ir;
    
    r = p3d_get_desc_curnum(P3D_ROBOT);
    nr = p3d_get_desc_number(P3D_ROBOT);
    
    for (ir = 0;ir < nr;ir++) {
      p3d_sel_desc_num(P3D_ROBOT, ir);
      robotPt = (p3d_rob *) p3d_get_desc_curid(P3D_ROBOT);
      if (robotPt)
      {
	      g3d_draw_tcur(robotPt, p3d_get_user_drawnjnt(), NB_KEY_FRAME);
      }
    }
    p3d_sel_desc_num(P3D_ROBOT, r);
  }
  
  if (ext_g3d_traj_debug) {
    ext_g3d_traj_debug();
  }
}

/*--------------------------------------------------------------------------*/
/*!
 *  \brief Draw a trace for the current trajectory of all the device
 *
 *  \return <NONE>
 *
 */
void g3d_draw_trace_all_tcur(void) {
  p3d_rob *robotPt;
  int r, nr;

  r = p3d_get_desc_curnum(P3D_ROBOT);
  nr = p3d_get_desc_number(P3D_ROBOT);
  for (int ir = 0;ir < nr;ir++) {
    p3d_sel_desc_num(P3D_ROBOT, ir);
    robotPt = (p3d_rob *) p3d_get_desc_curid(P3D_ROBOT);
    if (robotPt) {
      for (int i = 0; i < robotPt->no; i++) {
        if (robotPt->o[i]->GRAPHIC_TYPE == 3 || robotPt->o[i]->GRAPHIC_TYPE == 0 || robotPt->o[i]->GRAPHIC_TYPE == 1) {
          g3d_draw_tcur(robotPt, robotPt->o[i]->num, NB_KEY_FRAME);
        }
      }
    }
  }
  p3d_sel_desc_num(P3D_ROBOT, r);
}





