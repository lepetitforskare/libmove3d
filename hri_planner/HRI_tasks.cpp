//
// C++ Implementation: HRI_tasks
//
// Description: 
//
//
// Author: Amit Kumar Pandey <akpandey@verdier.laas.fr>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "Util-pkg.h"
#include "P3d-pkg.h"
#include "Rrt-pkg.h"
#include "Planner-pkg.h"
#include "Move3d-pkg.h"
#include "Localpath-pkg.h"
#include "Collision-pkg.h"
#include "Graphic-pkg.h"
#include "Hri_planner-pkg.h"
#include "math.h"
#include "include/hri_bitmap_util.h"
#include "include/hri_bitmap_draw.h"
#include "include/hri_bitmap_cost.h"
#include "include/hri_bitmap_bin_heap.h"
#include "include/HRI_tasks.h"

#include "ManipulationPlanner.hpp"
#include "ManipulationUtils.hpp"
#include "robotPos.h"
#include <list>
#include <string>
#include <iostream>

int CURRENT_HRI_MANIPULATION_TASK;
candidate_poins_for_task candidate_points_to_put;
candidate_poins_for_task candidate_points_to_show;
candidate_poins_for_task candidate_points_to_hide;
candidate_poins_for_task candidate_points_to_hide_away;
candidate_poins_for_task candidate_points_to_give;
candidate_poins_for_task candidate_points_to_putinto_by_jido;
candidate_poins_for_task candidate_points_to_putinto_blue_trashbin_by_jido;
candidate_poins_for_task candidate_points_to_putinto_pink_trashbin_by_jido;
candidate_poins_for_task current_candidate_points_to_putinto;
candidate_poins_for_task candidate_points_to_put_away;
candidate_poins_for_task candidate_points_to_displace_obj;

candidate_poins_for_task *CANDIDATE_POINTS_FOR_CURRENT_TASK;

extern int UPDATE_MIGHTABILITY_MAP_INFO;
extern int SHOW_MIGHTABILITY_MAP_INFO;
extern char CURRENT_OBJECT_TO_MANIPULATE[50];

extern p3d_env *envPt_MM;

static ManipulationPlanner *manipulation= NULL;

int set_current_HRI_manipulation_task(int arg)
{
 switch(arg)
 {
 case 0:
 CURRENT_HRI_MANIPULATION_TASK=MAKE_OBJECT_ACCESSIBLE;
 break;
 case 1:
 CURRENT_HRI_MANIPULATION_TASK=SHOW_OBJECT;
 break;
 case 2:
 CURRENT_HRI_MANIPULATION_TASK=GIVE_OBJECT;
 break;
 case 3:
 CURRENT_HRI_MANIPULATION_TASK=HIDE_OBJECT;
 break;
 case 4:
 CURRENT_HRI_MANIPULATION_TASK=PUT_AWAY_OBJECT;
 break;
 case 5:
 CURRENT_HRI_MANIPULATION_TASK=HIDE_AWAY_OBJECT;
 break;
 case 6:
 CURRENT_HRI_MANIPULATION_TASK=MAKE_SPACE_FREE_OF_OBJECT_OBJ;
 break;
 case 7:
 CURRENT_HRI_MANIPULATION_TASK=PUT_INTO_OBJECT;
 break;
 }
}



int show_current_task_candidate_points(int show_weight_by_color, int show_weight_by_length)
{
printf("<<<< CURRENT_HRI_MANIPULATION_TASK=%d for object = %s\n",CURRENT_HRI_MANIPULATION_TASK, CURRENT_OBJECT_TO_MANIPULATE);

 switch(CURRENT_HRI_MANIPULATION_TASK)
 {
 case MAKE_OBJECT_ACCESSIBLE:
  CANDIDATE_POINTS_FOR_CURRENT_TASK=&candidate_points_to_put;
  
  show_weighted_candidate_points_to_put_obj(show_weight_by_color);
 break;
 case SHOW_OBJECT:
  CANDIDATE_POINTS_FOR_CURRENT_TASK=&candidate_points_to_show;
  ////show_weighted_candidate_points_to_show_obj(show_weight_by_color);
 break;
 case GIVE_OBJECT:
  CANDIDATE_POINTS_FOR_CURRENT_TASK=&candidate_points_to_give;
  ////show_weighted_candidate_points_to_give_obj(show_weight_by_color);
 break;
 case HIDE_OBJECT:
  CANDIDATE_POINTS_FOR_CURRENT_TASK=&candidate_points_to_hide;
  ////show_weighted_candidate_points_to_hide_obj();
 break;
 }

show_candidate_points_for_current_task(show_weight_by_color,show_weight_by_length);
 
}


