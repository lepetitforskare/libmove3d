#include "Collision-pkg.h"
#include "Graphic-pkg.h"
#include "Util-pkg.h"
#include "P3d-pkg.h"
#include "Move3d-pkg.h"
#include "Planner-pkg.h"
#include "Localpath-pkg.h"
#include "GraspPlanning-pkg.h"
#include "../other_libraries/gbM/src/Proto_gbModeles.h"
#include <time.h>
#include <list>
#include <string>
#include "../lightPlanner/proto/lightPlannerApi.h"
#include "../lightPlanner/proto/lightPlanner.h"
#include <sys/stat.h>
#include <sys/types.h>

// static char OBJECT_GROUP_NAME[256]="jido-ob_lin"; // "jido-ob"; //

static bool display_grasps= false;
static p3d_rob *ROBOT= NULL; // the robot
static p3d_rob *HAND_ROBOT= NULL; // the hand robot
static p3d_rob *OBJECT= NULL; // the object to grasp
static p3d_polyhedre *POLYHEDRON= NULL; // the polyhedron associated to the object
static gpHand_properties HAND_PROP;  // information about the used hand
static gpArm_type ARM_TYPE= GP_PA10; // type of the robot's arm
static p3d_vector3 CMASS; // object's center of mass
static p3d_matrix3 IAXES; // object's main inertia axes
static double IAABB[6]; // bounding box aligned on the object's inertia axes
static std::list<gpGrasp> GRASPLIST;
static std::list<class gpDoubleGrasp> DOUBLEGRASPLIST;
static std::list<gpContact> CONTACTLIST;
static std::list<gpContact> CONTACTLIST2;
static std::list<gpVector3D> SAMPLES;
static std::list<gpVector3D> CLOSESTPOINTS;
static std::vector<gpVector3D> POINTS;
static std::vector<gpSphere> SPHERES;
static std::vector<gpHTMatrix> GFRAMES;
// static p3d_vector3 CENTER= {0.02,-0.05,0.1};
// static double RADIUS= 0.07;
// static gpKdTree *KDTREE= NULL;
static gpKdTreeTris *KDTREETRIS= NULL;
static int LEVEL= 0;
static gpGrasp GRASP;   // the current grasp
static gpDoubleGrasp DOUBLEGRASP;
static std::list<gpPose> POSELIST, POSELIST2;
static gpPose POSE;
static bool LOAD_LIST= false;
static bool INIT_IS_DONE= false;
static double DMAX_FAR= 0.05;
static double DMAX_NEAR= 0.003;
static p3d_vector3 Oi={0.0,0.0,0.0}, Of={0.0,0.0,0.0}, Ai={0.0,0.0,0.0}, Af={0.0,0.0,0.0}, Bi={0.0,0.0,0.0}, Bf={0.0,0.0,0.0}, E={0.0,0.0,0.0};


// static unsigned int CNT= 0;
static configPt *PATH= NULL;
static int NB_CONFIGS= 0;

void draw_trajectory ( configPt* configs, int nb_configs );
void draw_grasp_planner();
void draw_test();
void key1();
void key2();

// extern int GP_Init(char *objectName);
// extern p3d_cntrt* GP_GetArmCntrt(p3d_rob *robotPt);
// extern int GP_ComputeGraspList(char *objectName);
// extern configPt GP_FindGraspConfig(bool &needs_to_move);
// extern int GP_FindPath();
// extern int GP_FindPathForArmOnly();
// extern configPt* GP_GetTrajectory(p3d_rob *robotPt, p3d_traj *traj, int &nb_configs);
// extern configPt* GP_GetAllTrajectoriesAsOne(p3d_rob *robotPt, int &nb_configs);
// extern int GP_ConcateneAllTrajectories(p3d_rob *robotPt);
// extern void GP_Reset();
// extern void Gp_ResetGraph();

double COLOR_TAB[15][3]= {  {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 0.0}, {1.0, 0.0, 1.0}, {0.0, 1.0, 1.0} , {1.0, 0.5, 0.5}, {0.5, 1.0, 0.5}, {0.5, 0.5, 1.0}, {1.0, 0.25, 0.5}, {1.0, 0.5, 0.25}, {0.25, 1.0, 0.5}, {0.5, 1.0, 0.25}, {0.25, 0.5, 1.0}, {0.5, 0.25, 1.0}  };

#define NB_POINTS_MAX 10000
static p3d_vector3 RAND_POINT;
static bool INSIDE= false;
static gpConvexHull3D *chull= NULL;

/* --------- FORM VARIABLES ------- */
FL_FORM  * GRASP_PLANNING_FORM = NULL;
static FL_OBJECT * MOTIONGROUP;
static FL_OBJECT * BT_GRASP_OBJ;
static FL_OBJECT * BT_GO_AND_GRASP_OBJ;
static FL_OBJECT * BT_ARM_ONLY_OBJ;
static FL_OBJECT * BT_CAMERA_OBJ;
static FL_OBJECT * BT_RESET_OBJ;
static FL_OBJECT * BT_TEST_OBJ;
static FL_OBJECT * BT_DISPLAY_GRASPS_OBJ;
static FL_OBJECT * BT_LOAD_GRASP_LIST_OBJ;
/* ------------------------------------------ */


/* ---------- FUNCTION DECLARATIONS --------- */
static void g3d_create_grasp_planning_group ( void );
static void CB_grasp_planner_obj ( FL_OBJECT *obj, long arg );
static void CB_go_and_grasp_obj ( FL_OBJECT *obj, long arg );
static void CB_arm_only_obj ( FL_OBJECT *obj, long arg );
static void CB_camera_obj ( FL_OBJECT *obj, long arg );
static void CB_reset_obj ( FL_OBJECT *obj, long arg );
static void CB_test_obj ( FL_OBJECT *obj, long arg );
static void CB_display_grasps_obj ( FL_OBJECT *obj, long arg );
static void CB_load_grasp_list_obj ( FL_OBJECT *obj, long arg );
/* ------------------------------------------ */


/* -------------------- MAIN FORM CREATION GROUP --------------------- */
void g3d_create_grasp_planning_form ( void )
{
	GRASP_PLANNING_FORM = fl_bgn_form ( FL_UP_BOX, 150, 440 );

	g3d_create_grasp_planning_group();
	fl_end_form();
}

void g3d_show_grasp_planning_form ( void )
{
	fl_show_form ( GRASP_PLANNING_FORM, FL_PLACE_SIZE, TRUE, "Grasp Planning" );
}

void g3d_hide_grasp_planning_form ( void )
{
	fl_hide_form ( GRASP_PLANNING_FORM );
}

void g3d_delete_grasp_planning_form ( void )
{
	fl_free_form ( GRASP_PLANNING_FORM );
}
/* ------------------------------------------ */



/* -------------------- MAIN GROUP --------------------- */
static void g3d_create_grasp_planning_group ( void )
{
	int x, y, dy, w, h;
	FL_OBJECT *obj;

	obj = fl_add_labelframe ( FL_ENGRAVED_FRAME, 5, 15, 140, 410, "Grasp planning" );

	MOTIONGROUP = fl_bgn_group();

	x= 15;
	y= 30;
	w= 120;
	h= 40;
	dy= h + 10;
	BT_GRASP_OBJ = fl_add_button ( FL_NORMAL_BUTTON, x, y, w, h, "Grasp planner" );
	BT_GO_AND_GRASP_OBJ= fl_add_button ( FL_NORMAL_BUTTON, x, y + dy, w, h, "Go and grasp the object" );
	BT_ARM_ONLY_OBJ= fl_add_button ( FL_NORMAL_BUTTON, x, y + 2*dy, w, h, "Grasp the object" );
	BT_CAMERA_OBJ= fl_add_button ( FL_NORMAL_BUTTON, x, y + 3*dy, w, h, "Camera (+screenshot)" );
	BT_RESET_OBJ = fl_add_button ( FL_NORMAL_BUTTON, x, y + 4*dy, w, h, "Reset" );
	BT_TEST_OBJ  = fl_add_button ( FL_NORMAL_BUTTON, x, y + 5*dy, w, h, "Test" );
	BT_DISPLAY_GRASPS_OBJ  = fl_add_button ( FL_RADIO_BUTTON, x, y + 6*dy, w, h, "Display grasps" );
	BT_LOAD_GRASP_LIST_OBJ  = fl_add_button ( FL_RADIO_BUTTON, x, y + 7*dy, w, h, "Load grasp list" );

	fl_set_call_back ( BT_GRASP_OBJ, CB_grasp_planner_obj, 1 );
	fl_set_call_back ( BT_GO_AND_GRASP_OBJ, CB_go_and_grasp_obj, 2 );
	fl_set_call_back ( BT_ARM_ONLY_OBJ, CB_arm_only_obj, 2 );
	fl_set_call_back ( BT_CAMERA_OBJ, CB_camera_obj, 3 );
	fl_set_call_back ( BT_RESET_OBJ, CB_reset_obj, 1 );
	fl_set_call_back ( BT_TEST_OBJ, CB_test_obj, 1 );
	fl_set_call_back ( BT_DISPLAY_GRASPS_OBJ, CB_display_grasps_obj, 1 );
	fl_set_object_color ( BT_DISPLAY_GRASPS_OBJ,FL_MCOL,FL_GREEN );
	fl_set_button ( BT_DISPLAY_GRASPS_OBJ, FALSE );
	fl_set_call_back ( BT_LOAD_GRASP_LIST_OBJ, CB_load_grasp_list_obj, 1 );
	fl_set_object_color ( BT_LOAD_GRASP_LIST_OBJ,FL_MCOL,FL_GREEN );
	fl_set_button ( BT_LOAD_GRASP_LIST_OBJ, FALSE );

	fl_end_group();
}

void draw_trajectory ( configPt* configs, int nb_configs )
{
	int i;

	if ( configs==NULL || nb_configs<=0 )
		{  return;  }

	g3d_set_color_mat ( Red, NULL );
	for ( i=0; i<nb_configs; i++ )
		{  g3d_draw_solid_sphere ( configs[i][6], configs[i][7], 1.0, 0.07, 10 );  }

	g3d_set_color_mat ( Green, NULL );
	glBegin ( GL_LINES );
	for ( i=1; i<nb_configs; i++ )
	{
		glVertex3d ( configs[i-1][6], configs[i-1][7], 1 );
		glVertex3d ( configs[i][6], configs[i][7], 1 );
	}
	glEnd();
}


void redraw()
{
	g3d_win *win= NULL;

	win= g3d_get_cur_win();
	win->fct_draw2= & ( draw_grasp_planner );
	win->fct_key1= & ( key1 );
	win->fct_key2= & ( key2 );
	g3d_draw_allwin();
	g3d_draw_allwin_active();
}

