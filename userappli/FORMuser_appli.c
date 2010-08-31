#include "Graphic-pkg.h"
#include "UserAppli-pkg.h"
#include "GL/glx.h"
#include "Planner-pkg.h"
#include "Collision-pkg.h"
#include "P3d-pkg.h"
#include "Util-pkg.h"
#include "Move3d-pkg.h"
#ifdef LIGHT_PLANNER
  #include "../lightPlanner/proto/DlrPlanner.h"
  #include "../lightPlanner/proto/DlrParser.h"
  #include "../lightPlanner/proto/lightPlanner.h"
  #include "../lightPlanner/proto/lightPlannerApi.h"
#include "../lightPlanner/proto/robotPos.h"
#endif
#ifdef DPG
  #include "../planner/dpg/proto/p3d_chanEnv_proto.h"
#endif
#ifdef GRASP_PLANNING
  #include "GraspPlanning-pkg.h"
  #ifdef LIGHT_PLANNER
    #include "Manipulation.h"
  #endif
#endif
FL_FORM *USER_APPLI_FORM = NULL;
static void callbacks(FL_OBJECT *ob, long arg);
static int CB_userAppliForm_OnClose(FL_FORM *form, void *arg);
//PLANNING
static FL_OBJECT  *OFFLINE_FRAME;
static FL_OBJECT  *OFFLINE;
static FL_OBJECT  *OFFLINE_OPEN;
static FL_OBJECT  *OFFLINE_CLOSED;
static FL_OBJECT  *COMPUTE_PATH_FRAME;
static FL_OBJECT  *COMPUTE_PATH;
static FL_OBJECT  *COMPUTE_PATH_OPEN;
static FL_OBJECT  *COMPUTE_PATH_CLOSED;
static FL_OBJECT  *COMPUTE_PATH_NEAR;
//VISUALIZATION
static FL_OBJECT  *VISUALIZATION_FRAME;
static FL_OBJECT  *SHOW_TRAJ;
static FL_OBJECT  *SAVE_TRAJ;
static FL_OBJECT  *USE_LIN;
//SET POS
static FL_OBJECT  *SET_POS_FRAME;
static FL_OBJECT  *SET_INIT_OBJECT_POS;
static FL_OBJECT  *SET_GOTO_OBJECT_POS;
//MISC
static FL_OBJECT  *MISC_FRAME;
static FL_OBJECT  *TESTMODEL;
static FL_OBJECT  *SPECIFIC_MULTI;
static FL_OBJECT  *TESTS;
static FL_OBJECT  *TATT;
static G3D_Window *win;
//Grasp
static FL_OBJECT  *GRASP_FRAME;
static FL_OBJECT  *GRASPTEST;
static FL_OBJECT  *GRASPOBJECT;
static FL_OBJECT  *CARRYOBJECT;
static FL_OBJECT  *FINDTRANSFERTGRASP;
static FL_OBJECT  *DRAWSINGLEGRASP;
static FL_OBJECT  *DRAWDOUBLEGRASP;

extern FL_OBJECT  *user_obj;