int find_current_HRI_manip_task_solution()
{
printf("<<<< CURRENT_HRI_MANIPULATION_TASK=%d for object = %s\n",CURRENT_HRI_MANIPULATION_TASK, CURRENT_OBJECT_TO_MANIPULATE);

 switch(CURRENT_HRI_MANIPULATION_TASK)
 {
 #ifdef HRI_JIDO 
 case MAKE_OBJECT_ACCESSIBLE:
 JIDO_make_obj_accessible_to_human ( CURRENT_OBJECT_TO_MANIPULATE );
 break;
 case SHOW_OBJECT:
 JIDO_show_obj_to_human ( CURRENT_OBJECT_TO_MANIPULATE );
 break;
 case GIVE_OBJECT:
 ////JIDO_give_obj_to_human ( CURRENT_OBJECT_TO_MANIPULATE );
 break;
 case HIDE_OBJECT:
 ////JIDO_hide_obj_from_human ( CURRENT_OBJECT_TO_MANIPULATE );
 break;
 #endif
  #ifdef HRI_HRP2 
 case MAKE_OBJECT_ACCESSIBLE:
 HRP2_make_obj_accessible_to_human ( CURRENT_OBJECT_TO_MANIPULATE );
 break;
 case SHOW_OBJECT:
 printf(">>> CURRENT_HRI_MANIPULATION_TASK=%d\n",CURRENT_HRI_MANIPULATION_TASK);
 HRP2_show_obj_to_human ( CURRENT_OBJECT_TO_MANIPULATE );
 break;
 case GIVE_OBJECT:
 HRP2_give_obj_to_human ( CURRENT_OBJECT_TO_MANIPULATE );
 break;
 case HIDE_OBJECT:
 HRP2_hide_obj_from_human ( CURRENT_OBJECT_TO_MANIPULATE );
 break;
 #endif
 }

  UPDATE_MIGHTABILITY_MAP_INFO=1;
  SHOW_MIGHTABILITY_MAP_INFO=1;   
}

static void initManipulation() {
  if (manipulation == NULL) {
	p3d_rob * robotPt= p3d_get_robot_by_name("JIDOKUKA_ROBOT");//justin//JIDOKUKA_ROBOT
	manipulation= new ManipulationPlanner(robotPt);
//         manipulation->setArmType(GP_LWR); // set the arm type
  }
  return;
}