int init_graspPlanning ( char *objectName )
{
// 	int i;

	if ( p3d_col_get_mode() !=p3d_col_mode_pqp )
	{
		printf ( "The collision detector MUST be PQP to use graspPlanning module.\n" );
		printf ( "The graspPlanning module will not work.\n" );
		return GP_ERROR;
	}

	ROBOT= p3d_get_robot_by_name ( (char *)GP_ROBOT_NAME );

	HAND_ROBOT= NULL;

	//HAND_ROBOT= gpFind_hand_robot(HAND_PROP);

	HAND_ROBOT= HAND_PROP.initialize();

	if ( ROBOT==NULL )
	{
		printf ( "A robot named \"%s\" is required for some computations.\n", GP_ROBOT_NAME );
//     return GP_ERROR;
	}
	if ( HAND_ROBOT==NULL )
	{
		printf ( "There is no robot corresponding to one of the defined hand robots.\n" );
		printf ( "The graspPlanning module will not work.\n" );
		return GP_ERROR;
	}


	OBJECT= p3d_get_robot_by_name ( objectName );

	if ( OBJECT==NULL )
	{
		printf ( "%s: %d: There is no robot (object) with name \"%s\".\n", __FILE__, __LINE__, objectName );
		return GP_ERROR;
	}

	POLYHEDRON= OBJECT->o[0]->pol[0]->poly;
	poly_build_planes ( POLYHEDRON );

// 	Mass_properties mass_prop;
// 	gpCompute_mass_properties ( POLYHEDRON, &mass_prop );
// 	gpCompute_inertia_axes ( &mass_prop, IAXES );
// 	p3d_vectCopy ( mass_prop.r, CMASS );
// 	gpInertia_AABB ( POLYHEDRON, CMASS, IAXES, IAABB );
        gpCompute_mass_properties(POLYHEDRON);


	printf ( "center of mass: \n\t %f %f %f\n", POLYHEDRON->cmass[0], POLYHEDRON->cmass[1], POLYHEDRON->cmass[2]);
	printf ( "volume: \n\t %f\n", POLYHEDRON->volume);
	printf ( "inertia axes: \n\t %f %f %f \n", POLYHEDRON->inertia_axes[0][0], POLYHEDRON->inertia_axes[0][1], POLYHEDRON->inertia_axes[0][2] );
	printf ( "\t %f %f %f \n", POLYHEDRON->inertia_axes[1][0], POLYHEDRON->inertia_axes[1][1], POLYHEDRON->inertia_axes[1][2] );
	printf ( "\t %f %f %f \n", POLYHEDRON->inertia_axes[2][0], POLYHEDRON->inertia_axes[2][1], POLYHEDRON->inertia_axes[2][2] );

	return GP_OK;
}


void draw_grasp_planner()
{
// gpHand_properties handData;
// handData.initialize(GP_SAHAND_RIGHT);
// p3d_matrix4 Tg;
// p3d_mat4Copy(p3d_mat4IDENTITY, Tg);
// Tg[2][3]= 2; 
// handData.draw(Tg);

  g3d_set_color_mat(Yellow, NULL);
  g3d_draw_solid_sphere(Oi[0],Oi[1],Oi[2], 0.08, 10);
  g3d_draw_solid_sphere(Of[0],Of[1],Of[2], 0.08, 20);

  g3d_set_color_mat(Red, NULL);
  g3d_draw_solid_sphere(Ai[0],Ai[1],Ai[2], 0.08, 10);
  g3d_draw_solid_sphere(Af[0],Af[1],Af[2], 0.08, 20);

  g3d_set_color_mat(Green, NULL);
  g3d_draw_solid_sphere(Bi[0],Bi[1],Bi[2], 0.08, 10);
  g3d_draw_solid_sphere(Bf[0],Bf[1],Bf[2], 0.08, 20);

  g3d_set_color_mat(Violet, NULL);
  g3d_draw_solid_sphere(E[0],E[1],E[2], 0.08, 20);

  // display all the grasps from the list:
  if ( display_grasps )
  {
    for ( std::list<gpGrasp>::iterator iter= GRASPLIST.begin(); iter!=GRASPLIST.end(); iter++ )
            { ( *iter ).draw ( 0.005 );    }
  }

  return;

//   p3d_obj *obj = p3d_get_obst_by_name("test");
//   p3d_rob *rob = p3d_get_robot_by_name("Horse");
glPushMatrix();
glTranslatef(0,0,0.6);
  g3d_draw_p3d_polyhedre(XYZ_ENV->cur_robot->o[0]->pol[0]->poly);
glPopMatrix();
   GRASP.draw ( 0.03 );
	//DOUBLEGRASP.draw(0.03);
return;


// 	int result;
// 	double q[4];
// 	p3d_vector3 p, fingerpad_normal;
// 	p3d_matrix4 frame;
// 	p3d_rob *hand_robot= NULL;
// 
// 






	for ( unsigned int i=0; i<GFRAMES.size(); i++ )
	{
		GFRAMES[i].draw();
	}

	p3d_rob *robot= p3d_get_robot_by_name ( (char *)GP_OBJECT_NAME_DEFAULT );
// 	p3d_obj * object= p3d_get_robot_body_by_name ( robot, (char *)GP_OBJECT_NAME_DEFAULT );
//   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    g3d_draw_p3d_polyhedre(object->pol[0]->poly);
//   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	p3d_matrix4 T;
	p3d_get_body_pose ( robot, 0, T );
	g3d_draw_frame ( T,0.2 );

//   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//  g3d_draw_solid_sphere(CENTER[0],CENTER[1],CENTER[2], 1.1*RADIUS, 45);
//   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for ( unsigned int i=0; i<SPHERES.size(); i++ )
	{
//   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		g3d_draw_solid_sphere ( SPHERES[i].center[0],SPHERES[i].center[1],SPHERES[i].center[2], SPHERES[i].radius, 25 );
		glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
	}

	glPointSize ( 4 );
	glBegin ( GL_POINTS );
	for ( std::list<gpContact>::iterator iter= CONTACTLIST.begin(); iter!=CONTACTLIST.end(); iter++ )
	{
		glVertex3dv ( iter->position );
	}
	glEnd();

	return;
	glLineWidth ( 4 );
	std::list<gpVector3D>::iterator iter1, iter2;
	iter1= SAMPLES.begin();
	iter2= CLOSESTPOINTS.begin();
//   glBegin(GL_LINES);
	glBegin ( GL_POINTS );
	while ( iter1!=SAMPLES.end() )
	{
		glVertex3d ( iter1->x, iter1->y, iter1->z );
//       glVertex3d(iter2->x, iter2->y, iter2->z);
		iter1++;
		iter2++;
	}
	glEnd();
	glLineWidth ( 2 );
//   for(std::list<gpVector3D>::iterator iter= SAMPLES.begin(); iter!=SAMPLES.end(); iter++)
//   {
//     (*iter).draw(1,0,0);
//   }
	glPointSize ( 4 );
	glBegin ( GL_POINTS );
	for ( unsigned int i=0; i<POINTS.size(); i++ )
	{
		glVertex3d ( POINTS[i].x, POINTS[i].y, POINTS[i].z );
	}
	glEnd();

	return;




	return;
	glPushMatrix();
	glTranslatef ( 0, 0, 1.5 );
	CONTACTLIST2.clear();
//  if(KDTREE!=NULL) KDTREE->sphereIntersection(CENTER, RADIUS, CONTACTLIST2);
	glPointSize ( 4 );
	glDisable ( GL_LIGHTING );







	glEnable ( GL_LIGHTING );
//   if(KDTREE!=NULL) KDTREE->draw(LEVEL);
	if ( KDTREETRIS!=NULL ) KDTREETRIS->draw ( LEVEL );
//   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
// g3d_draw_solid_sphere(CENTER[0],CENTER[1],CENTER[2],RADIUS, 45);
//   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glPopMatrix();
	return;

// 	p3d_jnt *jnt= NULL;

	//g3d_draw_robot_joints((p3d_rob*)(p3d_get_desc_curid(P3D_ROBOT)), 0.1);


	gpDraw_inertia_AABB ( CMASS, IAXES, IAABB );

	glPushMatrix();
	glTranslatef ( 0, 0, 3 );
	if ( POLYHEDRON!=NULL )
		g3d_draw_p3d_polyhedre ( POLYHEDRON );
	glPopMatrix();

	return;
//   p3d_vector3 cp1, cp2;
//   p3d_rob *rob1= p3d_get_robot_by_name("gripper_robot");
//   p3d_rob *rob2= p3d_get_robot_by_name("robot");
//  if( pqp_robot_robot_distance(rob1, rob2, cp1, cp2)>0)
//  {
//    gpDraw_solid_sphere(cp1[0], cp1[1], cp1[2], 0.1, 6);
//    gpDraw_solid_sphere(cp2[0], cp2[1], cp2[2], 0.1, 6);
//    g3d_drawOneLine(cp1[0], cp1[1], cp1[2],cp2[0], cp2[1], cp2[2], Red, NULL);
//  }

	int cnt= 0;
	for ( std::list<gpPose>::iterator iter= POSELIST.begin(); iter!=POSELIST.end(); iter++ )
	{
//     if(cnt==1)
		( *iter ).draw ( 0.03 );
//break;
		cnt++;
	}

	static bool firstTime= true;
	if ( firstTime )
	{
		gpFind_poses_on_object ( OBJECT, p3d_get_obst_by_name ( (char *)"box7" ), POSELIST, 0.05, 15, POSELIST2 );
		printf ( "%d new poses\n", POSELIST2.size() );
		firstTime= false;
	}

	cnt= 0;
	for ( std::list<gpPose>::iterator iter= POSELIST2.begin(); iter!=POSELIST2.end(); iter++ )
	{
		( *iter ).draw ( 0.03 );
		cnt++;
		if ( cnt>200 )
		{
			printf ( "only the first 200 poses are displayed\n" );
			break;
		}
	}


	if ( chull!=NULL )
	{
		glPushMatrix();
		glTranslatef ( 0,0,3 );
		chull->draw ( false );
		glPopMatrix();


		glDisable ( GL_LIGHTING );
		if ( INSIDE )
			{ glColor3f ( 0, 0, 1 ); }
		else
			{ glColor3f ( 1, 0, 0 ); }
		g3d_drawSphere ( RAND_POINT[0], RAND_POINT[1], RAND_POINT[2], 0.15, Red, NULL );
		glEnable ( GL_LIGHTING );
	}


//  p3d_rob *robotPt= (p3d_rob *) p3d_get_desc_curid(P3D_ROBOT);


	//p3d_matrix4 Tend_eff;
	//gpForward_geometric_model_PA10(ROBOT, Tend_eff);
// draw_trajectory(PATH, NB_CONFIGS);

	//p3d_vector3 p1, p2;
	p3d_matrix4 pose;
	float mat[16];

	GRASP.draw ( 0.03 );

//  p3d_jnt *j1= get_robot_jnt_by_name(ROBOT, armJoint)

	if ( OBJECT!=NULL )
	{
//     p3d_get_obj_pos(OBJECT, pose);
		p3d_get_body_pose ( OBJECT, 0, pose );
		p3d_matrix4_to_OpenGL_format ( pose, mat );
		/*
		    p2[0]= pose[0][3];
		    p2[1]= pose[1][3];
		    p2[2]= pose[2][3] + 0.05;
		    p1[0]= p2[0] + 0.2;
		    p1[1]= p2[1] + 0.2;
		    p1[2]= p2[2] + 0.5;
		    draw_arrow(p1, p2, 1.0, 0.0, 0.0);
		*/
	}




	/*
	double q[4];
	printf("%f %f %f \n", randomPoint[0], randomPoint[1], randomPoint[2]);
	if(gpSAHfinger_inverse_kinematics(Twrist, HAND_PROP, randomPoint, q, 4))
	{
	   gpSet_SAHfinger_joint_angles(HAND_ROBOT, HAND_PROP, q, 4);
	   g3d_set_color_mat(Green, NULL);
	glPushMatrix();
	  glTranslatef(randomPoint[0], randomPoint[1], randomPoint[2]);
	  gpDraw_solid_sphere(0.01, 15);
	glPopMatrix();
	}
	else
	{
	   g3d_set_color_mat(Red, NULL);
	glPushMatrix();
	  glTranslatef(randomPoint[0], randomPoint[1], randomPoint[2]);
	  gpDraw_solid_sphere(0.01, 15);
	glPopMatrix();
	}
	*/
	/*
	   glDisable(GL_LIGHTING);
	   glPointSize(5);
	   glPushMatrix();
	   glMultMatrixf(mat);
	   for(int i=0; i<POLYHEDRON->nb_faces; i++)
	   {
	     ind= POLYHEDRON->the_faces[i].the_indexs_points;

	     surf_points= sample_triangle_surface(POLYHEDRON->the_points[ind[0]-1], POLYHEDRON->the_points[ind[1]-1], POLYHEDRON->the_points[ind[2]-1], 0.005, &nb_samples);

	     glColor3f(1, 0, 0);
	     glBegin(GL_POINTS);
	       for(int j=0; j<nb_samples; j++)
	         glVertex3dv(surf_points[j]);
	     glEnd();

	     free(surf_points);
	     surf_points= NULL;
	   }
	   glPopMatrix();
	   glEnable(GL_LIGHTING);
	*/

	return;
}