void g3d_create_user_appli_form(void){
  win = (G3D_Window *)malloc(sizeof(G3D_Window));
  g3d_create_form(&USER_APPLI_FORM, 300, 400, FL_UP_BOX);
  g3d_create_labelframe(&OFFLINE_FRAME, FL_ENGRAVED_FRAME, -1, -1, "testLP", (void**)&USER_APPLI_FORM, 1);
  g3d_create_button(&OFFLINE,FL_NORMAL_BUTTON,-1,30.0,"Offline planner",(void**)&OFFLINE_FRAME,0);
  fl_set_call_back(OFFLINE,callbacks,0);
  g3d_create_button(&OFFLINE_OPEN,FL_NORMAL_BUTTON,-1,30.0,"Offline open chain planner",(void**)&OFFLINE_FRAME,0);
  fl_set_call_back(OFFLINE_OPEN,callbacks,1);
  g3d_create_button(&OFFLINE_CLOSED,FL_NORMAL_BUTTON,-1,30.0,"Offline closed chain planner",(void**)&OFFLINE_FRAME,0);
  fl_set_call_back(OFFLINE_CLOSED,callbacks,2);

  g3d_create_labelframe(&COMPUTE_PATH_FRAME, FL_ENGRAVED_FRAME, -1, -1, "Path computing", (void**)&USER_APPLI_FORM, 1);
  g3d_create_button(&COMPUTE_PATH,FL_NORMAL_BUTTON,-1,30.0,"Compute path",(void**)&COMPUTE_PATH_FRAME,0);
  fl_set_call_back(COMPUTE_PATH,callbacks,3);
  g3d_create_button(&COMPUTE_PATH_OPEN,FL_NORMAL_BUTTON,-1,30.0,"Open chain Path",(void**)&COMPUTE_PATH_FRAME,0);
  fl_set_call_back(COMPUTE_PATH_OPEN,callbacks,4);
  g3d_create_button(&COMPUTE_PATH_CLOSED,FL_NORMAL_BUTTON,-1,30.0,"Closed chain path",(void**)&COMPUTE_PATH_FRAME,0);
  fl_set_call_back(COMPUTE_PATH_CLOSED,callbacks,5);
  g3d_create_button(&COMPUTE_PATH_NEAR,FL_NORMAL_BUTTON,-1,30.0,"Grasp position path",(void**)&COMPUTE_PATH_FRAME,0);
  fl_set_call_back(COMPUTE_PATH_NEAR,callbacks,6);

  g3d_create_labelframe(&VISUALIZATION_FRAME, FL_ENGRAVED_FRAME, -1, -1, "Visualization", (void**)&USER_APPLI_FORM, 1);
  g3d_create_button(&SHOW_TRAJ,FL_NORMAL_BUTTON,-1,30.0,"Play trajectory",(void**)&VISUALIZATION_FRAME,0);
  fl_set_call_back(SHOW_TRAJ,callbacks,7);
  g3d_create_button(&SAVE_TRAJ,FL_NORMAL_BUTTON,-1,30.0,"Save Traj.",(void**)&VISUALIZATION_FRAME,0);
  fl_set_call_back(SAVE_TRAJ,callbacks,10);
  g3d_create_button(&USE_LIN,FL_PUSH_BUTTON,-1,30.0,"Linear",(void**)&VISUALIZATION_FRAME,0);
  fl_set_call_back(USE_LIN,callbacks,11);

  g3d_create_labelframe(&SET_POS_FRAME, FL_ENGRAVED_FRAME, -1, -1, "Set Object Position", (void**)&USER_APPLI_FORM, 1);
  g3d_create_button(&SET_INIT_OBJECT_POS,FL_NORMAL_BUTTON,-1,30.0,"Init",(void**)&SET_POS_FRAME,0);
  fl_set_call_back(SET_INIT_OBJECT_POS,callbacks,8);
  g3d_create_button(&SET_GOTO_OBJECT_POS,FL_NORMAL_BUTTON,-1,30.0,"Goto",(void**)&SET_POS_FRAME,0);
  fl_set_call_back(SET_GOTO_OBJECT_POS,callbacks,9);

  g3d_create_labelframe(&MISC_FRAME, FL_ENGRAVED_FRAME, -1, -1, "Misc", (void**)&USER_APPLI_FORM, 1);
  g3d_create_button(&TESTMODEL,FL_NORMAL_BUTTON,30.0,30.0,"Dynamic",(void**)&MISC_FRAME,0);
  fl_set_call_back(TESTMODEL,callbacks,12);
  g3d_create_button(&SPECIFIC_MULTI,FL_NORMAL_BUTTON,60.0,30.0,"specific Multi",(void**)&MISC_FRAME,0);
  fl_set_call_back(SPECIFIC_MULTI,callbacks,13);
  g3d_create_button(&TESTS,FL_NORMAL_BUTTON,60.0,30.0,"Tests",(void**)&MISC_FRAME,0);
  fl_set_call_back(TESTS,callbacks,14);
  g3d_create_button(&TATT,FL_NORMAL_BUTTON,60.0,30.0,"Tatt",(void**)&MISC_FRAME,0);
  fl_set_call_back(TATT,callbacks,15);

  g3d_create_labelframe(&GRASP_FRAME, FL_ENGRAVED_FRAME, -1, -1, "Grasp", (void**)&USER_APPLI_FORM, 1);
  g3d_create_button(&GRASPTEST,FL_NORMAL_BUTTON,30.0,30.0,"test",(void**)&GRASP_FRAME,0);
  fl_set_call_back(GRASPTEST,callbacks,16);
  g3d_create_button(&GRASPOBJECT,FL_NORMAL_BUTTON,30.0,30.0,"Regrasp Task",(void**)&GRASP_FRAME,0);
  fl_set_call_back(GRASPOBJECT,callbacks,17);
  g3d_create_button(&CARRYOBJECT,FL_NORMAL_BUTTON,30.0,30.0,"Offline",(void**)&GRASP_FRAME,0);
  fl_set_call_back(CARRYOBJECT,callbacks,18);
  g3d_create_button(&FINDTRANSFERTGRASP,FL_NORMAL_BUTTON,30.0,30.0,"TGrasp",(void**)&GRASP_FRAME,0);
  fl_set_call_back(FINDTRANSFERTGRASP,callbacks,19);
  g3d_create_button(&DRAWSINGLEGRASP,FL_NORMAL_BUTTON,30.0,30.0,"SingleGrasp",(void**)&GRASP_FRAME,0);
  fl_set_call_back(DRAWSINGLEGRASP,callbacks,20);
  g3d_create_button(&DRAWDOUBLEGRASP,FL_NORMAL_BUTTON,30.0,30.0,"DoubleGrasp",(void**)&GRASP_FRAME,0);
  fl_set_call_back(DRAWDOUBLEGRASP,callbacks,21);
  
  fl_end_form();
  fl_set_form_atclose(USER_APPLI_FORM, CB_userAppliForm_OnClose, 0);
}