int JIDO_make_obj_accessible_to_humanOld ( char *obj_to_manipulate )
{
 int obj_index=get_index_of_robot_by_name ( obj_to_manipulate );

 get_set_of_points_to_put_object ( obj_to_manipulate );
 reverse_sort_weighted_candidate_points_to_put_obj();

 printf ( " <<<<<< candidate_points_to_put.no_points = %d >>>>>>>>\n", candidate_points_to_put.no_points );
 if ( candidate_points_to_put.no_points<=0 )
 {
	printf ( " AKP ERROR : No Candidate points\n" );
	return 0;
 }

configPt obj_actual_pos = MY_ALLOC(double,envPt_MM->robot[obj_index]->nb_dof); /* Allocation of temporary robot configuration */
p3d_get_robot_config_into(envPt_MM->robot[obj_index],&obj_actual_pos);

configPt obj_tmp_pos = MY_ALLOC(double,envPt_MM->robot[obj_index]->nb_dof); /* Allocation of temporary robot configuration */
p3d_get_robot_config_into(envPt_MM->robot[obj_index],&obj_tmp_pos);

 if (manipulation== NULL) {
	  initManipulation();
	}

	

//         manipulation->setObjectToManipulate((char*)OBJECT_NAME);
//         manipulation->setSupport((char*)SUPPORT_NAME);
//         manipulation->setCameraJnt((char*)CAMERA_JNT_NAME);
//         manipulation->setCameraFOV(CAMERA_FOV);
//         manipulation->setCameraImageSize(200, 200);

  std::vector <MANPIPULATION_TRAJECTORY_CONF_STR> confs;
  std::vector <SM_TRAJ> smTrajs;
std::vector <p3d_traj*> trajs;
ManipulationData configs(manipulation->robot());
p3d_rob* object= (p3d_rob*) p3d_get_robot_by_name((char*)obj_to_manipulate);
int armID= 0;
p3d_matrix4 T0, T, tAtt;
gpGrasp grasp;
  p3d_matrix4 handFrame;
  MANIPULATION_TASK_MESSAGE status;
  int result;
  gpHand_properties armHandProp = (*manipulation->robot()->armManipulationData)[0].getHandProperties();

//   status= manipulation->armPlanTask(ARM_PICK_GOTO,0,manipulation->robotStart(), manipulation->robotGoto(), (char*)obj_to_manipulate, (char*)"", confs, smTrajs);
std::vector <double>  m_objStart, m_objGoto;
status = manipulation->armPlanTask(ARM_PICK_GOTO,0,manipulation->robotStart(), manipulation->robotGoto(), m_objStart, m_objGoto, (char*)obj_to_manipulate, (char*)"", confs, smTrajs);
			
  if(status==MANIPULATION_TASK_OK)
    //grasp= manipulation->getCurrentGrasp();
    grasp= *( manipulation->getManipulationData().getGrasp() );

  else { 
    printf("%s: %d: ARM_PICK_GOTO failed \n",__FILE__,__LINE__);
    return 1;
  }

  ArmManipulationData mData = (*manipulation->robot()->armManipulationData)[0];

  p3d_mat4Mult(grasp.frame, armHandProp.Tgrasp_frame_hand, handFrame);
  p3d_mat4Mult(handFrame, mData.getCcCntrt()->Tatt2, tAtt);
  //Check if there is a valid configuration of the robot using this graspFrame
  configPt q = NULL;
  gpHand_properties handProp = mData.getHandProperties();
  gpSet_grasp_configuration(manipulation->robot(), grasp, handProp.type);
  gpFix_hand_configuration(manipulation->robot(), handProp, handProp.type);

p3d_get_freeflyer_pose(object, T0);
point_co_ordi goal_pos, point_to_put;

int i1=0;
result= 0;
for ( i1=0;i1<candidate_points_to_put.no_points;i1++ )
 {
printf("checking for place %d \n",i1);

	goal_pos.x=candidate_points_to_put.point[i1].x;
	goal_pos.y=candidate_points_to_put.point[i1].y;
	goal_pos.z=candidate_points_to_put.point[i1].z+0.02;

	point_to_put.x=goal_pos.x;
	point_to_put.y=goal_pos.y;
	point_to_put.z=goal_pos.z;

	obj_tmp_pos[6]=point_to_put.x;
	obj_tmp_pos[7]=point_to_put.y;
	obj_tmp_pos[8]=point_to_put.z;

        p3d_mat4Copy(T0, T);
	T[0][3]=point_to_put.x;
	T[1][3]=point_to_put.y;
	T[2][3]=point_to_put.z;

        p3d_set_freeflyer_pose(object, T);
        g3d_draw_allwin_active();
        for(int i=0; i<50; ++i)
        {
          q = setRobotGraspPosWithoutBase(manipulation->robot(), T, tAtt, FALSE, FALSE, armID, true);
          if(q!=NULL) break;
        }
	if(q==NULL) 
        {
        printf("q==NULL after setRobotGraspPosWithoutBase()\n"); 
        continue;
        }
        status= manipulation->armPlanTask(ARM_TAKE_TO_FREE,0,manipulation->robotStart(), q,  m_objStart, m_objGoto, (char*)obj_to_manipulate, (char*)"", confs, smTrajs);

        printf(" After armPlanTask, result = %d \n",status);
	p3d_set_object_to_carry_to_arm(manipulation->robot(), 0, object->name );

        if(status==MANIPULATION_TASK_OK)
        { 
           result= 1;
           break;
        }
//         else
//         { manipulation->cleanRoadmap(); }
 }//end for ( i1=0;i1<candidate_points_to_put.no_points;i1++ )
 
 p3d_set_freeflyer_pose(object, T0);
 return result;
 
}