void draw_test()
{

}

void key1()
{
//   if(LEVEL <= KDTREETRIS->depth())
//   LEVEL++;
//   printf("LEVEL= %d\n", LEVEL);
}

void key2()
{
//   if(LEVEL>0)
//   LEVEL--;
//   printf("LEVEL= %d\n", LEVEL);
}

//! Planification de prise dans le cas d'un objet n'ayant pas besoin d'être décomposé
//! en composantes convexes.
static void CB_grasp_planner_obj ( FL_OBJECT *obj, long arg )
{
//   gpGet_grasp_list_SAHand("Horse", 1, GRASPLIST);
// // GRASPLIST.clear();
// //   gpGet_grasp_list_SAHand("Horse", 2, GRASPLIST);
//  return;

  unsigned int i;
  static unsigned int count= 1;
  int result;
  float clock0, time;
  configPt qhand= NULL, qgrasp= NULL;
  p3d_matrix4 objectPose;
  p3d_vector3 objectCenter;
  std::list<gpGrasp>::iterator igrasp;
  std::string handFolderName, graspListName, graspListNameOld;
  p3d_rob* cur_robot= NULL;
  G3D_Window *win = NULL;

  cur_robot= XYZ_ENV->cur_robot;

  //compute the grasp list:
  if ( !INIT_IS_DONE )
  {
    result= init_graspPlanning ((char *) GP_OBJECT_NAME_DEFAULT );
    if ( result==GP_ERROR )
            {  return;  }
    INIT_IS_DONE= true;

    handFolderName= gpHand_type_to_folder_name ( HAND_PROP.type );
//     graspListName= std::string("./graspPlanning/graspLists/") + handFolderName + std::string("/") + std::string(GP_OBJECT_NAME_DEFAULT) + std::string("Grasps.xml");
    graspListName= std::string ( getenv ( "HOME_MOVE3D" ) ) + std::string ( "/graspPlanning/graspLists/" ) + handFolderName + std::string ( "/" ) + std::string ( GP_OBJECT_NAME_DEFAULT ) + std::string ( "Grasps.xml" );
    graspListNameOld= std::string ( getenv ( "HOME_MOVE3D" ) ) +  std::string ( "/graspPlanning/graspLists/" )  + handFolderName + std::string ( "/" ) + std::string ( GP_OBJECT_NAME_DEFAULT ) + std::string ( "Grasps_old.xml" );

    mkdir ( "./testFolder", S_IRWXU|S_IRWXG );


#ifdef LIGHT_PLANNER
   if ( ROBOT!=NULL )
   {
    if ( ROBOT->nbCcCntrts!=0 )
    {  p3d_desactivateCntrt ( ROBOT, ROBOT->ccCntrts[0] );    }
   }
#endif

    p3d_get_body_pose ( OBJECT, 0, objectPose );
    win= g3d_get_cur_win();
    win->x= objectPose[0][3];   win->y= objectPose[1][3];   win->z= objectPose[2][3]+0.1;

    if( LOAD_LIST )
    {
      if( gpLoad_grasp_list(graspListName, GRASPLIST)==GP_ERROR)
      {
         printf( "Can not load a grasp list.\n" );
         return;
      }
      gpGrasp_quality_filter(GRASPLIST);
    }
    else
    {
      clock0= clock();
      rename(graspListName.c_str(), graspListNameOld.c_str() ); //store the current grasp file (if it exists)

      gpGrasp_generation( HAND_ROBOT, OBJECT, 0, HAND_PROP, HAND_PROP.nb_positions, HAND_PROP.nb_directions, HAND_PROP.nb_rotations, GRASPLIST );
      printf ( "Before collision filter: %d grasps.\n", GRASPLIST.size() );
      if ( HAND_PROP.type==GP_GRIPPER )
      {
        gpGrasp_collision_filter ( GRASPLIST, HAND_ROBOT, OBJECT, HAND_PROP );
        printf ( "After collision filter: %d grasps.\n", GRASPLIST.size() );
      }
      gpGrasp_stability_filter ( GRASPLIST );
      gpGrasp_compute_open_configs( GRASPLIST, HAND_ROBOT, OBJECT, HAND_PROP );

      printf ( "After stability filter: %d grasps.\n", GRASPLIST.size() );
      time= ( clock()-clock0 ) /CLOCKS_PER_SEC;
      printf ( "Computation time: %2.1fs= %dmin%ds\n",time, ( int ) ( time/60.0 ), ( int ) ( time - 60* ( ( int ) ( time/60.0 ) ) ) );
      gpSave_grasp_list ( GRASPLIST, graspListName );
      //     gpGrasp_context_collision_filter(GRASPLIST, HAND_ROBOT, OBJECT, HAND_PROP);
      //     printf("For the current collision context: %d grasps.\n", GRASPLIST.size());
    }
//   gpSample_grasp_frames2(POLYHEDRON, HAND_PROP.nb_positions, HAND_PROP.nb_directions, HAND_PROP.nb_rotations, 1000, GFRAMES);
//      gpSample_obj_surface(OBJECT->o[0], 0.01, HAND_PROP.fingertip_radius, CONTACTLIST);


          p3d_col_deactivate_robot ( HAND_ROBOT );
  }

  if ( GRASPLIST.empty() )
  {
          printf ( "No grasp was found.\n" );
          XYZ_ENV->cur_robot= cur_robot;
          return;
  }

  i= 0;
  for ( igrasp=GRASPLIST.begin(); igrasp!=GRASPLIST.end(); igrasp++ )
  {
    GRASP= ( *igrasp );
    i++;
    if ( i>=count )
    {  break; }
  }
  count++;
  if ( count>GRASPLIST.size() )
  {  count= 1;  }

  p3d_get_body_pose ( OBJECT, 0, objectPose );
  objectCenter[0]= objectPose[0][3];
  objectCenter[1]= objectPose[1][3];
  objectCenter[2]= objectPose[2][3];


  //set hand configuration (for hand robot):
  if ( HAND_ROBOT!=NULL )
  {
      XYZ_ENV->cur_robot= HAND_ROBOT;
      qhand= p3d_alloc_config ( HAND_ROBOT );
      gpInverse_geometric_model_freeflying_hand ( HAND_ROBOT, objectPose, GRASP.frame, HAND_PROP, qhand );
      //   qhand[8]= -1; //to put the hand far under the floor
      gpDeactivate_hand_collisions ( HAND_ROBOT, 0 );
gpActivate_hand_collisions ( HAND_ROBOT, 0 );
      p3d_set_and_update_this_robot_conf ( HAND_ROBOT, qhand );
      p3d_destroy_config ( HAND_ROBOT, qhand );
      qhand= NULL;

      GRASP.print();
      
//      gpSet_grasp_open_configuration( HAND_ROBOT, HAND_PROP, GRASP, 0 );
     gpSet_grasp_configuration( HAND_ROBOT, HAND_PROP, GRASP, 0 );


      if ( qhand!=NULL )
      {  p3d_destroy_config ( HAND_ROBOT, qhand );  }
    }

    redraw();
    return;

  //find a configuration for the whole robot (mobile base + arm):
  configPt qcur= NULL;
  qcur= p3d_alloc_config ( ROBOT );
  p3d_get_robot_config_into ( ROBOT, &qcur );

  configPt qend= NULL;
  if ( ROBOT!=NULL )
  {
          for ( i=0; i<150; i++ )
          {
                  qgrasp= gpRandom_robot_base ( ROBOT, GP_INNER_RADIUS, GP_OUTER_RADIUS, objectCenter, ARM_TYPE );

                  if ( qgrasp==NULL )
                          {  break;  }

                  qend= NULL;
                  qend= gpFind_grasp_from_base_configuration ( ROBOT, OBJECT, GRASPLIST, ARM_TYPE, qgrasp, GRASP, HAND_PROP );

                  if ( qend!=NULL )
                  {
                          p3d_set_and_update_this_robot_conf ( ROBOT, qend );
                          XYZ_ENV->cur_robot= ROBOT;
                          p3d_copy_config_into ( ROBOT, qend, &ROBOT->ROBOT_POS );
                          p3d_destroy_config ( ROBOT, qend );
                          qend= NULL;
                          break;
                  }
                  p3d_destroy_config ( ROBOT, qgrasp );
                  qgrasp= NULL;
          }
          if ( qgrasp!=NULL )
                  {  p3d_destroy_config ( ROBOT, qgrasp );  }
          if ( i==150 )
                  {  printf ( "No platform configuration was found.\n" );  }
          else
                  {  printf ( "Grasp planning was successfull.\n" );  }
  }

  XYZ_ENV->cur_robot= cur_robot;

  win= g3d_get_cur_win();
  win->fct_draw2= & ( draw_grasp_planner );
  win->x= objectPose[0][3];   win->y= objectPose[1][3];   win->z= objectPose[2][3];
  g3d_draw_allwin();
  g3d_draw_allwin_active();

  return;
}