void g3d_delete_user_appli_form(void)
{
  //PLANNING
  g3d_fl_free_object(OFFLINE);
  g3d_fl_free_object(OFFLINE_OPEN);
  g3d_fl_free_object(OFFLINE_CLOSED);
  g3d_fl_free_object(OFFLINE_FRAME);
  g3d_fl_free_object(COMPUTE_PATH);
  g3d_fl_free_object(COMPUTE_PATH_OPEN);
  g3d_fl_free_object(COMPUTE_PATH_CLOSED);
  g3d_fl_free_object(COMPUTE_PATH_NEAR);
  g3d_fl_free_object(COMPUTE_PATH_FRAME);
  //VISUALIZATION
  g3d_fl_free_object(SHOW_TRAJ);
  g3d_fl_free_object(VISUALIZATION_FRAME);
  //SET POS
  g3d_fl_free_object(SET_INIT_OBJECT_POS);
  g3d_fl_free_object(SET_GOTO_OBJECT_POS);
  g3d_fl_free_object(SET_POS_FRAME);
  //MISC
  g3d_fl_free_object(TESTMODEL);
  g3d_fl_free_object(SPECIFIC_MULTI);
  g3d_fl_free_object(TESTS);
  g3d_fl_free_object(TATT);
  g3d_fl_free_object(MISC_FRAME);
  //GRASP
  g3d_fl_free_object(GRASPTEST);
  g3d_fl_free_object(GRASPOBJECT);
  g3d_fl_free_object(CARRYOBJECT);
  g3d_fl_free_object(FINDTRANSFERTGRASP);
  g3d_fl_free_object(GRASP_FRAME);
  
  g3d_fl_free_form(USER_APPLI_FORM);
}

/****************************************************************************/
/** \brief This function is called when the "User appli" window is closed from the X button
  If we do not use this call back, XForms tries to close the entire application
    and we do not want that. Instead we will just click on the Cancel button
 \param *form a pointer on the FL_FORM
 \param *arg argument for the call back function (not used)
 \return FL_IGNORE.
 */
/****************************************************************************/
static int CB_userAppliForm_OnClose(FL_FORM *form, void *arg)
{
  //Call the fonction closing the form.
  g3d_delete_user_appli_form();
  fl_set_button(user_obj,0);//release the button path_Deformation on planner FORM
  //If we return FL_OK, the application will continue to try to shut down itself
  //   if however we return FL_IGNORE, the application will not continue this event
  return FL_IGNORE;
}