int JIDO_make_obj_accessible_to_human ( char *obj_to_manipulate )
{
 int obj_index=get_index_of_robot_by_name ( obj_to_manipulate );

 get_set_of_points_to_put_object ( obj_to_manipulate );
 reverse_sort_weighted_candidate_points_to_put_obj();

 printf ( " <<<<<< candidate_points_to_put.no_points = %d >>>>>>>>\n", candidate_points_to_put.no_points );
 if ( candidate_points_to_put.no_points<=0 )
 {
	printf ( " AKP ERROR : No Candidate points\n" );
	return 0;
 }

configPt obj_actual_pos = MY_ALLOC(double,envPt_MM->robot[obj_index]->nb_dof); /* Allocation of temporary robot configuration */
p3d_get_robot_config_into(envPt_MM->robot[obj_index],&obj_actual_pos);

configPt obj_tmp_pos = MY_ALLOC(double,envPt_MM->robot[obj_index]->nb_dof); /* Allocation of temporary robot configuration */
p3d_get_robot_config_into(envPt_MM->robot[obj_index],&obj_tmp_pos);

 if (manipulation== NULL) {
	  initManipulation();
	}

	

//         manipulation->setObjectToManipulate((char*)OBJECT_NAME);
//         manipulation->setSupport((char*)SUPPORT_NAME);
//         manipulation->setCameraJnt((char*)CAMERA_JNT_NAME);
//         manipulation->setCameraFOV(CAMERA_FOV);
//         manipulation->setCameraImageSize(200, 200);
  std::list<gpGrasp> graspList;
  std::vector <MANPIPULATION_TRAJECTORY_CONF_STR> confs;
  std::vector <SM_TRAJ> smTrajs;
  std::vector <p3d_traj*> trajs;
  ManipulationData configs(manipulation->robot());
  ArmManipulationData mData = (*manipulation->robot()->armManipulationData)[0];
  p3d_rob* object= (p3d_rob*) p3d_get_robot_by_name((char*)obj_to_manipulate);
  int armID= 0;
  p3d_matrix4 T0, T, tAtt;
//   gpGrasp grasp;
  p3d_matrix4 handFrame;
  MANIPULATION_TASK_MESSAGE status;
  int result;
  gpHand_properties armHandProp = (*manipulation->robot()->armManipulationData)[0].getHandProperties();
  std::vector <double>  m_objStart(6), m_objGoto(6);
  configPt q = NULL;
  gpHand_properties handProp = mData.getHandProperties();
  point_co_ordi goal_pos, point_to_put;
  int i1=0;
  double x, y, z, rx, ry, rz;
  MANIPULATION_TASK_MESSAGE message;
  configPt qcur= p3d_get_robot_config(manipulation->robot());
  configPt q0= p3d_get_robot_config(manipulation->robot());
  bool quit= false;


  gpGet_grasp_list(object->name, armHandProp.type, graspList);
  p3d_get_freeflyer_pose(object, T0);
  p3d_get_freeflyer_pose2(object, &x, &y, &z, &rx, &ry, &rz);

//   status= manipulation->armPlanTask(ARM_PICK_GOTO,0,manipulation->robotStart(), manipulation->robotGoto(), (char*)obj_to_manipulate, (char*)"", confs, smTrajs);
  for(std::list<gpGrasp>::iterator iter=graspList.begin(); iter!=graspList.end(); ++iter)
  {
     p3d_set_and_update_this_robot_conf(manipulation->robot(), q0);
     p3d_copy_config_into(manipulation->robot(), q0, &manipulation->robot()->ROBOT_POS);

     printf("grasp id= %d\n",iter->ID);
     p3d_set_freeflyer_pose(object, T0);
    (*manipulation->robot()->armManipulationData)[armID].setManipState(handFree) ;
    
//     status = manipulation->armPlanTask(ARM_PICK_GOTO,0,manipulation->robotStart(), manipulation->robotGoto(), m_objStart, m_objGoto, (char*)obj_to_manipulate, (char*)"", *iter, confs, smTrajs);
    status = manipulation->armPlanTask(ARM_PICK_GOTO,0,manipulation->robotStart(), manipulation->robotGoto(), m_objStart, m_objGoto, (char*)obj_to_manipulate, (char*)"", *iter, confs, smTrajs);



    if(qcur!=NULL)
      p3d_destroy_config(manipulation->robot(), qcur);

    qcur= p3d_get_robot_config(manipulation->robot());
    p3d_copy_config_into(manipulation->robot(), manipulation->robot()->ROBOT_GOTO, &qcur);

    if(status==MANIPULATION_TASK_OK)
    { //grasp= manipulation->getCurrentGrasp();
    // grasp= *( manipulation->getManipulationData().getGrasp() );
     //return 0;
    }
    else { 
      printf("%s: %d: ARM_PICK_GOTO failed \n",__FILE__,__LINE__);
      continue;
    }
  //grasp.print();

  
    p3d_mat4Mult(iter->frame, armHandProp.Tgrasp_frame_hand, handFrame);
    p3d_mat4Mult(handFrame, mData.getCcCntrt()->Tatt2, tAtt);
    //Check if there is a valid configuration of the robot using this graspFrame

  // gpSet_grasp_configuration(manipulation->robot(), grasp, handProp.type);
  // gpFix_hand_configuration(manipulation->robot(), handProp, handProp.type);
  
    for ( i1=0;i1<candidate_points_to_put.no_points;i1++ )
    {
       printf("checking for place %d and grasp id= %d\n",i1, iter->ID);
//          if(i1==0)
//  	goal_pos.x=candidate_points_to_put.point[i1].x +0.5;
//          else
        goal_pos.x=candidate_points_to_put.point[i1].x;
	goal_pos.y=candidate_points_to_put.point[i1].y;
	goal_pos.z=candidate_points_to_put.point[i1].z+0.1;
//goal_pos.z= T0[2][3] + 0.15;
// 	goal_pos.x=T0[0][3];
// 	goal_pos.y=T0[1][3];
// 	goal_pos.z=T0[2][3]+0.04;


	point_to_put.x=goal_pos.x;
	point_to_put.y=goal_pos.y;
	point_to_put.z=goal_pos.z;

	obj_tmp_pos[6]=point_to_put.x;
	obj_tmp_pos[7]=point_to_put.y;
	obj_tmp_pos[8]=point_to_put.z;

        p3d_mat4Copy(T0, T);
	T[0][3]=point_to_put.x;
	T[1][3]=point_to_put.y;
	T[2][3]=point_to_put.z;

        p3d_set_freeflyer_pose2(object, point_to_put.x, point_to_put.y, point_to_put.z, rx, ry, rz);
        
        ////int is_object_visible_for_agent(primary_human_MM, p3d_rob *object, double threshold, int save, int draw_at_end)

        configPt qobject= p3d_get_robot_config(object);
        p3d_copy_config_into(object, qobject, &object->ROBOT_GOTO);
        p3d_destroy_config(object, qobject);



        g3d_draw_allwin_active();
        p3d_set_freeflyer_pose(object, T0);
        for(int i=0; i<1; ++i)
        {
          //q = setRobotGraspPosWithoutBase(manipulation->robot(), T, tAtt, FALSE, FALSE, armID, true);
         // message= manipulation->findArmGraspsConfigs(armID, object, grasp, configs);
          //if(q!=NULL) break;
        }
	if(message!=MANIPULATION_TASK_OK) 
        {
//         printf("findArmGraspsConfigs != MANIPULATION_TASK_OK\n"); 
//         return 1;
//         continue;
        }
// 	if(q==NULL) 
//         {
//         printf("q==NULL after setRobotGraspPosWithoutBase()\n"); 
//         continue;
//         }
        m_objStart.resize(6);
        m_objGoto.resize(6);
        m_objStart[0]= P3D_HUGE;
        m_objStart[1]= P3D_HUGE;
        m_objStart[2]= P3D_HUGE;
        m_objStart[3]= P3D_HUGE;
        m_objStart[4]= P3D_HUGE;
        m_objStart[5]= P3D_HUGE;

        m_objGoto[0]= point_to_put.x;
        m_objGoto[1]= point_to_put.y;
        m_objGoto[2]= point_to_put.z;
        m_objGoto[3]= rx;
        m_objGoto[4]= ry;
        m_objGoto[5]= rz;
//         m_objGoto[3]= P3D_HUGE;
//         m_objGoto[4]= P3D_HUGE;
//         m_objGoto[5]= P3D_HUGE;

       p3d_copy_config_into(manipulation->robot(), qcur, &manipulation->robot()->ROBOT_POS);
        status= manipulation->armPlanTask(ARM_TAKE_TO_FREE,0,qcur, manipulation->robotGoto(),  m_objStart, m_objGoto, (char*)obj_to_manipulate, (char*)"HRP2TABLE", confs, smTrajs);


         printf(" After armPlanTask, result = %d \n",status);
	 //p3d_set_object_to_carry_to_arm(manipulation->robot(), 0, object->name );
         printf("checking for place %d and grasp id= %d\n",i1, iter->ID);

        if(status==MANIPULATION_TASK_OK)
        { 
         printf(">>>>> Found for place %d and grasp id= %d\n",i1, iter->ID);
           result= 1;
           quit= true;
           break;
        }
     }
     if(quit==true)
     { 
       break;
     }
    

    
    result= 0;
  }

 p3d_destroy_config(manipulation->robot(), q0);
 p3d_destroy_config(manipulation->robot(), qcur);
 p3d_set_freeflyer_pose(object, T0);
 return result;
 
}