//! Centers the camera on the object position and takes a screenshot.
static void CB_camera_obj ( FL_OBJECT *obj, long arg )
{
	static int count= 0;
	static int firstTime= true;
	char filename[128];

	if ( firstTime )
	{
		firstTime= false;
		init_graspPlanning ( (char *)GP_OBJECT_NAME_DEFAULT );
	}

	gpCompute_stable_poses ( OBJECT->o[0], CMASS, POSELIST );
	printf ( "%d poses computed\n", POSELIST.size() );
	if ( !POSELIST.empty() )
	{
		POSE= POSELIST.back();
	}

	for ( std::list<gpPose>::iterator iter= POSELIST.begin(); iter!=POSELIST.end(); iter++ )
		{ ( *iter ).print();    }

	sprintf ( filename, "screenshot-%d.ppm", count++ );
	g3d_export_OpenGL_display ( filename );

	G3D_Window *win = g3d_get_cur_win();
	win->fct_draw2= & ( draw_grasp_planner );
	win->fct_key1= & ( key1 );
	win->fct_key2= & ( key2 );
	g3d_draw_allwin();
}



static void CB_reset_obj ( FL_OBJECT *obj, long arg )
{
	GP_Reset();

	printf ( "GraspPlanning static global values have been reset.\n" );
}

static void CB_go_and_grasp_obj ( FL_OBJECT *obj, long arg )
{
	bool needs_to_move, so_far_so_good= true;
	int result, path_found;
	double x, y, theta, q1, q2, q3, q4, q5, q6;
	std::vector<double> qhand;
	configPt qstart= NULL, qfinal= NULL, qinter1= NULL, qinter2= NULL, qinter3= NULL, qfar= NULL;
	p3d_rob *robotPt= NULL;
	p3d_cntrt* cntrt_arm = NULL;
	robotPt= p3d_get_robot_by_name ((char *) GP_ROBOT_NAME );
	XYZ_ENV->cur_robot= robotPt;

	// initializes everything:
	GP_Init ( (char *)GP_OBJECT_NAME_DEFAULT );

	redraw();

	cntrt_arm= GP_GetArmCntrt ( robotPt );

	if ( cntrt_arm==NULL )
	{
		printf ( "FATAL_ERROR : arm_IK constraint does not exist\n" );
		return;
	}

	/* Deactivate the arm_IK constrint */
	p3d_desactivateCntrt ( robotPt, cntrt_arm );

	//alloc all configs:
	qstart= p3d_alloc_config ( robotPt );
	qfinal= p3d_alloc_config ( robotPt );
	qinter1= p3d_alloc_config ( robotPt );
	qinter2= p3d_alloc_config ( robotPt );
	qinter3= p3d_alloc_config ( robotPt );
	qfar= p3d_alloc_config ( HAND_ROBOT );

	p3d_get_robot_config_into ( robotPt, &qstart );
#ifdef LIGHT_PLANNER
	p3d_update_virtual_object_config_for_pa10_6_arm_ik_constraint ( robotPt, qstart );
#endif
	g3d_draw_allwin_active();

	// computes the grasp list:
	if ( !LOAD_LIST )
	{
		result= GP_ComputeGraspList ( (char *)GP_OBJECT_NAME_DEFAULT );
		gpSave_grasp_list ( GRASPLIST, "./graspPlanning/graspList_new.xml" );
	}
	else // or loads it:
	{
		result= gpLoad_grasp_list ( "./graspPlanning/graspList.xml", GRASPLIST );
		if ( result==0 )
		{
			printf ( "Can not load a grasp list.\n" );
			return;
		}

		if ( !GRASPLIST.empty() )
		{
			if ( GRASPLIST.front().hand_type!=HAND_PROP.type )
			{
				printf ( "The loaded grasp list does not correspond to the current hand type.\n" );
				return;
			}
		}
	}


	// move away the hand robot:
	qfar= p3d_alloc_config ( HAND_ROBOT );
	qfar[7]= -100; //to put the hand far under the floor
	qfar[8]= -1; //to put the hand far under the floor
	p3d_set_and_update_this_robot_conf ( HAND_ROBOT, qfar );
	p3d_destroy_config ( HAND_ROBOT, qfar );

	qfinal= GP_FindGraspConfig ( needs_to_move );

	p3d_set_and_update_this_robot_conf ( robotPt, qstart );
	if ( p3d_col_test() )
	{
		printf ( "Start configuration is colliding.\n" );
		return;
	}


	if ( qfinal!=NULL )
	{
#ifdef LIGHT_PLANNER
		p3d_update_virtual_object_config_for_pa10_6_arm_ik_constraint ( robotPt, qfinal );
#endif
		printf ( "Grasp configuration list was successfully computed.\n" );
		XYZ_ENV->cur_robot= robotPt;
		p3d_set_ROBOT_GOTO ( qfinal );
	}
	else
	{
		printf ( "No grasp configuration was found.\n" );
		so_far_so_good= false;
		redraw();

		//free all configs:
		p3d_destroy_config ( robotPt, qstart );
		p3d_destroy_config ( robotPt, qfinal );
		p3d_destroy_config ( robotPt, qinter1 );
		p3d_destroy_config ( robotPt, qinter2 );
		p3d_destroy_config ( robotPt, qinter3 );
		return;
	}

	p3d_set_and_update_this_robot_conf ( robotPt, qfinal );
	if ( p3d_col_test() )
	{
		printf ( "Final configuration is colliding.\n" );
		return;
	}

	redraw();

	//if the robot needs to move, we have to introduce three intermediate configurations:
	//    qstart =  (qbase0 ; qarm0 ; qhand0)
	// -> qinter1=  (qbase0 ; qarm_folded ; qhand0)
	// -> qinter2=  (qbase1 ; qarm_folded ; qhand0)
	// -> qinter3=  (qbase1 ; qarm1 ; qhand0)
	// -> qfinal =  (qbase1 ; qarm1 ; qhand1)
	if ( needs_to_move )
	{
		// get platform final configuration and arm final configuration:
		p3d_set_and_update_this_robot_conf ( robotPt, qfinal );
		gpGet_platform_configuration ( robotPt, x, y, theta );
		gpGet_arm_configuration ( robotPt, ARM_TYPE, q1, q2, q3, q4, q5, q6 );
		gpGet_hand_configuration ( robotPt, HAND_PROP, 0, qhand );

		p3d_set_and_update_this_robot_conf ( robotPt, qstart );
		if ( HAND_PROP.type==GP_GRIPPER ) gpOpen_hand ( robotPt, HAND_PROP );
		result= gpFold_arm ( robotPt, ARM_TYPE );
		if ( result==0 )
		{
			printf ( "The arm can not be folded.\n" );
		}

		p3d_get_robot_config_into ( robotPt, &qinter1 );
#ifdef LIGHT_PLANNER
		p3d_update_virtual_object_config_for_pa10_6_arm_ik_constraint ( robotPt, qinter1 );
#endif

		gpSet_platform_configuration ( robotPt, x, y, theta );
		p3d_get_robot_config_into ( robotPt, &qinter2 );
#ifdef LIGHT_PLANNER
		p3d_update_virtual_object_config_for_pa10_6_arm_ik_constraint ( robotPt, qinter2 );
#endif

		gpSet_arm_configuration ( robotPt, ARM_TYPE, q1, q2, q3, q4, q5, q6 );
		p3d_get_robot_config_into ( robotPt, &qinter3 );
#ifdef LIGHT_PLANNER
		p3d_update_virtual_object_config_for_pa10_6_arm_ik_constraint ( robotPt, qinter3 );
#endif

		if ( p3d_col_test() )
		{
			printf ( "The robot can not open its hand/gripper in its final arm and base configuration.\n" );
			p3d_copy_config_into ( robotPt, qfinal, &qinter3 );
		}


		printf ( "qstart    qinter1    qinter2    qinter3    qfinal\n" );
		for ( int i=6; i<robotPt->nb_dof; i++ )
		{
			printf ( "%f %f %f %f %f\n", qstart[i], qinter1[i], qinter2[i], qinter3[i], qfinal[i] );
		}

		//test all the intermediate configurations:
		p3d_set_and_update_this_robot_conf ( robotPt, qstart );
		g3d_draw_allwin_active();
		if ( p3d_col_test() ) // if collision
		{
			printf ( "The start configuration is in collision.\n" );
			goto END_GO_AND_GRASP;
		}

		p3d_set_and_update_this_robot_conf ( robotPt, qinter1 );
		g3d_draw_allwin_active();
		if ( p3d_col_test() ) // if collision
		{
			printf ( "The arm can not be folded without collision.\n" );
			goto END_GO_AND_GRASP;
		}

		p3d_set_and_update_this_robot_conf ( robotPt, qinter2 );
		g3d_draw_allwin_active();
		if ( p3d_col_test() ) // if collision
		{
			printf ( "qinter2 is colliding.\n" );
			goto END_GO_AND_GRASP;
		}

		p3d_set_and_update_this_robot_conf ( robotPt, qinter3 );
		g3d_draw_allwin_active();
		if ( p3d_col_test() ) // if collision
		{
			printf ( "qinter3 is colliding.\n" );
			goto END_GO_AND_GRASP;
		}

		p3d_set_and_update_this_robot_conf ( robotPt, qfinal );
		g3d_draw_allwin_active();
		if ( p3d_col_test() ) // if collision
		{
			printf ( "qfinal is colliding.\n" );
			goto END_GO_AND_GRASP;
		}


		p3d_set_and_update_this_robot_conf ( robotPt, qstart );
		p3d_copy_config_into ( robotPt, qstart, & ( robotPt->ROBOT_POS ) );
		p3d_activateCntrt ( robotPt, cntrt_arm );
		g3d_draw_allwin_active();
		p3d_set_ROBOT_START ( qstart );
		p3d_set_ROBOT_GOTO ( qinter1 );

		p3d_set_env_dmax ( DMAX_FAR );
#ifdef MULTILOCALPATH
		p3d_multiLocalPath_disable_all_groupToPlan ( robotPt );
		p3d_multiLocalPath_set_groupToPlan_by_name ( robotPt, "jido-hand", 1 ) ;
		p3d_multiLocalPath_set_groupToPlan_by_name ( robotPt, OBJECT_GROUP_NAME, 1 ) ;
#endif
		path_found= GP_FindPath();
		if ( !path_found )
		{
			printf ( "The planner could not find a path to fold the arm.\n" );
			so_far_so_good= false;
			goto END_GO_AND_GRASP;
		}

		p3d_set_and_update_this_robot_conf ( robotPt, qinter1 );
		p3d_copy_config_into ( robotPt, qinter1, & ( robotPt->ROBOT_POS ) );
		g3d_draw_allwin_active();

#ifdef LIGHT_PLANNER
		setAndActivateTwoJointsFixCntrt ( robotPt, robotPt->curObjectJnt, robotPt->baseJnt );
		p3d_desactivateCntrt ( robotPt, cntrt_arm );
#endif

		p3d_realloc_iksol ( robotPt->cntrt_manager );

		p3d_set_ROBOT_START ( qinter1 );
		p3d_set_ROBOT_GOTO ( qinter2 );

#ifdef MULTILOCALPATH
		p3d_multiLocalPath_disable_all_groupToPlan ( robotPt );
		p3d_multiLocalPath_set_groupToPlan_by_name ( robotPt, "jido-base", 1 ) ;
#endif
		path_found= GP_FindPath();
		if ( !path_found )
		{
			printf ( "The planner could not find a path to go to the object.\n" );
			so_far_so_good= false;
			goto END_GO_AND_GRASP;
		}
#ifdef LIGHT_PLANNER
		desactivateTwoJointsFixCntrt ( robotPt, robotPt->curObjectJnt, robotPt->baseJnt );
		p3d_desactivateCntrt ( robotPt, cntrt_arm );
#endif

		p3d_set_and_update_this_robot_conf ( robotPt, qinter2 );
		p3d_copy_config_into ( robotPt, qinter2, & ( robotPt->ROBOT_POS ) );
		p3d_copy_config_into ( robotPt, qinter3, & ( robotPt->ROBOT_GOTO ) );
#ifdef LIGHT_PLANNER
		p3d_activateCntrt ( robotPt, cntrt_arm );
#endif

		g3d_draw_allwin_active();
		p3d_set_ROBOT_START ( qinter2 );
		p3d_set_ROBOT_GOTO ( qinter3 );

		p3d_set_env_dmax ( DMAX_NEAR );
#ifdef MULTILOCALPATH
		p3d_multiLocalPath_disable_all_groupToPlan ( robotPt );
		p3d_multiLocalPath_set_groupToPlan_by_name ( robotPt, OBJECT_GROUP_NAME, 1 );
#endif
//     gpDeactivate_object_fingertips_collisions(robotPt, OBJECT, HAND_PROP);
		path_found= GP_FindPath();
		if ( !path_found )
		{
			printf ( "The planner could not find a path to reach the object with the arm.\n" );
			so_far_so_good= false;
			goto END_GO_AND_GRASP;
		}
		p3d_desactivateCntrt ( robotPt, cntrt_arm );

		p3d_set_and_update_this_robot_conf ( robotPt, qinter3 );
		p3d_copy_config_into ( robotPt, qinter3, & ( robotPt->ROBOT_POS ) );
		g3d_draw_allwin_active();
		p3d_set_ROBOT_START ( qinter3 );
		p3d_set_ROBOT_GOTO ( qfinal );
//     gpDeactivate_object_fingertips_collisions(robotPt, OBJECT, HAND_PROP);
#ifdef MULTILOCALPATH
		p3d_multiLocalPath_disable_all_groupToPlan ( robotPt );
		p3d_multiLocalPath_set_groupToPlan_by_name ( robotPt, "jido-hand", 1 );
#endif

		path_found= GP_FindPath();
		if ( !path_found )
		{
			printf ( "The planner could not find a path to close the robot's hand.\n" );
			so_far_so_good= false;
			goto END_GO_AND_GRASP;
		}

		p3d_set_and_update_this_robot_conf ( robotPt, qstart );
		p3d_copy_config_into ( robotPt, qstart, & ( robotPt->ROBOT_POS ) );
#ifdef LIGHT_PLANNER
		p3d_activateCntrt ( robotPt, cntrt_arm );
#endif
		g3d_draw_allwin_active();

		GP_ConcateneAllTrajectories ( robotPt );
		robotPt->tcur= robotPt->t[0];
	}
	//if the robot does not need to move, we have to introduce one intermediate configurations:
	//    qstart =  (qbase0 ; qarm0 ; qhand0)
	// -> qinter1=  (qbase0 ; qarm1 ; qhand_inter=open)
	// -> qfinal =  (qbase0 ; qarm1 ; qhand1)
	else
	{
		// get arm final configuration:
		p3d_set_and_update_this_robot_conf ( robotPt, qfinal );
		g3d_draw_allwin_active();
#ifdef LIGHT_PLANNER
		p3d_update_virtual_object_config_for_pa10_6_arm_ik_constraint ( robotPt, qfinal );
#endif
		gpGet_arm_configuration ( robotPt, ARM_TYPE, q1, q2, q3, q4, q5, q6 );

		p3d_set_and_update_this_robot_conf ( robotPt, qstart );
		gpOpen_hand ( robotPt, HAND_PROP );
		g3d_draw_allwin_active();
		gpSet_arm_configuration ( robotPt, ARM_TYPE, q1, q2, q3, q4, q5, q6 );
#ifdef LIGHT_PLANNER
		p3d_update_virtual_object_config_for_pa10_6_arm_ik_constraint ( robotPt, qstart );
#endif
		p3d_get_robot_config_into ( robotPt, &qinter1 );
#ifdef LIGHT_PLANNER
		p3d_update_virtual_object_config_for_pa10_6_arm_ik_constraint ( robotPt, qinter1 );
#endif
		if ( p3d_col_test() )
		{
			printf ( "The robot can not open its hand/gripper in its final arm and base configuration.\n" );
			p3d_copy_config_into ( robotPt, qfinal, &qinter1 );
		}

		p3d_set_env_dmax ( DMAX_FAR );
		p3d_set_and_update_this_robot_conf ( robotPt, qstart );
		g3d_draw_allwin_active();
		p3d_set_ROBOT_START ( qstart );
		p3d_set_ROBOT_GOTO ( qinter1 );
#ifdef LIGHT_PLANNER
		p3d_activateCntrt ( robotPt, cntrt_arm );
#endif

		p3d_set_env_dmax ( DMAX_NEAR );
#ifdef MULTILOCALPATH
		p3d_multiLocalPath_disable_all_groupToPlan ( robotPt );
		p3d_multiLocalPath_set_groupToPlan_by_name ( robotPt, "jido-hand", 1 );
		p3d_multiLocalPath_set_groupToPlan_by_name ( robotPt, OBJECT_GROUP_NAME, 1 );
#endif
//     gpDeactivate_object_fingertips_collisions(robotPt, OBJECT, HAND_PROP);
		path_found= GP_FindPath();
		if ( !path_found )
		{
			printf ( "The planner could not find a path to reach the object with the arm.\n" );
			so_far_so_good= false;
			goto END_GO_AND_GRASP;
		}
		p3d_set_and_update_this_robot_conf ( robotPt, qinter1 );
		g3d_draw_allwin_active();
		p3d_set_ROBOT_START ( qinter1 );
		p3d_set_ROBOT_GOTO ( qfinal );
//     gpDeactivate_object_fingertips_collisions(robotPt, OBJECT, HAND_PROP);
#ifdef MULTILOCALPATH
		p3d_multiLocalPath_disable_all_groupToPlan ( robotPt );
		p3d_multiLocalPath_set_groupToPlan_by_name ( robotPt, "jido-hand", 1 );
#endif
		path_found= GP_FindPath();
		if ( !path_found )
		{
			printf ( "The planner could not find a path to close the robot's hand.\n" );
			so_far_so_good= false;
			goto END_GO_AND_GRASP;
		}

		p3d_set_and_update_this_robot_conf ( robotPt, qstart );
		g3d_draw_allwin_active();
		GP_ConcateneAllTrajectories ( robotPt );
	}

	PATH= GP_GetTrajectory ( robotPt, robotPt->t[0], NB_CONFIGS );
	printf ( "path found: %d configs \n", NB_CONFIGS );

//   gpDeactivate_object_fingertips_collisions(robotPt, OBJECT, HAND_PROP);
	p3d_copy_config_into ( robotPt, qstart, & ( robotPt->ROBOT_POS ) );

END_GO_AND_GRASP:
	//free all configs:
	p3d_destroy_config ( robotPt, qstart );
	p3d_destroy_config ( robotPt, qfinal );
	p3d_destroy_config ( robotPt, qinter1 );
	p3d_destroy_config ( robotPt, qinter2 );
	p3d_destroy_config ( robotPt, qinter3 );

	if ( so_far_so_good )
		{ printf ( "ALL IS DONE: SUCCESS.\n" ); }
	else
		{ printf ( "ALL IS DONE: THERE WAS SOMETHING WRONG.\n" ); }

	return;
}