static void callbacks(FL_OBJECT *ob, long arg){
#ifdef LIGHT_PLANNER
  p3d_matrix4 att1, att2;
  if(XYZ_ROBOT->ccCntrts != NULL){
    p3d_mat4Copy(XYZ_ROBOT->ccCntrts[0]->Tatt, att1);
    if(XYZ_ROBOT->nbCcCntrts == 2){
      p3d_mat4Copy(XYZ_ROBOT->ccCntrts[1]->Tatt, att2);
    }else{
      att2[0][0] = att2[0][1] = att2[0][2] = 0.0;
    }
    p3d_set_and_update_robot_conf_multisol(XYZ_ROBOT->ROBOT_POS, NULL);
  }
  static p3d_matrix4 objectInitPos, objectGotoPos;
  static int isObjectInitPosInitialised = FALSE, isObjectGotoPosInitialised = FALSE;
#ifdef MULTILOCALPATH
	initLightPlannerForMLP(XYZ_ROBOT);
#endif
#ifdef GRASP_PLANNING
  static gpGrasp grasp;
  static int whichArm = 0;
  static Manipulation manip(XYZ_ROBOT);
#endif
#endif
  switch (arg){
    case 0:{
#if defined(LIGHT_PLANNER) && !defined(GRASP_PLANNING)
    correctGraphForNewFixedJoints(XYZ_GRAPH, XYZ_ROBOT->ROBOT_POS, 1, &XYZ_ROBOT->baseJnt);
#endif
#if defined(LIGHT_PLANNER) && defined(GRASP_PLANNING)
//     correctGraphForHandsAndObject(XYZ_ROBOT, XYZ_GRAPH, int rightHandStatus, gpGrasp rightGrasp, int leftHandStatus, gpGrasp leftGrasp, bool carryobject, int whichArm, p3d_matrix4 tAtt);
#endif
      break;
    }
    case 1:{
#ifdef LIGHT_PLANNER
      if(!isObjectGotoPosInitialised){
        p3d_set_and_update_robot_conf(XYZ_ROBOT->ROBOT_GOTO);
        p3d_mat4Copy(XYZ_ROBOT->curObjectJnt->jnt_mat, objectGotoPos);
        isObjectGotoPosInitialised = TRUE;
      }
      carryObjectByMat(XYZ_ROBOT, objectGotoPos, att1, att2);
#endif
      break;
    }
    case 2:{
#ifdef LIGHT_PLANNER
      if(!isObjectInitPosInitialised){
        p3d_set_and_update_robot_conf(XYZ_ROBOT->ROBOT_POS);
        p3d_mat4Copy(XYZ_ROBOT->curObjectJnt->jnt_mat, objectInitPos);
        isObjectInitPosInitialised = TRUE;
      }
      computeOfflineClosedChain(XYZ_ROBOT, objectInitPos);
#endif
      break;
    }
    case 3:{
#ifdef LIGHT_PLANNER
      DlrPlanner* planner = new DlrPlanner((char*)"./trajFile");
      DlrParser parser((char*)"./planner_input.txt", planner);
      parser.parse();
      planner->process();
#endif
      break;
    }
    case 4:{
#ifdef LIGHT_PLANNER
      if(!isObjectInitPosInitialised){
        p3d_set_and_update_robot_conf(XYZ_ROBOT->ROBOT_POS);
        p3d_mat4Copy(XYZ_ROBOT->curObjectJnt->jnt_mat, objectInitPos);
        isObjectInitPosInitialised = TRUE;
      }
			platformGotoObjectByMat(XYZ_ROBOT, objectInitPos, att1, att2);
#endif
      break;
    }
    case 5:{
#ifdef LIGHT_PLANNER
      if(!isObjectGotoPosInitialised){
        p3d_set_and_update_robot_conf(XYZ_ROBOT->ROBOT_GOTO);
        p3d_mat4Copy(XYZ_ROBOT->curObjectJnt->jnt_mat, objectGotoPos);
        isObjectGotoPosInitialised = TRUE;
      }
			carryObject(XYZ_ROBOT, objectGotoPos, att1, att2);
#endif
      break;
    }
    case 6:{
#ifdef LIGHT_PLANNER
      if(!isObjectInitPosInitialised){
        p3d_set_and_update_robot_conf(XYZ_ROBOT->ROBOT_POS);
        p3d_mat4Copy(XYZ_ROBOT->curObjectJnt->jnt_mat, objectInitPos);
        isObjectInitPosInitialised = TRUE;
      }
      gotoObjectByMat(XYZ_ROBOT, objectInitPos, att1, att2);
#endif
      break;
    }
    case 7:{
//      viewTraj();
#ifdef LIGHT_PLANNER
      deactivateHandsVsObjectCol(XYZ_ROBOT);
#endif
      break;
    }
    case 8:{
#ifdef LIGHT_PLANNER
      p3d_mat4Copy(XYZ_ROBOT->curObjectJnt->jnt_mat, objectInitPos);
      isObjectInitPosInitialised = TRUE;
#endif
      break;
    }
    case 9:{
#ifdef LIGHT_PLANNER
      p3d_mat4Copy(XYZ_ROBOT->curObjectJnt->jnt_mat, objectGotoPos);
      isObjectGotoPosInitialised = TRUE;
#endif
      break;
    }
    case 10:{
#ifdef LIGHT_PLANNER
      saveTrajInFile("./trajFile.txt", (p3d_traj*) p3d_get_desc_curid(P3D_TRAJ), 1, p3d_get_env_graphic_dmax());
#endif
      break;
    }
    case 11:{
#ifdef LIGHT_PLANNER
      if (fl_get_button(ob) == 1){
        //fixJoint(XYZ_ROBOT, XYZ_ROBOT->baseJnt, XYZ_ROBOT->baseJnt->jnt_mat);
  //       fixJoint(XYZ_ROBOT, XYZ_ROBOT->curObjectJnt, XYZ_ROBOT->curObjectJnt->jnt_mat);
        shootTheObjectArroundTheBase(XYZ_ROBOT, XYZ_ROBOT->baseJnt,XYZ_ROBOT->curObjectJnt, -1);
        deactivateHandsVsObjectCol(XYZ_ROBOT);
      }else{
        unFixJoint(XYZ_ROBOT, XYZ_ROBOT->baseJnt);
  //       fixJoint(XYZ_ROBOT, XYZ_ROBOT->curObjectJnt, XYZ_ROBOT->curObjectJnt->jnt_mat);
        shootTheObjectInTheWorld(XYZ_ROBOT, XYZ_ROBOT->baseJnt);
        deactivateHandsVsObjectCol(XYZ_ROBOT);
      }
#endif
      break;
    }
    case 12:{
#ifdef DPG
    int j = 0, returnValue = 0, optimized = XYZ_ROBOT->tcur->isOptimized;
      if(optimized){
        p3dAddTrajToGraph(XYZ_ROBOT, XYZ_GRAPH, XYZ_ROBOT->tcur);
      }
      do{
        printf("Test %d\n", j);
        j++;
//        returnValue = replanForCollidingPath(XYZ_ROBOT, XYZ_ROBOT->tcur, XYZ_GRAPH, XYZ_ROBOT->ROBOT_POS, XYZ_ROBOT->tcur->courbePt, optimized);
        returnValue = checkCollisionsOnPathAndReplan(XYZ_ROBOT, XYZ_ROBOT->tcur, XYZ_GRAPH, optimized);
      }while(returnValue != 1 && returnValue != 0 && j < 10);
      if (optimized && j > 1){
        optimiseTrajectory(100,6);
      }
#endif
      break;
    }
    case 13:{
#ifdef MULTIGRAPH
     p3d_specificSuperGraphLearn();
#endif
      break;
    }
    case 14:{
     //p3d_computeTests();
//       nbLocalPathPerSecond();
//      nbCollisionPerSecond();

//       double curTime = 0;
//       int counter = 0, nFail = 0;
//       ChronoOn();
// 
//       while(curTime < 60){
//         configPt q = p3d_alloc_config(XYZ_ROBOT);
//         do {
//           p3d_shoot(XYZ_ROBOT, q, true);
//           nFail++;
//         } while (!p3d_set_and_update_this_robot_conf_with_partial_reshoot(XYZ_ROBOT, q));
// //        g3d_draw_allwin_active();
//         double tu = 0.0, ts = 0.0;
//         ChronoTimes(&tu, &ts);
//         curTime = tu;
//         counter++;
//       }
//       ChronoOff();
//       printf("Valid shoots in 1 min = %d, failed = %d\n", counter, nFail - counter);
//      for (int i = 0; i < 1; i++) {
//        char graphFile[1024], str[1024], newGraphFile[1024];
//        sprintf(graphFile, "%s/video/graphs/regrasp%d.graph", getenv("HOME_MOVE3D"), i);
//        sprintf(newGraphFile, "%s/video/graphs/regrasp.graph", getenv("HOME_MOVE3D"));
//        sprintf(str, "mv %s %s", graphFile, newGraphFile);
//        system(str);
//        p3d_readGraph(newGraphFile, DEFAULTGRAPH);
//      }
#if defined(PQP) && defined(LIGHT_PLANNER) && defined(GRASP_PLANNING)
      gpHand_properties prop1;
      prop1.initialize(GP_SAHAND_RIGHT);
      gpHand_properties prop2;
      prop2.initialize(GP_SAHAND_LEFT);
      gpFix_hand_configuration(XYZ_ROBOT, prop2, 2);
      gpSet_hand_rest_configuration(XYZ_ROBOT, prop2, 2);
      gpFix_hand_configuration(XYZ_ROBOT, prop1, 1);
      gpSet_hand_rest_configuration(XYZ_ROBOT, prop1, 1);
      gpDeactivate_hand_selfcollisions(XYZ_ROBOT, 1);
      gpDeactivate_hand_selfcollisions(XYZ_ROBOT, 2);
#endif
      break;
    }
    case 15:{
#ifdef LIGHT_PLANNER
      for(int i = 0; i < XYZ_ROBOT->nbCcCntrts; i++){
        p3d_compute_Tatt(XYZ_ROBOT->ccCntrts[i]);
        p3d_mat4Print(XYZ_ROBOT->ccCntrts[i]->Tatt, "Tatt");
      }
#endif
      break;
    }
    case 16 :{
//#if defined(PQP) && defined(LIGHT_PLANNER) && defined(GRASP_PLANNING)
//      configPt startConf = p3d_copy_config(XYZ_ROBOT, XYZ_ROBOT->ROBOT_POS);
//      configPt endConf = p3d_copy_config(XYZ_ROBOT, XYZ_ROBOT->ROBOT_GOTO);
//      for (int i = 0; i < 1; i++) {
////        manip.computeOfflineRoadmap();
//        char file[255];
//        sprintf(file, "%s/video/graphs/simple/mg50x50_%d.graph", getenv("HOME_MOVE3D"), i);
////        p3d_writeGraph(XYZ_ROBOT->mg, (char *)file, MGGRAPH);
////        XYZ_ROBOT->preComputedGraphs[1] = NULL;
//        manip.computeRegraspTask(p3d_copy_config(XYZ_ROBOT, startConf), p3d_copy_config(XYZ_ROBOT, endConf), file);
//        manip.clear();
//      }
//      manip.printStatDatas();
//      manip.clear();
//#else
//      configPt config = p3d_alloc_config(XYZ_ROBOT), rootConfig = p3d_get_robot_config(XYZ_ROBOT);
//      int singularity = -1, cntrt = -1;
//      
////      cntrt = p3d_isCloseToSingularityConfig(XYZ_ROBOT, XYZ_ROBOT->cntrt_manager, rootConfig , &singularity);
////      if (cntrt != -1) {
////        
//        p3d_APInode_shoot_singularity(XYZ_ROBOT, &config, &singularity, &cntrt, rootConfig, XYZ_ROBOT->ikSol);
//        g3d_draw_allwin_active();
//        p3d_unmark_for_singularity(XYZ_ROBOT->cntrt_manager, cntrt);
//        
//        p3d_destroy_config(XYZ_ROBOT, config);
//        p3d_destroy_config(XYZ_ROBOT, rootConfig);
////      }
//      
//
////      ENV.setBool(Env::UseDPGGrids, true);
//
//#endif
      
      
//Test Multigraph      
#ifdef MULTI_GRAPH      
      int maxNodes = p3d_get_NB_NODES();
      int i =0;
        printf("##########  TEST N %d  ############\n", i+1);
 //       deleteAllGraphs();
 //       p3d_set_NB_NODES(maxNodes);
 //       p3d_set_multiGraph(1);
 //       CB_global_search_obj(NULL,0);
 //       p3d_globalSuperGraphLearn();
        p3d_set_NB_NODES(10000);
        p3d_set_multiGraph(0);
        p3d_specificSuperGraphLearn();
        setTotalCountVar(XYZ_GRAPH);
        mergeStat(XYZ_GRAPH->stat, XYZ_ENV->stat);
        printStatsGraph(XYZ_GRAPH->stat, 1);
      printStatsEnv(XYZ_ENV->stat, 1);
#endif
//        p3d_APInode_shoot_singularity(XYZ_ROBOT, &config, &singularity, &cntrt, rootConfig, XYZ_ROBOT->ikSol);
//        g3d_draw_allwin_active();
//        p3d_unmark_for_singularity(XYZ_ROBOT->cntrt_manager, cntrt);
//        
//        p3d_destroy_config(XYZ_ROBOT, config);
//        p3d_destroy_config(XYZ_ROBOT, rootConfig);
////      }
//      
//
////      ENV.setBool(Env::UseDPGGrids, true);
//
//#endif
      
//Test Closed Chain Hard Exemple
//      static int i = 0;
//      unFixAllJointsExceptBaseAndObject(XYZ_ROBOT);
//      if (!i){
//        p3d_mat4Print(XYZ_ROBOT->baseJnt->jnt_mat, "baseJntMat");
//        fixJoint(XYZ_ROBOT, XYZ_ROBOT->baseJnt, XYZ_ROBOT->baseJnt->jnt_mat);
//        shootTheObjectArroundTheBase(XYZ_ROBOT, XYZ_ROBOT->baseJnt, XYZ_ROBOT->curObjectJnt, 1);
//        i = 1;
//      }else{
//        unFixJoint(XYZ_ROBOT, XYZ_ROBOT->baseJnt);
//        shootTheObjectInTheWorld(XYZ_ROBOT, XYZ_ROBOT->curObjectJnt);
//        i = 0;
      
//      unFixAllJointsExceptBaseAndObject(XYZ_ROBOT);
//      p3d_set_and_update_robot_conf(XYZ_ROBOT->ROBOT_POS);
//      fixJoint(XYZ_ROBOT, XYZ_ROBOT->baseJnt, XYZ_ROBOT->baseJnt->jnt_mat);
//      shootTheObjectArroundTheBase(XYZ_ROBOT, XYZ_ROBOT->baseJnt, XYZ_ROBOT->curObjectJnt, 1);
//      CB_global_search_obj(NULL,0);
//      unFixJoint(XYZ_ROBOT, XYZ_ROBOT->baseJnt);
//      shootTheObjectInTheWorld(XYZ_ROBOT, XYZ_ROBOT->curObjectJnt);
//      p3d_set_and_update_robot_conf(XYZ_ROBOT->ROBOT_GOTO);
//      fixJoint(XYZ_ROBOT, XYZ_ROBOT->baseJnt, XYZ_ROBOT->baseJnt->jnt_mat);
//      shootTheObjectArroundTheBase(XYZ_ROBOT, XYZ_ROBOT->baseJnt, XYZ_ROBOT->curObjectJnt, 1);
//      CB_global_search_obj(NULL,0);
//      unFixJoint(XYZ_ROBOT, XYZ_ROBOT->baseJnt);
//      shootTheObjectInTheWorld(XYZ_ROBOT, XYZ_ROBOT->curObjectJnt);
//      removeAloneNodesInGraph(XYZ_ROBOT, XYZ_GRAPH);
//        setTotalCountVar(XYZ_GRAPH);
//        mergeStat(XYZ_GRAPH->stat, XYZ_ENV->stat);
//        printStatsGraph(XYZ_GRAPH->stat, 1);
//      }
//      printStatsEnv(XYZ_ENV->stat, 1);
//      unFixJoint(XYZ_ROBOT, XYZ_ROBOT->baseJnt);
//      shootTheObjectInTheWorld(XYZ_ROBOT, XYZ_ROBOT->curObjectJnt);
//      removeAloneNodesInGraph(XYZ_ROBOT, XYZ_GRAPH);
#if defined(PQP) && defined(LIGHT_PLANNER) && defined(GRASP_PLANNING)      
      manip.computeRegraspTask(p3d_copy_config(XYZ_ROBOT, XYZ_ROBOT->ROBOT_POS), p3d_copy_config(XYZ_ROBOT, XYZ_ROBOT->ROBOT_GOTO), "",0);
#endif
      break;
    }
    case 17:{
#if defined(PQP) && defined(LIGHT_PLANNER) && defined(GRASP_PLANNING)
      configPt startConfig = p3d_copy_config(XYZ_ROBOT, XYZ_ROBOT->ROBOT_POS), gotoConfig = p3d_copy_config(XYZ_ROBOT, XYZ_ROBOT->ROBOT_GOTO);
      char graphFile[1024];
      sprintf(graphFile, "%s/video/regrasp19.graph", getenv("HOME_MOVE3D"));
      
      if(!manip.computeRegraspTask(p3d_copy_config(XYZ_ROBOT, startConfig), p3d_copy_config(XYZ_ROBOT, gotoConfig), graphFile,1)){
        p3d_destroy_config(XYZ_ROBOT, startConfig);
        p3d_destroy_config(XYZ_ROBOT, gotoConfig);
        break;
      }
      if(!manip.computeRegraspTask(p3d_copy_config(XYZ_ROBOT, startConfig), p3d_copy_config(XYZ_ROBOT, gotoConfig), graphFile,2)){
        p3d_destroy_config(XYZ_ROBOT, startConfig);
        p3d_destroy_config(XYZ_ROBOT, gotoConfig);
        break;
      }
      if(!manip.computeRegraspTask(p3d_copy_config(XYZ_ROBOT, startConfig), p3d_copy_config(XYZ_ROBOT, gotoConfig), graphFile,3)){
        p3d_destroy_config(XYZ_ROBOT, startConfig);
        p3d_destroy_config(XYZ_ROBOT, gotoConfig);
        break;
      }
      if(!manip.computeRegraspTask(p3d_copy_config(XYZ_ROBOT, startConfig), p3d_copy_config(XYZ_ROBOT, gotoConfig), graphFile,4)){
        p3d_destroy_config(XYZ_ROBOT, startConfig);
        p3d_destroy_config(XYZ_ROBOT, gotoConfig);
        break;
      }
      manip.printStatDatas();
      p3d_destroy_config(XYZ_ROBOT, startConfig);
      p3d_destroy_config(XYZ_ROBOT, gotoConfig);
#endif
      break;
    }
    case 18:{
#if defined(PQP) && defined(LIGHT_PLANNER) && defined(GRASP_PLANNING) && defined (MULTIGRAPH)
      for (int i = 0; i < 15; i++) {
        manip.computeOfflineRoadmap();
        char graphFile[1024], mgGraphFile[1024];
        sprintf(graphFile, "%s/video/graphs/regrasp%d.graph", getenv("HOME_MOVE3D"), i);
        sprintf(mgGraphFile, "%s/video/graphs/regraspMg%d.graph", getenv("HOME_MOVE3D"), i);
        p3d_writeGraph(XYZ_GRAPH, graphFile, DEFAULTGRAPH);
        p3d_writeGraph(XYZ_ROBOT->mg, mgGraphFile, MGGRAPH);
        deleteAllGraphs();
        XYZ_ROBOT->preComputedGraphs[1] = NULL;
      }
      manip.printStatDatas();
#endif
      break;
    }
    case 19:{
#if defined(PQP) && defined(LIGHT_PLANNER) && defined(GRASP_PLANNING)
      configPt startConf = p3d_copy_config(XYZ_ROBOT, XYZ_ROBOT->ROBOT_POS);
      configPt endConf = p3d_copy_config(XYZ_ROBOT, XYZ_ROBOT->ROBOT_GOTO);
      std::string graphFile(getenv("HOME_MOVE3D"));
      for (int i = 0; i < 15 ; i++) {
        char graphFileChar[1024];
        sprintf(graphFileChar, "%s/video/graphs4/regrasp%d.graph", getenv("HOME_MOVE3D"), i);
        std::string graphFile(graphFileChar);
        manip.clear();
//        manip.computeRegraspTask(p3d_copy_config(XYZ_ROBOT, startConf), p3d_copy_config(XYZ_ROBOT, endConf), graphFile, 0);
//        manip.computeRegraspTask(p3d_copy_config(XYZ_ROBOT, startConf), p3d_copy_config(XYZ_ROBOT, endConf), graphFile, 4);
        manip.computeRegraspTask(p3d_copy_config(XYZ_ROBOT, startConf), p3d_copy_config(XYZ_ROBOT, endConf), "", 0);
        manip.computeRegraspTask(p3d_copy_config(XYZ_ROBOT, startConf), p3d_copy_config(XYZ_ROBOT, endConf), "", 4);
      }
      manip.printStatDatas();
//      sprintf(newGraphFile, "%s/video/graphs/regrasp.graph", getenv("HOME_MOVE3D"));
#endif      
//      p3d_set_RANDOM_CHOICE(P3D_RANDOM_SAMPLING);
//      p3d_set_SAMPLING_CHOICE(P3D_UNIFORM_SAMPLING);
//      p3d_set_MOTION_PLANNER(P3D_DIFFUSION);
//      ENV.setBool(Env::isCostSpace,true);
//      ENV.setDouble(Env::extensionStep,20);
//      ENV.setBool(Env::biDir,false);
//      ENV.setBool(Env::expandToGoal,false);
//      ENV.setBool(Env::findLowCostConf,true);
//      ENV.setInt(Env::tRrtNbtry, 0);
//      ENV.setDouble(Env::bestCost, P3D_HUGE);
////       p3d_specific_search((char*)"");
////      p3d_specificSuperGraphLearn();
//      ENV.setBool(Env::findLowCostConf,false);
//      ENV.setBool(Env::isCostSpace,false);
//      ENV.setDouble(Env::extensionStep,3);
//      ENV.setBool(Env::biDir,true);
//      ENV.setBool(Env::expandToGoal,true);
//      p3d_list_node *bestNode = XYZ_GRAPH->nodes;
//      for(p3d_list_node *cur = XYZ_GRAPH->nodes; cur->next; cur = cur->next){
//        if(bestNode->N->cost > cur->N->cost){
//          bestNode = cur;
//        }
//      }
//      p3d_copy_config_into(XYZ_ROBOT, bestNode->N->q, &XYZ_ROBOT->ROBOT_POS);
//      printf("Minimal Cost = %f\n", bestNode->N->cost);

      break;
    }
    case 20:{
#if defined(PQP) && defined(LIGHT_PLANNER) && defined(GRASP_PLANNING)
      manip.drawSimpleGraspConfigs();
#endif
      break;
    }
    case 21:{
#if defined(PQP) && defined(LIGHT_PLANNER) && defined(GRASP_PLANNING)
      manip.drawDoubleGraspConfigs();
#endif
      break;
    }
      
  }
}