int JIDO_show_obj_to_human ( char *obj_to_manipulate )
{
 int obj_index=get_index_of_robot_by_name ( obj_to_manipulate );

 get_set_of_points_to_show_object ( obj_to_manipulate );
 reverse_sort_weighted_candidate_points_to_show_obj();

 printf ( " <<<<<< candidate_points_to_show.no_points = %d >>>>>>>>\n", candidate_points_to_show.no_points );
 if ( candidate_points_to_show.no_points<=0 )
 {
	printf ( " AKP ERROR : No Candidate points\n" );
	return 0;
 }

configPt obj_actual_pos = MY_ALLOC(double,envPt_MM->robot[obj_index]->nb_dof); /* Allocation of temporary robot configuration */
p3d_get_robot_config_into(envPt_MM->robot[obj_index],&obj_actual_pos);

configPt obj_tmp_pos = MY_ALLOC(double,envPt_MM->robot[obj_index]->nb_dof); /* Allocation of temporary robot configuration */
p3d_get_robot_config_into(envPt_MM->robot[obj_index],&obj_tmp_pos);

 if (manipulation== NULL) {
	  initManipulation();
	}

	

//         manipulation->setObjectToManipulate((char*)OBJECT_NAME);
//         manipulation->setSupport((char*)SUPPORT_NAME);
//         manipulation->setCameraJnt((char*)CAMERA_JNT_NAME);
//         manipulation->setCameraFOV(CAMERA_FOV);
//         manipulation->setCameraImageSize(200, 200);
  std::list<gpGrasp> graspList;
  std::vector <MANPIPULATION_TRAJECTORY_CONF_STR> confs;
  std::vector <SM_TRAJ> smTrajs;
  std::vector <p3d_traj*> trajs;
  ManipulationData configs(manipulation->robot());
  ArmManipulationData mData = (*manipulation->robot()->armManipulationData)[0];
  p3d_rob* object= (p3d_rob*) p3d_get_robot_by_name((char*)obj_to_manipulate);
  int armID= 0;
  p3d_matrix4 T0, T, tAtt;
//   gpGrasp grasp;
  p3d_matrix4 handFrame;
  MANIPULATION_TASK_MESSAGE status;
  int result;
  gpHand_properties armHandProp = (*manipulation->robot()->armManipulationData)[0].getHandProperties();
  std::vector <double>  m_objStart(6), m_objGoto(6);
  configPt q = NULL;
  gpHand_properties handProp = mData.getHandProperties();
  point_co_ordi goal_pos, point_to_show;
  int i1=0;
  double x, y, z, rx, ry, rz;
  MANIPULATION_TASK_MESSAGE message;
  configPt qcur= p3d_get_robot_config(manipulation->robot());
  configPt q0= p3d_get_robot_config(manipulation->robot());
  bool quit= false;


  gpGet_grasp_list(object->name, armHandProp.type, graspList);
  p3d_get_freeflyer_pose(object, T0);
  p3d_get_freeflyer_pose2(object, &x, &y, &z, &rx, &ry, &rz);

//   status= manipulation->armPlanTask(ARM_PICK_GOTO,0,manipulation->robotStart(), manipulation->robotGoto(), (char*)obj_to_manipulate, (char*)"", confs, smTrajs);
  for(std::list<gpGrasp>::iterator iter=graspList.begin(); iter!=graspList.end(); ++iter)
  {
     p3d_set_and_update_this_robot_conf(manipulation->robot(), q0);
     p3d_copy_config_into(manipulation->robot(), q0, &manipulation->robot()->ROBOT_POS);

     printf("grasp id= %d\n",iter->ID);
     p3d_set_freeflyer_pose(object, T0);
    (*manipulation->robot()->armManipulationData)[armID].setManipState(handFree) ;
    
//     status = manipulation->armPlanTask(ARM_PICK_GOTO,0,manipulation->robotStart(), manipulation->robotGoto(), m_objStart, m_objGoto, (char*)obj_to_manipulate, (char*)"", *iter, confs, smTrajs);
    status = manipulation->armPlanTask(ARM_PICK_GOTO,0,manipulation->robotStart(), manipulation->robotGoto(), m_objStart, m_objGoto, (char*)obj_to_manipulate, (char*)"", *iter, confs, smTrajs);



    if(qcur!=NULL)
      p3d_destroy_config(manipulation->robot(), qcur);

    qcur= p3d_get_robot_config(manipulation->robot());
    p3d_copy_config_into(manipulation->robot(), manipulation->robot()->ROBOT_GOTO, &qcur);

    if(status==MANIPULATION_TASK_OK)
    { //grasp= manipulation->getCurrentGrasp();
    // grasp= *( manipulation->getManipulationData().getGrasp() );
     //return 0;
    }
    else { 
      printf("%s: %d: ARM_PICK_GOTO failed \n",__FILE__,__LINE__);
      continue;
    }
  //grasp.print();

  
    p3d_mat4Mult(iter->frame, armHandProp.Tgrasp_frame_hand, handFrame);
    p3d_mat4Mult(handFrame, mData.getCcCntrt()->Tatt2, tAtt);
    //Check if there is a valid configuration of the robot using this graspFrame

  // gpSet_grasp_configuration(manipulation->robot(), grasp, handProp.type);
  // gpFix_hand_configuration(manipulation->robot(), handProp, handProp.type);
  
    for ( i1=0;i1<candidate_points_to_show.no_points;i1++ )
    {
       printf("checking for place %d and grasp id= %d\n",i1, iter->ID);
//          if(i1==0)
//  	goal_pos.x=candidate_points_to_put.point[i1].x +0.5;
//          else
        goal_pos.x=candidate_points_to_show.point[i1].x;
	goal_pos.y=candidate_points_to_show.point[i1].y;
	goal_pos.z=candidate_points_to_show.point[i1].z+0.1;
//goal_pos.z= T0[2][3] + 0.15;
// 	goal_pos.x=T0[0][3];
// 	goal_pos.y=T0[1][3];
// 	goal_pos.z=T0[2][3]+0.04;


	point_to_show.x=goal_pos.x;
	point_to_show.y=goal_pos.y;
	point_to_show.z=goal_pos.z;

	obj_tmp_pos[6]=point_to_show.x;
	obj_tmp_pos[7]=point_to_show.y;
	obj_tmp_pos[8]=point_to_show.z;

        p3d_mat4Copy(T0, T);
	T[0][3]=point_to_show.x;
	T[1][3]=point_to_show.y;
	T[2][3]=point_to_show.z;

        p3d_set_freeflyer_pose2(object, point_to_show.x, point_to_show.y, point_to_show.z, rx, ry, rz);

        configPt qobject= p3d_get_robot_config(object);
        p3d_copy_config_into(object, qobject, &object->ROBOT_GOTO);
        p3d_destroy_config(object, qobject);



        g3d_draw_allwin_active();
        p3d_set_freeflyer_pose(object, T0);
        for(int i=0; i<1; ++i)
        {
          //q = setRobotGraspPosWithoutBase(manipulation->robot(), T, tAtt, FALSE, FALSE, armID, true);
         // message= manipulation->findArmGraspsConfigs(armID, object, grasp, configs);
          //if(q!=NULL) break;
        }
	if(message!=MANIPULATION_TASK_OK) 
        {
//         printf("findArmGraspsConfigs != MANIPULATION_TASK_OK\n"); 
//         return 1;
//         continue;
        }
// 	if(q==NULL) 
//         {
//         printf("q==NULL after setRobotGraspPosWithoutBase()\n"); 
//         continue;
//         }
        m_objStart.resize(6);
        m_objGoto.resize(6);
        m_objStart[0]= P3D_HUGE;
        m_objStart[1]= P3D_HUGE;
        m_objStart[2]= P3D_HUGE;
        m_objStart[3]= P3D_HUGE;
        m_objStart[4]= P3D_HUGE;
        m_objStart[5]= P3D_HUGE;

        m_objGoto[0]= point_to_show.x;
        m_objGoto[1]= point_to_show.y;
        m_objGoto[2]= point_to_show.z;
        m_objGoto[3]= rx;
        m_objGoto[4]= ry;
        m_objGoto[5]= rz;
//         m_objGoto[3]= P3D_HUGE;
//         m_objGoto[4]= P3D_HUGE;
//         m_objGoto[5]= P3D_HUGE;

       p3d_copy_config_into(manipulation->robot(), qcur, &manipulation->robot()->ROBOT_POS);
        status= manipulation->armPlanTask(ARM_TAKE_TO_FREE,0,qcur, manipulation->robotGoto(),  m_objStart, m_objGoto, (char*)obj_to_manipulate, (char*)"HRP2TABLE", confs, smTrajs);


         printf(" After armPlanTask, result = %d \n",status);
	 //p3d_set_object_to_carry_to_arm(manipulation->robot(), 0, object->name );
         printf("checking for place %d and grasp id= %d\n",i1, iter->ID);

        if(status==MANIPULATION_TASK_OK)
        { 
         printf(">>>>> Found for place %d and grasp id= %d\n",i1, iter->ID);
           result= 1;
           quit= true;
           break;
        }
     }
     if(quit==true)
     { 
       break;
     }
    

    
    result= 0;
  }

 p3d_destroy_config(manipulation->robot(), q0);
 p3d_destroy_config(manipulation->robot(), qcur);
 p3d_set_freeflyer_pose(object, T0);
 return result;
 
}