static void CB_arm_only_obj ( FL_OBJECT *obj, long arg )
{
	GP_FindPathForArmOnly();
	return;
}


static void CB_test_obj ( FL_OBJECT *obj, long arg )
{

  p3d_rob *justin= NULL, *object= NULL;
  p3d_matrix4 T;

  justin= p3d_get_robot_by_name("ROBOT");
  object= p3d_get_robot_by_name("Horse");

  p3d_desactivateCntrt(justin, justin->ccCntrts[0]);
  p3d_desactivateCntrt(justin, justin->ccCntrts[1]);

  p3d_set_and_update_this_robot_conf(justin, justin->ROBOT_GOTO);
  p3d_mat4Copy(justin->ccCntrts[0]->pasjnts[justin->ccCntrts[0]->npasjnts -1]->abs_pos, T);
  Af[0]= T[0][3];  Af[1]= T[1][3];  Af[2]= T[2][3];
  p3d_mat4Copy(justin->ccCntrts[1]->pasjnts[justin->ccCntrts[1]->npasjnts -1]->abs_pos, T);
  Bf[0]= T[0][3];  Bf[1]= T[1][3];  Bf[2]= T[2][3];

  p3d_set_and_update_this_robot_conf(justin, justin->ROBOT_POS);
  p3d_mat4Copy(justin->ccCntrts[0]->pasjnts[justin->ccCntrts[0]->npasjnts -1]->abs_pos, T);
  Ai[0]= T[0][3];  Ai[1]= T[1][3];  Ai[2]= T[2][3];
  p3d_mat4Copy(justin->ccCntrts[1]->pasjnts[justin->ccCntrts[1]->npasjnts -1]->abs_pos, T);
  Bi[0]= T[0][3];  Bi[1]= T[1][3];  Bi[2]= T[2][3];


  p3d_set_and_update_this_robot_conf(object, object->ROBOT_GOTO);
  p3d_mat4Copy(object->joints[1]->abs_pos, T);
  Of[0]= T[0][3];  Of[1]= T[1][3];  Of[2]= T[2][3];
  p3d_set_and_update_this_robot_conf(object, object->ROBOT_POS);
  p3d_mat4Copy(object->joints[1]->abs_pos, T);
  Oi[0]= T[0][3];  Oi[1]= T[1][3];  Oi[2]= T[2][3];


  findBestExchangePosition(object, Oi, Of, Ai, Af, Bi, Bf, E);
  p3d_set_and_update_this_robot_conf(justin, justin->ROBOT_POS);

p3d_matrix4 Te;
p3d_mat4Pos(Te, E[0], E[1], E[2], 0, 0, 0);
  p3d_set_freeflyer_pose(object, Te);
p3d_get_robot_config_into(object, &object->ROBOT_POS);
  redraw(); 

// 
// return;
//   gpSwap_ghost_and_graphic_bodies((p3d_rob *)p3d_get_robot_by_name("Horse"));
//  gpGet_grasp_list_gripper("Horse", GRASPLIST);
// printf("before %d\n",GRASPLIST.size());
//  gpExpand_grasp_list((p3d_rob *)p3d_get_robot_by_name("gripper_robot"), GRASPLIST, 10000);
// printf("after %d\n",GRASPLIST.size());
// redraw(); 
return;

// redraw(); return;
//  gpPrint_robot_AABBs( (p3d_rob *)p3d_get_robot_by_name("SAHandRight_robot") );
//   double tx, ty, tz, ax, ay, az;
//   p3d_jnt *jnt= p3d_get_robot_jnt_by_name((p3d_rob *)p3d_get_robot_by_name("justin"),"rightGhostJoint");
//  p3d_mat4ExtractPosReverseOrder2(jnt->abs_pos, &tx,&ty,&tz,&ax,&ay,&az);
// printf("%f %f %f %f %f %f \n",tx, ty, tz, ax*RADTODEG, ay*RADTODEG, az*RADTODEG);
// return;

  static bool firstTime= true;
  p3d_matrix4 objectPose;
  configPt qhand;
  gpHand_properties handProp;
  std::list<gpGrasp> graspList1, graspList2;

//   gpGet_grasp_list_SAHand ( "Horse", 1, graspList1 );
//   gpGet_grasp_list_SAHand ( "Horse", 2, graspList2 );
  p3d_rob *SAHandRight_robot, *SAHandLeft_robot;
  SAHandRight_robot= p3d_get_robot_by_name ( "SAHandRight_robot" );
  SAHandLeft_robot= p3d_get_robot_by_name("SAHandLeft_robot");
  object= p3d_get_robot_by_name("Horse");

  handProp.initialize(GP_SAHAND_RIGHT);
//   gpSet_hand_rest_configuration(SAHandRight_robot, handProp, 0);
//   p3d_export_robot_as_one_body(SAHandRight_robot, p3d_get_robot_config(SAHandRight_robot));
  p3d_export_robot_as_multipart_OBJ(SAHandRight_robot, p3d_get_robot_config(SAHandRight_robot));
gpMirror_robot_bodies(SAHandRight_robot, "/home/jpsaut/BioMove3Dgit/BioMove3D/graspPlanning/p3d/", 1);
  handProp.initialize(GP_SAHAND_LEFT);
// gpSet_hand_rest_configuration(SAHandLeft_robot, handProp, 0);
  p3d_export_robot_as_multipart_OBJ(SAHandLeft_robot, p3d_get_robot_config(SAHandLeft_robot));
return;


  if(firstTime)
  {  
   gpDouble_grasp_generation(SAHandRight_robot, SAHandLeft_robot, object, graspList1, graspList2, DOUBLEGRASPLIST);
   firstTime= false;  
  }

  std::list<gpDoubleGrasp>::iterator iter;
  static int count= 0;
  int i= 0;
  for ( iter=DOUBLEGRASPLIST.begin(); iter!=DOUBLEGRASPLIST.end(); iter++ )
  {
    DOUBLEGRASP= ( *iter );
    i++;
    if ( i>=count )
    {  break; }
  }
  count++;
  if ( count>DOUBLEGRASPLIST.size() )
          {  count= 1;  }

  p3d_get_body_pose(object, 0, objectPose);

  qhand= p3d_alloc_config(SAHandRight_robot);
  handProp.initialize(GP_SAHAND_RIGHT);
  gpInverse_geometric_model_freeflying_hand(SAHandRight_robot, objectPose, DOUBLEGRASP.grasp1.frame, handProp, qhand);
  p3d_set_and_update_this_robot_conf(SAHandRight_robot, qhand);
  p3d_copy_config_into(SAHandRight_robot, qhand, &SAHandRight_robot->ROBOT_POS);
  gpSet_grasp_configuration(SAHandRight_robot, handProp, DOUBLEGRASP.grasp1, 0);


  handProp.initialize(GP_SAHAND_LEFT);
  gpInverse_geometric_model_freeflying_hand(SAHandLeft_robot, objectPose, DOUBLEGRASP.grasp2.frame, handProp, qhand);
  p3d_set_and_update_this_robot_conf(SAHandLeft_robot, qhand);
  p3d_copy_config_into(SAHandLeft_robot, qhand, &SAHandLeft_robot->ROBOT_POS);
  gpSet_grasp_configuration(SAHandLeft_robot, handProp, DOUBLEGRASP.grasp2, 0);
  p3d_destroy_config(SAHandRight_robot, qhand);




  redraw();
  return;
// double x, y, z, ax, ay, az;
// p3d_rob *robt= p3d_get_robot_by_name("justin");
// p3d_jnt *jnt= p3d_get_robot_jnt_by_name(robt, "RightWrist");
// p3d_mat4Print(jnt->abs_pos, "right");
// p3d_mat4ExtractPosReverseOrder2(jnt->abs_pos,  &x, &y, &z, &ax, &ay, &az);
// printf("[ %g %g %g ] [ %g %g %g ]\n", x, y, z, ax, ay, az);
// printf("[ %g %g %g ] [ %g %g %g ]\n", x, y, z, ax*RADTODEG, ay*RADTODEG, az*RADTODEG);
//
// jnt= p3d_get_robot_jnt_by_name(robt, "LeftWrist");
// p3d_mat4Print(jnt->abs_pos, "left");
// p3d_mat4ExtractPosReverseOrder2(jnt->abs_pos,  &x, &y, &z, &ax, &ay, &az);
// printf("[ %g %g %g ] [ %g %g %g ]\n", x, y, z, ax, ay, az);
// printf("[ %g %g %g ] [ %g %g %g ]\n", x, y, z, ax*RADTODEG, ay*RADTODEG, az*RADTODEG);
	int result;
	double q[4];
	p3d_vector3 p, fingerpad_normal;
	p3d_matrix4 wristFrame;
	p3d_rob *object_robot= p3d_get_robot_by_name ( "Mug" );
	p3d_rob *sahandRight= p3d_get_robot_by_name ( "SAHandRight_robot" );
	p3d_rob *sahandLeft= p3d_get_robot_by_name ( "SAHandLeft_robot" );


	/*
	    p3d_get_body_pose(object_robot, 0, objectPose);

	    qhand= p3d_alloc_config(sahandRight);
	    handProp.initialize(GP_SAHAND_RIGHT);
	    gpInverse_geometric_model_freeflying_hand(sahandRight, objectPose, p3d_mat4IDENTITY, handProp, qhand);
	    p3d_set_and_update_this_robot_conf(sahandRight, qhand);
	    p3d_copy_config_into(sahandRight, qhand, &sahandRight->ROBOT_POS);
	    p3d_destroy_config(sahandRight, qhand);

	    qhand= p3d_alloc_config(sahandLeft);
	    handProp.initialize(GP_SAHAND_LEFT);
	    gpInverse_geometric_model_freeflying_hand(sahandLeft, objectPose, p3d_mat4IDENTITY, handProp, qhand);
	    p3d_set_and_update_this_robot_conf(sahandLeft, qhand);
	    p3d_copy_config_into(sahandLeft, qhand, &sahandLeft->ROBOT_POS);
	    p3d_destroy_config(sahandLeft, qhand);
	*/

// 	p3d_rob *cursor_robot= p3d_get_robot_by_name ( "Cursor" );
// 	p3d_get_body_pose ( cursor_robot, 0, objectPose );
// 	p[0]= objectPose[0][3];
// 	p[1]= objectPose[1][3];
// 	p[2]= objectPose[2][3];


//   CONTACTLIST.clear();
//   poly_build_planes(object->pol[0]->poly);
//   gpSample_obj_surface(object, 0.4, 0.01, CONTACTLIST);
// printf("%d contacts\n", CONTACTLIST.size());
//   redraw();
//   if(KDTREE!=NULL) { delete KDTREE; KDTREE= NULL; }
//   KDTREE= new gpKdTree(CONTACTLIST);
//
//   if(KDTREETRIS!=NULL) { delete KDTREETRIS; KDTREETRIS= NULL; }
//   KDTREETRIS= new gpKdTreeTris(object->pol[0]->poly);
//   printf("depth= %d\n", KDTREETRIS->depth());
//   KDTREETRIS->pointCloud(0.05, SAMPLES);

// CONTACTLIST2.clear();
//   KDTREE->sphereIntersection(CENTER, RADIUS, CONTACTLIST2);
}

static void CB_display_grasps_obj ( FL_OBJECT *obj, long arg )
{
	display_grasps= !display_grasps;

	if ( display_grasps )
		{  fl_set_button ( BT_DISPLAY_GRASPS_OBJ, TRUE );  }
	else
		{  fl_set_button ( BT_DISPLAY_GRASPS_OBJ, FALSE ); }

	redraw();
}


static void CB_load_grasp_list_obj ( FL_OBJECT *obj, long arg )
{
	LOAD_LIST= !LOAD_LIST;

	if ( LOAD_LIST )
		{  fl_set_button ( BT_LOAD_GRASP_LIST_OBJ, TRUE );  }
	else
		{  fl_set_button ( BT_LOAD_GRASP_LIST_OBJ, FALSE ); }

	redraw();
}


/////////////////////FUNCTIONS USED BY THE GENOM MODULE: /////////////////////////////
p3d_cntrt* GP_GetArmCntrt ( p3d_rob *robotPt )
{
	int i;
	p3d_cntrt* cntrt_arm = NULL;

	if ( robotPt==NULL )
	{
		printf ( "%s: %d: GP_GetArmCntrt(): input p3d_rob* is NULL.\n", __FILE__,__LINE__ );
		return NULL;
	}

	for ( i=0; i<robotPt->cntrt_manager->ncntrts; i++ )
	{
		cntrt_arm = robotPt->cntrt_manager->cntrts[i];
		if ( strcmp ( cntrt_arm->namecntrt, "p3d_pa10_6_arm_ik" ) ==0 )
			{  break;  }
	}
	if ( i==robotPt->cntrt_manager->ncntrts )
	{
		printf ( "%s: %d: GP_GetArmCntrt(): fatal error: arm_IK constraint does not exist.\n", __FILE__,__LINE__ );
		return NULL;
	}

	return cntrt_arm;
}



int GP_Init ( char *objectName )
{
	unsigned int i;

	if ( !INIT_IS_DONE )
	{
		init_graspPlanning ( objectName );

		// deactivate collisions for all robots except for the two of them needed by the grasp planner:
		for ( i=0; i< ( unsigned int ) XYZ_ENV->nr; i++ )
		{
			if ( XYZ_ENV->robot[i]==ROBOT || XYZ_ENV->robot[i]==HAND_ROBOT )
				{   continue;    }
			else
				{  p3d_col_deactivate_robot ( XYZ_ENV->robot[i] );  }
		}

		INIT_IS_DONE= true;
	}

	return 1;
}



//! Computes a list of grasps (for the hand only)
//! that will make the hand/gripper grasp the specified object.
//! \param objectName name of the object to be grasped by the robot
//! \return 1 in case of success, 0 otherwise
int GP_ComputeGraspList ( char *objectName )
{
	GP_Init ( objectName );

	printf ( "Collisions are deactivated for other robots.\n" );

	gpGrasp_generation ( HAND_ROBOT, OBJECT, 0, HAND_PROP, HAND_PROP.nb_positions, HAND_PROP.nb_directions, HAND_PROP.nb_rotations, GRASPLIST );

	printf ( "Before collision filter: %d grasps.\n", GRASPLIST.size() );
	gpGrasp_collision_filter ( GRASPLIST, HAND_ROBOT, OBJECT, HAND_PROP );
	printf ( "After collision filter: %d grasps.\n", GRASPLIST.size() );
	gpGrasp_stability_filter ( GRASPLIST );
	printf ( "After stability filter: %d grasps.\n", GRASPLIST.size() );

//         gpGrasp_compute_open_configs(GRASPLIST, HAND_ROBOT, OBJECT, HAND_PROP );

	gpGrasp_context_collision_filter ( GRASPLIST, HAND_ROBOT, OBJECT, HAND_PROP );
	printf ( "For the current collision context: %d grasps.\n", GRASPLIST.size() );
	p3d_col_deactivate_robot ( HAND_ROBOT );

	redraw();


	if ( GRASPLIST.empty() )
	{
		printf ( "GP_ComputeGraspList(): No grasp was found.\n" );
		return 0;
	}

	return 1;
}


//! Finds a suitable grasp configuration for the whole robot from the previously computed
//! grasp list.
//! This function is meant to be used with the genom module (maybe partly) dedicated to grasp planning.
//! \param needs_to_move will be filled with true if the configuration of the mobile base is different
//! in the computed robot configuration and in the current robot configuration, filled with false otherwise
//! \return the found configuration vector in case of success, NULL in case of failure
configPt GP_FindGraspConfig ( bool &needs_to_move )
{
	if ( !INIT_IS_DONE )
	{
		printf ( "GP_FindGraspConfig(): grasp planner needs to be initialized first.\n" );
		return NULL;
	}

	if ( GRASPLIST.empty() )
	{
		printf ( "GP_FindGraspConfig(): The grasp list is empty.\n" );
		return NULL;
	}

	unsigned int i, nb_iters_max;
	p3d_vector3 objectCenter;
	p3d_matrix4 objectPose;
	configPt qcurrent= NULL, qbase= NULL, qresult= NULL;

	//we first check if the robot can grasp the object from its current position:
	//find a configuration for the current robot base configuration:
	qcurrent = p3d_alloc_config ( ROBOT );
	p3d_get_robot_config_into ( ROBOT, &qcurrent );

	qresult= gpFind_grasp_from_base_configuration ( ROBOT, OBJECT, GRASPLIST, ARM_TYPE, qcurrent, GRASP, HAND_PROP );

	p3d_destroy_config ( ROBOT, qcurrent );

	if ( qresult!=NULL )
	{
		GRASP.print();
		needs_to_move= false;

		// as the real Jido's gripper can only be completely opened or completely closed,
		// we set it to max opening:
//     if(HAND_PROP.type==GP_GRIPPER)
//     {
//       p3d_set_and_update_this_robot_conf(ROBOT, qresult);
//       gpOpen_hand(ROBOT, HAND_PROP);
//       p3d_get_robot_config_into(ROBOT, &qresult);
//       p3d_set_and_update_this_robot_conf(ROBOT, qcurrent);
//     }

		return qresult;
	}



// we must try to find a valid base configuration:
	needs_to_move= true;
	p3d_get_body_pose ( OBJECT, 0, objectPose );
	objectCenter[0]= objectPose[0][3] + CMASS[0];
	objectCenter[1]= objectPose[1][3] + CMASS[1];
	objectCenter[2]= objectPose[2][3] + CMASS[2];

	nb_iters_max= 300;
	for ( i=0; i<nb_iters_max; i++ )
	{
		qbase= gpRandom_robot_base ( ROBOT, GP_INNER_RADIUS, GP_OUTER_RADIUS, objectCenter, ARM_TYPE );
		if ( qbase==NULL )
			{   break;   }

		qresult= NULL;
		qresult= gpFind_grasp_from_base_configuration ( ROBOT, OBJECT, GRASPLIST, ARM_TYPE, qbase, GRASP, HAND_PROP );

		if ( qresult!=NULL )
			{   break;   }
		p3d_destroy_config ( ROBOT, qbase );
		qbase= NULL;
	}
	if ( qbase!=NULL )
		{  p3d_destroy_config ( ROBOT, qbase );  }

	if ( i==nb_iters_max )
	{
		printf ( "GP_FindGraspConfig: No valid platform configuration was found.\n" );
		return NULL;
	}

	// as the real Jido's gripper can only be completely opened or completely closed,
	// we set it to max opening:
//   if(HAND_PROP.type==GP_GRIPPER)
//   {
//     p3d_set_and_update_this_robot_conf(ROBOT, qresult);
//     gpOpen_hand(ROBOT, HAND_PROP);
//     p3d_get_robot_config_into(ROBOT, &qresult);
//     p3d_set_and_update_this_robot_conf(ROBOT, qcurrent);
//   }

	return qresult;
}

int GP_FindPathForArmOnly()
{
	bool needs_to_move, so_far_so_good= true;
	int result, path_found;
	std::vector<double> qhand;
	configPt qstart= NULL, qfinal= NULL, qfar= NULL;
	p3d_rob *robotPt= NULL;
	p3d_cntrt* cntrt_arm = NULL;
	robotPt= p3d_get_robot_by_name ( GP_ROBOT_NAME );
	XYZ_ENV->cur_robot= robotPt;

	// initializes everything:
	GP_Init ( GP_OBJECT_NAME_DEFAULT );

	redraw();

	cntrt_arm= GP_GetArmCntrt ( robotPt );

	if ( cntrt_arm==NULL )
	{
		printf ( "FATAL_ERROR : arm_IK constraint does not exist\n" );
		return 0;
	}

	/* Deactivate the arm_IK constrint */
	p3d_desactivateCntrt ( robotPt, cntrt_arm );

	//alloc all configs:
	qstart= p3d_alloc_config ( robotPt );
	qfinal= p3d_alloc_config ( robotPt );
	qfar= p3d_alloc_config ( HAND_ROBOT );

	gpOpen_hand ( robotPt, HAND_PROP );
	p3d_get_robot_config_into ( robotPt, &qstart );
#ifdef LIGHT_PLANNER
	p3d_update_virtual_object_config_for_pa10_6_arm_ik_constraint ( robotPt, qstart );
#endif

	p3d_set_and_update_this_robot_conf ( robotPt, qstart );
	if ( p3d_col_test() )
	{
		printf ( "GP_FindPathForArmOnly(): Start configuration is colliding.\n" );
		so_far_so_good= false;
		goto END_ARM_ONLY;
	}


	// computes the grasp list:
	if ( !LOAD_LIST )
	{
		result= GP_ComputeGraspList ( GP_OBJECT_NAME_DEFAULT );
		gpSave_grasp_list ( GRASPLIST, "./graspPlanning/graspList_new.xml" );
	}
	else // or loads it:
	{
		result= gpLoad_grasp_list ( "./graspPlanning/graspList.xml", GRASPLIST );
		if ( result==0 )
		{
			printf ( "Can not load a grasp list.\n" );
			so_far_so_good= false;
			goto END_ARM_ONLY;
		}

		if ( !GRASPLIST.empty() )
		{
			if ( GRASPLIST.front().hand_type!=HAND_PROP.type )
			{
				printf ( "The loaded grasp list does not correspond to the current hand type.\n" );
				so_far_so_good= false;
				goto END_ARM_ONLY;
			}
		}
	}

	if ( GRASPLIST.empty() )
	{
		printf ( "Could not compute any grasp.\n" );
		so_far_so_good= false;
		goto END_ARM_ONLY;
	}

	// move away the hand robot:
	qfar= p3d_alloc_config ( HAND_ROBOT );
	qfar[7]= -100;
	qfar[8]= -1; //to put the hand far under the floor
	p3d_set_and_update_this_robot_conf ( HAND_ROBOT, qfar );

	qfinal= GP_FindGraspConfig ( needs_to_move );

	if ( qfinal!=NULL )
	{
#ifdef LIGHT_PLANNER
		p3d_update_virtual_object_config_for_pa10_6_arm_ik_constraint ( robotPt, qfinal );
#endif
		p3d_set_and_update_this_robot_conf ( robotPt, qfinal );
		gpOpen_hand ( robotPt, HAND_PROP );
		p3d_get_robot_config_into ( robotPt, &qfinal );
		if ( p3d_col_test() )
		{
			printf ( "The robot can not open its hand in final configuration without collision.\n" );
			so_far_so_good= false;
			goto END_ARM_ONLY;
		}
		printf ( "Grasp configuration list was successfully computed.\n" );
	}
	else
	{
		printf ( "No grasp configuration was found.\n" );
		so_far_so_good= false;
		goto END_ARM_ONLY;
	}

	if ( needs_to_move )
	{
		printf ( "The robot can not reach the object from its current position. It needs to move.\n" );
		so_far_so_good= false;
		goto END_ARM_ONLY;
	}

	p3d_set_and_update_this_robot_conf ( robotPt, qstart );
	p3d_copy_config_into ( robotPt, qstart, & ( robotPt->ROBOT_POS ) );
	p3d_copy_config_into ( robotPt, qfinal, & ( robotPt->ROBOT_GOTO ) );
	p3d_activateCntrt ( robotPt, cntrt_arm );
	g3d_draw_allwin_active();
	XYZ_ENV->cur_robot= robotPt;
	p3d_set_ROBOT_START ( qstart );
	p3d_set_ROBOT_GOTO ( qfinal );


	p3d_set_env_dmax ( DMAX_NEAR );
#ifdef MULTILOCALPATH
	p3d_multiLocalPath_disable_all_groupToPlan ( robotPt );
	p3d_multiLocalPath_set_groupToPlan_by_name ( robotPt, OBJECT_GROUP_NAME, 1 );
#endif
	p3d_activateCntrt ( robotPt, cntrt_arm );
	path_found= GP_FindPath();
	if ( !path_found )
	{
		printf ( "The planner could not find a valid path for the arm.\n" );
		so_far_so_good= false;
		goto END_ARM_ONLY;
	}


END_ARM_ONLY:
	p3d_destroy_config ( robotPt, qstart );
	p3d_destroy_config ( robotPt, qfinal );
	p3d_destroy_config ( HAND_ROBOT, qfar );

	if ( so_far_so_good )
	{
		printf ( "ALL IS DONE: SUCCESS.\n" );
		return 1;
	}
	else
	{
		printf ( "ALL IS DONE: THERE WAS SOMETHING WRONG.\n" );
		return 0;
	}

}


//! Creates and fills an array of configPt with the configuration steps of the given trajectory.
//! \param robotPt pointer to the robot
//! \param traj pointer to the trajectory (that must be compatible with the robot)
//! \param nb_configs will be filled with the size of the array
//! \return the configuration array
configPt* GP_GetTrajectory ( p3d_rob *robotPt, p3d_traj *traj, int &nb_configs )
{
	nb_configs= 0;
	if ( robotPt==NULL )
	{
		PrintInfo ( ( "GP_GetTrajectory: robot is NULL.\n" ) );
		return NULL;
	}
	if ( traj==NULL )
	{
		PrintInfo ( ( "GP_GetTrajectory: traj is NULL.\n" ) );
		return NULL;
	}


	bool traj_found_in_robot= false;
	double umax; // parameters along the local path
	int i, *ikSol= NULL;
	pp3d_localpath localpathPt;
	configPt *configs= NULL;

	for ( i= 0; i<robotPt->nt; i++ )
	{
		if ( robotPt->t[i]==traj )
		{
			traj_found_in_robot= true;
			break;
		}
	}
	if ( traj_found_in_robot==false )
	{
		PrintInfo ( ( "GP_GetTrajectory: traj may not belong to the robot.\n" ) );
	}


	localpathPt = traj->courbePt;
	//distances = MY_ALLOC(double, njnt+1);

	i= 0;
	while ( localpathPt!=NULL )
	{
		( nb_configs ) ++;
		localpathPt= localpathPt->next_lp;
	}
	( nb_configs ) ++;
	configs= ( configPt * ) malloc ( nb_configs*sizeof ( configPt ) );

	localpathPt = traj->courbePt;
	i= 0;
	while ( localpathPt != NULL )
	{
		umax= localpathPt->range_param;

		if ( i==0 )
		{
			configs[i]= localpathPt->config_at_param ( robotPt, localpathPt, 0 );
			if ( !ikSol || !p3d_compare_iksol ( robotPt->cntrt_manager, localpathPt->ikSol, ikSol ) )
			{
				p3d_copy_iksol ( robotPt->cntrt_manager, localpathPt->ikSol, &ikSol );
				if ( p3d_get_ik_choice() != IK_NORMAL )
					{  p3d_print_iksol ( robotPt->cntrt_manager, localpathPt->ikSol );  }
			}
			p3d_set_and_update_this_robot_conf_multisol ( robotPt, configs[i], NULL, 0, localpathPt->ikSol );
			i++;
		}

		configs[i] = localpathPt->config_at_param ( robotPt, localpathPt, umax );
		if ( !ikSol || !p3d_compare_iksol ( robotPt->cntrt_manager, localpathPt->ikSol, ikSol ) )
		{
			p3d_copy_iksol ( robotPt->cntrt_manager, localpathPt->ikSol, &ikSol );
			if ( p3d_get_ik_choice() != IK_NORMAL )
				{   p3d_print_iksol ( robotPt->cntrt_manager, localpathPt->ikSol );  }
		}
		p3d_set_and_update_this_robot_conf_multisol ( robotPt, configs[i], NULL, 0, localpathPt->ikSol );
		i++;

		localpathPt = localpathPt->next_lp;
	}


	return configs;
}

//! Creates and fills a array of configPt with the configuration steps of the all the trajectories contained
//! in the trajectory array of a robot.
//! \param robotPt pointer to the robot
//! \param nb_configs will be filled with the size of the array
//! \return the configuration array
configPt* GP_GetAllTrajectoriesAsOne ( p3d_rob *robotPt, int &nb_configs )
{
	nb_configs= 0;
	if ( robotPt==NULL )
	{
		PrintInfo ( ( "GP_GetTrajectory: robot is NULL.\n" ) );
		return NULL;
	}

	int i, j, n;
	configPt* configs= NULL, *result= NULL;
	std::list<configPt> cfg_list;
	std::list<configPt>::iterator iter;

	for ( i=0; i<robotPt->nt; i++ )
	{
		n= 0;
		configs= GP_GetTrajectory ( robotPt, robotPt->t[i], n );
		for ( j=0; j<n; j++ )
			{  cfg_list.push_back ( configs[j] );   }
		free ( configs );
	}

	nb_configs= cfg_list.size();
	result= ( configPt * ) malloc ( nb_configs*sizeof ( configPt ) );

	i= 0;
	for ( iter=cfg_list.begin(); iter!=cfg_list.end(); iter++ )
	{
		result[i]= ( *iter );
		i++;
	}

	return result;
}

//! Concatenes all the current trajectories of the robot into the first one.
//! NB: only the first trajectory will remain (and grown up); the others are destroyed.
//! \param robotPt pointer to the robot
//! \return 1 in case of success, 0 otherwise
int GP_ConcateneAllTrajectories ( p3d_rob *robotPt )
{
	if ( robotPt==NULL )
	{
		PrintInfo ( ( "GP_ConcateneAllTrajectories: robot is NULL.\n" ) );
		return 0;
	}
	if ( robotPt->nt==0 )
	{
		PrintInfo ( ( "GP_ConcateneAllTrajectories: the robot has no trajectory.\n" ) );
		return 0;
	}

	int i;
	pp3d_localpath localpathPt, end;

	for ( i=0; i<robotPt->nt-1; i++ )
	{
		localpathPt = robotPt->t[i]->courbePt;
		while ( localpathPt!=NULL )
		{
			end= localpathPt;
			localpathPt = localpathPt->next_lp;
		}
		end->next_lp= robotPt->t[i+1]->courbePt;
		robotPt->t[i+1]->courbePt->prev_lp= end;
	}

	for ( i=1; i<robotPt->nt; i++ )
		{  free ( robotPt->t[i] );  }
	robotPt->nt= 1;

	robotPt->tcur= robotPt->t[0];
	FORMrobot_update ( p3d_get_desc_curnum ( P3D_ROBOT ) );

	return 1;
}

//! Computes a path for the arm and hand of the robot to drive it from ROBOT_POS to ROBOT_GOTO configurations.
//! that will make the robot grasp the specified object.
//! \param platform_motion indicates if the platform motion is allowed or not; if not, the platform DOFs will be
//! blocked before calling the motion planner
//! \param arm_motion indicates if the arm motion is allowed or not; if not, the arm DOFs will be
//! blocked before calling the motion planner
//! \param hand_motion indicates if the hand motion is allowed or not; if not, the hand DOFs will be
//! blocked before calling the motion planner
//! \return 1 in case of success, 0 otherwise
int GP_FindPath()
{
	if ( !INIT_IS_DONE )
	{
		printf ( "GP_FindPath(): grasp planner needs to be initialized first.\n" );
		return 0;
	}

	int i, result;

	// deactivate collisions for all other robots:
	for ( i=0; i<XYZ_ENV->nr; i++ )
	{
		if ( XYZ_ENV->robot[i]==ROBOT )
			{   continue;    }
		else
			{  p3d_col_deactivate_robot ( XYZ_ENV->robot[i] );  }
	}

	printf ( "Collision are desactivated for other robots\n" );

	ENV.setBool ( Env::biDir,true );
	ENV.setInt ( Env::NbTry, 100000 );
	ENV.setInt ( Env::MaxExpandNodeFail, 30000 );
	ENV.setInt ( Env::maxNodeCompco, 100000 );
	ENV.setExpansionMethod ( Env::Connect );

//   print_config(ROBOT, ROBOT->ROBOT_POS);
//   print_config(ROBOT, ROBOT->ROBOT_GOTO);

	if ( p3d_equal_config ( ROBOT, ROBOT->ROBOT_POS, ROBOT->ROBOT_GOTO ) )
	{
		printf ( "GP_FindPath(): Start and goal configurations are the same.\n" );
		return 1;
	}

	p3d_set_and_update_this_robot_conf ( ROBOT, ROBOT->ROBOT_POS );
	result= p3d_specific_search ( "out.txt" );

	// optimizes the trajectory:
	CB_start_optim_obj ( NULL, 0 );

	// reactivate collisions for all other robots:
	for ( i=0; i<XYZ_ENV->nr; i++ )
	{
		if ( XYZ_ENV->robot[i]==HAND_ROBOT || XYZ_ENV->robot[i]==ROBOT )
			{   continue;    }
		else
			{  p3d_col_activate_robot ( XYZ_ENV->robot[i] );  }
	}
	printf ( "Collision are re-activated for other robots\n" );

	p3d_SetTemperatureParam ( 1.0 );

#ifdef LIGHT_PLANNER
	deleteAllGraphs();
#endif

	return result;
}

void GP_Reset()
{
	g3d_win *win= NULL;

	for ( int i=0; i<NB_CONFIGS; i++ )
		{ p3d_destroy_config ( ROBOT, PATH[i] );  }
	free ( PATH );
	PATH= NULL;
	NB_CONFIGS= 0;

	if ( ROBOT!=NULL )
	{
		while ( ROBOT->nt!=0 )
			{   p3d_destroy_traj ( ROBOT, ROBOT->t[0] );  }
		FORMrobot_update ( p3d_get_desc_curnum ( P3D_ROBOT ) );
	}


	ROBOT= NULL;
	HAND_ROBOT= NULL;
	OBJECT= NULL;
	POLYHEDRON= NULL;
	GRASPLIST.clear();

	INIT_IS_DONE= false;

#ifdef LIGHT_PLANNER
	deleteAllGraphs();
#endif

	//reinit all the initial collision context:
#ifdef PQP
	pqp_create_collision_pairs();
#endif

	win= g3d_get_cur_win();
	win->fct_draw2= NULL;
	win->fct_key1 = NULL;
	win->fct_key2 = NULL;
}

