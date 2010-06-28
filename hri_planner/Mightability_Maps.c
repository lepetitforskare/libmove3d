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



///////////****** For graspability tests
#include "Manipulation.h"

#include "GraspPlanning-pkg.h"
#include "gbM/Proto_gbModeles.h"
#include "lightPlanner.h"
#include "lightPlannerApi.h"
#include <gp_grasp_generation_proto.h>
#include <gp_grasp_generation_proto.h>

#include "planner_cxx/plannerFunctions.hpp"
#include <list>
#include <string>
#include <iostream>


//#define OBJECT_NAME "YELLOW_BOTTLE"
//#define OBJECT_NAME "GREY_TAPE"
//#define SUPPORT_NAME "HRP2TABLE"
//#define HUMAN_NAME "ACHILE_HUMAN1"

static Manipulation_JIDO *manipulation= NULL;

/////////******* END for graspability tests

//#include "dynamicsJRLJapan/HumanoidDynamicMultiBody.h"

//// This will be used for exporting the object level mightability information to the external modules/functions
object_mightabilities_info_set Mightabilities_for_obj;


//AKP 
#ifdef USE_HRP2_GIK
extern ChppGikStandingRobot* attStandingRobot;
extern double attSamplingPeriod; // //Set a sampling period: 5ms second is the value used on HRP2, It should be 5e-3
extern vectorN combined_weights;
extern  std::vector<CjrlGikStateConstraint*> state_constraint_tasks;
extern ChppGikPositionConstraint *pc;

extern struct gik_solution curr_gik_sol; // To store the gik solution between two segments in the path. It will be reset with each call of HRP2_hand_reach
extern struct gik_solution HRP2_GIK_sol;// It will store entire configuration to reach from the start to the goal of the entire path
extern struct SOLUTION_CONFIGS_FOR_HRP2 cur_gik_sol_configs;//It will store the final set of configurations to be executed on HRP2 
#endif

extern hri_bitmapset * ACBTSET;


//AKP
int PERSPECTIVE_WIN_ENABLED=0;
int ONLINE_TRACKING_FLAG=0;
// AKP for storing the information about surfaces in the environment
struct env_surfaces curr_surfaces_in_env; 
//struct surface_grid_cell surf_grid[100][100]; 
//int grid_i_max=0;// max 1st index of surf_grid
//int grid_j_max=0;// max 2nd index of surf_grid
double surf_grid_samp_rate=0.05;//0.05;//0.1;// Sampling rate of the surface for constructing the grid
//int current_surface_index; // index i of the curr_surfaces_in_env.flat_surf[i] of env_surfaces

extern int CALCULATE_AFFORDANCE;

extern int SHOW_2D_COMMON_REACH_HRP2_HUMAN;
extern int SHOW_3D_COMMON_REACH_HRP2_HUMAN;
extern int SHOW_2D_COMMON_VISIBLE_HRP2_HUMAN;
extern int SHOW_3D_COMMON_VISIBLE_HRP2_HUMAN;;
extern int SHOW_HRP2_HUMAN_COMMON_REACHABLE_VISIBLE;


extern int SHOW_2D_BENDING_REACHABLE_HUM;
extern int SHOW_3D_BENDING_REACHABLE_HUM;
extern int SHOW_2D_DIRECT_REACHABLE_HUM;
extern int SHOW_3D_DIRECT_REACHABLE_HUM;
extern int SHOW_2D_DIRECT_REACHABLE_HRP2;
extern int SHOW_3D_DIRECT_REACHABLE_HRP2;
extern int SHOW_2D_VISIBLE_PLACES_FOR_HRP2;
extern int SHOW_3D_VISIBLE_PLACES_FOR_HRP2;
extern int SHOW_2D_VISIBLE_PLACE_HUM;
extern int SHOW_2D_VISIBLE_PLACE_STANDING_HUM;
extern int SHOW_3D_VISIBLE_PLACE_HUM;
extern int SHOW_3D_VISIBLE_PLACE_STANDING_HUM;
extern int SHOW_2D_TURNING_AROUND_REACHABLE_HUM;
extern int SHOW_3D_TURNING_AROUND_REACHABLE_HUM;
extern int SHOW_PUT_OBJ_CANDIDATES;
extern int SHOW_WEIGHT_FOR_CANDIDATE_POINTS;
extern int SHOW_MM_BASED_OBJECT_REACHABLE;
extern int SHOW_MM_BASED_OBJECT_VISIBLE;

//extern int HRP2_CURRENT_STATE;//1 for sitting, 2 for half sitting
int HRP2_CURRENT_STATE=1;//1 for sitting, 2 for half sitting

struct grid_3D grid_around_HRP2;
int HRP2_GIK_MANIP=0;// Just to set the type of the bitmap
int BT_AFFORDANCE_VISIBILITY=1;//For the bitmap which is used for calculating visibility on 3d grid

candidate_poins_for_task candidate_points_to_put;
candidate_poins_for_task candidate_points_to_show;
candidate_poins_for_task candidate_points_to_hide;
candidate_poins_for_task candidate_points_to_putinto_by_jido;
candidate_poins_for_task candidate_points_to_putinto_blue_trashbin_by_jido;
candidate_poins_for_task candidate_points_to_putinto_pink_trashbin_by_jido;
candidate_poins_for_task current_candidate_points_to_putinto;
////point_co_ordi candidate_points_to_put[100];

int no_candidate_points_to_put=0;
int grid_3d_affordance_calculated=0;

p3d_env *envPt;

point_co_ordi FOV_end_point_vertices[1000][8];//For every set there will be 8 vertices in the order mentioned in the function gpsp_computeFrustumVertices() in the file g3d_draw_camera.c
int no_FOV_end_point_vertices=0;
p3d_matrix4 frustum_transformation_mat;



extern unsigned int NO_DOF_HRP2;

extern double M3D_to_HRP2_GIK_sitting_Z_shift;//0.622+0.16;//0.510-0.112=0.398//This is the value which needs to be added to the M3D z value of robot to synchronize with the z value of HRP2_GIK because for M3D the z is the height of foot whereas for HRP2_GIK z is the height of the waist. 

point_co_ordi sphere_surface_pts[25000];
int no_sphere_surface_pts=0;

/*
int ROBOTj_RSHOULDER=19;//29;
   int ROBOTj_LSHOULDER=32;//42;
int ROBOTj_SHOULDER=4;//For Jido
*/



extern int CANDIDATE_POINTS_FOR_TASK_FOUND;


point_co_ordi agent_eye_pos;//To store the eye position for calculating visibility, for debuging only

int HUMAN_HAS_MOVED=0;//For updating the mightability maps
int JIDO_HAS_MOVED=0;//For updating the mightability maps
int HRP2_HAS_MOVED=0;//For updating the mightability maps
int NEED_HUMAN_VISIBILITY_UPDATE=0; //For updating the mightability maps
int NEED_HRP2_VISIBILITY_UPDATE=0; //For updating the mightability maps
int NEED_JIDO_VISIBILITY_UPDATE=0; //For updating the mightability maps
////int NEED_HUMAN_CURRENT_VISIBILITY_UPDATE=0;// For updating the visibility from the current head orientation

struct object_Symbolic_Mightability_Maps_Relation object_MM;

#ifdef USE_SYM_GEO_PLAN
struct constraints_set curr_constraints;
////curr_constraints.no_constraints=0;

struct geometric_plan curr_geo_plan;

configPt tmp_robot_config_after_exec[50]; //To store configurations of the robots after validating a particular node , the index should be synchronized with the index of the robots
int tmp_changed_robot_config[50];//To keep track of the indices of the robots which will be changed  after a particular action.

vectorN tmp_HRP2_GIK_final_config; //For storing the final configuration of HRP2 for GIK, for storing into node of symbolic planner

#endif 

struct robots_indices rob_indx; 

int UPDATE_MIGHTABILITY_MAP_INFO=1;
int SHOW_MIGHTABILITY_MAP_INFO=1;

int CURR_GEO_NODE=0;//To keep track of the current node in geometric plan while executing the entire plan
int SHOW_CANDIDATE_POINTS_FOR_GEO_NODE=0;

int show_obstacle_cells_belonging_to(int object_index);

point_co_ordi point_to_put;
int MM_RECORD_MOVIE_FRAMES=0;
int AKP_record_movie_frames();

robots_status robots_status_for_Mightability_Maps[100];
int JIDO_HOLDING_OBJECT=0;
int index_of_curr_obj_in_JIDO_hand=-1; //To update the status of this object for recalculating Mightability Maps, because when JIDO grasps an object it becomes different object
//================================

int execute_Mightability_Map_functions()
{
////////////g3d_drawDisc(point_to_put.x,point_to_put.y,point_to_put.z,0.02, Green, NULL);
 ////printf(" Inside execute_Mightability_Map_functions()\n");
   if(Affordances_Found==1)
   {
  
   // printf(" Inside Affordances_Found==1\n");
   if(UPDATE_MIGHTABILITY_MAP_INFO==1)
    {
   ////printf(" Inside UPDATE_MIGHTABILITY_MAP_INFO \n");
   if(JIDO_HOLDING_OBJECT==1)
     {
   printf(" Jido holding the object having index %d, so need to update MM\n",index_of_curr_obj_in_JIDO_hand);
   robots_status_for_Mightability_Maps[index_of_curr_obj_in_JIDO_hand].has_moved=1;
     }
   update_robots_and_objects_status();
   int expansion=1;
   ////////update_3D_grid_for_Mightability_Maps_new(grid_around_HRP2.GRID_SET,expansion,HRP2_GIK_MANIP);
   //printf(" **** Calling update_Mightability_Maps()\n");
   update_Mightability_Maps();

   //printf(" **** Calling find_symbolic_Mightability_Map_new()\n");
   ////find_symbolic_Mightability_Map();
   find_symbolic_Mightability_Map_new();
  
   //printf(" **** Calling get_object_mightabilities() \n");

   get_object_mightabilities();
    }

    if(SHOW_MIGHTABILITY_MAP_INFO==1)
    {

   show_object_Mightabilities();


   show_symbolic_Mightability_Map_Relations();
   ////////////show_3d_grid_Bounding_box_for_HRP2_GIK();
   show_3D_workspace_Bounding_Box();
   

     if(grid_3d_affordance_calculated==1)
     {
      show_3d_grid_affordances_new();  
     } 
    
    /* 
    ////To show the obstacle cell belonging to an object, For debuging 
    int index=get_index_of_robot_by_name("RED_BOTTLE");//("SPACENAVBOX");//("ACCESSKIT");//("YELLOW_BOTTLE");
    show_obstacle_cells_belonging_to(index);
    show_first_non_visible_cells(index);
    */
    }

   if(SHOW_OBSTACLE_CELLS==1)
   show_exact_obstacles_for_HRP2_GIK_manip(grid_around_HRP2.GRID_SET,HRP2_GIK_MANIP);
   ////show_3d_grid_for_HRP2_GIK();
   //show_3d_grid_affordances();  
   ////Affordances_Found=0;
/*
   int ROBOTj_RSHOULDER=19;
    double hum_R_shoulder_pos[3];
     hum_R_shoulder_pos[0]= ACBTSET->robot->joints[ROBOTj_RSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    hum_R_shoulder_pos[1] = ACBTSET->robot->joints[ROBOTj_RSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    hum_R_shoulder_pos[2] = ACBTSET->robot->joints[ROBOTj_RSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
   g3d_drawDisc(hum_R_shoulder_pos[0], hum_R_shoulder_pos[1], hum_R_shoulder_pos[2], 0.05, 4, NULL);
  */ 
   ////g3d_drawDisc(point_to_look[0], point_to_look[1], point_to_look[2], 0.1, 4, NULL);

    if(MM_RECORD_MOVIE_FRAMES==1)
    {
    AKP_record_movie_frames();
    }
   }
   
    
    //show_HRP2_gik_sol(); 
  #ifdef USE_HRP2_GIK
  if(HRP2_GIK_path_calculated==1)
   {
   hri_bt_show_path(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]);
   }

  if(HRP2_HAND_spline_path_calculated==1)
   {
   show_spline_path_for_HRP2_hand();
   }

  g3d_drawDisc(to_reach_target[0], to_reach_target[1], to_reach_target[2], 0.02, 4, NULL);
  #endif 
  
   if(CANDIDATE_POINTS_FOR_TASK_FOUND==1)
   {
   if(SHOW_PUT_OBJ_CANDIDATES==1)
    {
   //int show_weight=0;
   show_weighted_candidate_points_to_put_obj(SHOW_WEIGHT_FOR_CANDIDATE_POINTS);
    } 
   
   /////////show_weighted_candidate_points_to_show_obj();
   //////// show_weighted_candidate_points_to_hide_obj();
   } 
int show_weight=1;
show_weighted_candidate_points_for_putinto_obj(show_weight);
    
   #ifdef USE_SYM_GEO_PLAN
   if(SHOW_CANDIDATE_POINTS_FOR_GEO_NODE==1)
   {
    show_candidate_points_for_curr_geo_node();
   }
   #endif
  ////////pqp_print_colliding_pair();
  
return 1;
}

static int movie_count = 0;
/* static int movie_count_real = 0; */
static int image_rate = 1;
static int image_compress = 100;
int AKP_record_movie_frames()
{
  char str[512];
  char file[64];
  int count;

if((++movie_count)%image_rate == 0) {
    count = movie_count/image_rate;
    if(count < 10) sprintf(file,"0000%d.jpg",count);
    else if(count < 100) sprintf(file,"000%d.jpg",count);
    else sprintf(file,"00%d.jpg",count);
    sprintf(str,"/usr/bin/import -silent -window %d -quality %d %s",g3d_win_id(G3D_WIN),image_compress,file);
/*     sprintf(str,"/usr/local/imagetools/sparc-solaris/bin/import -silent -window %d -quality %d %s",g3d_win_id(G3D_WIN),image_compress,file); */
    system(str);
 printf("**** AKP >>>> Recorded Frame %s \n",file);
 }

 return 1;
}

#ifdef USE_SYM_GEO_PLAN
int init_geometric_plan_data()
{
 

  envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
 
    int nr = envPt->nr;
    int nr_ctr=0;
    for(;nr_ctr<nr;nr_ctr++)
     {
       tmp_robot_config_after_exec[nr_ctr]=MY_ALLOC(double,envPt->robot[nr_ctr]->nb_dof); 
       tmp_changed_robot_config[nr_ctr]=0;
     } 

curr_geo_plan.total_no_nodes=0;

return 1;
}

#endif

int assign_indices_of_robots()
{

 envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
 
    int nr = envPt->nr;
    int nr_ctr=0;
    for(;nr_ctr<nr;nr_ctr++)
     {
    if (strcasestr(envPt->robot[nr_ctr]->name,"HRP2_ROBOT"))//HRP2 has to perform the action
      {
       rob_indx.HRP2=nr_ctr;
      }
    else
      {
      if (strcasestr(envPt->robot[nr_ctr]->name,"JIDO_ROBOT"))//HRP2 has to perform the action
       {
       rob_indx.JIDO=nr_ctr;
       }
       else
       {
       if (strcasestr(envPt->robot[nr_ctr]->name,"YELLOW"))//HRP2 has to perform the action
        {
       rob_indx.YELLOW_BOTTLE=nr_ctr;
        }
       else
        {
        if (strcasestr(envPt->robot[nr_ctr]->name,"HUMAN1"))//HRP2 has to perform the action
         {
          rob_indx.HUMAN=nr_ctr;
         }
        }
       }
      } 
     }      
  return 1;
}


int Create_and_init_Mightability_Maps()
{
 printf(" Inside Create_and_init_Mightability_Maps()\n");
 p3d_init_robot_parameters(); //To remove the dependency on Watch button
 printf(" After p3d_init_robot_parameters()\n");

 assign_indices_of_robots();

 #ifdef USE_SYM_GEO_PLAN
 init_geometric_plan_data();
 #endif

 printf(" Calling find_affordance \n");
 find_affordance_new();
 printf(" After find_affordance()\n");
 ////find_symbolic_Mightability_Map();
 find_symbolic_Mightability_Map_new();
 printf(" After find_symbolic_Mightability_Map_new()\n");
   ////show_symbolic_Mightability_Map_Relations();
 CALCULATE_AFFORDANCE=1;

 Affordances_Found=1; 

 ////fl_check_forms();
 ////g3d_draw_allwin_active();
return 1;
}

int get_set_of_points_to_put_object()
{
   if(Affordances_Found==1)
   {
   //////////printf(" Inside get_set_of_points_to_put_object()\n");
   ////update_robots_and_objects_status();

/*
   envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
  
   int x,y,z;

  int no = envPt->no;
  int nr = envPt->nr;
  p3d_obj *o;
  p3d_rob *r;
  int r_ctr=0;
  for(r_ctr=0;r_ctr<nr;r_ctr++)
  {
   robots_status_for_Mightability_Maps[r_ctr].has_moved=1
  } 
*/      

   ////HRP2_HAS_MOVED=1;
   ////HUMAN_HAS_MOVED=1;
   ////JIDO_HAS_MOVED=1;
   ////update_Mightability_Maps();

   #ifdef HRI_JIDO
   JIDO_find_candidate_points_on_plane_to_put_obj();
   assign_weights_on_candidte_points_to_put_obj(); 
   #elif defined(HRI_HRP2)
   find_candidate_points_on_plane_to_put_obj_new();
   assign_weights_on_candidte_points_to_put_obj(); 
   #endif
   
   CANDIDATE_POINTS_FOR_TASK_FOUND=1;
   ////SHOW_PUT_OBJ_CANDIDATES=1;
   ////SHOW_WEIGHT_FOR_CANDIDATE_POINTS=0;
   ////show_3D_workspace_Bounding_Box();
   ////show_weighted_candidate_points_to_put_obj(0);
   return 1;
  } 

return 0;


}

void get_Frustum_Vertices(float l, float r, float b, float t, float n, float f)
{
point_co_ordi frustumVertices[8];
    float ratio;
    float farLeft;
    float farRight;
    float farBottom;
    float farTop;
	int  projectionMode = 0;
	// perspective mode
    if(projectionMode == 0)
        ratio     = f / n;
    // orthographic mode
    else
        ratio = 1;
    farLeft   = l * ratio;
    farRight  = r * ratio;
    farBottom = b * ratio;
    farTop    = t * ratio;
	
    // compute 8 vertices of the frustum
    // near top right
    frustumVertices[0].x = r;
    frustumVertices[0].y = t;
    frustumVertices[0].z = -n;
	
    // near top left
    frustumVertices[1].x = l;
    frustumVertices[1].y = t;
    frustumVertices[1].z = -n;
	
    // near bottom left
    frustumVertices[2].x = l;
    frustumVertices[2].y = b;
    frustumVertices[2].z = -n;
	
    // near bottom right
    frustumVertices[3].x = r;
    frustumVertices[3].y = b;
    frustumVertices[3].z = -n;
	
    // far top right
    frustumVertices[4].x = farRight;
    frustumVertices[4].y = farTop;
    frustumVertices[4].z = -f;
	
    // far top left
    frustumVertices[5].x = farLeft;
    frustumVertices[5].y = farTop;
    frustumVertices[5].z = -f;
	
    // far bottom left
    frustumVertices[6].x = farLeft;
    frustumVertices[6].y = farBottom;
    frustumVertices[6].z = -f;
	
    // far bottom right
    frustumVertices[7].x = farRight;
    frustumVertices[7].y = farBottom;
    frustumVertices[7].z = -f;
	
   
    //AKP: To store the frustumVertices
    int i=0;
    for(i=0;i<8&&no_FOV_end_point_vertices<1000;i++)
    {
     //printf(" get_Frustum_Vertices, i= %d\n",i);
     p3d_vector4 point, point2;
     point2[0] = frustumVertices[i].x;
     point2[1] = frustumVertices[i].y;
     point2[2] = frustumVertices[i].z;
     point2[3] = 1.0;
     //p3d_rob *rob=ACBTSET->human[ACBTSET->actual_human]->HumanPt;
     //p3d_matvec4Mult(rob->o[rob->cam_body_index]->jnt->abs_pos,point2,point);
     p3d_matvec4Mult(frustum_transformation_mat,point2,point);
     FOV_end_point_vertices[no_FOV_end_point_vertices][i].x=point[0];
     FOV_end_point_vertices[no_FOV_end_point_vertices][i].y=point[1];
     FOV_end_point_vertices[no_FOV_end_point_vertices][i].z=point[2];
    ////FOV_end_point_vertices[no_FOV_end_point_vertices][i].x=frustumVertices[i].x;
    ////FOV_end_point_vertices[no_FOV_end_point_vertices][i].y=frustumVertices[i].y;
    ////FOV_end_point_vertices[no_FOV_end_point_vertices][i].z=frustumVertices[i].z;
    }
    no_FOV_end_point_vertices++;
    ////////printf(" no_FOV_end_point_vertices=%d\n",no_FOV_end_point_vertices);
}

int get_points_on_FOV_screen(p3d_rob *r)
{
  ////////printf("get_points_on_FOV_screen, r->cam_body_index =%d\n",r->cam_body_index);
  p3d_obj *objPt = r->o[r->cam_body_index]; 
  
  p3d_jnt *jntPt = objPt->jnt;
  //p3d_jnt *jntPt = r->joints[1];
  
  p3d_matrix4 mattemp,mattemp2;
  int i,j;
  switch ( r->cam_axe) {
  case 0:
    p3d_mat4Pos(mattemp,r->cam_pos[0],r->cam_pos[1],r->cam_pos[2], M_PI-r->cam_tilt,r->cam_pan,0);
    break;
  case 1:
    p3d_mat4Pos(mattemp,r->cam_pos[0],r->cam_pos[1],r->cam_pos[2], M_PI_2+r->cam_pan,0,-r->cam_tilt);
    p3d_mat4Pos(mattemp2,0,0,0, 0,0,M_PI_2);
    break;
  case 2:
    p3d_mat4Pos(mattemp,r->cam_pos[0],r->cam_pos[1],r->cam_pos[2], 0, -M_PI_2-r->cam_pan, r->cam_tilt);
    break;
  default:
    break;
  }
  

  
  
  for(i=0 ; i<=3 ; i++){
    for(j=0 ; j<=3 ; j++){
      
      frustum_transformation_mat[i][j]=jntPt->abs_pos[i][j];
      ////printf(" %lf ",frustum_transformation_mat[i][j]);  
      
    }
   ////printf("\n");
  }
  //matrix[14]=0.0;
  //float degYang = (r->cam_v_angle * 180.0/M_PI);//good
  float degYang = (((r->cam_h_angle*2.0)/3.0) * 180.0/M_PI);
  
  
  ////g3d_draw_cone(r->cam_pos[0],r->cam_pos[1],r->cam_pos[2], r->cam_min_range, r->cam_max_range, r->cam_v_angle, r->cam_h_angle, r->cam_axe, r->cam_pan, r->cam_tilt);
  
  p3d_matrix4 tmp_res;
  p3d_mat4Mult(frustum_transformation_mat,mattemp,tmp_res);
  p3d_mat4Copy(tmp_res,frustum_transformation_mat);
  
  if ( r->cam_axe==1)
    {
      
      p3d_mat4Mult(frustum_transformation_mat,mattemp2,tmp_res);
      p3d_mat4Copy(tmp_res,frustum_transformation_mat);
      ////p3d_matvec4Mult(frustum_trans_mat,mattemp2,frustum_trans_mat);
    }
  //perspectiveGL(degYang, r->cam_h_angle/r->cam_v_angle ,0.001, 8.0);//original
  
  
  ////perspectiveGL(degYang, 3.0/2.0 ,0.001, 2.0);
  
  GLdouble fovY=degYang; 
  GLdouble aspect=3.0/2.0;
  GLdouble zNear=0.001;
  GLdouble zFar=2.0; 
  GLdouble fW, fH;
  //	Note:	tan( double ) uses radians but OpenGL works in degrees so we convert
  //			degrees to radians by dividing by 360 then multiplying by pi.
  fH = tanf( (fovY / 2.0) / 180.0 * M_PI ) * zNear;	
  // Same as fH = tan( fovY / 360 * pi ) * zNear;
  //	Calculate the distance from 0 of the x clipping plane based on the aspect ratio.
  //fW = tanf( (aspect / 2.0) / 180.0 * M_PI ) * zNear;
  fW = fH * aspect;
  //printf("fh %f   fw %f\n",fH, fW);	
  //glFrustum( -fW, fW, -fH, fH, zNear, zFar );
  
  //glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
  //g3d_set_color_mat(Any,tBluev);
  get_Frustum_Vertices(-fW, fW, -fH, fH, zNear, zFar );	
  
  return 1;

}



int check_inside_polygon(int no_vertices, point_co_ordi *vertices, point_co_ordi point)//the order of vertices should be clockwise or counter clockwise 
{
 //Equation of one edge is (y-y1)-((y2-y1)/(x2-x1))*(x-x1)=0;
 int i=0;
 int ok=0;
 for(i=0;i<no_vertices;i++)
 {
  double sign_of_next_vertex=(vertices[(i+2)%no_vertices].y-vertices[i%no_vertices].y)-((vertices[(i+1)%no_vertices].y-vertices[i%no_vertices].y)/(vertices[(i+1)%no_vertices].x-vertices[i%no_vertices].x))/(vertices[(i+2)%no_vertices].x-vertices[(i)%no_vertices].x);
  double sign_of_point=(point.y-vertices[i%no_vertices].y)-((vertices[(i+1)%no_vertices].y-vertices[i%no_vertices].y)/(vertices[(i+1)%no_vertices].x-vertices[i%no_vertices].x))/(point.x-vertices[(i)%no_vertices].x);
  if((sign_of_next_vertex>0&&sign_of_point>0)||(sign_of_next_vertex<0&&sign_of_point<0))
  {
  ok=1;
  }
  else
  {
  ok=0;
  return 0;
  }
  
 }

 if(ok==1)
 return 1;

}

int is_point_visible_by_st_line(configPt from_point, configPt to_point, int visibility_test_for)// Finds if there exists a collision free straight line from from_point to to_point 
{
//printf(" Finding st line path from (%lf,%lf,%lf) to (%lf,%lf,%lf) \n",from_point[6],from_point[7], from_point[8], to_point[6], to_point[7], to_point[8]);

if(visibility_test_for==1)// 1 means test for human
 {
int ntest;
   p3d_localpath * localpath;

////p3d_col_activate_robots();
////p3d_col_activate_rob_env(ACBTSET->visball);
p3d_col_deactivate_rob_rob(ACBTSET->visball,ACBTSET->human[ACBTSET->actual_human]->HumanPt);
p3d_col_activate_rob_rob(ACBTSET->visball,ACBTSET->robot);
double v = p3d_get_env_dmax();
//printf(" current v = %lf \n",v);
p3d_set_env_dmax(0.05);


        if( (localpath = p3d_local_planner(ACBTSET->visball, from_point, to_point)) )
        { 
	if(p3d_unvalid_localpath_test(ACBTSET->visball, localpath, &ntest))
         {
          p3d_set_env_dmax(v);
	  destroy_list_localpath(ACBTSET->visball, localpath);
	  ////printf("\nThere exits no straight line path from (%lf, %lf) to (%lf, %lf)\n",from_point[6],from_point[7],to_point[6],to_point[7]);
          ////fflush(stdout);
	  return 0;
	 }
        }
      p3d_set_env_dmax(v);
      destroy_list_localpath(ACBTSET->visball, localpath);
    	p3d_col_activate_rob_rob(ACBTSET->visball,ACBTSET->human[ACBTSET->actual_human]->HumanPt);
  ////printf("\nThere exits straight line path from (%lf, %lf) to (%lf, %lf)\n",from_point[6],from_point[7],to_point[6],to_point[7]);
      ////    fflush(stdout);
  return 1;
 }	
 if(visibility_test_for==2)// 2 means test for HRP2
 {
int ntest;
   p3d_localpath * localpath;

////p3d_col_activate_robots();
////p3d_col_activate_rob_env(ACBTSET->visball);
p3d_col_deactivate_rob_rob(ACBTSET->visball,ACBTSET->robot);
double v = p3d_get_env_dmax();
//printf(" current v = %lf \n",v);
p3d_set_env_dmax(0.05);


        if( (localpath = p3d_local_planner(ACBTSET->visball, from_point, to_point)) )
        { 
	if(p3d_unvalid_localpath_test(ACBTSET->visball, localpath, &ntest))
         {
          p3d_set_env_dmax(v);
	  destroy_list_localpath(ACBTSET->visball, localpath);
	  ////printf("\nThere exits no straight line path from (%lf, %lf) to (%lf, %lf)",from_point[6],from_point[7],to_point[6],to_point[7]);
          ////fflush(stdout);
	  return 0;
	 }
        }
      p3d_set_env_dmax(v);
      destroy_list_localpath(ACBTSET->visball, localpath);
    	p3d_col_activate_rob_rob(ACBTSET->visball,ACBTSET->robot);
  ////printf("\nThere exits straight line path from (%lf, %lf) to (%lf, %lf)",from_point[6],from_point[7],to_point[6],to_point[7]);
      ////    fflush(stdout);
  return 1;
 }	
}

int is_point_in_fov(p3d_rob* robot, p3d_vector4 p)
{
  
  p3d_rob* rtemp = PSP_ROBOT;
  PSP_ROBOT = robot;
  //p3d_vector4 objectCenter;
  //double tempAngH = robot->cam_h_angle;
  //double tempAngW = robot->cam_h_angle;
  
  //p3d_get_robot_center(object, objectCenter); 
  //robot->cam_h_angle = angleH;
  //robot->cam_v_angle = angleW;
  int plane;
  G3D_Window *win = g3d_get_win_by_name("Perspective");
  g3d_refresh_win(win);
  int is_in_FOV=0;
  for(plane = 0; plane < 6; plane++ ) // for all perspective window frustum planes
    {
      // if the point is in the negative side of the frustum plan means that it i maybe inside the frustum box
      if(win->vs.frustum[plane][0] * (p[0]) + win->vs.frustum[plane][1] * (p[1])
	 + win->vs.frustum[plane][2] * (p[2]) + win->vs.frustum[plane][3] > 0 ) 
	{
	  is_in_FOV=1;
	}
      else
	{
	  is_in_FOV= 0;
	  plane=1000;
	}
    }
  PSP_ROBOT = rtemp;
  
  
  return is_in_FOV;
}

int is_point_on_surface_visible(point_co_ordi point, int visibility_test_for) 
{
  if(visibility_test_for==1)// 1 means test for human
 {
  

    configPt hum_cur_pos  = p3d_get_robot_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt);
    configPt hum_head_pos  = p3d_get_robot_config(ACBTSET->visball);  // We need to use a visball as the robot for finding the collosion free straight line path otherwise if we will use human it might return collision
    hum_head_pos[6] = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_NECK_PAN]->abs_pos[0][3];//hum_cur_pos[6];
    hum_head_pos[7] = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_NECK_PAN]->abs_pos[1][3];//hum_cur_pos[7];
    hum_head_pos[8] = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_NECK_PAN]->abs_pos[2][3]+0.1;
    
    configPt point_to_look = p3d_get_robot_config(ACBTSET->visball);  
    point_to_look[6] = point.x;
    point_to_look[7] = point.y;
    point_to_look[8] = point.z;
    


  p3d_vector4 objCenter,robCenter;
  int i,j;
 
  p3d_vector4 pointHead,pointDisc, pointAhead;
  double cone_length=1.5;
  double disttocenter;
  //p3d_rob *human = PSP_BTSET->human[PSP_BTSET->actual_human]->HumanPt;

  p3d_rob * currRob=ACBTSET->human[ACBTSET->actual_human]->HumanPt;
 
  //mocap_get_disc_point(pointDisc);
  psu_get_point_ahead_cam(currRob, cone_length, pointAhead); 

 
  //p3d_get_object_center(currRob->o[currRob->cam_body_index], pointHead);

  pointHead[0]=hum_head_pos[6];
  pointHead[1]=hum_head_pos[7];
  pointHead[2]=hum_head_pos[8];

  objCenter[0]=point_to_look[6];
  objCenter[1]=point_to_look[7];
  objCenter[2]=point_to_look[8];
  //lx1 = pointHead [0];
  //ly1 = pointHead [1];
  //lz1 = pointHead [2];
  //printf("point Head %f %f %f \n", pointHead [0],pointHead [1],pointHead [2] );
 // lx2 = pointAhead [0];
 // ly2 = pointAhead [1];
 // lz2 = pointAhead [2];
  //printf("point ahead %f %f %f \n", pointAhead [0],pointAhead [1],pointAhead [2] );

 // psp_deselect_all();
  //  sphereActive = 1;  
  //Static Obstacles
 
	//  p3d_get_object_center(o,objCenter);
	
	 // if(linearDistance(robCenter[0],robCenter[1], objCenter[0], objCenter[1])<=radius)
	 //   {
	     
		  
		  
	  

   //if (p3d_psp_is_point_in_a_cone(objCenter, pointHead, pointAhead, M_PI/1.5, &disttocenter))
  //// printf("currRob->cam_h_angle=%lf\n",currRob->cam_h_angle);
   if(is_point_in_fov(currRob, objCenter)) //*****Commented temporary because not to restrict in FOV
   {
    
    int is_visible=is_point_visible_by_st_line(hum_head_pos, point_to_look, visibility_test_for); 
    p3d_destroy_config(ACBTSET->visball, hum_head_pos);
    p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
    p3d_destroy_config(ACBTSET->visball, point_to_look);
  
   return is_visible;
   }
   /*
   p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
   if(psp_is_object_visible(currRob, ACBTSET->visball, 80))
   return 1;
   else
   return 0;
   */
   /*else
   {
   p3d_destroy_config(ACBTSET->visball, hum_head_pos);
    p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
    p3d_destroy_config(ACBTSET->visball, point_to_look);
   return 0; 
   }*/
 }

  if(visibility_test_for==2)// 2 means test for HRP2
 {
  
    configPt rob_cur_pos = MY_ALLOC(double,ACBTSET->robot->nb_dof); /* Allocation of temporary robot configuration */
    p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos);
 
    
    
    
    configPt rob_head_pos  = MY_ALLOC(double,ACBTSET->visball->nb_dof); // We need to use a visball as the robot for finding the collosion free straight line path otherwise if we will use human it might return collision
    p3d_get_robot_config_into(ACBTSET->visball,&rob_head_pos);

    rob_head_pos[6] = ACBTSET->robot->joints[ROBOTj_LOOK]->abs_pos[0][3];
    rob_head_pos[7] = ACBTSET->robot->joints[ROBOTj_LOOK]->abs_pos[1][3];//hum_cur_pos[7];
    rob_head_pos[8] = ACBTSET->robot->joints[ROBOTj_LOOK]->abs_pos[2][3];
    
    configPt point_to_look   = MY_ALLOC(double,ACBTSET->visball->nb_dof); // We need to use a visball as the robot for finding the collosion free straight line path otherwise if we will use human it might return collision
    p3d_get_robot_config_into(ACBTSET->visball,&point_to_look );
    
    point_to_look[6] = point.x;
    point_to_look[7] = point.y;
    point_to_look[8] = point.z;
    


  p3d_vector4 objCenter,robCenter;
  int i,j;
 
  p3d_vector4 pointHead,pointDisc, pointAhead;
  double cone_length=1.5;
  double disttocenter;
  //p3d_rob *human = PSP_BTSET->human[PSP_BTSET->actual_human]->HumanPt;

  p3d_rob * currRob=ACBTSET->robot;
 
  //mocap_get_disc_point(pointDisc);
  psu_get_point_ahead_cam(currRob, cone_length, pointAhead); 

 
  //p3d_get_object_center(currRob->o[currRob->cam_body_index], pointHead);

  pointHead[0]=rob_head_pos[6];
  pointHead[1]=rob_head_pos[7];
  pointHead[2]=rob_head_pos[8];

  objCenter[0]=point_to_look[6];
  objCenter[1]=point_to_look[7];
  objCenter[2]=point_to_look[8];
  //lx1 = pointHead [0];
  //ly1 = pointHead [1];
  //lz1 = pointHead [2];
  //printf("point Head %f %f %f \n", pointHead [0],pointHead [1],pointHead [2] );
 // lx2 = pointAhead [0];
 // ly2 = pointAhead [1];
 // lz2 = pointAhead [2];
  //printf("point ahead %f %f %f \n", pointAhead [0],pointAhead [1],pointAhead [2] );

 // psp_deselect_all();
  //  sphereActive = 1;  
  //Static Obstacles
 
	//  p3d_get_object_center(o,objCenter);
	
	 // if(linearDistance(robCenter[0],robCenter[1], objCenter[0], objCenter[1])<=radius)
	 //   {
	     
		  
		  
	  

   //if (p3d_psp_is_point_in_a_cone(objCenter, pointHead, pointAhead, M_PI/1.5, &disttocenter))
   if(is_point_in_fov(currRob, objCenter))//*****Commented temporary because not to restrict in FOV
   {
    
    int is_visible=is_point_visible_by_st_line(rob_head_pos, point_to_look, visibility_test_for); 
    MY_FREE(rob_cur_pos, double,ACBTSET->robot->nb_dof); 
    MY_FREE(rob_head_pos, double,ACBTSET->visball->nb_dof); 
    MY_FREE(point_to_look, double,ACBTSET->visball->nb_dof); 

   
   return is_visible;
   }
   ////else
   ////return 0;
   /*else
   {
   p3d_destroy_config(ACBTSET->visball, hum_head_pos);
    p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
    p3d_destroy_config(ACBTSET->visball, point_to_look);
   return 0; 
   }*/
 }
 

}

//double rob_shoulder_pos
int is_point_directly_reachable_by_HRP2(point_co_ordi point)
{
    int by_LHand=0;
    int by_RHand=0;
    double maxi_reach_dist=0.75;
    
   /*
    int ROBOTj_RSHOULDER=19;//29;
    int ROBOTj_LSHOULDER=32;//42;
   */

    //configPt hum_cur_pos  = p3d_get_robot_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt);
    configPt hum_shoulder_pos  = p3d_get_robot_config(ACBTSET->visball);  // We need to use a visball as the robot for finding the collosion free straight line path otherwise if we will use human it might return collision
    
    // Reachability Test by right hand
    hum_shoulder_pos[6] = ACBTSET->robot->joints[ROBOTj_RSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    hum_shoulder_pos[7] = ACBTSET->robot->joints[ROBOTj_RSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    hum_shoulder_pos[8] = ACBTSET->robot->joints[ROBOTj_RSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    
    configPt point_to_reach = p3d_get_robot_config(ACBTSET->visball);  
    point_to_reach[6] = point.x;
    point_to_reach[7] = point.y;
    point_to_reach[8] = point.z;
    
    
    double dist=sqrt((hum_shoulder_pos[6]-point_to_reach[6])*(hum_shoulder_pos[6]-point_to_reach[6])+(hum_shoulder_pos[7]-point_to_reach[7])*(hum_shoulder_pos[7]-point_to_reach[7])+(hum_shoulder_pos[8]-point_to_reach[8])*(hum_shoulder_pos[8]-point_to_reach[8]));
    
    if(dist<maxi_reach_dist)
    by_RHand=1;
    

    // Reachability Test by left hand
    hum_shoulder_pos[6] = ACBTSET->robot->joints[ROBOTj_LSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    hum_shoulder_pos[7] = ACBTSET->robot->joints[ROBOTj_LSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    hum_shoulder_pos[8] =ACBTSET->robot->joints[ROBOTj_LSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively

    dist=sqrt((hum_shoulder_pos[6]-point_to_reach[6])*(hum_shoulder_pos[6]-point_to_reach[6])+(hum_shoulder_pos[7]-point_to_reach[7])*(hum_shoulder_pos[7]-point_to_reach[7])+(hum_shoulder_pos[8]-point_to_reach[8])*(hum_shoulder_pos[8]-point_to_reach[8]));

    if(dist<maxi_reach_dist)
    by_LHand=1;
    
    p3d_destroy_config(ACBTSET->visball, hum_shoulder_pos);
    //p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
    p3d_destroy_config(ACBTSET->visball, point_to_reach);    

    if(by_RHand==1&&by_LHand==1)
    return 3; // Reachable by both hands
    if(by_RHand==1)
    return 2; // reachable by right hand only
    if(by_LHand==1)
    return 1; // reachable by left hand only
    else
    return 0; // not reachable
 
}


int is_point_directly_reachable_by_human(point_co_ordi point)
{
    int by_LHand=0;
    int by_RHand=0;
    double maxi_reach_dist=0.5;
    

    //configPt hum_cur_pos  = p3d_get_robot_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt);
    configPt hum_shoulder_pos  = p3d_get_robot_config(ACBTSET->visball);  // We need to use a visball as the robot for finding the collosion free straight line path otherwise if we will use human it might return collision
    
    // Reachability Test by right hand
    hum_shoulder_pos[6] = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    hum_shoulder_pos[7] = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    hum_shoulder_pos[8] =ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    
    configPt point_to_reach = p3d_get_robot_config(ACBTSET->visball);  
    point_to_reach[6] = point.x;
    point_to_reach[7] = point.y;
    point_to_reach[8] = point.z;
    
    
    double dist=sqrt((hum_shoulder_pos[6]-point_to_reach[6])*(hum_shoulder_pos[6]-point_to_reach[6])+(hum_shoulder_pos[7]-point_to_reach[7])*(hum_shoulder_pos[7]-point_to_reach[7])+(hum_shoulder_pos[8]-point_to_reach[8])*(hum_shoulder_pos[8]-point_to_reach[8]));
    
    if(dist<maxi_reach_dist)
    by_RHand=1;
    

    // Reachability Test by left hand
    hum_shoulder_pos[6] = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    hum_shoulder_pos[7] = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    hum_shoulder_pos[8] =ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively

    dist=sqrt((hum_shoulder_pos[6]-point_to_reach[6])*(hum_shoulder_pos[6]-point_to_reach[6])+(hum_shoulder_pos[7]-point_to_reach[7])*(hum_shoulder_pos[7]-point_to_reach[7])+(hum_shoulder_pos[8]-point_to_reach[8])*(hum_shoulder_pos[8]-point_to_reach[8]));

    if(dist<maxi_reach_dist)
    by_LHand=1;
    
    p3d_destroy_config(ACBTSET->visball, hum_shoulder_pos);
    //p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
    p3d_destroy_config(ACBTSET->visball, point_to_reach);    

    if(by_RHand==1&&by_LHand==1)
    return 3; // Reachable by both hands
    if(by_RHand==1)
    return 2; // reachable by right hand only
    if(by_LHand==1)
    return 1; // reachable by left hand only
    else
    return 0; // not reachable
 
}



int initialize_surfaces_in_env()
{


  envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
  p3d_vector4 objCenter,robCenter;
  int i,j;
  int no = envPt->no;
  int nr = envPt->nr;
  p3d_obj *o;
  p3d_rob *r;
  int contObj =0;
  p3d_vector4 pointHead,pointDisc, pointAhead;
 

 curr_surfaces_in_env.total_no_of_surfaces=0;
 int ns=0;
    //Robot body parts
  
    for(i=0;i<nr;i++)
    {
      r = envPt->robot[i];
      if (strcasestr(r->name,"table"))
     {
 	//// printf("envPt->robot[%d]->name =%s\n", i, envPt->robot[i]->name);
     ////printf(" envPt->robot[%d]->BB.xmin, xmax, ymin, ymax, zmin, zmax : ( %lf, %lf, %lf, %lf, %lf, %lf )\n",i,envPt->robot[i]->BB.xmin,envPt->robot[i]->BB.xmax,envPt->robot[i]->BB.ymin,envPt->robot[i]->BB.ymax, envPt->robot[i]->BB.zmin,envPt->robot[i]->BB.zmax);	

		curr_surfaces_in_env.flat_surf[ns].no_vertices=4;
  		curr_surfaces_in_env.flat_surf[ns].surface_shape=3;//Rectangle

//AKP NOTE : Take care that vertices should be in clockwise or anticlockwise order
// TO DO : AKP : WARNING : Now assuming that the rectangle sides are parallel to the x and y axes, so the vertices are directly assigned as the bounding box values. In future make changes to assign the vertices of such rectangles wgich are not parallel to the axes

  		 curr_surfaces_in_env.flat_surf[ns].vertices[0].x=envPt->robot[i]->BB.xmin;
 		 curr_surfaces_in_env.flat_surf[ns].vertices[0].y=envPt->robot[i]->BB.ymin;
 		 curr_surfaces_in_env.flat_surf[ns].vertices[0].z=envPt->robot[i]->BB.zmax;
 
		  curr_surfaces_in_env.flat_surf[ns].vertices[1].x=envPt->robot[i]->BB.xmin;
		  curr_surfaces_in_env.flat_surf[ns].vertices[1].y=envPt->robot[i]->BB.ymax;
		  curr_surfaces_in_env.flat_surf[ns].vertices[1].z=envPt->robot[i]->BB.zmax;
 
		  curr_surfaces_in_env.flat_surf[ns].vertices[2].x=envPt->robot[i]->BB.xmax;
		  curr_surfaces_in_env.flat_surf[ns].vertices[2].y=envPt->robot[i]->BB.ymax;
		  curr_surfaces_in_env.flat_surf[ns].vertices[2].z=envPt->robot[i]->BB.zmax;
 
  		curr_surfaces_in_env.flat_surf[ns].vertices[3].x=envPt->robot[i]->BB.xmax;
 		 curr_surfaces_in_env.flat_surf[ns].vertices[3].y=envPt->robot[i]->BB.ymin;
 		 curr_surfaces_in_env.flat_surf[ns].vertices[3].z=envPt->robot[i]->BB.zmax;

 		 curr_surfaces_in_env.flat_surf[ns].BR_x_min=envPt->robot[i]->BB.xmin;
		  curr_surfaces_in_env.flat_surf[ns].BR_x_max=envPt->robot[i]->BB.xmax;
 		 curr_surfaces_in_env.flat_surf[ns].BR_y_min=envPt->robot[i]->BB.ymin;;
		  curr_surfaces_in_env.flat_surf[ns].BR_y_max=envPt->robot[i]->BB.ymax;;

 		 curr_surfaces_in_env.flat_surf[ns].BR_z=envPt->robot[i]->BB.zmax;
		  curr_surfaces_in_env.total_no_of_surfaces++;
                  ns++;
     }
    /*
      for(j=0;j<r->no;j++)
	{
	  o = r->o[j];
           printf("envPt->robot[%d]->o[%d]->name =%s, envPt->robot[%d]->o[%d]->np =%d\n", i,j,  envPt->robot[i]->o[j]->name,i,j,envPt->robot[i]->o[j]->np);
	  if (strcasestr(o->name,"table"))
	    {
	      //p3d_get_object_center(o,objCenter);
	      printf(" envPt->robot[%d]->o[%d]->BB.xmin, xmax, ymin, ymax, zmin, zmax : ( %lf, %lf, %lf, %lf, %lf, %lf )\n",i,j,envPt->robot[i]->o[j]->BB.xmin,envPt->robot[i]->o[j]->BB.xmax,envPt->robot[i]->o[j]->BB.ymin,envPt->robot[i]->o[j]->BB.ymax, envPt->robot[i]->o[j]->BB.zmin,envPt->robot[i]->o[j]->BB.zmax);	

              
	    }
	}
    */
      //define if robot is near or not? here or in observation? od we need a different list
      // if ((ContObjTmp/r->no)>.4)
      // 

    }

// 1st surface
/*
  curr_surfaces_in_env.flat_surf[0].no_vertices=4;
  curr_surfaces_in_env.flat_surf[0].surface_shape=3;//Rectangle

  curr_surfaces_in_env.flat_surf[0].vertices[0].x=0.5+4.6;
  curr_surfaces_in_env.flat_surf[0].vertices[0].y=0.5-3;
  curr_surfaces_in_env.flat_surf[0].vertices[0].z=0.75;
 
  curr_surfaces_in_env.flat_surf[0].vertices[1].x=0.5+4.6;
  curr_surfaces_in_env.flat_surf[0].vertices[1].y=-0.5-3;
  curr_surfaces_in_env.flat_surf[0].vertices[1].z=0.75;
 
  curr_surfaces_in_env.flat_surf[0].vertices[2].x=-0.5+4.6;
  curr_surfaces_in_env.flat_surf[0].vertices[2].y=-0.5-3;
  curr_surfaces_in_env.flat_surf[0].vertices[2].z=0.75;
 
  curr_surfaces_in_env.flat_surf[0].vertices[3].x=-0.5+4.6;
  curr_surfaces_in_env.flat_surf[0].vertices[3].y=0.5-3;
  curr_surfaces_in_env.flat_surf[0].vertices[3].z=0.75;

  curr_surfaces_in_env.flat_surf[0].BR_x_min=-0.5+4.6;
  curr_surfaces_in_env.flat_surf[0].BR_x_max=0.5+4.6;
  curr_surfaces_in_env.flat_surf[0].BR_y_min=-0.5-3;
  curr_surfaces_in_env.flat_surf[0].BR_y_max=0.5-3;

  curr_surfaces_in_env.flat_surf[0].BR_z=0.75;
  curr_surfaces_in_env.total_no_of_surfaces=1;

//2nd surface
curr_surfaces_in_env.flat_surf[1].no_vertices=4;
  curr_surfaces_in_env.flat_surf[1].surface_shape=3;//Rectangle

  curr_surfaces_in_env.flat_surf[1].vertices[0].x=1.2+4.6;
  curr_surfaces_in_env.flat_surf[1].vertices[0].y=1.75-3;
  curr_surfaces_in_env.flat_surf[1].vertices[0].z=0.5;
 
  curr_surfaces_in_env.flat_surf[1].vertices[1].x=1.2+4.6;
  curr_surfaces_in_env.flat_surf[1].vertices[1].y=0.75-3;
  curr_surfaces_in_env.flat_surf[1].vertices[1].z=0.5;
 
  curr_surfaces_in_env.flat_surf[1].vertices[2].x=0.2+4.6;
  curr_surfaces_in_env.flat_surf[1].vertices[2].y=0.75-3;
  curr_surfaces_in_env.flat_surf[1].vertices[2].z=0.5;
 
  curr_surfaces_in_env.flat_surf[1].vertices[3].x=0.2+4.6;
  curr_surfaces_in_env.flat_surf[1].vertices[3].y=1.75-3;
  curr_surfaces_in_env.flat_surf[1].vertices[3].z=0.5;

  curr_surfaces_in_env.flat_surf[1].BR_x_min=0.2+4.6;
  curr_surfaces_in_env.flat_surf[1].BR_x_max=1.2+4.6;
  curr_surfaces_in_env.flat_surf[1].BR_y_min=0.75-3;
  curr_surfaces_in_env.flat_surf[1].BR_y_max=1.75-3;

  curr_surfaces_in_env.flat_surf[1].BR_z=0.5;
  curr_surfaces_in_env.total_no_of_surfaces=2;
  
  */
  return 1;
}


int show_3d_grid_affordances_new()
{
/*
     //tmp for showing the points for putting inside 
envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
 double increment=3.0/4.0*grid_around_HRP2.GRID_SET->pace;
int nr_ctr=0;
  int nr = envPt->nr;
 for(nr_ctr=0;nr_ctr<nr;nr_ctr++)
  {
    if(strcasestr(envPt->robot[nr_ctr]->name,"ROBOT")||strcasestr(envPt->robot[nr_ctr]->name,"HUMAN")||strcasestr(envPt->robot[nr_ctr]->name,"TABLE"))
   {
    //No need to calculate PUTABILITY :-)
   }
    else
   {
     double top_z=envPt->robot[nr_ctr]->BB.zmax;
     int cell_top_z=(top_z- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace;
     ////cell_top_z+=1;
     double BB_x_ctr;
     double BB_y_ctr;

 if(cell_top_z>=0&&cell_top_z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
    { 
     for(BB_x_ctr=envPt->robot[nr_ctr]->BB.xmin;BB_x_ctr<envPt->robot[nr_ctr]->BB.xmax;BB_x_ctr+=increment)
     {
      int cell_x=(BB_x_ctr- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  

      if(cell_x>=0&&cell_x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
      {
      for(BB_y_ctr=envPt->robot[nr_ctr]->BB.ymin;BB_y_ctr<envPt->robot[nr_ctr]->BB.ymax;BB_y_ctr+=increment)
       {
      
       int cell_y=(BB_y_ctr- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace;  
 
       if(cell_y>=0&&cell_y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
        {

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_top_z].val>=0&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_top_z-1].val>=0)//No obstacle
         {

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_top_z].Mightability_Map.reachable_by_JIDO_Hand==1)
          {  
       //g3d_drawDisc(BB_x_ctr, BB_y_ctr, top_z, grid_around_HRP2.GRID_SET->pace/2.0, Red, NULL);
          }
    
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_top_z].Mightability_Map.reachable_by_human_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_top_z].Mightability_Map.reachable_by_human_RHand==1)
          {  
       g3d_drawDisc(BB_x_ctr, BB_y_ctr, top_z, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
          }

         }
        }
       } 
      }
     }
    }
   }
  }
 */
    

/* point_co_ordi shoulder_pos;
shoulder_pos.x = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    g3d_drawDisc(shoulder_pos.x, shoulder_pos.y, shoulder_pos.z, .1, Green, NULL);

 shoulder_pos.x = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    g3d_drawDisc(shoulder_pos.x, shoulder_pos.y, shoulder_pos.z, .1, Red, NULL);
*/
/*
point_co_ordi neck_pos;
neck_pos.x = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_NECK_PAN]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    neck_pos.y = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_NECK_PAN]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    neck_pos.z = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_NECK_PAN]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    g3d_drawDisc(neck_pos.x, neck_pos.y, neck_pos.z, .1, Blue, NULL);
*/
 ////printf("Inside show_3d_grid_affordances()\n");
  /*printf("Inside show_3d_grid_affordances_new()\n");
  point_co_ordi shoulder_pos;
      ////point_co_ordi sphere_pt;
      
    // Reachability Test by right hand
    shoulder_pos.x = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    g3d_drawDisc(shoulder_pos.x, shoulder_pos.y, shoulder_pos.z, .1, Green, NULL);
 
  no_sphere_surface_pts=0;
    find_reachable_sphere_surface(1,1);
double interval=grid_around_HRP2.GRID_SET->pace/1.0;
 for(int sp_ctr=0;sp_ctr<no_sphere_surface_pts;sp_ctr++)
      {
      double t=0;
       for(;t<1;t+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t)*shoulder_pos.x+t*sphere_surface_pts[sp_ctr].x;
      double y2=(1-t)*shoulder_pos.y+t*sphere_surface_pts[sp_ctr].y;
      double z2=(1-t)*shoulder_pos.z+t*sphere_surface_pts[sp_ctr].z; 
      g3d_drawDisc(x2, y2, z2, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
       }
       //g3d_drawDisc(sphere_pts[sp_ctr].x, sphere_pts[sp_ctr].y, sphere_pts[sp_ctr].z, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
      }
   
    
    shoulder_pos.x = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    g3d_drawDisc(shoulder_pos.x, shoulder_pos.y, shoulder_pos.z, .1, Red, NULL);
  
  
  no_sphere_surface_pts=0;
    find_reachable_sphere_surface(2,1);

 for(int sp_ctr=0;sp_ctr<no_sphere_surface_pts;sp_ctr++)
      {
      double t=0;
       for(;t<1;t+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t)*shoulder_pos.x+t*sphere_surface_pts[sp_ctr].x;
      double y2=(1-t)*shoulder_pos.y+t*sphere_surface_pts[sp_ctr].y;
      double z2=(1-t)*shoulder_pos.z+t*sphere_surface_pts[sp_ctr].z; 
      g3d_drawDisc(x2, y2, z2, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
       }
      //// g3d_drawDisc(sphere_pts[sp_ctr].x, sphere_pts[sp_ctr].y, sphere_pts[sp_ctr].z, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
      }
*/

/////////g3d_drawDisc(agent_eye_pos.x, agent_eye_pos.y,agent_eye_pos.z, 0.1, Red, NULL);

 int x=0;
 for(x=0;x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx;x++)
 {
 int y=0;
 for(y=0;y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny;y++)
  {
  int z=0;
  for(z=0;z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz;z++)
   {
    ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].val<0)
    ////{
       ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.visible_by_human=0;

       double cell_x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double cell_y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double cell_z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);

  
      
       int show_showing_points=0;

       
       if(SHOW_3D_VISIBLE_PLACE_HUM==1)
       {        

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
        ////else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human_neck_turn==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
         }
        }
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human_straight_head_orientation==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
         } 
       }

       if(SHOW_3D_VISIBLE_PLACE_STANDING_HUM==1)
       {        

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_standing_human==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
        ////else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_standing_human_neck_turn==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
         }
         if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human_straight_head_orientation_standing==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
         }
        } 
       }
       
       if(SHOW_3D_VISIBLE_PLACES_FOR_HRP2==1)
       {        

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_HRP2==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
        ////else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_HRP2_neck_turn==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
         }
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_HRP2_straight_head_orientation==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
         }
        }
       } 
      
       
       if(SHOW_3D_VISIBLE_PLACES_FOR_HRP2==1)
       {        

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_JIDO==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
        ////else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_JIDO_neck_turn==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
         }
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_JIDO_straight_head_orientation==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
         }
        }
       }        


       if(SHOW_3D_COMMON_VISIBLE_HRP2_HUMAN==1)
       {        
        //printf("**** SHOW_2D_COMMON_VISIBLE_HRP2_HUMAN=%d, SHOW_3D_COMMON_VISIBLE_HRP2_HUMAN=%d\n",SHOW_2D_COMMON_VISIBLE_HRP2_HUMAN,SHOW_3D_COMMON_VISIBLE_HRP2_HUMAN);
        
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_HRP2==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
       }
       
       if(SHOW_3D_DIRECT_REACHABLE_HUM==1)
       {        

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
        else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Yellow, NULL);
         }
   
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1)
         {
        //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
         }
        }
       }


       if(SHOW_3D_BENDING_REACHABLE_HUM==1)
       {        

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
        else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Yellow, NULL);
         }
   
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1)
         {
        //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
         }
        }
       }

       if(SHOW_3D_TURNING_AROUND_REACHABLE_HUM==1)
       {        

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_turning_around_bending==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_turning_around_bending==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
        else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_turning_around_bending==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Yellow, NULL);
         }
   
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_turning_around_bending==1)
         {
        //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
         }
        }
       }
       

       if(SHOW_3D_DIRECT_REACHABLE_HRP2==1)
       {        

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
        else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Yellow, NULL);
         }
   
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)
         {
        //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
         }
        }
       }

       if(SHOW_3D_DIRECT_REACHABLE_HRP2==1)
       {        

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_JIDO_Hand==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
        
        
       }

       if(SHOW_3D_COMMON_REACH_HRP2_HUMAN==1)
       {
        //printf("SHOW_3D_COMMON_REACH_HRP2_HUMAN=%d\n",SHOW_3D_COMMON_REACH_HRP2_HUMAN);
        if((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)&&((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1)||(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1)))//Points which are reachable by both hands of both, human and HRP2
        {
         g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
        else
        {
          if((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)&&(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1||(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1)))//Points which are reachable by both hands of HRP2 and atleast one hand of human
         {
         g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
         }
         else
         { 
         if((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)&&(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1))//Point which is reachable by either hand of both HRP2 and human
          {
          g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
          }
         }
        }
       }

       if(SHOW_3D_COMMON_REACH_HRP2_HUMAN==1)
       {
        //printf("SHOW_3D_COMMON_REACH_HRP2_HUMAN=%d\n",SHOW_3D_COMMON_REACH_HRP2_HUMAN);
        if((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_JIDO_Hand==1)&&((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1)))//Points which are reachable by both hands of both, human and HRP2
        {
         g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
        else
        {
          if((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_JIDO_Hand==1)&&(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1))//Points which are reachable by both hands of HRP2 and atleast one hand of human
         {
         g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
         }
         
        }
       }


       //======START for 2D on table surfaces========//
      if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_map_cell_obj_info.is_horizontal_surface==1)
      {
       ////printf(" is_horizontal_surface\n");
       if(SHOW_2D_VISIBLE_PLACE_HUM==1)
       {        
        
       
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
        }
        ////else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human_neck_turn==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Red, NULL);
         }
        }
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human_straight_head_orientation==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
        }  

       }
       
       if(SHOW_2D_VISIBLE_PLACE_STANDING_HUM==1)
       {        
        
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_standing_human==1)
        {
        ////printf(" visible by human\n");
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
        }
        
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_standing_human_neck_turn==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Red, NULL);
        }

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human_straight_head_orientation_standing==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
        }
       }

       if(SHOW_2D_VISIBLE_PLACES_FOR_HRP2==1)
       {        

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_HRP2==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
        }
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_HRP2_neck_turn==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Red, NULL);
        }
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_HRP2_straight_head_orientation==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
        }
       } 

       if(SHOW_2D_VISIBLE_PLACES_FOR_HRP2==1)
       {        

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_JIDO==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
        }
       } 
      
       if(SHOW_2D_COMMON_VISIBLE_HRP2_HUMAN==1)
       {        
        //printf("**** SHOW_2D_COMMON_VISIBLE_HRP2_HUMAN=%d, SHOW_3D_COMMON_VISIBLE_HRP2_HUMAN=%d\n",SHOW_2D_COMMON_VISIBLE_HRP2_HUMAN,SHOW_3D_COMMON_VISIBLE_HRP2_HUMAN);
        
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_HRP2==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
        }
       }
       
       if(SHOW_2D_DIRECT_REACHABLE_HUM==1)
       {        

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1)
        {
       //// printf(" Drawing disc\n"); 
        //////////g3d_drawDisc(cell_x_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_y_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
        }
        else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1)
         {
       //// printf(" Drawing disc\n"); 
        //////////g3d_drawDisc(cell_x_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_y_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Yellow, NULL);
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Yellow, NULL);
         }
   
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1)
         {
        //// printf(" Drawing disc\n"); 
        //////////g3d_drawDisc(cell_x_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_y_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
         }
        }
       }

       if(SHOW_2D_BENDING_REACHABLE_HUM==1)
       {        

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1)
        {
       //// printf(" Drawing disc\n"); 
        //////////g3d_drawDisc(cell_x_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_y_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
         g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
        }
        else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1)
         {
       //// printf(" Drawing disc\n"); 
        //////////g3d_drawDisc(cell_x_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_y_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Yellow, NULL);
         g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Yellow, NULL);
         }
   
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1)
         {
        //// printf(" Drawing disc\n"); 
        //////////g3d_drawDisc(cell_x_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_y_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
         g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
         }
        }
       }

      if(SHOW_2D_TURNING_AROUND_REACHABLE_HUM==1)
       {        

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_turning_around_bending==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_turning_around_bending==1)
        {
       //// printf(" Drawing disc\n"); 
        //////////g3d_drawDisc(cell_x_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_y_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
         g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
        }
        else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_turning_around_bending==1)
         {
       //// printf(" Drawing disc\n"); 
        //////////g3d_drawDisc(cell_x_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_y_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Yellow, NULL);
         g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Yellow, NULL); 
         }
   
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_turning_around_bending==1)
         {
         ////printf(" RHand reach by turn and bend Drawing disc\n"); 
        //////////g3d_drawDisc(cell_x_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_y_world+grid_around_HRP2.GRID_SET->pace/2.0, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
         g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
         }
        }
       }
        
       if(SHOW_2D_DIRECT_REACHABLE_HRP2==1)
       {        

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
        }
        else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Yellow, NULL);
         }
   
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)
         {
        //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
         }
        }
       }
    
       if(SHOW_2D_DIRECT_REACHABLE_HRP2==1)
       {        

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_JIDO_Hand==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
        }
       }

       if(SHOW_2D_COMMON_REACH_HRP2_HUMAN==1)
       {
        ////////printf("inside SHOW_3D_COMMON_REACH_HRP2_HUMAN=%d\n",SHOW_3D_COMMON_REACH_HRP2_HUMAN);
         if((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)&&((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1)||(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1)))//Points which are reachable by both hands of both, human and HRP2
        {
         g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
        }
        else
        {
          if((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)&&(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1||(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1)))//Points which are reachable by both hands of HRP2 and atleast one hand of human
         {
         g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
         }
         else
         { 
         if((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)&&(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1))//Point which is reachable by either hand of both HRP2 and human
          {
          g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
          }
         }
        }
       } 
      } 
       //======END for 2D on table surfaces ========//

      int show_horizontal_surfaces=0;
      if(show_horizontal_surfaces==1)
      {  
       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_map_cell_obj_info.is_horizontal_surface==1)
        {
        //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world+0.0, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
      }
 
       if(show_showing_points==1) // To calculate the candidate points to show some object to human
       {
        if((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)&&(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==1))
        {
          
          g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
         
          /*
          p3d_vector3 target_in_global_frame;
          target_in_global_frame[0]=cell_x_world;
          target_in_global_frame[1]=cell_y_world;
          target_in_global_frame[2]=cell_z_world;
          int HRP2_state=HRP2_CURRENT_STATE; //AKP : 1 is sitting, 2 is standing 
          int hand_by_reach=2; //1 for left, 2 for right hand
          double task_duration=3.0;//in s
          int thumb_up_constraint=1;//Means the x axis of the hand will be made parallel to the z axis og global frame
          vectorN backupConfig = attStandingRobot->robot()->currentConfiguration();
          int reachable_by_R_hand=HRP2_hand_reach(target_in_global_frame, hand_by_reach, task_duration, HRP2_state,thumb_up_constraint);
          attStandingRobot->staticState ( backupConfig );
          if(reachable_by_R_hand==1)
          g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
          else
          g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
         */
          
           
        } 
       }
        //fl_check_forms();
       //g3d_draw_allwin_active();
    ////}
    ////else
    ////{
    ////  double cell_x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
    ////  double cell_y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
    ////  double cell_z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
    ////  g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, 0.005, Green, NULL);
      
    ////} 
    //  fl_check_forms();
   }
  } 
 }
//fl_check_forms();
  //     g3d_draw_allwin_active();
return 1;
}


int show_3d_grid_affordances()
{
 //////////printf("Inside show_3d_grid_affordances()\n");
 int x=0;
 for(x=0;x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx;x++)
 {
 int y=0;
 for(y=0;y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny;y++)
  {
  int z=0;
  for(z=0;z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz;z++)
   {
    ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].val<0)
    ////{
       ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.visible_by_human=0;

       double cell_x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double cell_y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double cell_z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
 
       int show_visibility=0;
       int show_HRP2_reachability=0;
       int show_human_reachability=0;
       int show_showing_points=1;

       if(show_visibility==1)
       {        

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_HRP2==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
        }
        else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
         }
   
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_HRP2==1)
         {
        //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
         }
        }
       }
       if(show_HRP2_reachability==1)
       {        

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
        }
        else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
         }
   
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)
         {
        //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
         }
        }
       }
       
       if(show_human_reachability==1)
       {        

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1)
        {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Blue, NULL);
        }
        else
        {
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1)
         {
       //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
         }
   
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1)
         {
        //// printf(" Drawing disc\n"); 
        g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
         }
        }
       }
 
       if(show_showing_points==1) // To calculate the candidate points to show some object to human
       {
        if((grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)&&(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==1))
        {
          
          g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
         
          /*
          p3d_vector3 target_in_global_frame;
          target_in_global_frame[0]=cell_x_world;
          target_in_global_frame[1]=cell_y_world;
          target_in_global_frame[2]=cell_z_world;
          int HRP2_state=HRP2_CURRENT_STATE; //AKP : 1 is sitting, 2 is standing 
          int hand_by_reach=2; //1 for left, 2 for right hand
          double task_duration=3.0;//in s
          int thumb_up_constraint=1;//Means the x axis of the hand will be made parallel to the z axis og global frame
          vectorN backupConfig = attStandingRobot->robot()->currentConfiguration();
          int reachable_by_R_hand=HRP2_hand_reach(target_in_global_frame, hand_by_reach, task_duration, HRP2_state,thumb_up_constraint);
          attStandingRobot->staticState ( backupConfig );
          if(reachable_by_R_hand==1)
          g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
          else
          g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
         */
          
           
        } 
       }
        //fl_check_forms();
       //g3d_draw_allwin_active();
    ////}
    ////else
    ////{
    ////  double cell_x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
    ////  double cell_y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
    ////  double cell_z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
    ////  g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, 0.005, Green, NULL);
      
    ////} 
    //  fl_check_forms();
   }
  } 
 }
//fl_check_forms();
  //     g3d_draw_allwin_active();
return 1;
}

int update_3d_grid_reachability_for_human_new()
{

  
  int for_agent=1; //1 for human, 2 for HRP2

  

   //////////printf("Inside update_3d_grid_reachability_for_human_new()\n");
   point_co_ordi shoulder_pos;
      ////point_co_ordi sphere_pt;
configPt hum_tmp_pos=p3d_get_robot_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt);
//////////double yaw_ang=hum_tmp_pos[11]; 
double yaw_ang=hum_tmp_pos[HUMANq_TORSO_PAN]; 
double orig_yaw_ang=yaw_ang;
//////////double pitch_ang=hum_tmp_pos[14]; 
double pitch_ang=hum_tmp_pos[HUMANq_TORSO_TILT]; 
double orig_pitch_ang=pitch_ang;

int for_actual_pitch=1;
int for_actual_yaw=1;

int turn_collision=0;
//double yaw_
////for(;yaw_ang<2.0*M_PI&&turn_collision==0;yaw_ang+=0.5)
int turn_human=1;
double maxi_left_turn=M_PI/2.0;
double maxi_right_turn=M_PI/2.0;
double curr_left_turn=0.0;
double curr_right_turn=0.0;

int init_yaw=1;

double interval=grid_around_HRP2.GRID_SET->pace-0.005;///1.5;

while(turn_human==1)
 {
////////printf(" **** yaw_ang = %lf\n",yaw_ang);

   ////pitch_ang=hum_tmp_pos[14]; 
   ////orig_pitch_ang=pitch_ang;
   int collision=0;
   
pitch_ang=orig_pitch_ang;
hum_tmp_pos[HUMANq_TORSO_TILT]=pitch_ang; // Around 5 degrees
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_TILT]=hum_tmp_pos[HUMANq_TORSO_TILT];
  
for(;pitch_ang<0.785&&collision==0;pitch_ang+=0.25)
  {
//////////printf(" **** for yaw_ang = %lf, pitch_ang = %lf\n",yaw_ang, pitch_ang);

hum_tmp_pos[HUMANq_TORSO_TILT]=pitch_ang; // Around 5 degrees
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_TILT]=hum_tmp_pos[HUMANq_TORSO_TILT];

int kcd_with_report=0;
p3d_rob *human=ACBTSET->human[ACBTSET->actual_human]->HumanPt;

//////////g3d_draw_allwin_active();

int res = p3d_col_test_robot(human,kcd_with_report);
 if(res>0)
   {
  kcd_with_report=0;
  res = p3d_col_test_self_collision(human,kcd_with_report);
   if(res>0)
   {
  //////////printf(" There is self collision with human, res=%d \n", res);
  //return 0;
   }
   else
   {
  //////////printf(" **** There is collision with human, for pitch_ang = %lf res=%d \n", pitch_ang, res);
  for_actual_pitch=0;
  collision=1;
  hum_tmp_pos[HUMANq_TORSO_TILT]=orig_pitch_ang; 
  pitch_ang=orig_pitch_ang;
  p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
  ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_TILT]=hum_tmp_pos[HUMANq_TORSO_TILT];

  break;
    }
   }

   shoulder_pos.x = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
 
    int for_hand=1;//1 for left, 2 for right
    no_sphere_surface_pts=0;
    int no_of_sph_surf_pts=find_reachable_sphere_surface(for_hand, for_agent);
    
////double interval=grid_around_HRP2.GRID_SET->pace/1.5;
 for(int sp_ctr=0;sp_ctr<no_of_sph_surf_pts;sp_ctr++)
      {
      double t=0;
       for(;t<1;t+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t)*shoulder_pos.x+t*sphere_surface_pts[sp_ctr].x;
      double y2=(1-t)*shoulder_pos.y+t*sphere_surface_pts[sp_ctr].y;
      double z2=(1-t)*shoulder_pos.z+t*sphere_surface_pts[sp_ctr].z; 
      
      int cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  
     
      if(cell_x<0||cell_x>=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
      {
       break;
      } 
 
      int cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 
      
      if(cell_y<0||cell_y>=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
      {
       break;
      } 
 
      int cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 
 
       if(cell_z<0||cell_z>=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
      {
       break;
      } 
 
   
      if(for_actual_pitch==1&&for_actual_yaw==1)
        {
       grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_human_LHand=1;
       grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_bending=1;  
       grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_turning_around_bending=1; 

       ////object_MM.object_indx[nr_ctr].
        }
      else
        {
         if(for_actual_pitch==0&&for_actual_yaw==1)
         {
         grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_bending=1;  
         grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_turning_around_bending=1;
         }
         else
         {
         grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_turning_around_bending=1;  
         }
        }
      ////g3d_drawDisc(x2, y2, z2, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
       }
       //g3d_drawDisc(sphere_pts[sp_ctr].x, sphere_pts[sp_ctr].y, sphere_pts[sp_ctr].z, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
      }
   
    
    shoulder_pos.x = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    ////////////g3d_drawDisc(shoulder_pos.x, shoulder_pos.y, shoulder_pos.z, .1, Red, NULL);
  
    for_hand=2;//1 for left, 2 for right
    no_sphere_surface_pts=0;
    no_of_sph_surf_pts=find_reachable_sphere_surface(for_hand, for_agent);

 for(int sp_ctr=0;sp_ctr<no_of_sph_surf_pts;sp_ctr++)
      {
      double t=0;
       for(;t<1;t+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t)*shoulder_pos.x+t*sphere_surface_pts[sp_ctr].x;
      double y2=(1-t)*shoulder_pos.y+t*sphere_surface_pts[sp_ctr].y;
      double z2=(1-t)*shoulder_pos.z+t*sphere_surface_pts[sp_ctr].z; 

      int cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;

      if(cell_x<0||cell_x>=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
      {
       break;
      } 
 
     int cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 
      
      if(cell_y<0||cell_y>=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
      {
       break;
      } 
 
      int cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 
 
      if(cell_z<0||cell_z>=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
      {
       break;
      } 

        
      
      
   
       if(for_actual_pitch==1&&for_actual_yaw==1)
        {
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_human_RHand=1;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_bending=1;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_turning_around_bending=1; 
        }
      else
        {
         if(for_actual_pitch==0&&for_actual_yaw==1)
         {
         grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_bending=1; 
         grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_turning_around_bending=1; 
         }
         else
         {
         grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_turning_around_bending=1;  
         }
        }
      ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].affordances.reachable_by_human_RHand=1;   

      //g3d_drawDisc(x2, y2, z2, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
       }
      //// g3d_drawDisc(sphere_pts[sp_ctr].x, sphere_pts[sp_ctr].y, sphere_pts[sp_ctr].z, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
      }
 
 
 
 for_actual_pitch=0;
 

  }//loop for pitch

 
 for_actual_yaw=0;
 if(curr_left_turn<=maxi_left_turn)
  {
  
  curr_left_turn+=0.5;
  //////////printf(" curr_left_turn = %lf\n",curr_left_turn);
  yaw_ang+=0.5;
  }
 else
  {
  if(curr_left_turn>maxi_left_turn&&curr_right_turn<=maxi_right_turn)
   {
   
   curr_right_turn+=0.5;
    //////////printf(" curr_right_turn = %lf\n",curr_right_turn);
    if(init_yaw==1)
    {
   yaw_ang=orig_yaw_ang; 
   init_yaw=0;
    }
   yaw_ang-=0.5;
   }
  else
   {
   turn_human=0;
   }
  }
 
hum_tmp_pos[HUMANq_TORSO_PAN]=yaw_ang; 
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_PAN]=hum_tmp_pos[HUMANq_TORSO_PAN];
   
hum_tmp_pos[HUMANq_TORSO_TILT]=orig_pitch_ang; 
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_TILT]=hum_tmp_pos[HUMANq_TORSO_TILT];

////g3d_draw_allwin_active();

int kcd_with_report=0;
p3d_rob *human=ACBTSET->human[ACBTSET->actual_human]->HumanPt;

int res = p3d_col_test_robot(human,kcd_with_report);
 if(res>0)
   {
  kcd_with_report=0;
  res = p3d_col_test_self_collision(human,kcd_with_report);
  if(res>0)
    {
  //////////printf(" There is self collision with human, res=%d \n", res);
  //return 0;
    }
   else
    { 
   //////////printf(" There is collision with human, for yaw_ang = %lf and pitch_ang = %lf res=%d \n", yaw_ang, pitch_ang, res);
  turn_collision=1;
  turn_human=0;
   break;
    }
   }





 }//Loop for yaw 
 
hum_tmp_pos[HUMANq_TORSO_PAN]=orig_yaw_ang; 
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_PAN]=hum_tmp_pos[HUMANq_TORSO_PAN];
////p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);

hum_tmp_pos[HUMANq_TORSO_TILT]=orig_pitch_ang; 
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_TILT]=hum_tmp_pos[HUMANq_TORSO_TILT];
p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);

return 1;

 

}

int update_3d_grid_reachability_for_human_standing_new()
{

  
  int for_agent=1; //1 for human, 2 for HRP2

  

   //////////printf("Inside update_3d_grid_reachability_for_human_standing_new()\n");
   point_co_ordi shoulder_pos;
      ////point_co_ordi sphere_pt;
configPt hum_tmp_pos=p3d_get_robot_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt);
double yaw_ang=hum_tmp_pos[HUMANq_TORSO_PAN]; 
double orig_yaw_ang=yaw_ang;
double pitch_ang=hum_tmp_pos[HUMANq_TORSO_TILT]; 
double orig_pitch_ang=pitch_ang;

int for_actual_pitch=1;
int for_actual_yaw=1;

int turn_collision=0;
//double yaw_
////for(;yaw_ang<2.0*M_PI&&turn_collision==0;yaw_ang+=0.5)
int turn_human=1;
double maxi_left_turn=M_PI/2.0;
double maxi_right_turn=M_PI/2.0;
double curr_left_turn=0.0;
double curr_right_turn=0.0;

int init_yaw=1;

double interval=grid_around_HRP2.GRID_SET->pace-0.005;///1.5;

while(turn_human==1)
 {
////////printf(" **** yaw_ang = %lf\n",yaw_ang);

   ////pitch_ang=hum_tmp_pos[14]; 
   ////orig_pitch_ang=pitch_ang;
   int collision=0;
   
pitch_ang=orig_pitch_ang;
hum_tmp_pos[HUMANq_TORSO_TILT]=pitch_ang; // Around 5 degrees
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_TILT]=hum_tmp_pos[HUMANq_TORSO_TILT];
  
for(;pitch_ang<0.785&&collision==0;pitch_ang+=0.25)
  {
//////printf(" **** for yaw_ang = %lf, pitch_ang = %lf\n",yaw_ang, pitch_ang);

hum_tmp_pos[HUMANq_TORSO_TILT]=pitch_ang; // Around 5 degrees
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_TILT]=hum_tmp_pos[HUMANq_TORSO_TILT];

int kcd_with_report=0;
p3d_rob *human=ACBTSET->human[ACBTSET->actual_human]->HumanPt;

int res = p3d_col_test_robot(human,kcd_with_report);
 if(res>0)
   {
 //////// printf(" There is collision with human, for pitch_ang = %lf res=%d \n", pitch_ang, res);
  for_actual_pitch=0;
  collision=1;
  hum_tmp_pos[HUMANq_TORSO_TILT]=orig_pitch_ang; 
  pitch_ang=orig_pitch_ang;
  p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
  ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_TILT]=hum_tmp_pos[HUMANq_TORSO_TILT];

  break;
   }

   shoulder_pos.x = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_LSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
 
    int for_hand=1;//1 for left, 2 for right
    no_sphere_surface_pts=0;
    int no_of_sph_surf_pts=find_reachable_sphere_surface(for_hand, for_agent);
    
////double interval=grid_around_HRP2.GRID_SET->pace/1.5;
 for(int sp_ctr=0;sp_ctr<no_of_sph_surf_pts;sp_ctr++)
      {
      double t=0;
       for(;t<1;t+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t)*shoulder_pos.x+t*sphere_surface_pts[sp_ctr].x;
      double y2=(1-t)*shoulder_pos.y+t*sphere_surface_pts[sp_ctr].y;
      double z2=(1-t)*shoulder_pos.z+t*sphere_surface_pts[sp_ctr].z; 
      
      int cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  
     
      if(cell_x<0||cell_x>=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
      {
       break;
      } 
 
      int cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 
      
      if(cell_y<0||cell_y>=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
      {
       break;
      } 
 
      int cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 
 
       if(cell_z<0||cell_z>=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
      {
       break;
      } 
 
   
      if(for_actual_pitch==1&&for_actual_yaw==1)
        {
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_standing=1;
       grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_standing_bending=1;  
       grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_standing_turning_around_bending=1; 
        }
      else
        {
         if(for_actual_pitch==0&&for_actual_yaw==1)
         {
         grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_standing_bending=1;  
       grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_standing_turning_around_bending=1; 
         }
         else
         {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_standing_turning_around_bending=1; 
         }
        }
      ////g3d_drawDisc(x2, y2, z2, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
       }
       //g3d_drawDisc(sphere_pts[sp_ctr].x, sphere_pts[sp_ctr].y, sphere_pts[sp_ctr].z, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
      }
   
    
    shoulder_pos.x = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_RSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    ////////////g3d_drawDisc(shoulder_pos.x, shoulder_pos.y, shoulder_pos.z, .1, Red, NULL);
  
    for_hand=2;//1 for left, 2 for right
    no_sphere_surface_pts=0;
    no_of_sph_surf_pts=find_reachable_sphere_surface(for_hand, for_agent);

 for(int sp_ctr=0;sp_ctr<no_of_sph_surf_pts;sp_ctr++)
      {
      double t=0;
       for(;t<1;t+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t)*shoulder_pos.x+t*sphere_surface_pts[sp_ctr].x;
      double y2=(1-t)*shoulder_pos.y+t*sphere_surface_pts[sp_ctr].y;
      double z2=(1-t)*shoulder_pos.z+t*sphere_surface_pts[sp_ctr].z; 

      int cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;

      if(cell_x<0||cell_x>=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
      {
       break;
      } 
 
     int cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 
      
      if(cell_y<0||cell_y>=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
      {
       break;
      } 
 
      int cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 
 
      if(cell_z<0||cell_z>=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
      {
       break;
      } 

        
      
      
   
       if(for_actual_pitch==1&&for_actual_yaw==1)
        {
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_standing=1;
       grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_standing_bending=1;  
       grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_standing_turning_around_bending=1; 
        }
      else
        {
         if(for_actual_pitch==0&&for_actual_yaw==1)
         {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_standing_bending=1;  
       grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_standing_turning_around_bending=1; 
         }
         else
         {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_standing_turning_around_bending=1; 
         }
        }
      ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].affordances.reachable_by_human_RHand=1;   

      //g3d_drawDisc(x2, y2, z2, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
       }
      //// g3d_drawDisc(sphere_pts[sp_ctr].x, sphere_pts[sp_ctr].y, sphere_pts[sp_ctr].z, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
      }
 
 
 
 for_actual_pitch=0;
 

  }//loop for pitch

 
 for_actual_yaw=0;
 if(curr_left_turn<=maxi_left_turn)
  {
  
  curr_left_turn+=0.5;
  ////////printf(" curr_left_turn = %lf\n",curr_left_turn);
  yaw_ang+=0.5;
  }
 else
  {
  if(curr_left_turn>maxi_left_turn&&curr_right_turn<=maxi_right_turn)
   {
   
   curr_right_turn+=0.5;
    //////printf(" curr_right_turn = %lf\n",curr_right_turn);
    if(init_yaw==1)
    {
   yaw_ang=orig_yaw_ang; 
   init_yaw=0;
    }
   yaw_ang-=0.5;
   }
  else
   {
   turn_human=0;
   }
  }
 
hum_tmp_pos[HUMANq_TORSO_PAN]=yaw_ang; 
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_PAN]=hum_tmp_pos[HUMANq_TORSO_PAN];
   
hum_tmp_pos[HUMANq_TORSO_TILT]=orig_pitch_ang; 
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_TILT]=hum_tmp_pos[HUMANq_TORSO_TILT];

int kcd_with_report=0;
p3d_rob *human=ACBTSET->human[ACBTSET->actual_human]->HumanPt;

int res = p3d_col_test_robot(human,kcd_with_report);
 if(res>0)
   {
  //////printf(" There is collision with human, for yaw_ang = %lf and pitch_ang = %lf res=%d \n", yaw_ang, pitch_ang, res);
  turn_collision=1;
  turn_human=0;
   break;
   }





 }//Loop for yaw 
 
hum_tmp_pos[HUMANq_TORSO_PAN]=orig_yaw_ang; 
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_PAN]=hum_tmp_pos[HUMANq_TORSO_PAN];
////p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);

hum_tmp_pos[HUMANq_TORSO_TILT]=orig_pitch_ang; 
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TORSO_TILT]=hum_tmp_pos[HUMANq_TORSO_TILT];
p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_tmp_pos);

return 1;

 

}


int update_3d_grid_reachability_for_human()
{
  configPt rob_cur_pos = MY_ALLOC(double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof); /* Allocation of temporary robot configuration */
  p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&rob_cur_pos); 
 
  //////////printf("Inside update_3D_grid_based_on_current_human_pos()\n");
 int x=0;
 for(x=0;x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx;x++)
 {
 int y=0;
 for(y=0;y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny;y++)
  {
  int z=0;
  for(z=0;z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz;z++)
   {
    ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].val<0)
    ////{
      //// grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.visible_by_human=0;

       double cell_x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double cell_y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double cell_z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
       point_co_ordi point;
        point.x=cell_x_world;
        point.y=cell_y_world;
        point.z=cell_z_world;
    if(fabs(rob_cur_pos[6]-point.x)>1.5||fabs(rob_cur_pos[7]-point.y)>1.5||fabs(rob_cur_pos[8]-point.z)>3.0||point.z<0.3)   // Surely not reachable by HRP2 from its current position
    {
  
    }
   else
    {
   /////p3d_set_and_update_this_robot_conf(ACBTSET->robot,rob_cur_pos); // Restore the robot's initial position before calculating the reachability for next point; 
   //////int state=1; //AKP : 1 is sitting, 2 is standing 
   //////create_HRP2_robot(state);//Everytime restore the original position of the robot
   //vectorN backupConfig = attStandingRobot->robot()->currentConfiguration();
   
   ////int is_reachable_directly=is_point_on_surface_directly_reachable(point, 2); // 2 means test for HRP2
    int is_reachable_directly=is_point_directly_reachable_by_human(point);
    
    if(is_reachable_directly==3)
    {
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand=1;    
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand=1;    
    } 
    else
    {
     if(is_reachable_directly==2)
     {
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand=1;    
   
     }
     else
     {
      if(is_reachable_directly==1)
      {
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand=1;    
   
      }
      else
      {
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand=-1;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand=-1;
      } 
     }
     
    } 
   }//End else
  
   }
  } 
 }

  

 MY_FREE(rob_cur_pos, double,ACBTSET->robot->nb_dof);
return 1;

}



int find_reachable_sphere_surface(int for_hand, int for_agent)
{

   point_co_ordi shoulder_pos;
      ////point_co_ordi sphere_pt;
   no_sphere_surface_pts=0;
   
    ////int for_hand=1;//1 for left, 2 for right
    int shoulder_indx; 
    if(for_agent==1)//for human
    {
    if(for_hand==1)//1 for left, 2 for right
     {
   shoulder_indx=HUMANj_LSHOULDER;
     }
    else
     {
     if(for_hand==2)
      {
     shoulder_indx=HUMANj_RSHOULDER;
      }
     }
    }
    else
    {
    if(for_agent==2)//for HRP2
     {
    if(for_hand==1)//1 for left, 2 for right
      {
   shoulder_indx=ROBOTj_LSHOULDER;
      }
    else
      {
     if(for_hand==2)
       {
     shoulder_indx=ROBOTj_RSHOULDER;
       }
      }
     }
     else
     {
       if(for_agent==3)//for JIDO
      {
   
     shoulder_indx=ROBOTj_RSHOULDER;
   
      }
     }  
    }

    double r=0;    
    double curr_yaw=0;

    double shoulder_back_limit;// The maxi possible angle away from the front axis of the sgent
    double shoulder_front_limit;// The maxi possible angle crossing the front axis of the agent

    if(for_agent==1)//For human
    {
    shoulder_pos.x = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[shoulder_indx]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[shoulder_indx]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[shoulder_indx]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    r=0.7;//Maximum reach boundary for human
    curr_yaw=ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_RZ];   

    shoulder_back_limit=M_PI/2.0+M_PI/6.0;// The maxi possible angle away from the front axis of human
    shoulder_front_limit=M_PI/3.0;// The maxi possible angle crossing the front axis of human

    }
    else
    {
     if(for_agent==2)//for HRP2
     {
    shoulder_pos.x = ACBTSET->robot->joints[shoulder_indx]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->robot->joints[shoulder_indx]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->robot->joints[shoulder_indx]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
     r=0.7;//Maximum reach boundary for HRP2
     curr_yaw=ACBTSET->robot->ROBOT_POS[11];   

    shoulder_back_limit=M_PI/2.0;// The maxi possible angle away from the front axis of HRP2
    shoulder_front_limit=M_PI/3.0;// The maxi possible angle crossing the front axis of HRP2

     }
     else
     {
      if(for_agent==3)//for JIDO
      {
    shoulder_pos.x = ACBTSET->robot->joints[shoulder_indx]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->robot->joints[shoulder_indx]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->robot->joints[shoulder_indx]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
     r=1.25;//Maximum reach boundary for JIDO
     curr_yaw=ACBTSET->robot->ROBOT_POS[11];   

    shoulder_back_limit=5.0*M_PI/6.0;// The maxi possible angle away from the front axis of JIDO
    shoulder_front_limit=4.0*M_PI/6.0;// The maxi possible angle crossing the front axis of JIDO

    for_hand=2;//Just to comply with rest of the code
      }
     }
    }

    
    double theta, phi;
    
    ////printf("curr_yaw=%lf\n",curr_yaw);
    double phi_st, phi_end;
    //phi_st=phi;
    
    
    if(for_hand==2)//For right hand
    {
    phi_st=curr_yaw-shoulder_back_limit;
    phi_end=curr_yaw+shoulder_front_limit;
    }
    if(for_hand==1)
    {
    phi_st=curr_yaw-shoulder_front_limit;
    phi_end= (curr_yaw)+(shoulder_back_limit);
    
    }
    /*
     if(phi_st>2.0*M_PI)
     {
     phi_st=phi_st-2.0*M_PI;
     }*/
   
    double increment=grid_around_HRP2.GRID_SET->pace/r;  
    ////////printf("phi_st=%lf, phi_end=%lf, increment=%lf\n",phi_st,phi_end, increment);
     //// phi=hum_yaw;
     
     
      for(phi=phi_st;phi<=phi_end;phi+=increment)
      { 
       for(theta=0.0;theta<=M_PI;theta+=increment)
       {
    sphere_surface_pts[no_sphere_surface_pts].x=shoulder_pos.x+r*sin(theta)*cos(phi);
    sphere_surface_pts[no_sphere_surface_pts].y=shoulder_pos.y+r*sin(theta)*sin(phi);
    sphere_surface_pts[no_sphere_surface_pts].z=shoulder_pos.z+r*cos(theta);
   //////// printf("no_sphere_surface_pts =%d\n",no_sphere_surface_pts);
    no_sphere_surface_pts++;
    
       }
      } 
////////printf("no_sphere_surface_pts =%d\n",no_sphere_surface_pts);
  return no_sphere_surface_pts;
}

int update_3d_grid_reachability_for_JIDO_new()
{

  
  int for_agent=3; //1 for human, 2 for HRP2, 3 for Jido
  
  

   //////////printf("Inside update_3d_grid_reachability_for_JIDO_new()\n");
   point_co_ordi shoulder_pos;
      ////point_co_ordi sphere_pt;
      
    // Reachability Test by left hand
    shoulder_pos.x = ACBTSET->robot->joints[ROBOTj_RSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->robot->joints[ROBOTj_RSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->robot->joints[ROBOTj_RSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    ////g3d_drawDisc(shoulder_pos.x, shoulder_pos.y, shoulder_pos.z, .1, Green, NULL);
 
    int for_hand=2;//Although JIDO has only one arm, but just to comply with find_reachable_sphere_surface() function, we should always pass 2
    no_sphere_surface_pts=0;
    int no_of_sph_surf_pts=find_reachable_sphere_surface(for_hand, for_agent);
    
double interval=grid_around_HRP2.GRID_SET->pace/1.5;
 for(int sp_ctr=0;sp_ctr<no_of_sph_surf_pts;sp_ctr++)
      {
       
      double t=0;
       for(;t<1;t+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2;
      double y2;
      double z2;
      int cell_x;
      int cell_y;
      int cell_z;
 
      x2=(1-t)*shoulder_pos.x+t*sphere_surface_pts[sp_ctr].x;
      cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  

      if(cell_x>=0&&cell_x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
        {
      y2=(1-t)*shoulder_pos.y+t*sphere_surface_pts[sp_ctr].y;
      cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 

      if(cell_y>=0&&cell_y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
         { 
        z2=(1-t)*shoulder_pos.z+t*sphere_surface_pts[sp_ctr].z; 
        cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 

        if(cell_z>=0&&cell_z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
          {
        ////printf(" Cell (%d, %d, %d) is reachabke by JIDO\n", cell_x,cell_y,cell_z);
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_JIDO_Hand=1;   
          }
         }
      ////g3d_drawDisc(x2, y2, z2, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
        }
       }

       //g3d_drawDisc(sphere_pts[sp_ctr].x, sphere_pts[sp_ctr].y, sphere_pts[sp_ctr].z, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
     }
   
    
 
return 1;

 

}


int update_3d_grid_reachability_for_HRP2_new()
{

  
  int for_agent=2; //1 for human, 2 for HRP2

  

   //////////printf("Inside update_3d_grid_reachability_for_HRP2_new()\n");
   point_co_ordi shoulder_pos;
      ////point_co_ordi sphere_pt;
      
    // Reachability Test by left hand
    shoulder_pos.x = ACBTSET->robot->joints[ROBOTj_LSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->robot->joints[ROBOTj_LSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->robot->joints[ROBOTj_LSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    ////g3d_drawDisc(shoulder_pos.x, shoulder_pos.y, shoulder_pos.z, .1, Green, NULL);
 
    int for_hand=1;//1 for left, 2 for right
    no_sphere_surface_pts=0;
    int no_of_sph_surf_pts=find_reachable_sphere_surface(for_hand, for_agent);
    
double interval=grid_around_HRP2.GRID_SET->pace/1.5;
 for(int sp_ctr=0;sp_ctr<no_of_sph_surf_pts;sp_ctr++)
      {
      double t=0;
       for(;t<1;t+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t)*shoulder_pos.x+t*sphere_surface_pts[sp_ctr].x;
      double y2=(1-t)*shoulder_pos.y+t*sphere_surface_pts[sp_ctr].y;
      double z2=(1-t)*shoulder_pos.z+t*sphere_surface_pts[sp_ctr].z; 
      int cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  
      int cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 
      int cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 
   
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_HRP2_LHand=1;   
      ////g3d_drawDisc(x2, y2, z2, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
       }
       //g3d_drawDisc(sphere_pts[sp_ctr].x, sphere_pts[sp_ctr].y, sphere_pts[sp_ctr].z, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
      }
   
    
    // Reachability Test by right hand
    shoulder_pos.x = ACBTSET->robot->joints[ROBOTj_RSHOULDER]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.y = ACBTSET->robot->joints[ROBOTj_RSHOULDER]->abs_pos[1][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    shoulder_pos.z = ACBTSET->robot->joints[ROBOTj_RSHOULDER]->abs_pos[2][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    ////g3d_drawDisc(shoulder_pos.x, shoulder_pos.y, shoulder_pos.z, .1, Green, NULL);
    for_hand=2;//1 for left, 2 for right
    no_sphere_surface_pts=0;
    no_of_sph_surf_pts=find_reachable_sphere_surface(for_hand, for_agent);

 for(int sp_ctr=0;sp_ctr<no_of_sph_surf_pts;sp_ctr++)
      {
      double t=0;
       for(;t<1;t+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t)*shoulder_pos.x+t*sphere_surface_pts[sp_ctr].x;
      double y2=(1-t)*shoulder_pos.y+t*sphere_surface_pts[sp_ctr].y;
      double z2=(1-t)*shoulder_pos.z+t*sphere_surface_pts[sp_ctr].z; 
      int cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  
      int cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 
      int cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 
   
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_HRP2_RHand=1;   

      //g3d_drawDisc(x2, y2, z2, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
       }
      //// g3d_drawDisc(sphere_pts[sp_ctr].x, sphere_pts[sp_ctr].y, sphere_pts[sp_ctr].z, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
      }
 
return 1;

 

}


int update_3d_grid_reachability_for_HRP2()
{
  configPt rob_cur_pos = MY_ALLOC(double,ACBTSET->robot->nb_dof); /* Allocation of temporary robot configuration */
  p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos); 
 
  //////////printf("Inside update_3D_grid_based_on_current_human_pos()\n");
 int x=0;
 for(x=0;x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx;x++)
 {
 int y=0;
 for(y=0;y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny;y++)
  {
  int z=0;
  for(z=0;z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz;z++)
   {
    ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].val<0)
    ////{
      //// grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.visible_by_human=0;

       double cell_x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double cell_y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double cell_z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
       point_co_ordi point;
        point.x=cell_x_world;
        point.y=cell_y_world;
        point.z=cell_z_world;
    if(fabs(rob_cur_pos[6]-point.x)>1.5||fabs(rob_cur_pos[7]-point.y)>1.5||fabs(rob_cur_pos[8]-point.z)>3.0||point.z<0.3)   // Surely not reachable by HRP2 from its current position
    {
  
    }
   else
    {
   /////p3d_set_and_update_this_robot_conf(ACBTSET->robot,rob_cur_pos); // Restore the robot's initial position before calculating the reachability for next point; 
   //////int state=1; //AKP : 1 is sitting, 2 is standing 
   //////create_HRP2_robot(state);//Everytime restore the original position of the robot
   //vectorN backupConfig = attStandingRobot->robot()->currentConfiguration();
   
   ////int is_reachable_directly=is_point_on_surface_directly_reachable(point, 2); // 2 means test for HRP2
    int is_reachable_directly=is_point_directly_reachable_by_HRP2(point);
   
    if(is_reachable_directly==3)
    {
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand=1;    
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand=1;    
    } 
    else
    {
     if(is_reachable_directly==2)
     {
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand=1;    
   
     }
     else
     {
      if(is_reachable_directly==1)
      {
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand=1;    
   
      }
      else
      {
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand=-1;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand=-1;
      } 
     }
     
    } 
   }//End else
  
   }
  } 
 }

 MY_FREE(rob_cur_pos, double,ACBTSET->robot->nb_dof);
return 1;

}



int update_3d_grid_visibility(int type)//1 means human, 2 means HRP2, 3 means JIDO
{

 envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
  
  int nr = envPt->nr;

//////////printf(" Inside update_3d_grid_visibility(),  no_FOV_end_point_vertices=%d\n",no_FOV_end_point_vertices);
point_co_ordi eye_pos;
   
eye_pos.x=(FOV_end_point_vertices[0][0].x+FOV_end_point_vertices[0][1].x+FOV_end_point_vertices[0][2].x+FOV_end_point_vertices[0][3].x)/4.0;//Average of the 4 vertices of the near screen
eye_pos.y=(FOV_end_point_vertices[0][0].y+FOV_end_point_vertices[0][1].y+FOV_end_point_vertices[0][2].y+FOV_end_point_vertices[0][3].y)/4.0;//Average of the 4 vertices of the near screen
eye_pos.z=(FOV_end_point_vertices[0][0].z+FOV_end_point_vertices[0][1].z+FOV_end_point_vertices[0][2].z+FOV_end_point_vertices[0][3].z)/4.0;//Average of the 4 vertices of the near screen

    
    configPt hum_head_pos  = p3d_get_robot_config(ACBTSET->visball);  // We need to use a visball as the robot for finding the collosion free straight line path otherwise if we will use human it might return collision
    hum_head_pos[6] = eye_pos.x;//hum_cur_pos[6];
    hum_head_pos[7] = eye_pos.y;//hum_cur_pos[7];
    hum_head_pos[8] = eye_pos.z;
 
    agent_eye_pos.x=eye_pos.x;       
    agent_eye_pos.y=eye_pos.y;
    agent_eye_pos.z=eye_pos.z;

    configPt point_to_look = p3d_get_robot_config(ACBTSET->visball);  
    

 double interval=grid_around_HRP2.GRID_SET->pace/2.0;
   //////////printf("interval=%lf, no_FOV_end_point_vertices=%d\n",interval,no_FOV_end_point_vertices);
 
int visible_ctr=0;  
int not_visible_ctr=0;
 int j=0;
    for(j=0;j<no_FOV_end_point_vertices;j++)
    {

    int i=4;
    for(i=4;i<8;i++)
     {
   //// if(i<4)
    //// {
    ////g3d_drawDisc(FOV_end_point_vertices[j][i].x,FOV_end_point_vertices[j][i].y, FOV_end_point_vertices[j][i].z, grid_around_HRP2.GRID_SET->pace/4.0, 3, NULL);
    //// }  
    ////else
    // {
    ////g3d_drawDisc(FOV_end_point_vertices[j][i].x,FOV_end_point_vertices[j][i].y, FOV_end_point_vertices[j][i].z, grid_around_HRP2.GRID_SET->pace/4.0, 2, NULL);
 
  //// printf(" j=%d, i=%d\n",j,i);
 double t=0;

      for(;t<1;t+=interval)
      {
      
      double x;
      double y;
      double z;
      
      if(i<7)
        {
      x=(1-t)*FOV_end_point_vertices[j][i].x+t*FOV_end_point_vertices[j][i+1].x;
      y=(1-t)*FOV_end_point_vertices[j][i].y+t*FOV_end_point_vertices[j][i+1].y;
      z=(1-t)*FOV_end_point_vertices[j][i].z+t*FOV_end_point_vertices[j][i+1].z;
        } 
      else
        {
      x=(1-t)*FOV_end_point_vertices[j][i].x+t*FOV_end_point_vertices[j][4].x;
      y=(1-t)*FOV_end_point_vertices[j][i].y+t*FOV_end_point_vertices[j][4].y;
      z=(1-t)*FOV_end_point_vertices[j][i].z+t*FOV_end_point_vertices[j][4].z;
        }
       ////g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
      ////printf(" x,y,z=(%lf,%lf,%lf)\n",x,y,z);
      int obs_found=0;
      int first_obs_cell=0;
      double t2=0.1;//Just to avoid very checking very near to human
      
      for(;t2<1;t2+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t2)*eye_pos.x+t2*x;
      double y2=(1-t2)*eye_pos.y+t2*y;
      double z2=(1-t2)*eye_pos.z+t2*z; 
      
  /*  double x2=(1-t2)*x+t*eye_pos.x;
      double y2=(1-t2)*y+t*eye_pos.y;
      double z2=(1-t2)*z+t*eye_pos.z; 
  */
       //////////hri_bitmap_cell* curr_cell=NULL;
  
       ////curr_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[BT_AFFORDANCE_VISIBILITY], x2, y2, z2);

       //////////curr_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP], x2, y2, z2);
  
       int cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  
       int cell_y;
       int cell_z;

       int cell_valid=0;       

       if(cell_x>=0&&cell_x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
       {
      
 
      cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 
      
      if(cell_y>=0&&cell_y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
       {
       cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 
 
       if(cell_z>=0&&cell_z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
        {
       ////curr_cell=
       ////curr_cell->x=cell_x;
       ////curr_cell->y=cell_y;
       ////curr_cell->z=cell_z;
       cell_valid=1;
        } 
       }
      }

      ////printf(" curr_cell (%d, %d, %d)\n",cell_x,cell_y,cell_z);

       ////////if(curr_cell!=NULL)
        if(cell_valid==1)
        { 
        /*
        if(type==1)//means for human
         {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human=0;
         }
        else
         {
        if(type==2)//means for HRP2
          {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2=0;
          }
        else
          {
           if(type==3)//means for JIDO
           {
           grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_JIDO=0;
           } 
          }
         }
         */

        int is_visible=1; 
        ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human=0;

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].val==-1)//Exact obstacle cells
         {
       ////g3d_drawDisc(x2,y2,z2,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
       ////printf(" point (%lf,%lf,%lf) of which the cell (%d,%d,%d) is not empty and the cell value is %lf \n",x2,y2,z2,curr_cell->x,curr_cell->y,curr_cell->z,grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].val);
        ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].affordances.visible_by_human=1;
          ////////is_visible=0;
 

          //******AKP NOTE :Check for more expensive test, because we are not using directed BB so many cell unnecessary becomes obstacle cell
          /*
         if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].is_horizontal_surface==1)
          {

          point_to_look[6] = x2;
          point_to_look[7] = y2;
          point_to_look[8] = z2;
          
          //Using perspective taking function
          if(type==1) //for human
          {
          p3d_rob * currRob=ACBTSET->human[ACBTSET->actual_human]->HumanPt;
          p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
          is_visible=psp_is_object_visible(currRob, ACBTSET->visball, 50);
          }
          else
          {
           if(type==2) //for HRP2
           {
          p3d_rob * currRob=ACBTSET->robot;
          p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
          is_visible=psp_is_object_visible(currRob, ACBTSET->visball, 50);
           } 
          }
          
          //Using collison test by local path method  
          //is_visible=is_point_visible_by_st_line(hum_head_pos, point_to_look, type); 
          
          if(is_visible==1)
          {  
          }
          else
          {
          is_visible=0;
          ////t2=1000;// Break the ray beyond this, because the first obstacle has been found, so human can't see beyond this point.
          }
          }
          //****** END Check for more expensive test, because we are not using directed BB so many cell unnecessary becomes obstacle cell
          else*/
          ////////{
          is_visible=0;
          obs_found=1;
          if(first_obs_cell==0)
           {
          first_obs_cell=1;
           ////This is the first obstacle cell found in the ray
           if(type==1)//means for human
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human=1;
            } 

            if(type==2)//means for HRP2
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2=1;
            } 

             if(type==3)//means for JIDO
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO=1;
            } 
           }

          //printf(" Not Visible ");
          not_visible_ctr++;
          ////////////t2=1000;// Break the ray beyond this, because the first obstacle has been found, so human can't see beyond this point.
          ////////}

           
         /*
         //, so assume that the object corresponding to this obstacle is visible
          
          int i=0;
          for(i=0;i<nr;i++)
          {
           
       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.belongs_to_objects_indx[i]==1)
           {
            object_MM.object[i].visible_by_human=1;
           }
          }
          */

         }
        //else
         if(obs_found==0&&is_visible==1)
         {
         if(type==1)//means for human
          {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human=1;//NOTE :In EARLIER version affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
          }
        else
          {
        if(type==2)//means for HRP2
           {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2=1;//NOTE : In EARLIER version  affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
           }
           else
           {
           if(type==3)//means for JIDO
            {
           grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_JIDO=1;
            } 
           }
          } 
         ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].affordances.visible_by_human=1; //NOTE : In EARLIER version  affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
         visible_ctr++;
         //g3d_drawDisc(x2,y2,z2,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
         } 
        }
       }
      }
     //}

     }
    }
  
    p3d_destroy_config(ACBTSET->visball, hum_head_pos);
    p3d_destroy_config(ACBTSET->visball, point_to_look);   
    //////////printf(" Visible_ctr=%d, not_visible_ctr=%d\n",visible_ctr,not_visible_ctr); 
return 1;
}

int update_3d_grid_straight_visibility(int type)//1 means human, 2 means HRP2, 3 means JIDO
{
//////////printf(" Inside update_3d_grid_visibility(),  no_FOV_end_point_vertices=%d\n",no_FOV_end_point_vertices);
point_co_ordi eye_pos;
   
eye_pos.x=(FOV_end_point_vertices[0][0].x+FOV_end_point_vertices[0][1].x+FOV_end_point_vertices[0][2].x+FOV_end_point_vertices[0][3].x)/4.0;//Average of the 4 vertices of the near screen
eye_pos.y=(FOV_end_point_vertices[0][0].y+FOV_end_point_vertices[0][1].y+FOV_end_point_vertices[0][2].y+FOV_end_point_vertices[0][3].y)/4.0;//Average of the 4 vertices of the near screen
eye_pos.z=(FOV_end_point_vertices[0][0].z+FOV_end_point_vertices[0][1].z+FOV_end_point_vertices[0][2].z+FOV_end_point_vertices[0][3].z)/4.0;//Average of the 4 vertices of the near screen

    
    configPt hum_head_pos  = p3d_get_robot_config(ACBTSET->visball);  // We need to use a visball as the robot for finding the collosion free straight line path otherwise if we will use human it might return collision
    hum_head_pos[6] = eye_pos.x;//hum_cur_pos[6];
    hum_head_pos[7] = eye_pos.y;//hum_cur_pos[7];
    hum_head_pos[8] = eye_pos.z;
 
    agent_eye_pos.x=eye_pos.x;       
    agent_eye_pos.y=eye_pos.y;
    agent_eye_pos.z=eye_pos.z;

    configPt point_to_look = p3d_get_robot_config(ACBTSET->visball);  
    

 double interval=grid_around_HRP2.GRID_SET->pace/2.0;
   //////////printf("interval=%lf, no_FOV_end_point_vertices=%d\n",interval,no_FOV_end_point_vertices);
 
int visible_ctr=0;  
int not_visible_ctr=0;
 int j=0;
    for(j=0;j<no_FOV_end_point_vertices;j++)
    {

    int i=4;
    for(i=4;i<8;i++)
     {
   //// if(i<4)
    //// {
    ////g3d_drawDisc(FOV_end_point_vertices[j][i].x,FOV_end_point_vertices[j][i].y, FOV_end_point_vertices[j][i].z, grid_around_HRP2.GRID_SET->pace/4.0, 3, NULL);
    //// }  
    ////else
    // {
    ////g3d_drawDisc(FOV_end_point_vertices[j][i].x,FOV_end_point_vertices[j][i].y, FOV_end_point_vertices[j][i].z, grid_around_HRP2.GRID_SET->pace/4.0, 2, NULL);
 
  //// printf(" j=%d, i=%d\n",j,i);
 double t=0;

      for(;t<1;t+=interval)
      {
      
      double x;
      double y;
      double z;
      
      if(i<7)
        {
      x=(1-t)*FOV_end_point_vertices[j][i].x+t*FOV_end_point_vertices[j][i+1].x;
      y=(1-t)*FOV_end_point_vertices[j][i].y+t*FOV_end_point_vertices[j][i+1].y;
      z=(1-t)*FOV_end_point_vertices[j][i].z+t*FOV_end_point_vertices[j][i+1].z;
        } 
      else
        {
      x=(1-t)*FOV_end_point_vertices[j][i].x+t*FOV_end_point_vertices[j][4].x;
      y=(1-t)*FOV_end_point_vertices[j][i].y+t*FOV_end_point_vertices[j][4].y;
      z=(1-t)*FOV_end_point_vertices[j][i].z+t*FOV_end_point_vertices[j][4].z;
        }
       ////g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
      ////printf(" x,y,z=(%lf,%lf,%lf)\n",x,y,z);
      int obs_found=0;
      int first_obs_cell=0;
      double t2=0.1;//Just to avoid very checking very near to human
      
      for(;t2<1;t2+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t2)*eye_pos.x+t2*x;
      double y2=(1-t2)*eye_pos.y+t2*y;
      double z2=(1-t2)*eye_pos.z+t2*z; 
      
  /*  double x2=(1-t2)*x+t*eye_pos.x;
      double y2=(1-t2)*y+t*eye_pos.y;
      double z2=(1-t2)*z+t*eye_pos.z; 
  */
       //////////hri_bitmap_cell* curr_cell=NULL;
  
       ////curr_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[BT_AFFORDANCE_VISIBILITY], x2, y2, z2);

       //////////curr_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP], x2, y2, z2);
  
       int cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  
       int cell_y;
       int cell_z;

       int cell_valid=0;       

       if(cell_x>=0&&cell_x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
       {
      
 
      cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 
      
      if(cell_y>=0&&cell_y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
       {
       cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 
 
       if(cell_z>=0&&cell_z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
        {
       ////curr_cell=
       ////curr_cell->x=cell_x;
       ////curr_cell->y=cell_y;
       ////curr_cell->z=cell_z;
       cell_valid=1;
        } 
       }
      }

      ////printf(" curr_cell (%d, %d, %d)\n",cell_x,cell_y,cell_z);

       ////////if(curr_cell!=NULL)
        if(cell_valid==1)
        { 
        /*
        if(type==1)//means for human
         {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_straight_head_orientation=0;
         }
        else
         {
        if(type==2)//means for HRP2
          {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2_straight_head_orientation=0;
          }
        else
          {
           if(type==3)//means for JIDO
           {
           grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_JIDO_straight_head_orientation=0;
           } 
          }
         }
         */

        int is_visible=1; 
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].val==-1)//Exact obstacle cells
         {
       ////g3d_drawDisc(x2,y2,z2,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
       ////printf(" point (%lf,%lf,%lf) of which the cell (%d,%d,%d) is not empty and the cell value is %lf \n",x2,y2,z2,curr_cell->x,curr_cell->y,curr_cell->z,grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].val);
        ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].affordances.visible_by_human=1;
          ////////is_visible=0;
 

          //******AKP NOTE :Check for more expensive test, because we are not using directed BB so many cell unnecessary becomes obstacle cell
          /*
         if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].is_horizontal_surface==1)
          {

          point_to_look[6] = x2;
          point_to_look[7] = y2;
          point_to_look[8] = z2;
          
          //Using perspective taking function
          if(type==1) //for human
          {
          p3d_rob * currRob=ACBTSET->human[ACBTSET->actual_human]->HumanPt;
          p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
          is_visible=psp_is_object_visible(currRob, ACBTSET->visball, 50);
          }
          else
          {
           if(type==2) //for HRP2
           {
          p3d_rob * currRob=ACBTSET->robot;
          p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
          is_visible=psp_is_object_visible(currRob, ACBTSET->visball, 50);
           } 
          }
          
          //Using collison test by local path method  
          //is_visible=is_point_visible_by_st_line(hum_head_pos, point_to_look, type); 
          
          if(is_visible==1)
          {  
          }
          else
          {
          is_visible=0;
          ////t2=1000;// Break the ray beyond this, because the first obstacle has been found, so human can't see beyond this point.
          }
          }
          //****** END Check for more expensive test, because we are not using directed BB so many cell unnecessary becomes obstacle cell
          else*/
          ////////{
          is_visible=0;
          obs_found=1;
          //printf(" Not Visible ");
 
          if(first_obs_cell==0)
           {
          first_obs_cell=1;
           ////This is the first obstacle cell found in the ray
           if(type==1)//means for human
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human_straight_head_orientation=1;
            } 

            if(type==2)//means for HRP2
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2_straight_head_orientation=1;
            } 

             if(type==3)//means for JIDO
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO_straight_head_orientation=1;
            } 
           }

          not_visible_ctr++;
          ////////////t2=1000;// Break the ray beyond this, because the first obstacle has been found, so human can't see beyond this point.
          ////////}
         }
        //else
         if(obs_found==0&&is_visible==1)
         {
         if(type==1)//means for human
          {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_straight_head_orientation=1;//NOTE :In EARLIER version affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
          }
        else
          {
        if(type==2)//means for HRP2
           {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2_straight_head_orientation=1;//NOTE : In EARLIER version  affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
           }
           else
           {
           if(type==3)//means for JIDO
            {
           grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_JIDO_straight_head_orientation=1;
            } 
           }
          } 
         ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].affordances.visible_by_human=1; //NOTE : In EARLIER version  affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
         visible_ctr++;
         //g3d_drawDisc(x2,y2,z2,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
         } 
        }
       }
      }
     //}

     }
    }
  
    p3d_destroy_config(ACBTSET->visball, hum_head_pos);
    p3d_destroy_config(ACBTSET->visball, point_to_look);   
    //////////printf(" Visible_ctr=%d, not_visible_ctr=%d\n",visible_ctr,not_visible_ctr); 
 return 1;
}

int update_3d_grid_visibility_standing(int type)//1 means human, 2 means HRP2
{
//////////printf(" Inside update_3d_grid_visibility(),  no_FOV_end_point_vertices=%d\n",no_FOV_end_point_vertices);
point_co_ordi eye_pos;
   
eye_pos.x=(FOV_end_point_vertices[0][0].x+FOV_end_point_vertices[0][1].x+FOV_end_point_vertices[0][2].x+FOV_end_point_vertices[0][3].x)/4.0;//Average of the 4 vertices of the near screen
eye_pos.y=(FOV_end_point_vertices[0][0].y+FOV_end_point_vertices[0][1].y+FOV_end_point_vertices[0][2].y+FOV_end_point_vertices[0][3].y)/4.0;//Average of the 4 vertices of the near screen
eye_pos.z=(FOV_end_point_vertices[0][0].z+FOV_end_point_vertices[0][1].z+FOV_end_point_vertices[0][2].z+FOV_end_point_vertices[0][3].z)/4.0;//Average of the 4 vertices of the near screen

    
    configPt hum_head_pos  = p3d_get_robot_config(ACBTSET->visball);  // We need to use a visball as the robot for finding the collosion free straight line path otherwise if we will use human it might return collision
    hum_head_pos[6] = eye_pos.x;//hum_cur_pos[6];
    hum_head_pos[7] = eye_pos.y;//hum_cur_pos[7];
    hum_head_pos[8] = eye_pos.z;
    
    configPt point_to_look = p3d_get_robot_config(ACBTSET->visball);  
    

 double interval=grid_around_HRP2.GRID_SET->pace/2.0;
   //////////printf("interval=%lf\n",interval);
 
int visible_ctr=0;  
 int j=0;
    for(j=0;j<no_FOV_end_point_vertices;j++)
    {

    int i=4;
    for(i=4;i<8;i++)
     {
   //// if(i<4)
    //// {
    ////g3d_drawDisc(FOV_end_point_vertices[j][i].x,FOV_end_point_vertices[j][i].y, FOV_end_point_vertices[j][i].z, grid_around_HRP2.GRID_SET->pace/4.0, 3, NULL);
    //// }  
    ////else
    // {
    ////g3d_drawDisc(FOV_end_point_vertices[j][i].x,FOV_end_point_vertices[j][i].y, FOV_end_point_vertices[j][i].z, grid_around_HRP2.GRID_SET->pace/4.0, 2, NULL);
 
  //// printf(" j=%d, i=%d\n",j,i);
 double t=0;

      for(;t<1;t+=interval)
      {
      
      double x;
      double y;
      double z;
      
      if(i<7)
        {
      x=(1-t)*FOV_end_point_vertices[j][i].x+t*FOV_end_point_vertices[j][i+1].x;
      y=(1-t)*FOV_end_point_vertices[j][i].y+t*FOV_end_point_vertices[j][i+1].y;
      z=(1-t)*FOV_end_point_vertices[j][i].z+t*FOV_end_point_vertices[j][i+1].z;
        } 
      else
        {
      x=(1-t)*FOV_end_point_vertices[j][i].x+t*FOV_end_point_vertices[j][4].x;
      y=(1-t)*FOV_end_point_vertices[j][i].y+t*FOV_end_point_vertices[j][4].y;
      z=(1-t)*FOV_end_point_vertices[j][i].z+t*FOV_end_point_vertices[j][4].z;
        }
       ////g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
      ////printf(" x,y,z=(%lf,%lf,%lf)\n",x,y,z);
      int obs_found=0;
      int first_obs_cell=0;

      double t2=0.1;//Just to avoid very checking very near to human
      for(;t2<1;t2+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t2)*eye_pos.x+t2*x;
      double y2=(1-t2)*eye_pos.y+t2*y;
      double z2=(1-t2)*eye_pos.z+t2*z; 
      
  /*  double x2=(1-t2)*x+t*eye_pos.x;
      double y2=(1-t2)*y+t*eye_pos.y;
      double z2=(1-t2)*z+t*eye_pos.z; 
  */
       //////////hri_bitmap_cell* curr_cell=NULL;
  
       ////curr_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[BT_AFFORDANCE_VISIBILITY], x2, y2, z2);

       //////////curr_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP], x2, y2, z2);
  
       int cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  
       int cell_y;
       int cell_z;

       int cell_valid=0;       

       if(cell_x>=0&&cell_x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
       {
      
 
      cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 
      
      if(cell_y>=0&&cell_y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
       {
       cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 
 
       if(cell_z>=0&&cell_z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
        {
       ////curr_cell=
       ////curr_cell->x=cell_x;
       ////curr_cell->y=cell_y;
       ////curr_cell->z=cell_z;
      cell_valid=1;
        } 
       }
      }

      ////printf(" curr_cell (%d, %d, %d)\n",cell_x,cell_y,cell_z);

       ////////if(curr_cell!=NULL)
        if(cell_valid==1)
        { 
        /*
        if(type==1)//means for human
         {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_standing_human=0;
         }
        else
         {
        if(type==2)//means for HRP2
          {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_standing_HRP2=0;
          }
         }
        */
        int is_visible=1; 
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].val==-1)//Exact obstacle cells
         {
       ////g3d_drawDisc(x2,y2,z2,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
       ////printf(" point (%lf,%lf,%lf) of which the cell (%d,%d,%d) is not empty and the cell value is %lf \n",x2,y2,z2,curr_cell->x,curr_cell->y,curr_cell->z,grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].val);
        ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].affordances.visible_by_human=1;
          ////////is_visible=0;
 

          //******AKP NOTE :Check for more expensive test, because we are not using directed BB so many cell unnecessary becomes obstacle cell
          /*
         if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].is_horizontal_surface==1)
          {

          point_to_look[6] = x2;
          point_to_look[7] = y2;
          point_to_look[8] = z2;
          
          //Using perspective taking function
          if(type==1) //for human
          {
          p3d_rob * currRob=ACBTSET->human[ACBTSET->actual_human]->HumanPt;
          p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
          is_visible=psp_is_object_visible(currRob, ACBTSET->visball, 50);
          }
          else
          {
           if(type==2) //for HRP2
           {
          p3d_rob * currRob=ACBTSET->robot;
          p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
          is_visible=psp_is_object_visible(currRob, ACBTSET->visball, 50);
           } 
          }
          
          //Using collison test by local path method  
          //is_visible=is_point_visible_by_st_line(hum_head_pos, point_to_look, type); 
          
          if(is_visible==1)
          {  
          }
          else
          {
          is_visible=0;
          ////t2=1000;// Break the ray beyond this, because the first obstacle has been found, so human can't see beyond this point.
          }
          }
          //****** END Check for more expensive test, because we are not using directed BB so many cell unnecessary becomes obstacle cell
          else*/
          ////////{
          obs_found=1;
          is_visible=0;

          
          if(first_obs_cell==0)
           {
          first_obs_cell=1;
           ////This is the first obstacle cell found in the ray
           if(type==1)//means for human
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_standing_human=1;
            } 

            if(type==2)//means for HRP2
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_standing_HRP2=1;
            } 

           }
          ////////////t2=1000;// Break the ray beyond this, because the first obstacle has been found, so human can't see beyond this point.
          ////////}
         }
        //else
         if(obs_found==0&&is_visible==1)
         {
         if(type==1)//means for human
          {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_standing_human=1;//NOTE :In EARLIER version affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
          }
        else
          {
        if(type==2)//means for HRP2
           {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_standing_HRP2=1;//NOTE : In EARLIER version  affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
           }
          } 
         ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].affordances.visible_by_human=1; //NOTE : In EARLIER version  affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
         visible_ctr++;
         //g3d_drawDisc(x2,y2,z2,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
         } 
        }
       }
      }
     //}

     }
    }
  
    p3d_destroy_config(ACBTSET->visball, hum_head_pos);
    p3d_destroy_config(ACBTSET->visball, point_to_look);   
    //////////printf(" Visible_ctr=%d\n",visible_ctr); 
  return 1;
}



int update_3d_grid_visibility_by_neck_turn(int type)//1 means human, 2 means HRP2, 3 means JIDO
{
//////////printf(" **** Inside update_3d_grid_visibility_by_neck_turn() for %d,  no_FOV_end_point_vertices=%d\n",type,no_FOV_end_point_vertices);
point_co_ordi eye_pos;
   
eye_pos.x=(FOV_end_point_vertices[0][0].x+FOV_end_point_vertices[0][1].x+FOV_end_point_vertices[0][2].x+FOV_end_point_vertices[0][3].x)/4.0;//Average of the 4 vertices of the near screen
eye_pos.y=(FOV_end_point_vertices[0][0].y+FOV_end_point_vertices[0][1].y+FOV_end_point_vertices[0][2].y+FOV_end_point_vertices[0][3].y)/4.0;//Average of the 4 vertices of the near screen
eye_pos.z=(FOV_end_point_vertices[0][0].z+FOV_end_point_vertices[0][1].z+FOV_end_point_vertices[0][2].z+FOV_end_point_vertices[0][3].z)/4.0;//Average of the 4 vertices of the near screen

    
    configPt hum_head_pos  = p3d_get_robot_config(ACBTSET->visball);  // We need to use a visball as the robot for finding the collosion free straight line path otherwise if we will use human it might return collision
    hum_head_pos[6] = eye_pos.x;//hum_cur_pos[6];
    hum_head_pos[7] = eye_pos.y;//hum_cur_pos[7];
    hum_head_pos[8] = eye_pos.z;
    
    configPt point_to_look = p3d_get_robot_config(ACBTSET->visball);  
    

 double interval=grid_around_HRP2.GRID_SET->pace/2.0;
   //////////printf("interval=%lf\n",interval);
 
int visible_ctr=0;  
 int j=0;
    for(j=0;j<no_FOV_end_point_vertices;j++)
    {

    int i=4;
    for(i=4;i<8;i++)
     {
   //// if(i<4)
    //// {
    ////g3d_drawDisc(FOV_end_point_vertices[j][i].x,FOV_end_point_vertices[j][i].y, FOV_end_point_vertices[j][i].z, grid_around_HRP2.GRID_SET->pace/4.0, 3, NULL);
    //// }  
    ////else
    // {
    ////g3d_drawDisc(FOV_end_point_vertices[j][i].x,FOV_end_point_vertices[j][i].y, FOV_end_point_vertices[j][i].z, grid_around_HRP2.GRID_SET->pace/4.0, 2, NULL);
 
  //// printf(" j=%d, i=%d\n",j,i);
 double t=0;

      for(;t<1;t+=interval)
      {
      
      double x;
      double y;
      double z;
      
      if(i<7)
        {
      x=(1-t)*FOV_end_point_vertices[j][i].x+t*FOV_end_point_vertices[j][i+1].x;
      y=(1-t)*FOV_end_point_vertices[j][i].y+t*FOV_end_point_vertices[j][i+1].y;
      z=(1-t)*FOV_end_point_vertices[j][i].z+t*FOV_end_point_vertices[j][i+1].z;
        } 
      else
        {
      x=(1-t)*FOV_end_point_vertices[j][i].x+t*FOV_end_point_vertices[j][4].x;
      y=(1-t)*FOV_end_point_vertices[j][i].y+t*FOV_end_point_vertices[j][4].y;
      z=(1-t)*FOV_end_point_vertices[j][i].z+t*FOV_end_point_vertices[j][4].z;
        }
       ////g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
      ////printf(" x,y,z=(%lf,%lf,%lf)\n",x,y,z);
      int obs_found=0;
      int first_obs_cell=0;
      double t2=0.1;//Just to avoid very checking very near to human
      for(;t2<1;t2+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t2)*eye_pos.x+t2*x;
      double y2=(1-t2)*eye_pos.y+t2*y;
      double z2=(1-t2)*eye_pos.z+t2*z; 
      
  /*  double x2=(1-t2)*x+t*eye_pos.x;
      double y2=(1-t2)*y+t*eye_pos.y;
      double z2=(1-t2)*z+t*eye_pos.z; 
  */
       //////////hri_bitmap_cell* curr_cell=NULL;
  
       ////curr_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[BT_AFFORDANCE_VISIBILITY], x2, y2, z2);

       //////////curr_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP], x2, y2, z2);
  
       //////////if(curr_cell!=NULL)
      int cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  
       int cell_y;
       int cell_z;

       int cell_valid=0;       

       if(cell_x>=0&&cell_x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
       {
      
 
      cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 
      
      if(cell_y>=0&&cell_y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
       {
       cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 
 
       if(cell_z>=0&&cell_z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
        {
       ////curr_cell=
       ////curr_cell->x=cell_x;
       ////curr_cell->y=cell_y;
       ////curr_cell->z=cell_z;
      cell_valid=1;
        } 
       }
      }

      ////printf(" curr_cell (%d, %d, %d)\n",cell_x,cell_y,cell_z);

       ////////if(curr_cell!=NULL)
        if(cell_valid==1)
        { 
        /*
        if(type==1)//means for human
         {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_neck_turn=0;
         }
        else
         {
        if(type==2)//means for HRP2
          {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2_neck_turn=0;
          }
        else
          //{
           if(type==3)//means for JIDO
           {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_JIDO_neck_turn=0;
           }
          //}
         }*/

        int is_visible=1; 
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].val==-1)//Exact obstacle cells
         {
       ////g3d_drawDisc(x2,y2,z2,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
       ////printf(" point (%lf,%lf,%lf) of which the cell (%d,%d,%d) is not empty and the cell value is %lf \n",x2,y2,z2,curr_cell->x,curr_cell->y,curr_cell->z,grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].val);
        ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].affordances.visible_by_human=1;
          //////////is_visible=0;
 

          //******AKP NOTE :Check for more expensive test, because we are not using directed BB so many cell unnecessary becomes obstacle cell
          /*
         if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].is_horizontal_surface==1)
          {

          point_to_look[6] = x2;
          point_to_look[7] = y2;
          point_to_look[8] = z2;
          
          //Using perspective taking function
          if(type==1) //for human
          {
          p3d_rob * currRob=ACBTSET->human[ACBTSET->actual_human]->HumanPt;
          p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
          is_visible=psp_is_object_visible(currRob, ACBTSET->visball, 50);
          }
          else
          {
           if(type==2) //for HRP2
           {
          p3d_rob * currRob=ACBTSET->robot;
          p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
          is_visible=psp_is_object_visible(currRob, ACBTSET->visball, 50);
           } 
          }
          
          //Using collison test by local path method  
          //is_visible=is_point_visible_by_st_line(hum_head_pos, point_to_look, type); 
          
          if(is_visible==1)
          {  
          }
          else
          {
          is_visible=0;
          ////t2=1000;// Break the ray beyond this, because the first obstacle has been found, so human can't see beyond this point.
          }
          }
          //****** END Check for more expensive test, because we are not using directed BB so many cell unnecessary becomes obstacle cell
          else*/
          ////////{
          is_visible=0;
          obs_found=1;

          if(first_obs_cell==0)
           {
          first_obs_cell=1;
           ////This is the first obstacle cell found in the ray
           if(type==1)//means for human
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human_neck_turn=1;
            } 

            if(type==2)//means for HRP2
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2_neck_turn=1;
            } 

             if(type==3)//means for JIDO
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO_neck_turn=1;
          ////printf(" ************ first_non_visible_by_JIDO_neck_turn \n");
            } 
           }
          ////////////t2=1000;// Break the ray beyond this, because the first obstacle has been found, so human can't see beyond this point.
          ////////}
         }
        //else
         if(obs_found==0&&is_visible==1)
         {
         if(type==1)//means for human
          {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_neck_turn=1;//NOTE :In EARLIER version affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
          }
        else
          {
        if(type==2)//means for HRP2
           {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2_neck_turn=1;//NOTE : In EARLIER version  affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
           }
          else
           {
           if(type==3)//means for JIDO
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_JIDO_neck_turn=1;
            }
           }
          } 
         ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].affordances.visible_by_human=1; //NOTE : In EARLIER version  affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
         visible_ctr++;
         //g3d_drawDisc(x2,y2,z2,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
         } 
        }
       }
      }
     //}

     }
    }
  
    p3d_destroy_config(ACBTSET->visball, hum_head_pos);
    p3d_destroy_config(ACBTSET->visball, point_to_look);   
 return 1;
  //////////printf(" Visible_ctr=%d\n",visible_ctr); 
}


int update_3d_grid_straight_visibility_standing(int type)//1 means human, 2 means HRP2
{
//////////printf(" Inside update_3d_grid_visibility(),  no_FOV_end_point_vertices=%d\n",no_FOV_end_point_vertices);
point_co_ordi eye_pos;
   
eye_pos.x=(FOV_end_point_vertices[0][0].x+FOV_end_point_vertices[0][1].x+FOV_end_point_vertices[0][2].x+FOV_end_point_vertices[0][3].x)/4.0;//Average of the 4 vertices of the near screen
eye_pos.y=(FOV_end_point_vertices[0][0].y+FOV_end_point_vertices[0][1].y+FOV_end_point_vertices[0][2].y+FOV_end_point_vertices[0][3].y)/4.0;//Average of the 4 vertices of the near screen
eye_pos.z=(FOV_end_point_vertices[0][0].z+FOV_end_point_vertices[0][1].z+FOV_end_point_vertices[0][2].z+FOV_end_point_vertices[0][3].z)/4.0;//Average of the 4 vertices of the near screen

    
    configPt hum_head_pos  = p3d_get_robot_config(ACBTSET->visball);  // We need to use a visball as the robot for finding the collosion free straight line path otherwise if we will use human it might return collision
    hum_head_pos[6] = eye_pos.x;//hum_cur_pos[6];
    hum_head_pos[7] = eye_pos.y;//hum_cur_pos[7];
    hum_head_pos[8] = eye_pos.z;
    
    configPt point_to_look = p3d_get_robot_config(ACBTSET->visball);  
    

 double interval=grid_around_HRP2.GRID_SET->pace/2.0;
   //////////printf("interval=%lf\n",interval);
 
int visible_ctr=0;  
 int j=0;
    for(j=0;j<no_FOV_end_point_vertices;j++)
    {

    int i=4;
    for(i=4;i<8;i++)
     {
   //// if(i<4)
    //// {
    ////g3d_drawDisc(FOV_end_point_vertices[j][i].x,FOV_end_point_vertices[j][i].y, FOV_end_point_vertices[j][i].z, grid_around_HRP2.GRID_SET->pace/4.0, 3, NULL);
    //// }  
    ////else
    // {
    ////g3d_drawDisc(FOV_end_point_vertices[j][i].x,FOV_end_point_vertices[j][i].y, FOV_end_point_vertices[j][i].z, grid_around_HRP2.GRID_SET->pace/4.0, 2, NULL);
 
  //// printf(" j=%d, i=%d\n",j,i);
 double t=0;

      for(;t<1;t+=interval)
      {
      
      double x;
      double y;
      double z;
      
      if(i<7)
        {
      x=(1-t)*FOV_end_point_vertices[j][i].x+t*FOV_end_point_vertices[j][i+1].x;
      y=(1-t)*FOV_end_point_vertices[j][i].y+t*FOV_end_point_vertices[j][i+1].y;
      z=(1-t)*FOV_end_point_vertices[j][i].z+t*FOV_end_point_vertices[j][i+1].z;
        } 
      else
        {
      x=(1-t)*FOV_end_point_vertices[j][i].x+t*FOV_end_point_vertices[j][4].x;
      y=(1-t)*FOV_end_point_vertices[j][i].y+t*FOV_end_point_vertices[j][4].y;
      z=(1-t)*FOV_end_point_vertices[j][i].z+t*FOV_end_point_vertices[j][4].z;
        }
       ////g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
      ////printf(" x,y,z=(%lf,%lf,%lf)\n",x,y,z);
      int obs_found=0;
      int first_obs_cell=0;
      double t2=0.1;//Just to avoid very checking very near to human
      for(;t2<1;t2+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t2)*eye_pos.x+t2*x;
      double y2=(1-t2)*eye_pos.y+t2*y;
      double z2=(1-t2)*eye_pos.z+t2*z; 
      
  /*  double x2=(1-t2)*x+t*eye_pos.x;
      double y2=(1-t2)*y+t*eye_pos.y;
      double z2=(1-t2)*z+t*eye_pos.z; 
  */
       //////////hri_bitmap_cell* curr_cell=NULL;
  
       ////curr_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[BT_AFFORDANCE_VISIBILITY], x2, y2, z2);

       //////////curr_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP], x2, y2, z2);
  
       //////////if(curr_cell!=NULL)
      int cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  
       int cell_y;
       int cell_z;

       int cell_valid=0;       

       if(cell_x>=0&&cell_x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
       {
      
 
      cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 
      
      if(cell_y>=0&&cell_y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
       {
       cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 
 
       if(cell_z>=0&&cell_z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
        {
       ////curr_cell=
       ////curr_cell->x=cell_x;
       ////curr_cell->y=cell_y;
       ////curr_cell->z=cell_z;
      cell_valid=1;
        } 
       }
      }

      ////printf(" curr_cell (%d, %d, %d)\n",cell_x,cell_y,cell_z);

       ////////if(curr_cell!=NULL)
        if(cell_valid==1)
        { 
        /*
        if(type==1)//means for human
         {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_straight_head_orientation_standing=0;
         }
        else
         {
        if(type==2)//means for HRP2
          {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2_straight_head_orientation_standing=0;
          }
         }
        */
        int is_visible=1; 
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].val==-1)//Exact obstacle cells
         {
       ////g3d_drawDisc(x2,y2,z2,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
       ////printf(" point (%lf,%lf,%lf) of which the cell (%d,%d,%d) is not empty and the cell value is %lf \n",x2,y2,z2,curr_cell->x,curr_cell->y,curr_cell->z,grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].val);
        ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].affordances.visible_by_human=1;
          //////////is_visible=0;
 

          //******AKP NOTE :Check for more expensive test, because we are not using directed BB so many cell unnecessary becomes obstacle cell
          /*
         if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].is_horizontal_surface==1)
          {

          point_to_look[6] = x2;
          point_to_look[7] = y2;
          point_to_look[8] = z2;
          
          //Using perspective taking function
          if(type==1) //for human
          {
          p3d_rob * currRob=ACBTSET->human[ACBTSET->actual_human]->HumanPt;
          p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
          is_visible=psp_is_object_visible(currRob, ACBTSET->visball, 50);
          }
          else
          {
           if(type==2) //for HRP2
           {
          p3d_rob * currRob=ACBTSET->robot;
          p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
          is_visible=psp_is_object_visible(currRob, ACBTSET->visball, 50);
           } 
          }
          
          //Using collison test by local path method  
          //is_visible=is_point_visible_by_st_line(hum_head_pos, point_to_look, type); 
          
          if(is_visible==1)
          {  
          }
          else
          {
          is_visible=0;
          ////t2=1000;// Break the ray beyond this, because the first obstacle has been found, so human can't see beyond this point.
          }
          }
          //****** END Check for more expensive test, because we are not using directed BB so many cell unnecessary becomes obstacle cell
          else*/
          ////////{
          obs_found=1;
          is_visible=0;

          if(first_obs_cell==0)
           {
          first_obs_cell=1;
           ////This is the first obstacle cell found in the ray
           if(type==1)//means for human
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human_straight_head_orientation_standing=1;
            } 

            if(type==2)//means for HRP2
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2_straight_head_orientation_standing=1;
            } 

           }
          ////////////t2=1000;// Break the ray beyond this, because the first obstacle has been found, so human can't see beyond this point.
          ////////}
         }
        //else
         if(obs_found==0&&is_visible==1)
         {
         if(type==1)//means for human
          {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_straight_head_orientation_standing=1;//NOTE :In EARLIER version affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
          }
        else
          {
        if(type==2)//means for HRP2
           {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_straight_head_orientation_standing=1;//NOTE : In EARLIER version  affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
           }
          } 
         ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].affordances.visible_by_human=1; //NOTE : In EARLIER version  affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
         visible_ctr++;
         //g3d_drawDisc(x2,y2,z2,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
         } 
        }
       }
      }
     //}

     }
    }
  
    p3d_destroy_config(ACBTSET->visball, hum_head_pos);
    p3d_destroy_config(ACBTSET->visball, point_to_look);   
  //////////printf(" Visible_ctr=%d\n",visible_ctr); 
   return 1;
}


int update_3d_grid_visibility_by_neck_turn_standing(int type)//1 means human, 2 means HRP2
{
//////////printf(" Inside update_3d_grid_visibility(),  no_FOV_end_point_vertices=%d\n",no_FOV_end_point_vertices);
point_co_ordi eye_pos;
   
eye_pos.x=(FOV_end_point_vertices[0][0].x+FOV_end_point_vertices[0][1].x+FOV_end_point_vertices[0][2].x+FOV_end_point_vertices[0][3].x)/4.0;//Average of the 4 vertices of the near screen
eye_pos.y=(FOV_end_point_vertices[0][0].y+FOV_end_point_vertices[0][1].y+FOV_end_point_vertices[0][2].y+FOV_end_point_vertices[0][3].y)/4.0;//Average of the 4 vertices of the near screen
eye_pos.z=(FOV_end_point_vertices[0][0].z+FOV_end_point_vertices[0][1].z+FOV_end_point_vertices[0][2].z+FOV_end_point_vertices[0][3].z)/4.0;//Average of the 4 vertices of the near screen

    
    configPt hum_head_pos  = p3d_get_robot_config(ACBTSET->visball);  // We need to use a visball as the robot for finding the collosion free straight line path otherwise if we will use human it might return collision
    hum_head_pos[6] = eye_pos.x;//hum_cur_pos[6];
    hum_head_pos[7] = eye_pos.y;//hum_cur_pos[7];
    hum_head_pos[8] = eye_pos.z;
    
    configPt point_to_look = p3d_get_robot_config(ACBTSET->visball);  
    

 double interval=grid_around_HRP2.GRID_SET->pace/2.0;
   //////////printf("interval=%lf\n",interval);
 
int visible_ctr=0;  
 int j=0;
    for(j=0;j<no_FOV_end_point_vertices;j++)
    {

    int i=4;
    for(i=4;i<8;i++)
     {
   //// if(i<4)
    //// {
    ////g3d_drawDisc(FOV_end_point_vertices[j][i].x,FOV_end_point_vertices[j][i].y, FOV_end_point_vertices[j][i].z, grid_around_HRP2.GRID_SET->pace/4.0, 3, NULL);
    //// }  
    ////else
    // {
    ////g3d_drawDisc(FOV_end_point_vertices[j][i].x,FOV_end_point_vertices[j][i].y, FOV_end_point_vertices[j][i].z, grid_around_HRP2.GRID_SET->pace/4.0, 2, NULL);
 
  //// printf(" j=%d, i=%d\n",j,i);
 double t=0;

      for(;t<1;t+=interval)
      {
      
      double x;
      double y;
      double z;
      
      if(i<7)
        {
      x=(1-t)*FOV_end_point_vertices[j][i].x+t*FOV_end_point_vertices[j][i+1].x;
      y=(1-t)*FOV_end_point_vertices[j][i].y+t*FOV_end_point_vertices[j][i+1].y;
      z=(1-t)*FOV_end_point_vertices[j][i].z+t*FOV_end_point_vertices[j][i+1].z;
        } 
      else
        {
      x=(1-t)*FOV_end_point_vertices[j][i].x+t*FOV_end_point_vertices[j][4].x;
      y=(1-t)*FOV_end_point_vertices[j][i].y+t*FOV_end_point_vertices[j][4].y;
      z=(1-t)*FOV_end_point_vertices[j][i].z+t*FOV_end_point_vertices[j][4].z;
        }
       ////g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
      ////printf(" x,y,z=(%lf,%lf,%lf)\n",x,y,z);
      int obs_found=0;
      int first_obs_cell=0;
      double t2=0.1;//Just to avoid very checking very near to human
      for(;t2<1;t2+=interval) 
       { 
      
      //g3d_drawDisc(x,y,z,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);

      double x2=(1-t2)*eye_pos.x+t2*x;
      double y2=(1-t2)*eye_pos.y+t2*y;
      double z2=(1-t2)*eye_pos.z+t2*z; 
      
  /*  double x2=(1-t2)*x+t*eye_pos.x;
      double y2=(1-t2)*y+t*eye_pos.y;
      double z2=(1-t2)*z+t*eye_pos.z; 
  */
       //////////hri_bitmap_cell* curr_cell=NULL;
  
       ////curr_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[BT_AFFORDANCE_VISIBILITY], x2, y2, z2);

       //////////curr_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP], x2, y2, z2);
  
       //////////if(curr_cell!=NULL)
      int cell_x=(x2- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  
       int cell_y;
       int cell_z;

       int cell_valid=0;       

       if(cell_x>=0&&cell_x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
       {
      
 
      cell_y=(y2- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace; 
      
      if(cell_y>=0&&cell_y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
       {
       cell_z=(z2- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace; 
 
       if(cell_z>=0&&cell_z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
        {
       ////curr_cell=
       ////curr_cell->x=cell_x;
       ////curr_cell->y=cell_y;
       ////curr_cell->z=cell_z;
      cell_valid=1;
        } 
       }
      }

      ////printf(" curr_cell (%d, %d, %d)\n",cell_x,cell_y,cell_z);

       ////////if(curr_cell!=NULL)
        if(cell_valid==1)
        { 
         /*
        if(type==1)//means for human
         {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_standing_human_neck_turn=0;
         }
        else
         {
        if(type==2)//means for HRP2
          {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_standing_HRP2_neck_turn=0;
          }
         }
         */
        int is_visible=1; 
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].val==-1)//Exact obstacle cells
         {
       ////g3d_drawDisc(x2,y2,z2,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
       ////printf(" point (%lf,%lf,%lf) of which the cell (%d,%d,%d) is not empty and the cell value is %lf \n",x2,y2,z2,curr_cell->x,curr_cell->y,curr_cell->z,grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].val);
        ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].affordances.visible_by_human=1;
          //////////is_visible=0;
 

          //******AKP NOTE :Check for more expensive test, because we are not using directed BB so many cell unnecessary becomes obstacle cell
          /*
         if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].is_horizontal_surface==1)
          {

          point_to_look[6] = x2;
          point_to_look[7] = y2;
          point_to_look[8] = z2;
          
          //Using perspective taking function
          if(type==1) //for human
          {
          p3d_rob * currRob=ACBTSET->human[ACBTSET->actual_human]->HumanPt;
          p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
          is_visible=psp_is_object_visible(currRob, ACBTSET->visball, 50);
          }
          else
          {
           if(type==2) //for HRP2
           {
          p3d_rob * currRob=ACBTSET->robot;
          p3d_set_and_update_this_robot_conf(ACBTSET->visball, point_to_look);
          is_visible=psp_is_object_visible(currRob, ACBTSET->visball, 50);
           } 
          }
          
          //Using collison test by local path method  
          //is_visible=is_point_visible_by_st_line(hum_head_pos, point_to_look, type); 
          
          if(is_visible==1)
          {  
          }
          else
          {
          is_visible=0;
          ////t2=1000;// Break the ray beyond this, because the first obstacle has been found, so human can't see beyond this point.
          }
          }
          //****** END Check for more expensive test, because we are not using directed BB so many cell unnecessary becomes obstacle cell
          else*/
          ////////{
          obs_found=1;
          is_visible=0;

           if(first_obs_cell==0)
           {
          first_obs_cell=1;
           ////This is the first obstacle cell found in the ray
           if(type==1)//means for human
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_standing_human_neck_turn=1;
            } 

            if(type==2)//means for HRP2
            {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_standing_human_neck_turn=1;
            } 

           }
          ////////////t2=1000;// Break the ray beyond this, because the first obstacle has been found, so human can't see beyond this point.
          ////////}
         }
        //else
         if(obs_found==0&&is_visible==1)
         {
         if(type==1)//means for human
          {
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_standing_human_neck_turn=1;//NOTE :In EARLIER version affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
          }
        else
          {
        if(type==2)//means for HRP2
           {
          grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_standing_HRP2_neck_turn=1;//NOTE : In EARLIER version  affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
           }
          } 
         ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[curr_cell->x][curr_cell->y][curr_cell->z].affordances.visible_by_human=1; //NOTE : In EARLIER version  affordance values like visibility, reachability etc will be set in HRP2_GIK_MANIP bitmap, only for checking the obstacle values for visibility,BT_AFFORDANCE_VISIBILITY bitmap will be used.
         visible_ctr++;
         //g3d_drawDisc(x2,y2,z2,grid_around_HRP2.GRID_SET->pace/4.0,2,NULL);
         } 
        }
       }
      }
     //}

     }
    }
  
    p3d_destroy_config(ACBTSET->visball, hum_head_pos);
    p3d_destroy_config(ACBTSET->visball, point_to_look);   
  //////////printf(" Visible_ctr=%d\n",visible_ctr); 
 return 1;
}


int update_3D_grid_based_on_current_human_pos()
{
 //////////printf("Inside update_3D_grid_based_on_current_human_pos()\n");
 int x=0;
 for(x=0;x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx;x++)
 {
 int y=0;
 for(y=0;y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny;y++)
  {
  int z=0;
  for(z=0;z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz;z++)
   {
    ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].val<0)
    ////{
       grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human=0;

       double cell_x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double cell_y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double cell_z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
       point_co_ordi point;
        point.x=cell_x_world;
        point.y=cell_y_world;
        point.z=cell_z_world;
        //AKP: Following function is very slow for testing visibility for many points  
        int is_visible=is_point_on_surface_visible(point, 1); // 1 means test for Human
        if(is_visible==1)
        {
       //// printf(" point visible \n");
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human=1;
        }
         
       //g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, grid_around_HRP2.GRID_SET->pace/4.0, Red, NULL);
        //fl_check_forms();
       //g3d_draw_allwin_active();
    ////}
    ////else
    ////{
    ////  double cell_x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
    ////  double cell_y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
    ////  double cell_z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
    ////  g3d_drawDisc(cell_x_world, cell_y_world, cell_z_world, 0.005, Green, NULL);
      
    ////} 
    //  fl_check_forms();
   }
  } 
 }
return 1;
}

int human_state_updated=0;
int update_human_state(int state) //1 means sitting 0 means standing
{
 
 //////////printf("Before updating human state, ACBTSET->human[ACBTSET->actual_human]->actual_state=%d\n",ACBTSET->human[ACBTSET->actual_human]->actual_state);
 if(human_state_updated==0)
 { //ACBTSET->human[ACBTSET->actual_human]->actual_state = 1; //// AKP Note: Comment it if using motion capture button
  ////update_human_state(1);//For sitting //// AKP Note: Comment it if using motion capture button
  configPt config;
  config = p3d_copy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS);
  ////ACBTSET->actual_human=0;
 hri_human * human=ACBTSET->human[ACBTSET->actual_human];
 ////int state=1;
 
	//  q[8] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c7;
	//  q[43] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c1;
	//  q[44] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c2;
	//  q[46] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c3;
	//  q[47] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c4;
	//  q[50] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c5;
	//  q[53] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c6;
    config[8] = human->state[state].c7;
    config[43] = human->state[state].c1;
    config[44] = human->state[state].c2;
    config[46] = human->state[state].c3;
    config[47] = human->state[state].c4;
    config[50] = human->state[state].c5;
    config[53] = human->state[state].c6;
    // Right Hand 
    config[66] = config[6] + cos(config[11]-0.4)*0.5; // REVIEW 0.4 --> 0.2 
    config[67] = config[7] + sin(config[11]-0.4)*0.5;
    config[68] = config[68]-0.34+0.1;
    // Left Hand 
    config[72] = config[6] + cos(config[11]+0.4)*0.5;
    config[73] = config[7] + sin(config[11]+0.4)*0.5;
    config[74] = config[74]-0.34+0.1;
    human->actual_state=state;
	////hri_set_human_state_SICK(ACBTSET->human[ACBTSET->actual_human], 1, q, FALSE);

	p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt,config);

	p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt,config);
ACBTSET->changed = TRUE;

	if(BTSET!=NULL)
		hri_bt_refresh_all(BTSET);
	if(INTERPOINT!=NULL){
		hri_bt_3drefresh_all(INTERPOINT);
	}
 g3d_draw_env();
 fl_check_forms();
 g3d_draw_allwin_active();
	////g3d_draw_allwin_active();
  human_state_updated=0;
 }
 
//////////printf("After updating human state, ACBTSET->human[ACBTSET->actual_human]->actual_state=%d\n",ACBTSET->human[ACBTSET->actual_human]->actual_state);
return 1;
}
/*
int update_human_state_new(int state) //1 means sitting 0 means standing
{
 
 printf("Before updating human state, ACBTSET->human[ACBTSET->actual_human]->actual_state=%d\n",ACBTSET->human[ACBTSET->actual_human]->actual_state);
 if(human_state_updated==0)
 { //ACBTSET->human[ACBTSET->actual_human]->actual_state = 1; //// AKP Note: Comment it if using motion capture button
  ////update_human_state(1);//For sitting //// AKP Note: Comment it if using motion capture button
  configPt config;
  config = p3d_copy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS);
  ////ACBTSET->actual_human=0;
 hri_human * human=ACBTSET->human[ACBTSET->actual_human];
 ////int state=1;
 
	//  q[8] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c7;
	//  q[43] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c1;
	//  q[44] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c2;
	//  q[46] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c3;
	//  q[47] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c4;
	//  q[50] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c5;
	//  q[53] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c6;
     
    if(strcasestr(human->HumanPt->name,"superman"))
    {
    config[8] = human->state[state].c7;
    config[43] = human->state[state].c1;
    config[44] = human->state[state].c2;
    config[46] = human->state[state].c3;
    config[47] = human->state[state].c4;
    config[50] = human->state[state].c5;
    config[53] = human->state[state].c6;
    // Right Hand 
    config[66] = config[6] + cos(config[11]-0.4)*0.5; // REVIEW 0.4 --> 0.2 
    config[67] = config[7] + sin(config[11]-0.4)*0.5;
    config[68] = config[68]-0.34+0.1;
    // Left Hand 
    config[72] = config[6] + cos(config[11]+0.4)*0.5;
    config[73] = config[7] + sin(config[11]+0.4)*0.5;
    config[74] = config[74]-0.34+0.1;
    }
    else
    {
     if(strcasestr(human->HumanPt->name,"achile"))
     {
   
      config[8] = human->state[state].c7;
      config[32] = human->state[state].c3;
      config[35] = human->state[state].c4;
      config[39] = human->state[state].c1;
      config[42] = human->state[state].c2;
     }   
    } 
    human->actual_state=state;
	////hri_set_human_state_SICK(ACBTSET->human[ACBTSET->actual_human], 1, q, FALSE);

	p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt,config);

	p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt,config);
ACBTSET->changed = TRUE;

	
 ////g3d_draw_env();
 ////fl_check_forms();
 ////g3d_draw_allwin_active();
	////g3d_draw_allwin_active();
  human_state_updated=0;
 }
 
printf("After updating human state, ACBTSET->human[ACBTSET->actual_human]->actual_state=%d\n",ACBTSET->human[ACBTSET->actual_human]->actual_state);

}
*/

int virtually_update_human_state(int state) //1 means sitting 0 means standing
{
 
 //////////printf("Before updating human state, ACBTSET->human[ACBTSET->actual_human]->actual_state=%d\n",ACBTSET->human[ACBTSET->actual_human]->actual_state);
 if(human_state_updated==0)
 { //ACBTSET->human[ACBTSET->actual_human]->actual_state = 1; //// AKP Note: Comment it if using motion capture button
  ////update_human_state(1);//For sitting //// AKP Note: Comment it if using motion capture button
  configPt config;
  config = p3d_copy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS);
  ////ACBTSET->actual_human=0;
 hri_human * human=ACBTSET->human[ACBTSET->actual_human];
 ////int state=1;
 
	//  q[8] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c7;
	//  q[43] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c1;
	//  q[44] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c2;
	//  q[46] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c3;
	//  q[47] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c4;
	//  q[50] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c5;
	//  q[53] = ACBTSET->human[ACBTSET->actual_human]->state[new_state].c6;
 config[8] = human->state[state].c7;
    config[43] = human->state[state].c1;
    config[44] = human->state[state].c2;
    config[46] = human->state[state].c3;
    config[47] = human->state[state].c4;
    config[50] = human->state[state].c5;
    config[53] = human->state[state].c6;
    // Right Hand 
    config[66] = config[6] + cos(config[11]-0.4)*0.5; // REVIEW 0.4 --> 0.2 
    config[67] = config[7] + sin(config[11]-0.4)*0.5;
    config[68] = config[68]-0.34+0.1;
    // Left Hand 
    config[72] = config[6] + cos(config[11]+0.4)*0.5;
    config[73] = config[7] + sin(config[11]+0.4)*0.5;
    config[74] = config[74]-0.34+0.1;
 human->actual_state=state;
	////hri_set_human_state_SICK(ACBTSET->human[ACBTSET->actual_human], 1, q, FALSE);

	p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt,config);

	p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt,config);
ACBTSET->changed = TRUE;

	if(BTSET!=NULL)
		hri_bt_refresh_all(BTSET);
	if(INTERPOINT!=NULL){
		hri_bt_3drefresh_all(INTERPOINT);
	}
 ////////g3d_draw_env();
 ////////fl_check_forms();
 ////////g3d_draw_allwin_active();
	////g3d_draw_allwin_active();
  human_state_updated=0;
 }
 
//////////printf("After updating human state, ACBTSET->human[ACBTSET->actual_human]->actual_state=%d\n",ACBTSET->human[ACBTSET->actual_human]->actual_state);
return 1;
}


int virtually_update_human_state_new(int state) //1 means sitting 0 means standing
{
 
 //////////printf("Before updating human state, ACBTSET->human[ACBTSET->actual_human]->actual_state=%d\n",ACBTSET->human[ACBTSET->actual_human]->actual_state);
 if(human_state_updated==0)
 { //ACBTSET->human[ACBTSET->actual_human]->actual_state = 1; //// AKP Note: Comment it if using motion capture button
  ////update_human_state(1);//For sitting //// AKP Note: Comment it if using motion capture button
  configPt config;
  config = p3d_copy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS);
  ////ACBTSET->actual_human=0;
 hri_human * human=ACBTSET->human[ACBTSET->actual_human];
 ////int state=1;
 
    if(strcasestr(human->HumanPt->name,"superman"))
    {
    config[8] = human->state[state].c7;
    config[43] = human->state[state].c1;
    config[44] = human->state[state].c2;
    config[46] = human->state[state].c3;
    config[47] = human->state[state].c4;
    config[50] = human->state[state].c5;
    config[53] = human->state[state].c6;
    // Right Hand 
    config[66] = config[6] + cos(config[11]-0.4)*0.5; // REVIEW 0.4 --> 0.2 
    config[67] = config[7] + sin(config[11]-0.4)*0.5;
    config[68] = config[68]-0.34+0.1;
    // Left Hand 
    config[72] = config[6] + cos(config[11]+0.4)*0.5;
    config[73] = config[7] + sin(config[11]+0.4)*0.5;
    config[74] = config[74]-0.34+0.1;
    }
    else
    {
     if(strcasestr(human->HumanPt->name,"achile"))
     {
      config[8] = human->state[state].c7;
      config[33] = human->state[state].c3;
      config[35] = human->state[state].c4;
      config[40] = human->state[state].c1;
      config[42] = human->state[state].c2;
     }   
    } 
 human->actual_state=state;
	////hri_set_human_state_SICK(ACBTSET->human[ACBTSET->actual_human], 1, q, FALSE);

	p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt,config);

	p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt,config);
ACBTSET->changed = TRUE;

	/*if(BTSET!=NULL)
		hri_bt_refresh_all(BTSET);
	if(INTERPOINT!=NULL){
		hri_bt_3drefresh_all(INTERPOINT);
	}*/
 ////////g3d_draw_env();
 ////////fl_check_forms();
 ////////g3d_draw_allwin_active();
	////g3d_draw_allwin_active();
  human_state_updated=0;
 }
 
//////////printf("After updating human state, ACBTSET->human[ACBTSET->actual_human]->actual_state=%d\n",ACBTSET->human[ACBTSET->actual_human]->actual_state);
return 1;
}

int find_affordance_new()
{
ChronoOn();
////M3D_GIK_TEST();
 ////printf(" Inside find_affordance \n");

int kcd_with_report=0;
p3d_rob *human=ACBTSET->human[ACBTSET->actual_human]->HumanPt;

int res = p3d_col_test_robot(human,kcd_with_report);
 if(res>0)
 {
  printf(" There is collision with human, res=%d \n", res);
  pqp_print_colliding_pair();
  //return 0;
 }
kcd_with_report=0;
 res = p3d_col_test_self_collision(human,kcd_with_report);
 if(res>0)
 {
  printf(" There is self collision with human, res=%d \n", res);
  pqp_print_colliding_pair();
  //return 0;
 }

int HRP2_state=HRP2_CURRENT_STATE; //AKP : 1 is sitting, 2 is standing 
///////create_HRP2_robot(HRP2_state);

p3d_rob *cur_rob=ACBTSET->robot;

	
	for(int j=0;j<cur_rob->no;j++)
	{
	p3d_obj *o = cur_rob->o[j];
	//if (strstr(o->name,"head") || strstr(o->name,"HEAD") || strstr(o->name,"hand") || strstr(o->name,"HAND"))
	// {
	////p3d_get_object_center(o,objCenter);
        //////////printf("%s\n",o->name);
					// }	  
	}
			
			//define if robot is near or not? here or in observation? od we need a different list
			// if ((ContObjTmp/r->no)>.4)
			// 
			
		


 kcd_with_report=0;
 //res = p3d_col_test_self_collision(cur_rob,kcd_with_report);
 res = p3d_col_test_self_collision(cur_rob,2);
 ////res = p3d_col_test_robot(cur_rob,2);
		//printf("collision 2:   %i \n",res2);
      set_kcd_which_test(P3D_KCD_ROB_ALL);
      res = p3d_col_does_robot_collide(cur_rob->num, p3d_numcoll);
 //// res = p3d_col_test_self_collision(cur_rob->num, p3d_numcoll);
 if(res>0)
 {
  printf(" There is self collision with robot, res=%d \n", res);
  ////return 0;
 }

kcd_with_report=0;
  res = p3d_col_test_robot(cur_rob,2);
		//printf("collision 2:   %i \n",res2);
      set_kcd_which_test(P3D_KCD_ROB_ALL);
      res = p3d_col_does_robot_collide(cur_rob->num, p3d_numcoll);
 //// res = p3d_col_test_self_collision(cur_rob->num, p3d_numcoll);
 if(res>0)
 {
  printf(" There is collision with robot, res=%d \n", res);
  ////return 0;
 }
 

 if(ONLINE_TRACKING_FLAG==0)
 initialize_surfaces_in_env(); // AKP Note: Comment it if using motion capture button


 
////int HRP2_state=HRP2_CURRENT_STATE; //AKP : 1 is sitting, 2 is standing 

//////////printf(" Before create_3d_grid_for_HRP2_GIK() \n");
////////////ChronoPrint("***");
create_3d_grid_for_HRP2_GIK();
//////////ChronoPrint("Time for create_3d_grid_for_HRP2_GIK()");
//////////printf(" After create_3d_grid_for_HRP2_GIK() \n");
ChronoOff();

//////////printf(" **** 3D grid dimension is (%d x %d x %d) cells.  \n",grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx,grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny,grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz);

double cur_h_angle;
double interval;

/*
//////////////////////tmp for jido
ChronoOn();
grid_3d_affordance_calculated=1; 


////p3d_rob *cur_rob=ACBTSET->robot;
cur_h_angle=cur_rob->cam_h_angle;
interval=grid_around_HRP2.GRID_SET->pace;
printf("cur_h_angle=%lf\n",cur_h_angle);
no_FOV_end_point_vertices=0;
while(cur_rob->cam_h_angle>0.001)
{ 
cur_rob->cam_h_angle-=interval;
 
get_points_on_FOV_screen(cur_rob);
}
cur_rob->cam_h_angle=cur_h_angle;
////ChronoOn();
update_3d_grid_visibility(3); //2 for JIDO

ChronoOff();

update_3d_grid_reachability_for_JIDO_new();


return 1;
///////////////////////end tmp for jido
*/


ChronoOn();

//*****AKP: Tmp uncomment/comment because of problem in new human model for sitting as well as standing
virtually_update_human_state_new(1);// Sitting



cur_h_angle=human->cam_h_angle;
printf(" cur_h_angle=%lf\n",cur_h_angle);


//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);

}
////return 0;
human->cam_h_angle=cur_h_angle;
////int expansion=1; //To populate the number of cells, around a cell having obstacle, with obstacles also
////create_exact_obstacles_for_HRP2_GIK_manip(grid_around_HRP2.GRID_SET,expansion,BT_AFFORDANCE_VISIBILITY); 
////ChronoOn();	    

update_3d_grid_visibility(1); //1 for human

////////////ChronoPrint("TIME of 3D Visibility calculation for sitting Human from current position for current head orientation");

//Now making the head straight along of axis of torso
configPt hum_cur_pos = MY_ALLOC(double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof); /* Allocation of temporary robot configuration */

 p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);

double fixed_pitch=M_PI/8.0;

double yaw=0.0;
double pitch=fixed_pitch;
double orig_pan=hum_cur_pos[HUMANq_PAN];
double orig_tilt=hum_cur_pos[HUMANq_TILT];
//////////printf(" Original pan = %lf, tilt= %lf \n",orig_pan, orig_tilt); 

hum_cur_pos[HUMANq_PAN]=yaw; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=pitch; // Human pitch angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

cur_h_angle=human->cam_h_angle;
//////////printf(" cur_h_angle=%lf\n",cur_h_angle);
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}
update_3d_grid_straight_visibility(1); //1 for human
human->cam_h_angle=cur_h_angle;

hum_cur_pos[HUMANq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

//Now turning the head only

hum_cur_pos[HUMANq_PAN]=0.0;
yaw=M_PI/3.0;
pitch=fixed_pitch;
////orig_pan=hum_cur_pos[HUMANq_PAN];
////printf(" Original pan = %lf \n",orig_pan); 
hum_cur_pos[HUMANq_PAN]=yaw; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=pitch; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

cur_h_angle=human->cam_h_angle;
//////////printf(" cur_h_angle=%lf\n",cur_h_angle);
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}
update_3d_grid_visibility_by_neck_turn(1); //1 for human
human->cam_h_angle=cur_h_angle;

hum_cur_pos[HUMANq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
////hum_cur_pos[HUMANq_PAN]=0.0;// Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);

hum_cur_pos[HUMANq_PAN]=-yaw; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=pitch; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];


cur_h_angle=human->cam_h_angle;
//////////printf(" cur_h_angle=%lf\n",cur_h_angle);
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}
update_3d_grid_visibility_by_neck_turn(1); //1 for human
human->cam_h_angle=cur_h_angle;

hum_cur_pos[HUMANq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];


MY_FREE(hum_cur_pos,double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof);

double tu,ts;
//////////ChronoPrint("TIME of 3D Visibility calculation for sitting Human");
ChronoTimes(&tu,&ts);
//////////printf(" %lf, %lf \n",tu,ts);
ChronoOff();

#ifdef MM_FOR_VIRTUALLY_STANDING_HUMAN
///////////////3D visibility calculation for making human virtually standing//////////////////////////

ChronoOn();
virtually_update_human_state_new(0);// Standing
cur_h_angle=human->cam_h_angle;
//////////printf(" cur_h_angle for standing human=%lf\n",cur_h_angle);
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}

human->cam_h_angle=cur_h_angle;
////int expansion=1; //To populate the number of cells, around a cell having obstacle, with obstacles also
////create_exact_obstacles_for_HRP2_GIK_manip(grid_around_HRP2.GRID_SET,expansion,BT_AFFORDANCE_VISIBILITY); 
////ChronoOn();	    

update_3d_grid_visibility_standing(1); //1 for human

/////////////ChronoPrint("TIME of 3D Visibility calculation for standing Human from current position");

//Now making the head straight
hum_cur_pos = MY_ALLOC(double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof); 

 p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);


yaw=0.0;
pitch=fixed_pitch;
orig_pan=hum_cur_pos[HUMANq_PAN];
orig_tilt=hum_cur_pos[HUMANq_TILT];
//////////printf(" Original pan = %lf \n",orig_pan); 
hum_cur_pos[HUMANq_PAN]=yaw; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=pitch; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

cur_h_angle=human->cam_h_angle;
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}
update_3d_grid_straight_visibility_standing(1); //1 for human
human->cam_h_angle=cur_h_angle;

hum_cur_pos[HUMANq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];


//Now turning the head only
////hum_cur_pos = MY_ALLOC(double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof); 

 ////p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);


yaw=M_PI/3.0;
pitch=fixed_pitch;
////orig_pan=hum_cur_pos[HUMANq_PAN];
////printf(" Original pan = %lf \n",orig_pan); 

hum_cur_pos[HUMANq_PAN]=yaw; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=pitch; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

cur_h_angle=human->cam_h_angle;
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}
update_3d_grid_visibility_by_neck_turn_standing(1); //1 for human
human->cam_h_angle=cur_h_angle;

hum_cur_pos[HUMANq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);

hum_cur_pos[HUMANq_PAN]=-yaw; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=pitch; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

cur_h_angle=human->cam_h_angle;
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}
update_3d_grid_visibility_by_neck_turn_standing(1); //1 for human
human->cam_h_angle=cur_h_angle;

hum_cur_pos[HUMANq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

MY_FREE(hum_cur_pos,double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof);


//////////ChronoPrint("TIME of 3D Visibility calculation for standing Human");
ChronoTimes(&tu,&ts);
////printf(" %lf, %lf \n",tu,ts);
////////update_human_state(1);// Again make it to Sitting
virtually_update_human_state_new(1);// // Again make it to Sitting
ChronoOff();

///////////////END of 3D visibility calculation for making human virtually standing//////////////////////////
#endif


ChronoOn();
grid_3d_affordance_calculated=1; 


////p3d_rob *cur_rob=ACBTSET->robot;
cur_h_angle=cur_rob->cam_h_angle;
no_FOV_end_point_vertices=0;
while(cur_rob->cam_h_angle>0.001)
{ 
cur_rob->cam_h_angle-=interval;
get_points_on_FOV_screen(cur_rob);
}
cur_rob->cam_h_angle=cur_h_angle;
////ChronoOn();
#ifdef HRI_JIDO
update_3d_grid_visibility(3); //3 for JIDO
#elif defined(HRI_HRP2)
update_3d_grid_visibility(2); //2 for HRP2
#endif


//Now making the head straight
configPt rob_cur_pos = MY_ALLOC(double,ACBTSET->robot->nb_dof); /* Allocation of temporary robot configuration */

 p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos);

orig_pan=rob_cur_pos[ROBOTq_PAN];
orig_tilt=rob_cur_pos[ROBOTq_TILT];
//////////printf(" Original pan = %lf \n",orig_pan); 

yaw=0.0;
pitch=fixed_pitch;

rob_cur_pos[ROBOTq_PAN]=yaw; // Human Yaw angle relative to the human body frame
rob_cur_pos[ROBOTq_TILT]=pitch; // Human Yaw angle relative to the human body frame

p3d_set_and_update_this_robot_conf(ACBTSET->robot, rob_cur_pos);
ACBTSET->robot->ROBOT_POS[ROBOTq_PAN]=rob_cur_pos[ROBOTq_PAN];
ACBTSET->robot->ROBOT_POS[ROBOTq_TILT]=rob_cur_pos[ROBOTq_TILT];

cur_h_angle=cur_rob->cam_h_angle;
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(cur_rob->cam_h_angle>0.001)
{ 
cur_rob->cam_h_angle-=interval;
get_points_on_FOV_screen(cur_rob);
}

#ifdef HRI_JIDO
update_3d_grid_straight_visibility(3); //3 for JIOD
#elif defined(HRI_HRP2)
update_3d_grid_straight_visibility(2); //2 for HRP2
#endif
cur_rob->cam_h_angle=cur_h_angle;

rob_cur_pos[ROBOTq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
rob_cur_pos[ROBOTq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->robot, rob_cur_pos);
ACBTSET->robot->ROBOT_POS[ROBOTq_PAN]=rob_cur_pos[ROBOTq_PAN];
ACBTSET->robot->ROBOT_POS[ROBOTq_TILT]=rob_cur_pos[ROBOTq_TILT];

p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos);


//Now turning the head only
p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos);
yaw=M_PI/4.0;
pitch=fixed_pitch;
rob_cur_pos[ROBOTq_PAN]=yaw; // Human Yaw angle relative to the human body frame
rob_cur_pos[ROBOTq_TILT]=pitch; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->robot, rob_cur_pos);
ACBTSET->robot->ROBOT_POS[ROBOTq_PAN]=rob_cur_pos[ROBOTq_PAN];
ACBTSET->robot->ROBOT_POS[ROBOTq_TILT]=rob_cur_pos[ROBOTq_TILT];

cur_h_angle=cur_rob->cam_h_angle;
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(cur_rob->cam_h_angle>0.001)
{ 
cur_rob->cam_h_angle-=interval;
get_points_on_FOV_screen(cur_rob);
}

#ifdef HRI_JIDO
update_3d_grid_visibility_by_neck_turn(3); //3 for JIOD
#elif defined(HRI_HRP2)
update_3d_grid_visibility_by_neck_turn(2); //2 for HRP2
#endif

cur_rob->cam_h_angle=cur_h_angle;

rob_cur_pos[ROBOTq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
rob_cur_pos[ROBOTq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->robot, rob_cur_pos);
ACBTSET->robot->ROBOT_POS[ROBOTq_PAN]=rob_cur_pos[ROBOTq_PAN];
ACBTSET->robot->ROBOT_POS[ROBOTq_TILT]=rob_cur_pos[ROBOTq_TILT];

p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos);

rob_cur_pos[ROBOTq_PAN]=-yaw; // Human Yaw angle relative to the human body frame
rob_cur_pos[ROBOTq_TILT]=pitch; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->robot, rob_cur_pos);

ACBTSET->robot->ROBOT_POS[ROBOTq_PAN]=rob_cur_pos[ROBOTq_PAN];
ACBTSET->robot->ROBOT_POS[ROBOTq_TILT]=rob_cur_pos[ROBOTq_TILT];

cur_h_angle=cur_rob->cam_h_angle;
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(cur_rob->cam_h_angle>0.001)
{ 
cur_rob->cam_h_angle-=interval;
get_points_on_FOV_screen(cur_rob);
}
#ifdef HRI_JIDO
update_3d_grid_visibility_by_neck_turn(3); //3 for JIOD
#elif defined(HRI_HRP2)
update_3d_grid_visibility_by_neck_turn(2); //2 for HRP2
#endif
//////////update_3d_grid_visibility_by_neck_turn(2); //2 for HRP2
cur_rob->cam_h_angle=cur_h_angle;

rob_cur_pos[ROBOTq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
rob_cur_pos[ROBOTq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->robot, rob_cur_pos);
ACBTSET->robot->ROBOT_POS[ROBOTq_PAN]=rob_cur_pos[ROBOTq_PAN];
ACBTSET->robot->ROBOT_POS[ROBOTq_TILT]=rob_cur_pos[ROBOTq_TILT];

MY_FREE(rob_cur_pos,double,ACBTSET->robot->nb_dof);


////////////ChronoPrint("TIME of 3D Visibility calculation for ROBOT");
////ChronoOff();
////int HRP2_state=HRP2_CURRENT_STATE; //AKP : 1 is sitting, 2 is standing 
////create_HRP2_robot(HRP2_state);
////g3d_draw_env();
//// fl_check_forms();
//// g3d_draw_allwin_active();


ChronoOff();
ChronoOn();
//// create_HRP2_robot(HRP2_state);
#ifdef HRI_JIDO
update_3d_grid_reachability_for_JIDO_new();
#elif defined(HRI_HRP2)
update_3d_grid_reachability_for_HRP2_new();
#endif
//////////update_3d_grid_reachability_for_HRP2_new();
////ChronoPrint("****TIME of 3D reachability calculation for ROBOT\n");



////return 1;
ChronoOff();

ChronoOn();

update_3d_grid_reachability_for_human_new();

#ifdef MM_FOR_VIRTUALLY_STANDING_HUMAN
//////////update_human_state(0);// Standing
virtually_update_human_state_new(0);// Standing
update_3d_grid_reachability_for_human_standing_new();

////////update_human_state(1);
virtually_update_human_state_new(1);// Sitting
#endif

////ChronoPrint("****TIME of 3D reachability calculation for human\n");

ChronoOff();


return 1;

//return 1;
/*
ChronoPrint("Time before calculating affordance on surfaces ");
 //current_surface_index=0;
 int i=0;
 for(i=0;i<curr_surfaces_in_env.total_no_of_surfaces;i++)
 {
 printf(" For surface %d\n",i);
 ChronoPrint("Time Before update_surface_grid_based_on_curr_pos");
 update_surface_grid_based_on_curr_pos(surf_grid_samp_rate, &curr_surfaces_in_env.flat_surf[i]);

 ChronoPrint("Time Before update_surface_grid_by_bending_human_at_curr_pos");
 update_surface_grid_by_bending_human_at_curr_pos(surf_grid_samp_rate, &curr_surfaces_in_env.flat_surf[i]);

 ChronoPrint("Time Before update_surface_grid_by_turning_human_at_curr_pos");
 update_surface_grid_by_turning_human_at_curr_pos(surf_grid_samp_rate, &curr_surfaces_in_env.flat_surf[i]);
 
 

 
 
 HRP2_state=HRP2_CURRENT_STATE; //AKP : 1 is sitting, 2 is standing 
 //////ChronoPrint(" Time before create_HRP2_robot");
 //////create_HRP2_robot(HRP2_state);
 //////ChronoPrint(" Time after create_HRP2_robot and before update_surface_grid_for_HRP2_without_GIK");
 ////update_surface_grid_for_HRP2_with_GIK(surf_grid_samp_rate, &curr_surfaces_in_env.flat_surf[i]);
 update_surface_grid_for_HRP2_without_GIK(surf_grid_samp_rate, &curr_surfaces_in_env.flat_surf[i]);
 ChronoPrint(" Time after update_surface_grid_for_HRP2_without_GIK");

 }
*/
////ChronoPrint("TIME for all affordance calculation");
ChronoOff();
////create_3d_grid_for_HRP2_GIK();
////update_3D_grid_based_on_current_human_pos();
//// grid_3d_affordance_calculated=1; 
 
 //update_surface_grid(surf_grid_samp_rate, &curr_surfaces_in_env.flat_surf[1]);
}

int update_Mightability_Maps()
{
double total_time=0.0;
ChronoOn();
////M3D_GIK_TEST();
 ////printf(" Inside find_affordance \n");

int kcd_with_report=0;
p3d_rob *human=ACBTSET->human[ACBTSET->actual_human]->HumanPt;

/*
int res = p3d_col_test_robot(human,kcd_with_report);
 if(res>0)
 {
  printf(" There is collision with human, res=%d \n", res);
  //return 0;
 }
kcd_with_report=0;
 res = p3d_col_test_self_collision(human,kcd_with_report);
 if(res>0)
 {
  printf(" There is self collision with human, res=%d \n", res);
  //return 0;
 }
*/
int HRP2_state=HRP2_CURRENT_STATE; //AKP : 1 is sitting, 2 is standing 
///////create_HRP2_robot(HRP2_state);

p3d_rob *cur_rob=ACBTSET->robot;

	/*
	for(int j=0;j<cur_rob->no;j++)
	{
	p3d_obj *o = cur_rob->o[j];
	//if (strstr(o->name,"head") || strstr(o->name,"HEAD") || strstr(o->name,"hand") || strstr(o->name,"HAND"))
	// {
	////p3d_get_object_center(o,objCenter);
        printf("%s\n",o->name);
					// }	  
	}
	*/		
			//define if robot is near or not? here or in observation? od we need a different list
			// if ((ContObjTmp/r->no)>.4)
			// 
			
		

/*
 kcd_with_report=0;
 //res = p3d_col_test_self_collision(cur_rob,kcd_with_report);
 res = p3d_col_test_self_collision(cur_rob,2);
 ////res = p3d_col_test_robot(cur_rob,2);
		//printf("collision 2:   %i \n",res2);
      set_kcd_which_test(P3D_KCD_ROB_ALL);
      res = p3d_col_does_robot_collide(cur_rob->num, p3d_numcoll);
 //// res = p3d_col_test_self_collision(cur_rob->num, p3d_numcoll);
 if(res>0)
 {
  printf(" There is self collision with robot, res=%d \n", res);
  ////return 0;
 }

kcd_with_report=0;
  res = p3d_col_test_robot(cur_rob,2);
		//printf("collision 2:   %i \n",res2);
      set_kcd_which_test(P3D_KCD_ROB_ALL);
      res = p3d_col_does_robot_collide(cur_rob->num, p3d_numcoll);
 //// res = p3d_col_test_self_collision(cur_rob->num, p3d_numcoll);
 if(res>0)
 {
  printf(" There is collision with robot, res=%d \n", res);
  ////return 0;
 }
 */
/*
 if(ONLINE_TRACKING_FLAG==0)
 initialize_surfaces_in_env(); // AKP Note: Comment it if using motion capture button
*/

 
////int HRP2_state=HRP2_CURRENT_STATE; //AKP : 1 is sitting, 2 is standing 
////update_robots_status();

////printf(" Before update_3D_grid_for_Mightability_Maps_new() \n");
////////////ChronoPrint("***");
////update_3d_grid_for_Mightability_Maps();
int expansion=1;
update_3D_grid_for_Mightability_Maps_new(grid_around_HRP2.GRID_SET,expansion,HRP2_GIK_MANIP);

////if(HUMAN_HAS_MOVED==1)
 ////{
int cell_x,cell_y,cell_z;
  for(cell_x=0; cell_x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx; cell_x++)
  {
     for(cell_y=0; cell_y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny; cell_y++)
   {
       for(cell_z=0; cell_z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz; cell_z++)
    {
     ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human=0;
     if(HUMAN_HAS_MOVED==0&&NEED_HUMAN_VISIBILITY_UPDATE==1)
     {
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_neck_turn=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_straight_head_orientation=0;

      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_standing_human=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_standing_human_neck_turn=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_straight_head_orientation_standing=0;
      
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human_straight_head_orientation=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human_neck_turn=0;

      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_standing_human=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_standing_human_neck_turn=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human_straight_head_orientation_standing=0;
       
     }
      
     if(HUMAN_HAS_MOVED==1)
     {  
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_neck_turn=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_straight_head_orientation=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_standing_human=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_standing_human_neck_turn=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_human_straight_head_orientation_standing=0;

     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_human_LHand=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_human_RHand=0;
     
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_bending=0;  
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_bending=0;  

     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_turning_around_bending=0; 
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_turning_around_bending=0;
     
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human_straight_head_orientation=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human_neck_turn=0;

      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_standing_human=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_standing_human_neck_turn=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_human_straight_head_orientation_standing=0;

     NEED_HUMAN_VISIBILITY_UPDATE=1; //To update visibility also
     }

     if(HRP2_HAS_MOVED==0&&NEED_HRP2_VISIBILITY_UPDATE==1)
     {
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2_neck_turn=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2_straight_head_orientation=0;

      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2_straight_head_orientation=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2_neck_turn=0;

    
      
     }
 
     if(HRP2_HAS_MOVED==1)
     {  
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2_neck_turn=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_HRP2_straight_head_orientation=0;

     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_HRP2_LHand=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_HRP2_RHand=0;
     

      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2_straight_head_orientation=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2_neck_turn=0;

     NEED_HRP2_VISIBILITY_UPDATE=1; //To update visibility also
     ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_bending=0;  
     ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_bending=0;  

     ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_turning_around_bending=0; 
     ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_turning_around_bending=0;
     }
    

     if(JIDO_HAS_MOVED==0&&NEED_JIDO_VISIBILITY_UPDATE==1)
     {
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_JIDO=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_JIDO_neck_turn=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_JIDO_straight_head_orientation=0;

      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO_straight_head_orientation=0;
      grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO_neck_turn=0;
     }

     if(JIDO_HAS_MOVED==1)
     {  
     
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_JIDO=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_JIDO_neck_turn=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.visible_by_JIDO_straight_head_orientation=0;

     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_JIDO_Hand=0;

     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO_straight_head_orientation=0;
     grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO_neck_turn=0;
     
     NEED_JIDO_VISIBILITY_UPDATE=1; //To update visibility also
     ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_bending=0;  
     ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_bending=0;  

     ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_LHand_by_turning_around_bending=0; 
     ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_Map.reachable_by_RHand_by_turning_around_bending=0;
     }
    }
   }
  }   
//// }
//////////ChronoPrint("Time for update_3D_grid_for_Mightability_Maps_new()");
//////////printf(" After update_3D_grid_for_Mightability_Maps_new() \n");
double tu,ts;
ChronoTimes(&tu,&ts);
//////////printf(" tu=%lf, ts=%lf \n",tu,ts);
total_time+=tu;//In sec
ChronoOff();

//////////printf(" **** 3D grid dimension is (%d x %d x %d) cells.  \n",grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx,grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny,grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz);

double cur_h_angle;
double interval=grid_around_HRP2.GRID_SET->pace;
double yaw;
double pitch;
double orig_pan;
double orig_tilt;
double fixed_pitch=M_PI/8.0;

//////////printf(" NEED_HUMAN_VISIBILITY_UPDATE=%d\n",NEED_HUMAN_VISIBILITY_UPDATE);
/*
//////////////////////tmp for jido
ChronoOn();
grid_3d_affordance_calculated=1; 


////p3d_rob *cur_rob=ACBTSET->robot;
cur_h_angle=cur_rob->cam_h_angle;
interval=grid_around_HRP2.GRID_SET->pace;
printf("cur_h_angle=%lf\n",cur_h_angle);
no_FOV_end_point_vertices=0;
while(cur_rob->cam_h_angle>0.001)
{ 
cur_rob->cam_h_angle-=interval;
 
get_points_on_FOV_screen(cur_rob);
}
cur_rob->cam_h_angle=cur_h_angle;
////ChronoOn();
update_3d_grid_visibility(3); //2 for JIDO

ChronoOff();

update_3d_grid_reachability_for_JIDO_new();


return 1;
///////////////////////end tmp for jido
*/

if(NEED_HUMAN_VISIBILITY_UPDATE==1)
 {

ChronoOn();

//*****AKP: Tmp uncomment/comment because of problem in new human model for sitting as well as standing
virtually_update_human_state_new(1);// Sitting


cur_h_angle=human->cam_h_angle;
//////////printf(" cur_h_angle=%lf\n",cur_h_angle);

//int i_h_a=0;
////////interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}

human->cam_h_angle=cur_h_angle;
////int expansion=1; //To populate the number of cells, around a cell having obstacle, with obstacles also
////create_exact_obstacles_for_HRP2_GIK_manip(grid_around_HRP2.GRID_SET,expansion,BT_AFFORDANCE_VISIBILITY); 
////ChronoOn();	    

update_3d_grid_visibility(1); //1 for human

//////////ChronoPrint("TIME of 3D Visibility calculation for sitting Human from current position");

////return 1;

//Now making the head straight
configPt hum_cur_pos = MY_ALLOC(double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof); /* Allocation of temporary robot configuration */

 p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);


double yaw=0.0;
double pitch=fixed_pitch;
double orig_pan=hum_cur_pos[HUMANq_PAN];
double orig_tilt=hum_cur_pos[HUMANq_TILT];
//////////printf(" Original pan = %lf, tilt= %lf \n",orig_pan, orig_tilt); 

hum_cur_pos[HUMANq_PAN]=yaw; // Human Yaw/pan angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=pitch; // Human pitch/tilt angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

cur_h_angle=human->cam_h_angle;
//////////printf(" cur_h_angle=%lf\n",cur_h_angle);
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}
update_3d_grid_straight_visibility(1); //1 for human
human->cam_h_angle=cur_h_angle;

hum_cur_pos[HUMANq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];


//Now turning the head only
//////configPt hum_cur_pos = MY_ALLOC(double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof); /* Allocation of temporary robot configuration */

 //////p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);


yaw=M_PI/3.0;
pitch=fixed_pitch;
//////orig_pan=hum_cur_pos[HUMANq_PAN];
//////printf(" Original pan = %lf \n",orig_pan); 
hum_cur_pos[HUMANq_PAN]=yaw; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=pitch; // Human pitch/tilt angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

cur_h_angle=human->cam_h_angle;
//////////printf(" cur_h_angle=%lf\n",cur_h_angle);
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}
update_3d_grid_visibility_by_neck_turn(1); //1 for human
human->cam_h_angle=cur_h_angle;

hum_cur_pos[HUMANq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);

hum_cur_pos[HUMANq_PAN]=-yaw; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=pitch; // Human pitch/tilt angle relative to the human body frame

p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

cur_h_angle=human->cam_h_angle;
//////////printf(" cur_h_angle=%lf\n",cur_h_angle);
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}
update_3d_grid_visibility_by_neck_turn(1); //1 for human
human->cam_h_angle=cur_h_angle;

hum_cur_pos[HUMANq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

MY_FREE(hum_cur_pos,double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof);

////double tu,ts;
//////////ChronoPrint("TIME for all 3D Visibility calculation for sitting Human");
ChronoTimes(&tu,&ts);
//////////printf(" tu=%lf, ts=%lf \n",tu,ts);
total_time+=tu;//In sec
ChronoOff();
 
////return 1;
#ifdef MM_FOR_VIRTUALLY_STANDING_HUMAN
///////////////3D visibility calculation for making human virtually standing//////////////////////////

ChronoOn();
virtually_update_human_state_new(0);// Standing
cur_h_angle=human->cam_h_angle;
//////////printf(" cur_h_angle for standing human=%lf\n",cur_h_angle);
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}

human->cam_h_angle=cur_h_angle;
////int expansion=1; //To populate the number of cells, around a cell having obstacle, with obstacles also
////create_exact_obstacles_for_HRP2_GIK_manip(grid_around_HRP2.GRID_SET,expansion,BT_AFFORDANCE_VISIBILITY); 
////ChronoOn();	    

update_3d_grid_visibility_standing(1); //1 for human

////////////ChronoPrint("TIME of 3D Visibility calculation for standing Human from current position");

//Now making the head straight
hum_cur_pos = MY_ALLOC(double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof);

 p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);


yaw=0.0;
pitch=fixed_pitch;

orig_pan=hum_cur_pos[HUMANq_PAN];
orig_tilt=hum_cur_pos[HUMANq_TILT];
//////////printf(" Original pan = %lf \n",orig_pan); 
hum_cur_pos[HUMANq_PAN]=yaw; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=pitch; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

cur_h_angle=human->cam_h_angle;
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}
update_3d_grid_straight_visibility_standing(1); //1 for human
human->cam_h_angle=cur_h_angle;

hum_cur_pos[HUMANq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];



//Now turning the head only
//////hum_cur_pos = MY_ALLOC(double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof);

 //////p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);


yaw=M_PI/3.0;
pitch=fixed_pitch;
//////orig_pan=hum_cur_pos[HUMANq_PAN];
//////printf(" Original pan = %lf \n",orig_pan); 
hum_cur_pos[HUMANq_PAN]=yaw; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=pitch; // Human Yaw angle relative to the human body frame

p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

cur_h_angle=human->cam_h_angle;
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}
update_3d_grid_visibility_by_neck_turn_standing(1); //1 for human
human->cam_h_angle=cur_h_angle;

hum_cur_pos[HUMANq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_PAN]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);

hum_cur_pos[HUMANq_PAN]=-yaw; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=pitch; // Human Yaw angle relative to the human body frame

p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

cur_h_angle=human->cam_h_angle;
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(human->cam_h_angle>0.001)
{ 
human->cam_h_angle-=interval;
get_points_on_FOV_screen(human);
}
update_3d_grid_visibility_by_neck_turn_standing(1); //1 for human
human->cam_h_angle=cur_h_angle;

hum_cur_pos[HUMANq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
hum_cur_pos[HUMANq_TILT]=orig_tilt; // Human Yaw angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt, hum_cur_pos);
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_PAN]=hum_cur_pos[HUMANq_PAN];
ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[HUMANq_TILT]=hum_cur_pos[HUMANq_TILT];

MY_FREE(hum_cur_pos,double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof);


//////////ChronoPrint("TIME for all 3D Visibility calculation for standing Human");
ChronoTimes(&tu,&ts);
////printf(" %lf, %lf \n",tu,ts);
virtually_update_human_state_new(1);// Again make it to Sitting
ChronoTimes(&tu,&ts);
//////////printf(" tu=%lf, ts=%lf \n",tu,ts);
total_time+=tu;//In sec
ChronoOff();

NEED_HUMAN_VISIBILITY_UPDATE=0;
///////////////END of 3D visibility calculation for making human virtually standing//////////////////////////
#endif

 }//END if(NEED_HUMAN_VISIBILITY_UPDATE==1)
////////return 1;



if(NEED_HRP2_VISIBILITY_UPDATE==1||NEED_JIDO_VISIBILITY_UPDATE==1)
 {
//////////printf(" Need visibility update for robot\n");
ChronoOn();
grid_3d_affordance_calculated=1; 


////p3d_rob *cur_rob=ACBTSET->robot;
cur_h_angle=cur_rob->cam_h_angle;
no_FOV_end_point_vertices=0;
while(cur_rob->cam_h_angle>0.001)
{ 
cur_rob->cam_h_angle-=interval;
get_points_on_FOV_screen(cur_rob);
}
cur_rob->cam_h_angle=cur_h_angle;
////ChronoOn();
#ifdef HRI_JIDO
update_3d_grid_visibility(3); //3 for JIDO
#elif defined(HRI_HRP2)
update_3d_grid_visibility(2); //2 for HRP2
#endif


//Now making the head straight
configPt rob_cur_pos = MY_ALLOC(double,ACBTSET->robot->nb_dof); /* Allocation of temporary robot configuration */

p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos);

orig_pan=rob_cur_pos[ROBOTq_PAN];
orig_tilt=rob_cur_pos[ROBOTq_TILT];
yaw=0.0;
pitch=fixed_pitch;
//////////printf(" Original pan = %lf, tilt = %lf \n",orig_pan, orig_tilt); 

rob_cur_pos[ROBOTq_PAN]=yaw; // Human Yaw angle relative to the human body frame
rob_cur_pos[ROBOTq_TILT]=pitch; // Human Pitch angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->robot, rob_cur_pos);
ACBTSET->robot->ROBOT_POS[ROBOTq_PAN]=rob_cur_pos[ROBOTq_PAN];
ACBTSET->robot->ROBOT_POS[ROBOTq_TILT]=rob_cur_pos[ROBOTq_TILT];

cur_h_angle=cur_rob->cam_h_angle;
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(cur_rob->cam_h_angle>0.001)
{ 
cur_rob->cam_h_angle-=interval;
get_points_on_FOV_screen(cur_rob);
}

#ifdef HRI_JIDO
update_3d_grid_straight_visibility(3); //3 for JIOD
#elif defined(HRI_HRP2)
update_3d_grid_straight_visibility(2); //2 for HRP2
#endif
cur_rob->cam_h_angle=cur_h_angle;

rob_cur_pos[ROBOTq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
rob_cur_pos[ROBOTq_TILT]=orig_tilt; // Human Pitch angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->robot, rob_cur_pos);
ACBTSET->robot->ROBOT_POS[ROBOTq_PAN]=rob_cur_pos[ROBOTq_PAN];
ACBTSET->robot->ROBOT_POS[ROBOTq_TILT]=rob_cur_pos[ROBOTq_TILT];

p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos);

//Now turning the head only
////configPt rob_cur_pos = MY_ALLOC(double,ACBTSET->robot->nb_dof); /* Allocation of temporary robot configuration */

 ////p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos);


yaw=M_PI/4.0;
pitch=fixed_pitch;
////orig_pan=rob_cur_pos[ROBOTq_PAN];
////printf(" Original pan = %lf \n",orig_pan); 
rob_cur_pos[ROBOTq_PAN]=yaw; // Human Yaw angle relative to the human body frame
rob_cur_pos[ROBOTq_TILT]=pitch; // Human Pitch angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->robot, rob_cur_pos);
ACBTSET->robot->ROBOT_POS[ROBOTq_PAN]=rob_cur_pos[ROBOTq_PAN];
ACBTSET->robot->ROBOT_POS[ROBOTq_TILT]=rob_cur_pos[ROBOTq_TILT];

cur_h_angle=cur_rob->cam_h_angle;
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(cur_rob->cam_h_angle>0.001)
{ 
cur_rob->cam_h_angle-=interval;
get_points_on_FOV_screen(cur_rob);
}

#ifdef HRI_JIDO
update_3d_grid_visibility_by_neck_turn(3); //3 for JIOD
#elif defined(HRI_HRP2)
update_3d_grid_visibility_by_neck_turn(2); //2 for HRP2
#endif

cur_rob->cam_h_angle=cur_h_angle;

rob_cur_pos[ROBOTq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
rob_cur_pos[ROBOTq_TILT]=orig_tilt; // Human Pitch angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->robot, rob_cur_pos);
ACBTSET->robot->ROBOT_POS[ROBOTq_PAN]=rob_cur_pos[ROBOTq_PAN];
ACBTSET->robot->ROBOT_POS[ROBOTq_TILT]=rob_cur_pos[ROBOTq_TILT];

p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos);

rob_cur_pos[ROBOTq_PAN]=-yaw; // Human Yaw angle relative to the human body frame
rob_cur_pos[ROBOTq_TILT]=pitch; // Human Pitch angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->robot, rob_cur_pos);

ACBTSET->robot->ROBOT_POS[ROBOTq_PAN]=rob_cur_pos[ROBOTq_PAN];
ACBTSET->robot->ROBOT_POS[ROBOTq_TILT]=rob_cur_pos[ROBOTq_TILT];

cur_h_angle=cur_rob->cam_h_angle;
//int i_h_a=0;
interval=grid_around_HRP2.GRID_SET->pace;
no_FOV_end_point_vertices=0;
while(cur_rob->cam_h_angle>0.001)
{ 
cur_rob->cam_h_angle-=interval;
get_points_on_FOV_screen(cur_rob);
}
#ifdef HRI_JIDO
update_3d_grid_visibility_by_neck_turn(3); //3 for JIOD
#elif defined(HRI_HRP2)
update_3d_grid_visibility_by_neck_turn(2); //2 for HRP2
#endif
//////////update_3d_grid_visibility_by_neck_turn(2); //2 for HRP2
cur_rob->cam_h_angle=cur_h_angle;

rob_cur_pos[ROBOTq_PAN]=orig_pan; // Human Yaw angle relative to the human body frame
rob_cur_pos[ROBOTq_TILT]=orig_tilt; // Human Pitch angle relative to the human body frame
p3d_set_and_update_this_robot_conf(ACBTSET->robot, rob_cur_pos);
ACBTSET->robot->ROBOT_POS[ROBOTq_PAN]=rob_cur_pos[ROBOTq_PAN];
ACBTSET->robot->ROBOT_POS[ROBOTq_TILT]=rob_cur_pos[ROBOTq_TILT];

MY_FREE(rob_cur_pos,double,ACBTSET->robot->nb_dof);


//////////ChronoPrint("TIME of 3D Visibility calculation for ROBOT");
////ChronoOff();
////int HRP2_state=HRP2_CURRENT_STATE; //AKP : 1 is sitting, 2 is standing 
////create_HRP2_robot(HRP2_state);
////g3d_draw_env();
//// fl_check_forms();
//// g3d_draw_allwin_active();

ChronoTimes(&tu,&ts);
//////////printf(" tu=%lf, ts=%lf \n",tu,ts);
total_time+=tu;//In sec
ChronoOff();

 NEED_HRP2_VISIBILITY_UPDATE=0;
 NEED_JIDO_VISIBILITY_UPDATE=0;

 }

 
ChronoOn();
//// create_HRP2_robot(HRP2_state);

#ifdef HRI_JIDO
if(JIDO_HAS_MOVED==1)
 {
 update_3d_grid_reachability_for_JIDO_new();
 JIDO_HAS_MOVED=0;
 }
#elif defined(HRI_HRP2)
if(HRP2_HAS_MOVED==1)
 {
  update_3d_grid_reachability_for_HRP2_new();
  HRP2_HAS_MOVED=0;
 }
#endif
//////////update_3d_grid_reachability_for_HRP2_new();
//////////ChronoPrint("****TIME of 3D reachability calculation for ROBOT\n");

ChronoTimes(&tu,&ts);
//////////printf(" tu=%lf, ts=%lf \n",tu,ts);
total_time+=tu;//In sec

////return 1;
ChronoOff();

ChronoOn();

if(HUMAN_HAS_MOVED==1)
 {
 update_3d_grid_reachability_for_human_new();
 
 #ifdef MM_FOR_VIRTUALLY_STANDING_HUMAN
 
 virtually_update_human_state_new(0);// Standing
update_3d_grid_reachability_for_human_standing_new();
 virtually_update_human_state_new(1);
 #endif

 HUMAN_HAS_MOVED=0;
 }

//////////ChronoPrint("****TIME of 3D reachability calculation for human\n");
 
ChronoTimes(&tu,&ts);
//////////printf(" tu=%lf, ts=%lf \n",tu,ts);
total_time+=tu;//In sec

ChronoOff();

//////////printf(" <<<<<<<<<< Total Time for updating all the Mightability Maps=%lf s >>>>>>>>>>\n",total_time);
/*
  configPt visq;
 visq= MY_ALLOC(double,ACBTSET->visball->nb_dof); 
 p3d_get_robot_config_into(ACBTSET->visball,&visq);
 visq[6]=0.0;
 visq[7]=0.0;
 visq[8]=0.0;
  
 p3d_set_and_update_this_robot_conf(ACBTSET->visball, visq);
*/

  ACBTSET->visball->joints[1]->abs_pos[0][3]=0;
  ACBTSET->visball->joints[1]->abs_pos[1][3]=0;
  ACBTSET->visball->joints[1]->abs_pos[2][3]=0;

return 1;

//return 1;
/*
ChronoPrint("Time before calculating affordance on surfaces ");
 //current_surface_index=0;
 int i=0;
 for(i=0;i<curr_surfaces_in_env.total_no_of_surfaces;i++)
 {
 printf(" For surface %d\n",i);
 ChronoPrint("Time Before update_surface_grid_based_on_curr_pos");
 update_surface_grid_based_on_curr_pos(surf_grid_samp_rate, &curr_surfaces_in_env.flat_surf[i]);

 ChronoPrint("Time Before update_surface_grid_by_bending_human_at_curr_pos");
 update_surface_grid_by_bending_human_at_curr_pos(surf_grid_samp_rate, &curr_surfaces_in_env.flat_surf[i]);

 ChronoPrint("Time Before update_surface_grid_by_turning_human_at_curr_pos");
 update_surface_grid_by_turning_human_at_curr_pos(surf_grid_samp_rate, &curr_surfaces_in_env.flat_surf[i]);
 
 

 
 
 HRP2_state=HRP2_CURRENT_STATE; //AKP : 1 is sitting, 2 is standing 
 //////ChronoPrint(" Time before create_HRP2_robot");
 //////create_HRP2_robot(HRP2_state);
 //////ChronoPrint(" Time after create_HRP2_robot and before update_surface_grid_for_HRP2_without_GIK");
 ////update_surface_grid_for_HRP2_with_GIK(surf_grid_samp_rate, &curr_surfaces_in_env.flat_surf[i]);
 update_surface_grid_for_HRP2_without_GIK(surf_grid_samp_rate, &curr_surfaces_in_env.flat_surf[i]);
 ChronoPrint(" Time after update_surface_grid_for_HRP2_without_GIK");

 }
*/
////ChronoPrint("TIME for all affordance calculation");
////ChronoOff();
////create_3d_grid_for_HRP2_GIK();
////update_3D_grid_based_on_current_human_pos();
//// grid_3d_affordance_calculated=1; 
 
 //update_surface_grid(surf_grid_samp_rate, &curr_surfaces_in_env.flat_surf[1]);
}



// AKP: To draw a cylinder
void g3d_draw_vertical_cylinder(double x, double y, double z, double radius, double height, double slice_distance, int color, double *color_vect)
{
 double i=z;
 for(;i<z+height; i+=slice_distance)
 {
 g3d_drawDisc(x, y, i, radius, color, color_vect);
 }
}


//AKP : To show the different affordance values 
int show_affordance()
{

int show_affordance_type=2;//1 for showing by cylinders, 2 for drwaing vertical line and a circle at top


    int HUMANj_WAIST=7;
    int HUMANj_HIP=34;
    // Reachability Test by right hand
    double hum_waist_pos_x = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_BODY]->abs_pos[0][3]; // AKP: In the abs_pos[4][4] matrix the x,y,z are stored at indices [0][3], [1][3], [2][3] respectively
    double hum_waist_pos_y = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_BODY]->abs_pos[1][3]; // AKP:
    double hum_waist_pos_z = ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_BODY]->abs_pos[2][3]; // AKP:

    ////g3d_drawDisc(hum_waist_pos_x, hum_waist_pos_y, hum_waist_pos_z, 0.5, 0.5, NULL);   

int current_surface_index=0;
for(;current_surface_index<curr_surfaces_in_env.total_no_of_surfaces;current_surface_index++)
{

int no_vert=0;
for(;no_vert<curr_surfaces_in_env.flat_surf[current_surface_index].no_vertices-1;no_vert++)
 {
  g3d_drawOneLine(curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].x,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].y,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].z,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert+1].x,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert+1].y,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert+1].z,4,NULL);
 }
g3d_drawOneLine(curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].x,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].y,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].z,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[0].x,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[0].y,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[0].z,4,NULL);


////printf(" Inside show_affordance grid_i_max =%d , grid_j_max = %d \n", curr_surfaces_in_env.flat_surf[current_surface_index].grid_i_max, curr_surfaces_in_env.flat_surf[current_surface_index].grid_j_max);
 int i=0;
 for(;i<curr_surfaces_in_env.flat_surf[current_surface_index].grid_i_max;i++)
 {
  int j=0;
  int show_cell=0;
  int reachable_by_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_HRP2_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int visible_by_human=0;
  int visible_by_HRP2=0;
  //printf("\n");
  for(;j<curr_surfaces_in_env.flat_surf[current_surface_index].grid_j_max;j++)
  {
   show_cell=0;
   reachable_by_hand=0;
   reachable_by_HRP2_hand=0;
   double x=i*surf_grid_samp_rate;
   double y=j*surf_grid_samp_rate;
   x+=curr_surfaces_in_env.flat_surf[current_surface_index].BR_x_min;
   y+=curr_surfaces_in_env.flat_surf[current_surface_index].BR_y_min;
   double z=curr_surfaces_in_env.flat_surf[current_surface_index].BR_z;

   double cur_z_st=z;
   double height=0.1;//0.3;
   double cur_z_end=z;
   //double cur_z_end=;
   //printf("%d ", curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].visible);
   ////printf("SHOW_VISIBLE_PLACE=%d\n",SHOW_VISIBLE_PLACE);
   if(SHOW_2D_VISIBLE_PLACE_HUM==1)
   {
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].visible==1)
    {
 
   //g3d_draw_line_with_width(x,y,z,x,y,z+0.2, 6, 3, NULL);
   //cur_z_st+=cur_z_end;
   //cur_z_end+=cur_z_st;
    show_cell=1;
    visible_by_human=1;
     ////printf(" Cell %d,%d is visible \n",i,j);
    ////g3d_drawDisc(x, y, z+0.01, surf_grid_samp_rate/2.0, 3, NULL);
    
   
   if(show_affordance_type==1)
     {
  
   g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0, height, 0.01, 3, NULL);
   cur_z_st+=height;
     }
   cur_z_end+=height;
    
    }
   }
   if(SHOW_2D_VISIBLE_PLACES_FOR_HRP2==1)
   {
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].visible_by_HRP2==1)
    {
 
   //g3d_draw_line_with_width(x,y,z,x,y,z+0.2, 6, 3, NULL);
   //cur_z_st+=cur_z_end;
   //cur_z_end+=cur_z_st;
    show_cell=1;
    //g3d_drawDisc(x, y, z+0.01, surf_grid_samp_rate/2.0, 3, NULL);
    
   
   if(show_affordance_type==1)
     {
  
   g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0, height, 0.01, 3, NULL);
   cur_z_st+=height;
     }
   cur_z_end+=height;
    
    }
   }
   if(SHOW_2D_DIRECT_REACHABLE_HRP2==1)
   {
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_HRP2_RHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
     show_cell=1;
     reachable_by_HRP2_hand=1;
    //// g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
    if(show_affordance_type==1)
     {
   
    g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0-0.0075, height, 0.01, 4, NULL);
    cur_z_st+=height;
     }
     cur_z_end+=height;
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_HRP2_LHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
     show_cell=1;
     if(reachable_by_HRP2_hand==1)//Already reachable by right hand
     reachable_by_HRP2_hand=3; //Reachable by both hands
     else
     reachable_by_HRP2_hand=2; //Reachable by left hand only
  
   ////  g3d_drawDisc(x, y, z+0.03, surf_grid_samp_rate/2.0-0.015, 6, NULL);
     if(show_affordance_type==1)
     {
    
    g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0-0.015, height, 0.01, 6, NULL);
    cur_z_st+=height;
     }
     cur_z_end+=height;
    }
   }
   if(SHOW_2D_DIRECT_REACHABLE_HUM==1)
   {
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
     show_cell=1;
     reachable_by_hand=1;
    //// g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
    if(show_affordance_type==1)
     {
   
    g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0-0.0075, height, 0.01, 4, NULL);
    cur_z_st+=height;
     }
     cur_z_end+=height;
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
     show_cell=1;
     if(reachable_by_hand==1)//Already reachable by right hand
     reachable_by_hand=3; //Reachable by both hands
     else
     reachable_by_hand=2; //Reachable by left hand only
  
   ////  g3d_drawDisc(x, y, z+0.03, surf_grid_samp_rate/2.0-0.015, 6, NULL);
     if(show_affordance_type==1)
     {
    
    g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0-0.015, height, 0.01, 6, NULL);
    cur_z_st+=height;
     }
     cur_z_end+=height;
    }
   }
   if(SHOW_2D_BENDING_REACHABLE_HUM==1)
   {
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand_by_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
     show_cell=1;
      reachable_by_hand=1;
    //// g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
     if(show_affordance_type==1)
     {
   
    g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0-0.0075, height, 0.01, 5, NULL);
    cur_z_st+=height;
     }
     cur_z_end+=height;
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand_by_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
     show_cell=1;
     if(reachable_by_hand==1)//Already reachable by right hand
     reachable_by_hand=3; //Reachable by both hands
     else
     reachable_by_hand=2; //Reachable by left hand only

   //// g3d_drawDisc(x, y, z+0.03, surf_grid_samp_rate/2.0-0.015, 6, NULL);
    if(show_affordance_type==1)
     {
    
    g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0-0.015, height, 0.01, 2, NULL);
    cur_z_st+=height;
     }
     cur_z_end+=height;
    }
   }
   if(SHOW_2D_TURNING_AROUND_REACHABLE_HUM==1)
   {
  /* if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand_by_turning_around==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
    g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
    g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0-0.0075, height, 0.01, 5, NULL);
    cur_z_st+=height;
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand_by_turning_around==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
    g3d_drawDisc(x, y, z+0.03, surf_grid_samp_rate/2.0-0.015, 6, NULL);
    g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0-0.015, height, 0.01, 2, NULL);
    cur_z_st+=height;
    }
   */
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand_by_turning_around_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
    show_cell=1;
  reachable_by_hand=1;
 ////   g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
    if(show_affordance_type==1)
     {
    
    g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0-0.0075, height, 0.01, 5, NULL);
    cur_z_st+=height;
     }
    cur_z_end+=height;
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand_by_turning_around_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
    show_cell=1;

      if(reachable_by_hand==1)//Already reachable by right hand
     reachable_by_hand=3; //Reachable by both hands
     else
     reachable_by_hand=2; //Reachable by left hand only

  ////  g3d_drawDisc(x, y, z+0.03, surf_grid_samp_rate/2.0-0.015, 6, NULL);
    if(show_affordance_type==1)
     {
    
    g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0-0.015, height, 0.01, 2, NULL);
    cur_z_st+=height;
     }
    cur_z_end+=height;
    }
   }
  if(show_affordance_type==2)//By line
   {
    if(show_cell==1)
    { 
    ////cur_z_end=z+0.01;
    if(visible_by_human==1)
     {
    ////printf(" Cell %d,%d is visible \n",i,j);
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 3, NULL);
    
     }
    cur_z_end=z+0.01;
    g3d_drawOneLine(x,y,z+0.01,x,y,cur_z_end,4,NULL);
    if(reachable_by_hand==1)
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 1, NULL);
    else
     {
     if(reachable_by_hand==2)
      {
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 2, NULL);
      }
     else
      {
     if(reachable_by_hand==3) //Reachable by both hands
       { 
     g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 4, NULL);
       }
     //else
     //g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 3, NULL);
      }
     }
    cur_z_end=z+0.01;
   // g3d_drawOneLine(x,y,z+0.01,x,y,cur_z_end,4,NULL);
    if(reachable_by_HRP2_hand==1)
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 1, NULL);
    else
     {
     if(reachable_by_HRP2_hand==2)
      {
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 2, NULL);
      }
     else
      {
     if(reachable_by_HRP2_hand==3) //Reachable by both hands
       { 
     g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 4, NULL);
       }
     //else
     //g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 3, NULL);
      }
     }
    }
   
   }
  }
 }
}//END for(;current_surface_index<curr_surfaces_in_env.total_no_of_surfaces;current_surface_index++)
/*
int no_path_pts=0;
for(no_path_pts=0;no_path_pts<cur_manipulation_path.total_no_pts;no_path_pts++)
 {
 g3d_drawDisc(cur_manipulation_path.path_points[no_path_pts].x, cur_manipulation_path.path_points[no_path_pts].y, cur_manipulation_path.path_points[no_path_pts].z, 0.01, 4, NULL);
 }
*/
return 1;
}

//AKP : To show the different affordance values 
int show_affordance_new()
{
no_candidate_points_to_put=0;

int current_surface_index=0;
for(;current_surface_index<curr_surfaces_in_env.total_no_of_surfaces;current_surface_index++)
{

int no_vert=0;
for(;no_vert<curr_surfaces_in_env.flat_surf[current_surface_index].no_vertices-1;no_vert++)
 {
  g3d_drawOneLine(curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].x,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].y,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].z,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert+1].x,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert+1].y,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert+1].z,4,NULL);
 }
g3d_drawOneLine(curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].x,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].y,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].z,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[0].x,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[0].y,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[0].z,4,NULL);


////printf(" Inside show_affordance grid_i_max =%d , grid_j_max = %d \n", curr_surfaces_in_env.flat_surf[current_surface_index].grid_i_max, curr_surfaces_in_env.flat_surf[current_surface_index].grid_j_max);
 int i=0;
 for(;i<curr_surfaces_in_env.flat_surf[current_surface_index].grid_i_max;i++)
 {
  int j=0;
  int show_cell=0;
  int reachable_by_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_HRP2_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_bending=0;
  int reachable_by_turning_around=0;
  int visible_by_human=0;
  int visible_by_HRP2=0;
  //printf("\n");
  for(;j<curr_surfaces_in_env.flat_surf[current_surface_index].grid_j_max;j++)
  {
   show_cell=0;
   reachable_by_hand=0;
   reachable_by_HRP2_hand=0;
   visible_by_human=0;
   visible_by_HRP2=0;
   reachable_by_turning_around=0;
   reachable_by_bending=0;

   double x=i*surf_grid_samp_rate;
   double y=j*surf_grid_samp_rate;
   x+=curr_surfaces_in_env.flat_surf[current_surface_index].BR_x_min;
   y+=curr_surfaces_in_env.flat_surf[current_surface_index].BR_y_min;
   double z=curr_surfaces_in_env.flat_surf[current_surface_index].BR_z;

   double cur_z_st=z;
   double height=0.1;//0.3;
   double cur_z_end=z;
   //double cur_z_end=;
   //printf("%d ", curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].visible);
   ////printf("SHOW_VISIBLE_PLACE=%d\n",SHOW_VISIBLE_PLACE);
    if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].visible==1)
    {
 
   //g3d_draw_line_with_width(x,y,z,x,y,z+0.2, 6, 3, NULL);
   //cur_z_st+=cur_z_end;
   //cur_z_end+=cur_z_st;
    show_cell=1;
    visible_by_human=1;

    }
    if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].visible_by_HRP2==1)
    {
 
   //g3d_draw_line_with_width(x,y,z,x,y,z+0.2, 6, 3, NULL);
   //cur_z_st+=cur_z_end;
   //cur_z_end+=cur_z_st;
    show_cell=1;
    visible_by_HRP2=1;
    }

    if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
     show_cell=1;
     reachable_by_hand=1;
    }
    if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
     show_cell=1;
     if(reachable_by_hand==1)//Already reachable by right hand
     reachable_by_hand=3; //Reachable by both hands
     else
     reachable_by_hand=2; //Reachable by left hand only
  
    }
     if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand_by_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
     show_cell=1;
      reachable_by_bending=1;
    //// g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
    
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand_by_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
     show_cell=1;
     if(reachable_by_bending==1)//Already reachable by right hand
     reachable_by_bending=3; //Reachable by both hands
     else
     reachable_by_bending=2; //Reachable by left hand only
    }
     if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand_by_turning_around_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
    show_cell=1;
  reachable_by_turning_around=1;
 ////   g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
    
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand_by_turning_around_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
    show_cell=1;

      if(reachable_by_turning_around==1)//Already reachable by right hand
     reachable_by_turning_around=3; //Reachable by both hands
     else
     reachable_by_turning_around=2; //Reachable by left hand only
    }

   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_HRP2_RHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
     show_cell=1;
     reachable_by_HRP2_hand=1;
    //// g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
   
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_HRP2_LHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
     show_cell=1;
     if(reachable_by_HRP2_hand==1)//Already reachable by right hand
     reachable_by_HRP2_hand=3; //Reachable by both hands
     else
     reachable_by_HRP2_hand=2; //Reachable by left hand only
    }
    
   if(SHOW_2D_VISIBLE_PLACE_HUM==1&&visible_by_human==1)
    {
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 3, NULL);
    
    }

    if(SHOW_2D_VISIBLE_PLACES_FOR_HRP2==1&&visible_by_HRP2==1)
    {
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 3, NULL);
    
    }

    if(SHOW_2D_COMMON_VISIBLE_HRP2_HUMAN==1&&visible_by_human==1&&visible_by_HRP2==1)
    {
     //printf("SHOW_2D_COMMON_VISIBLE_HRP2_HUMAN=%d, SHOW_3D_COMMON_VISIBLE_HRP2_HUMAN=%d\n",SHOW_2D_COMMON_VISIBLE_HRP2_HUMAN,SHOW_3D_COMMON_VISIBLE_HRP2_HUMAN);
     g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, Green, NULL);
    }

    if(SHOW_2D_DIRECT_REACHABLE_HUM==1)
    {
     if(reachable_by_hand==1)
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 1, NULL);
    else
     {
     if(reachable_by_hand==2)
      {
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 2, NULL);
      }
     else
      {
     if(reachable_by_hand==3) //Reachable by both hands
       { 
     g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 4, NULL);
       }
     //else
     //g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 3, NULL);
      }
     }
    }
     
    if(SHOW_2D_BENDING_REACHABLE_HUM==1)
    {
     if(reachable_by_bending==1)
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 1, NULL);
    else
     {
     if(reachable_by_bending==2)
      {
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 2, NULL);
      }
     else
      {
     if(reachable_by_bending==3) //Reachable by both hands
       { 
     g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 4, NULL);
       }
     //else
     //g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 3, NULL);
      }
     }
    }
   
    if(SHOW_2D_TURNING_AROUND_REACHABLE_HUM==1)
    {
     if(reachable_by_turning_around==1)
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 1, NULL);
    else
     {
     if(reachable_by_turning_around==2)
      {
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 2, NULL);
      }
     else
      {
     if(reachable_by_turning_around==3) //Reachable by both hands
       { 
     g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 4, NULL);
       }
     //else
     //g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 3, NULL);
      }
     }
    }
    
   if(SHOW_2D_DIRECT_REACHABLE_HRP2==1)
    {
     if(reachable_by_HRP2_hand==1)
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 1, NULL);
    else
     {
     if(reachable_by_HRP2_hand==2)
      {
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 2, NULL);
      }
     else
      {
     if(reachable_by_HRP2_hand==3) //Reachable by both hands
       { 
     g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 4, NULL);
       }
     //else
     //g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 3, NULL);
      }
     }
    }
    
   if(SHOW_2D_COMMON_REACH_HRP2_HUMAN==1)
    {
    
    if(reachable_by_HRP2_hand==3&&(reachable_by_hand==3||reachable_by_bending==3||reachable_by_turning_around==3)) //Reachable by both hands of both human and HRP2
     {
     g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, Green, NULL);
     }
    else
     {
    if((reachable_by_HRP2_hand==1||reachable_by_HRP2_hand==2)&&(reachable_by_hand==1||reachable_by_bending==1||reachable_by_turning_around==1||reachable_by_hand==2||reachable_by_bending==2||reachable_by_turning_around==2))
      {
        g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, Red, NULL);
      }
     else
      {
       if((reachable_by_HRP2_hand==3)&&(reachable_by_hand==1||reachable_by_bending==1||reachable_by_turning_around==1||reachable_by_hand==2||reachable_by_bending==2||reachable_by_turning_around==2))
       {
        g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, Yellow, NULL);
       }
       else
       {
        if((reachable_by_HRP2_hand==1||reachable_by_HRP2_hand==2)&&(reachable_by_hand==3||reachable_by_bending==3||reachable_by_turning_around==3))
        {
        g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, Blue, NULL);
        }
       } 
      }
     }
    } 
   
    if(SHOW_HRP2_HUMAN_COMMON_REACHABLE_VISIBLE==1)
    {
    if(visible_by_HRP2==1&&visible_by_human==1)
     {
    if(reachable_by_HRP2_hand==3&&(reachable_by_hand==3||reachable_by_bending==3||reachable_by_turning_around==3)) //Reachable by both hands of both human and HRP2
      {
     g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 4, NULL);
     
     candidate_points_to_put.point[no_candidate_points_to_put].x=x;
     candidate_points_to_put.point[no_candidate_points_to_put].y=y;
     candidate_points_to_put.point[no_candidate_points_to_put].z=z;
     candidate_points_to_put.no_points++;
     no_candidate_points_to_put++;
     
      }
    if((reachable_by_HRP2_hand==1||reachable_by_HRP2_hand==2)&&(reachable_by_hand==1||reachable_by_bending==1||reachable_by_turning_around==1||reachable_by_hand==2||reachable_by_bending==2||reachable_by_turning_around==2))
      {
     g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 2, NULL);
     
     candidate_points_to_put.point[no_candidate_points_to_put].x=x;
     candidate_points_to_put.point[no_candidate_points_to_put].y=y;
     candidate_points_to_put.point[no_candidate_points_to_put].z=z;
     candidate_points_to_put.no_points++;
     no_candidate_points_to_put++;
      }
     } 
    }

  
   
   
  }
 }
}//END for(;current_surface_index<curr_surfaces_in_env.total_no_of_surfaces;current_surface_index++)
/*
int no_path_pts=0;
for(no_path_pts=0;no_path_pts<cur_manipulation_path.total_no_pts;no_path_pts++)
 {
 g3d_drawDisc(cur_manipulation_path.path_points[no_path_pts].x, cur_manipulation_path.path_points[no_path_pts].y, cur_manipulation_path.path_points[no_path_pts].z, 0.01, 4, NULL);
 }
*/

}


/*
int show_common_reachable_places()
{


int current_surface_index=0;
for(;current_surface_index<curr_surfaces_in_env.total_no_of_surfaces;current_surface_index++)
{

int no_vert=0;
for(;no_vert<curr_surfaces_in_env.flat_surf[current_surface_index].no_vertices-1;no_vert++)
 {
  g3d_drawOneLine(curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].x,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].y,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].z,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert+1].x,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert+1].y,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert+1].z,4,NULL);
 }
g3d_drawOneLine(curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].x,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].y,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[no_vert].z,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[0].x,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[0].y,curr_surfaces_in_env.flat_surf[current_surface_index].vertices[0].z,4,NULL);


////printf(" Inside show_affordance grid_i_max =%d , grid_j_max = %d \n", curr_surfaces_in_env.flat_surf[current_surface_index].grid_i_max, curr_surfaces_in_env.flat_surf[current_surface_index].grid_j_max);
 int i=0;
 for(;i<curr_surfaces_in_env.flat_surf[current_surface_index].grid_i_max;i++)
 {
  int j=0;
  int show_cell=0;
  int reachable_by_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_HRP2_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;

  //printf("\n");
  for(;j<curr_surfaces_in_env.flat_surf[current_surface_index].grid_j_max;j++)
  {
   show_cell=0;
   reachable_by_hand=0;
   reachable_by_HRP2_hand=0;
   double x=i*surf_grid_samp_rate;
   double y=j*surf_grid_samp_rate;
   x+=curr_surfaces_in_env.flat_surf[current_surface_index].BR_x_min;
   y+=curr_surfaces_in_env.flat_surf[current_surface_index].BR_y_min;
   double z=curr_surfaces_in_env.flat_surf[current_surface_index].BR_z;

   double cur_z_st=z;
   double height=0.1;//0.3;
   double cur_z_end=z;
   //double cur_z_end=;
   //printf("%d ", curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].visible);
   
   
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_HRP2_RHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
     show_cell=1;
     reachable_by_HRP2_hand=1;
    //// g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
    if(show_affordance_type==1)
     {
   
    g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0-0.0075, height, 0.01, 4, NULL);
    cur_z_st+=height;
     }
     cur_z_end+=height;
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_HRP2_LHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
     show_cell=1;
     if(reachable_by_HRP2_hand==1)//Already reachable by right hand
     reachable_by_HRP2_hand=3; //Reachable by both hands
     else
     reachable_by_HRP2_hand=2; //Reachable by left hand only
  
   ////  g3d_drawDisc(x, y, z+0.03, surf_grid_samp_rate/2.0-0.015, 6, NULL);
     if(show_affordance_type==1)
     {
    
    g3d_draw_vertical_cylinder( x, y, cur_z_st, surf_grid_samp_rate/2.0-0.015, height, 0.01, 6, NULL);
    cur_z_st+=height;
     }
     cur_z_end+=height;
    }
   
   if(SHOW_DIRECT_REACHABLE==1)
   {
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
     show_cell=1;
     reachable_by_hand=1;
    //// g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
    
     cur_z_end+=height;
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
     show_cell=1;
     if(reachable_by_hand==1)//Already reachable by right hand
     reachable_by_hand=3; //Reachable by both hands
     else
     reachable_by_hand=2; //Reachable by left hand only
  
   
     cur_z_end+=height;
    }
   }
   if(SHOW_BENDING_REACHABLE==1)
   {
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand_by_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
     show_cell=1;
      reachable_by_hand=1;
    //// g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
    
     cur_z_end+=height;
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand_by_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
     show_cell=1;
     if(reachable_by_hand==1)//Already reachable by right hand
     reachable_by_hand=3; //Reachable by both hands
     else
     reachable_by_hand=2; //Reachable by left hand only

   //// g3d_drawDisc(x, y, z+0.03, surf_grid_samp_rate/2.0-0.015, 6, NULL);
    
     cur_z_end+=height;
    }
   }
   if(SHOW_TURNING_AROUND_REACHABLE==1)
   {
  
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand_by_turning_around_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
    show_cell=1;
  reachable_by_hand=1;
 ////   g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
    
    cur_z_end+=height;
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand_by_turning_around_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
    show_cell=1;

      if(reachable_by_hand==1)//Already reachable by right hand
     reachable_by_hand=3; //Reachable by both hands
     else
     reachable_by_hand=2; //Reachable by left hand only

  ////  g3d_drawDisc(x, y, z+0.03, surf_grid_samp_rate/2.0-0.015, 6, NULL);
    
    cur_z_end+=height;
    }
   }
  if(show_affordance_type==2)//By line
   {
    if(show_cell==1)
    { 
   //// g3d_drawDisc(x, y, z+0.01, surf_grid_samp_rate/2.0, 3, NULL);
    cur_z_end=z+0.01;
    g3d_drawOneLine(x,y,z+0.01,x,y,cur_z_end,4,NULL);
    if((reachable_by_hand==1||reachable_by_hand==2)&&(reachable_by_HRP2_hand==1||reachable_by_HRP2_hand==2))
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 1, NULL);
    if((reachable_by_hand==3)&&(reachable_by_HRP2_hand==3))
    g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 2, NULL);
    }
   
   
   }
  }
 }
}//END for(;current_surface_index<curr_surfaces_in_env.total_no_of_surfaces;current_surface_index++)


}
*/

void update_human_state_old(int state) //1 means sitting 0 means standing
{
  configPt q;
  q = MY_ALLOC(double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof); /* Allocation of temporary robot configuration */
  //p3d_rob* robotPt;
  //////////printf(" Inside update_human_state \n");
	
  ACBTSET->human[ACBTSET->actual_human]->actual_state = state;
  //q = p3d_copy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS);  
  q[8] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c7;
  q[43] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c1;
  q[44] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c2;
  q[46] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c3;
  q[47] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c4;
  q[50] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c5;
  q[53] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c6;
	
  p3d_set_and_update_this_robot_conf(ACBTSET->human[ACBTSET->actual_human]->HumanPt,q);

  
  /*robotPt = (p3d_rob*) p3d_get_desc_curid(P3D_ROBOT);
  p3d_sel_desc_name(P3D_ROBOT,ACBTSET->human[ACBTSET->actual_human]->HumanPt->name);
	
  p3d_set_and_update_robot_conf(q); 
  p3d_sel_desc_name(P3D_ROBOT,robotPt->name);
  */
      
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[8]  = q[8];
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[43] = q[43];
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[44] = q[44];
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[46] = q[46];
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[47] = q[47];
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[50] = q[50];
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[53] = q[53];

  
  //    ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[6]  = q[6];
   //   ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[7]  = q[7];
    //  ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[11] = q[11];
      //if(fabs(forehead_pos.theta-hum_cur_pos[11])>M_PI/4)
     // ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[64] = q[64];
     // ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[65] = q[65];

  //p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, q);
  MY_FREE(q, double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof);  /* Freeing temporary robot config structure */
	
  
}

/*
void update_human_state(int state) //1 means sitting 0 means standing
{
  configPt q;
  p3d_rob* robotPt;
  printf(" Inside update_human_state \n");
	
  ACBTSET->human[ACBTSET->actual_human]->actual_state = state;
  q = p3d_copy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS);  
  q[8] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c7;
  q[43] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c1;
  q[44] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c2;
  q[46] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c3;
  q[47] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c4;
  q[50] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c5;
  q[53] = ACBTSET->human[ACBTSET->actual_human]->state[ACBTSET->human[ACBTSET->actual_human]->actual_state].c6;
	
  robotPt = (p3d_rob*) p3d_get_desc_curid(P3D_ROBOT);
  p3d_sel_desc_name(P3D_ROBOT,ACBTSET->human[ACBTSET->actual_human]->HumanPt->name);
	
  p3d_set_and_update_robot_conf(q); 
  p3d_sel_desc_name(P3D_ROBOT,robotPt->name);

      
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[8]  = q[8];
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[43] = q[43];
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[44] = q[44];
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[46] = q[46];
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[47] = q[47];
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[50] = q[50];
      ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[53] = q[53];

  
  //    ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[6]  = q[6];
   //   ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[7]  = q[7];
    //  ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[11] = q[11];
      //if(fabs(forehead_pos.theta-hum_cur_pos[11])>M_PI/4)
     // ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[64] = q[64];
     // ACBTSET->human[ACBTSET->actual_human]->HumanPt->ROBOT_POS[65] = q[65];

  p3d_destroy_config(ACBTSET->human[ACBTSET->actual_human]->HumanPt, q);
	
  
}

*/

// ****** AKP : Automatic surface detection *****////
/*
int detect_horizontal_surfaces()
{
   curr_surfaces_in_env.total_no_of_surfaces=0;

AKP_env = (p3d_env *) p3d_get_desc_curid(P3D_ENV);

double env_x1=AKP_env->box.x1;
double env_y1=AKP_env->box.y1;
double env_x2=AKP_env->box.x2;
double env_y2=AKP_env->box.y2;curr_surfaces_in_env.flat_surf[current_surface_index].

  double x_coord;
  double y_coord;
  double z_coord=2;

  //env_x1=-15;
  //env_y1=-12;
  //env_x2=15;
  //env_y2=12;

  for(x_coord=env_x1;x_coord<=env_x2;x_coord+=0.05)
  {
   for(y_coord=env_y1;y_coord<=env_y2;y_coord+=0.05)
   {
    ////printf(" For x_coord=%lf, y_coord=%lf \n", x_coord, y_coord);
     
    current_cell = hri_bt_get_cell(bitmap,(int)((x_coord-ACBTSET->realx)/ACBTSET->pace),
					 (int)((y_coord-ACBTSET->realy)/ACBTSET->pace),
					 (int)(0));  
    
    if(current_cell == NULL) 
    {
    ////printf(" For x_coord=%lf, y_coord=%lf current cell is NULL\n", x_coord, y_coord);
    ////PrintWarning(("AKP Warning: Point for finding Obstacle does not exist\n"));
    //bitmapset->pathexist = FALSE;
    //return -4;
    }
    else
    {
     
     if(ACBTSET->bitmap[BT_OBSTACLES]->data[current_cell->x][current_cell->y][current_cell->z].val <0) //|| bitmapset->bitmap[BT_COMBINED]->calculate_cell_value(bitmapset,current_cell->x,current_cell->y,current_cell->z)<0)
     {
      
     }
    }
   }
  }
}

*/


double get_cell_value_3D_grid(hri_bitmapset * btset, int x, int y, int z)
{
  if(btset == NULL){
    PrintWarning(("Cant get obstacle value: btset=null"));
    return -1;
  }
  if(btset->bitmap[HRP2_GIK_MANIP] == NULL){
    PrintWarning(("Cant get obstacle value: bitmap=null"));
    return -1;
  }

  /*if(btset->bitmap[HRP2_GIK_MANIP]->data[x][y][z].val == -1)
    return -1;
  else
    return 0;
  */
   return btset->bitmap[HRP2_GIK_MANIP]->data[x][y][z].val;
}

int hri_bt_initialize_affordance_data(hri_bitmapset* bitmapset,int bt_type)
{
  int x,y,z;
  for(x=0; x<bitmapset->bitmap[bt_type]->nx; x++)
  {
     for(y=0; y<bitmapset->bitmap[bt_type]->ny; y++)
   {
       for(z=0; z<bitmapset->bitmap[bt_type]->nz; z++)
    {
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_human = 0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_human_neck_turn = 0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_human_torso_neack_turn=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_standing_human = 0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_standing_human_neck_turn = 0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_standing_human_torso_neack_turn=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_human_straight_head_orientation=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_human_straight_head_orientation_standing=0;

  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_human_LHand = 0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_human_RHand = 0;
  
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending=0; 
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending=0; 

  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_turning_around=0; 
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_turning_around=0;

  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_turning_around_bending=0; 
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_turning_around_bending=0;  

  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_standing=0; 
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_standing=0;

  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_standing_bending=0; 
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_standing_bending=0;

  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_standing_turning_around=0; 
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_standing_turning_around=0;


  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_standing_turning_around_bending=0; 
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_standing_turning_around_bending=0;

  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human_straight_head_orientation=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human_neck_turn=0;

  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_standing_human=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_standing_human_neck_turn=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human_straight_head_orientation_standing=0;


#ifdef HRI_HRP2
//For HRP2
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand=0; 
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand=0; 
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_HRP2=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_HRP2_neck_turn=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_HRP2_torso_neck_turn=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_standing_HRP2=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_standing_HRP2_neck_turn=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_standing_HRP2_torso_neck_turn=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_HRP2_straight_head_orientation=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_HRP2_straight_head_orientation_standing=0;

  
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2_straight_head_orientation=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2_neck_turn=0;
#endif

#ifdef HRI_JIDO
//For Jido
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.reachable_by_JIDO_Hand=0; 
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_JIDO=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_JIDO_neck_turn=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_Map.visible_by_JIDO_straight_head_orientation=0;
  
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO_straight_head_orientation=0;
  bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO_neck_turn=0;
#endif 

    }
   }
  }   
}


hri_bitmapset* create_3D_grid(int BB_length,int BB_width,int BB_height, double sampling_rate)
{
  ////////////ChronoPrint("<<<<<<<<Entering create_3D_grid");
  int dimx,dimy, dimz;
  configPt humanConf;
  double hx,hy,hz;
  double Ccoord[6];
  
  double xsize=6,ysize=6,zsize=3;

  hri_bitmapset * btset;
  btset = hri_bt_create_bitmaps();

  btset->pace = sampling_rate;

  dimx = (int)(BB_length/sampling_rate);
  dimy = (int)(BB_width/sampling_rate);
  dimz = (int)(BB_height/sampling_rate);

  btset->n = 1;
  btset->bitmap = MY_ALLOC(hri_bitmap*,btset->n);
  
  
  btset->bitmap[HRP2_GIK_MANIP]=hri_bt_create_empty_bitmap(dimx, dimy, dimz, sampling_rate, HRP2_GIK_MANIP, get_cell_value_3D_grid); // AKP : get_cell_value_3D_grid is the function name passed as the argument and will be assigned to the function calculate_cell_value of the btset.
   ////btset->bitmap[BT_AFFORDANCE_VISIBILITY]=hri_bt_create_empty_bitmap(dimx, dimy, dimz, sampling_rate, BT_AFFORDANCE_VISIBILITY, get_cell_value_3D_grid); // AKP : get_cell_value_3D_grid is the function name passed as the argument and will be assigned to the function calculate_cell_value of the btset.
  int i=0;

  for(;i<btset->n;i++)
  {
  if(btset->bitmap[i] == NULL)
   {
    printf("AKP WARNING : Could not create the empty bitmap %d inside the function create_3D_grid, so returning with NULL \n",i);
    return NULL;
   }
  }
  // create all cells
  for(i=0;i<btset->n;i++)
  {
  hri_bt_create_data(btset->bitmap[i]); // AKP : Creates necessary data field for an empty bitmap and populate with the value 0
  hri_bt_initialize_affordance_data(btset,i);
  }
  
  

  btset->path = NULL;
  btset->pathexist = FALSE;
  btset->combine_type = BT_COMBINE_SUM; /* default value */
  btset->changed = FALSE;

  

  //hri_exp_fill_obstacles(btset);
 ////////////ChronoPrint(">>>>>>>>>>>>Returning create_3D_grid");
  return btset;
}

//AKP : For populating the obstacles in the bitmap 

int create_obstacles_for_HRP2_GIK_manip( hri_bitmapset* btset )
{
  int i;
  p3d_env* env = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
  int expand_rate, minimum_expand_rate;
  configPt robotq;
	
  if(btset == NULL)
    return FALSE;
  
  
  
  if(btset->robot == NULL)
    {
      expand_rate = 0;
      PrintWarning(("WARNING: btset->robot NULL\n"));
    }
  /*else {
    robotq = p3d_get_robot_config(btset->robot);
    //  expand_rate = (bitmapset->robot->BB.xmax-bitmapset->robot->BB.xmin>bitmapset->robot->BB.ymax-bitmapset->robot->BB.ymin)? 
    //       ((bitmapset->robot->BB.xmax-bitmapset->robot->BB.xmin)/(bitmapset->bitmap[BT_OBSTACLES]->pace*2)): 
    //       ((bitmapset->robot->BB.ymax-bitmapset->robot->BB.ymin)/(bitmapset->bitmap[BT_OBSTACLES]->pace*2)); 
    if(DISTANCE2D(btset->robot->BB.xmax,btset->robot->BB.ymax,robotq[ROBOTq_X],robotq[ROBOTq_Y]) >
       DISTANCE2D(btset->robot->BB.xmin,btset->robot->BB.ymin,robotq[ROBOTq_X],robotq[ROBOTq_Y]))
      
      expand_rate = (btset->robot->BB.xmax-robotq[ROBOTq_X] > btset->robot->BB.ymax-robotq[ROBOTq_Y])?
	((btset->robot->BB.xmax-robotq[ROBOTq_X])/btset->pace):
	((btset->robot->BB.ymax-robotq[ROBOTq_Y])/btset->pace);
    
    else
      expand_rate = (btset->robot->BB.xmin-robotq[ROBOTq_X] > btset->robot->BB.ymin-robotq[ROBOTq_Y])?
	((robotq[ROBOTq_X]-btset->robot->BB.xmin)/btset->pace):
	((robotq[ROBOTq_Y]-btset->robot->BB.ymin)/btset->pace);	 
    
    p3d_destroy_config(btset->robot,robotq);
    //expand_rate--;		
  }


expand_rate=1;

  hri_bt_reset_bitmap_data(btset->bitmap[BT_OBSTACLES]);
  minimum_expand_rate = (int) (0.40/btset->pace) - 1; 	
  
  printf("expand rate %i min rate %i \n",expand_rate, minimum_expand_rate);

  if (expand_rate <=  minimum_expand_rate )
    expand_rate += minimum_expand_rate;
*/
  
  double expand_rate_for_HRP2_GIK=0.01;
  double minimum_expand_rate_for_HRP2_GIK=0.005;

  for(i=0; i<env->no ; i++){  
    //////////printf(" Inserting OBJECT %s \n",env->o[i]->name); 
    hri_bt_insert_obs(btset,btset->bitmap[HRP2_GIK_MANIP], env->o[i], env, minimum_expand_rate_for_HRP2_GIK, -2, 1);
    // potential 3d collision
    //hri_bt_insert_obs(btset,btset->bitmap[HRP2_GIK_MANIP], env->o[i], env, safe_expand_rate, BT_OBST_POTENTIAL_OBJECT_COLLISION, 0);
    //hri_bt_insert_1obs2bitmap(btset,btset->bitmap[BT_OBSTACLES], env->o[i], env, expand_rate_for_voronoi, -1)  ;
    /* if(hri_bt_insert_1obs2bitmap(btset,btset->bitmap[BT_OBSTACLES], env->o[i], env, expand_rate, -1)) */
    /* printf("Obstacle placed\n"); */
  }  

 /* for(i=0; i<env->no ; i++){                             
    // hri_bt_insert_1obs2bitmap(btset,btset->bitmap[BT_OBSTACLES], env->o[i], env, (int)(expand_rate/2)+1, -2);
     hri_bt_insert_1obs2bitmap(btset,btset->bitmap[BT_OBSTACLES], env->o[i], env, minimum_expand_rate_for_voronoi, -2);
    
  }*/

  env = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
  for(i=0; i<env->nr; i++){
    
     int is_robot=0;
     int is_human=0;
     int is_HRP2_chair=0;
     //printf(" Inserting ROBOT %s \n",env->robot[i]->name);
     if(!strncmp("HRP2ROBOT",env->robot[i]->name,9))
     is_robot=1;
     if((!strncmp("human",env->robot[i]->name,5))||(!strncmp("HUMAN",env->robot[i]->name,5)))
     is_human=1;
     if(!strncmp("HRP2CHAIR",env->robot[i]->name,9))
     is_HRP2_chair=1;
    
     if(is_robot==0&&is_human==0&& is_HRP2_chair==0)
     {
   ////  if(dynamic_obj[i].is_curr_visible==1)
      ////{
        //////////printf(" Inserting ROBOT %s \n",env->robot[i]->name);
       hri_bt_insert_obsrobot(btset, btset->bitmap[HRP2_GIK_MANIP], env->robot[i], env, minimum_expand_rate_for_HRP2_GIK, -2, 1);
     //hri_bt_insert_1obs2bitmaprobot(btset,btset->bitmap[BT_OBSTACLES],env->robot[i] , env, minimum_expand_rate_for_voronoi, -2);
      ////printf("Obstacles updated for %s\n",env->robot[i]->name);
      ////printf(" BB : %lf, %lf, %lf, %lf, %lf, %lf \n",env->robot[i]->BB.xmin,env->robot[i]->BB.xmax,env->robot[i]->BB.ymin,env->robot[i]->BB.ymax,env->robot[i]->BB.zmin,env->robot[i]->BB.zmax);
      ////}
     
    }
  }
  
  return TRUE;  
}

int update_HRP2_in_bitmap(p3d_rob* rob)
{
 //double i=0;
 
 //for(i=rob->BB.xmin;i<rob->BB.xmax;i
return 0; 
}

int disactivate_collision_among_parts_of_HRP2_RHAND()
{
  printf(" Inside disactivate_collision_among_parts_of_HRP2_RHAND()\n");
  envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
 int nr = envPt->nr;
  p3d_obj **o, *tmp_o;
  p3d_rob *r;
  int x,y,z;
  
  int hand_part_ctr=0;
 

  int r_ctr=0;
  
  for(r_ctr=0;r_ctr<nr;r_ctr++)
  {
  
  r = envPt->robot[r_ctr];
   /////printf(" r->name = %s ,r->no=%d \n",r->name,r->no);
   if(strcasecmp(r->name,"HRP2ROBOT")==0)//Exact match
   {
   r_ctr=100000; 
   printf("HRP2ROBOT has been found, Now disactivating collision among its Right hand parts\n");
   o = MY_ALLOC(p3d_obj*,r->no);
   int r_o_ctr=0; 
   for(r_o_ctr=0;r_o_ctr<r->no;r_o_ctr++)
    {
    tmp_o = r->o[r_o_ctr];
    
    /////printf(" tmp_o->name = %s \n",tmp_o->name);
    if(strcasecmp(tmp_o->name,"HRP2ROBOT.RHAND")>=15)//Atleast 15 char match
     {
    o[hand_part_ctr] = r->o[r_o_ctr];
    hand_part_ctr++;
     }
    }
   }  
  }
 printf(" hand_part_ctr=%d\n",hand_part_ctr);
 int i=0;
 int j=0;
 for(i=0;i<hand_part_ctr;i++)
 {
  for(j=0;j<hand_part_ctr;j++)
  {
  if(i!=j)
   {
  printf(" Disactivating collision between %s & %s \n",o[i]->name,o[j]->name);
  p3d_col_deactivate_pair_of_objects(o[i], o[j]);
   }
  }
 }
MY_FREE(o, p3d_obj*, r->no);
return 1;
}



int show_exact_obstacles_for_HRP2_GIK_manip(hri_bitmapset * bitmapset, int bt_type)
{
  if(bt_type==HRP2_GIK_MANIP)
 {
  hri_bitmap * bitmap;
bitmap = bitmapset->bitmap[bt_type];
   int x,y,z;
  for(x=0; x<bitmap->nx; x++)
  {
     for(y=0; y<bitmap->ny; y++)
   {
       for(z=0; z<bitmap->nz; z++)
    {
   if(bitmapset->bitmap[bt_type]->data[x][y][z].val < 0)  
     {
      double tmp_x  = x*bitmapset->pace+bitmapset->realx;
	double tmp_y  = y*bitmapset->pace+bitmapset->realy;
	 double tmp_z  = z*bitmapset->pace+bitmapset->realz;
     if(bitmapset->bitmap[bt_type]->data[x][y][z].val == -3)  
     g3d_drawDisc(tmp_x, tmp_y, tmp_z, 0.02, 3, NULL);
     else
     g3d_drawDisc(tmp_x, tmp_y, tmp_z, 0.02, 4, NULL);
     }
    }
   }
  }  
  //////////g3d_drawDisc(point_of_curr_collision.x, point_of_curr_collision.y,point_of_curr_collision.z, 0.1,4, NULL);
 } 
return 1;
}


int update_robots_and_objects_status()
{
   envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
  int no = envPt->no;
  int nr = envPt->nr;
  p3d_obj *o;
  p3d_rob *r;
  int r_ctr=0;
  //double cur_x;
  //double cur_y;
  //double cur_z;
  HUMAN_HAS_MOVED=0;
 
  for(r_ctr=0;r_ctr<nr;r_ctr++)
  {
  r = envPt->robot[r_ctr];

  //double cur_x=r->ROBOT_POS[6];
   if(strcasestr(r->name,"visball"))
   {
   }
  else
   {
  if(robots_status_for_Mightability_Maps[r_ctr].has_moved==0)
    {
  
  //////////if(fabs(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[6]-r->ROBOT_POS[6])>=0.01||fabs(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[7]-r->ROBOT_POS[7])>=0.01||fabs(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[8]-r->ROBOT_POS[8])>=0.01||fabs(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[9]-r->ROBOT_POS[9])>=0.01||fabs(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[10]-r->ROBOT_POS[10])>=0.01||fabs(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[11]-r->ROBOT_POS[11])>=0.01)
   if(fabs(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[6]-r->ROBOT_POS[6])>=0.01||fabs(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[7]-r->ROBOT_POS[7])>=0.01||fabs(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[8]-r->ROBOT_POS[8])>=0.01||fabs(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[9]-r->ROBOT_POS[9])>=0.01||fabs(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[10]-r->ROBOT_POS[10])>=0.01||fabs(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[11]-r->ROBOT_POS[11])>=0.01)
     {
    
    robots_status_for_Mightability_Maps[r_ctr].has_moved=1;
      
    printf(" >>>> Robot = %s has moved.\n",r->name);
    /*
    robots_status_for_Mightability_Maps[r_ctr].prev_x=robots_status_for_Mightability_Maps[r_ctr].curr_x;
    robots_status_for_Mightability_Maps[r_ctr].prev_y=robots_status_for_Mightability_Maps[r_ctr].curr_y;
    robots_status_for_Mightability_Maps[r_ctr].prev_z=robots_status_for_Mightability_Maps[r_ctr].curr_z;
    
    robots_status_for_Mightability_Maps[r_ctr].curr_x=r->ROBOT_POS[6];
    robots_status_for_Mightability_Maps[r_ctr].curr_y=r->ROBOT_POS[7];
    robots_status_for_Mightability_Maps[r_ctr].curr_z=r->ROBOT_POS[8];
    */
      if(strcasestr(r->name,"HUMAN"))
      {
       HUMAN_HAS_MOVED=1;
      }
      else
      {
       if(strcasestr(r->name,"HRP2"))
       {
       HRP2_HAS_MOVED=1;
       }
       else
       {
        if(strcasestr(r->name,"JIDO"))
        {
        JIDO_HAS_MOVED=1;
        }
       } 
      }
     }
     
    }
   }
  }
return 1;
}

int update_3D_grid_for_Mightability_Maps(hri_bitmapset * bitmapset, int expansion, int bt_type)
{
  configPt visq;
  configPt cur_rob_pos;
  hri_bitmap * bitmap;
  double increment=3.0/4.0*bitmapset->pace;
  visq = p3d_get_robot_config(bitmapset->visball);
  bitmap = bitmapset->bitmap[bt_type];
   envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
  
   int x,y,z;

  int no = envPt->no;
  int nr = envPt->nr;
  p3d_obj *o;
  p3d_rob *r;
  int r_ctr=0;
  for(r_ctr=0;r_ctr<nr;r_ctr++)
  {
   if(robots_status_for_Mightability_Maps[r_ctr].has_moved==1)
   {
  
  r = envPt->robot[r_ctr];

  cur_rob_pos=MY_ALLOC(double,r->nb_dof); 
  p3d_get_robot_config_into(r,&cur_rob_pos);
  
  //////////printf(" Robot name = %s \n",r->name);

    NEED_HUMAN_VISIBILITY_UPDATE=1;
   #ifdef HRI_JIDO
    NEED_JIDO_VISIBILITY_UPDATE=1;
   #elif defined(HRI_HRP2)
   NEED_HRP2_VISIBILITY_UPDATE=1;
   #endif
  
 //////////// printf(" inside update_3D_grid_for_Mightability_Maps, NEED_HUMAN_VISIBILITY_UPDATE=%d\n",NEED_HUMAN_VISIBILITY_UPDATE);
  ////**** First making previously occupied cells as free cells  
  p3d_set_and_update_this_robot_conf(r, robots_status_for_Mightability_Maps[r_ctr].rob_prev_config);
  p3d_col_deactivate_rob_rob(bitmapset->visball,r);//To make a cell occupied only if there is collision of visball with other objects not with robot
  int r_o_ctr=0; 
  for(r_o_ctr=0;r_o_ctr<r->no;r_o_ctr++)
   {
   o = r->o[r_o_ctr];
   ////printf(" o->name = %s \n",o->name);
   double BBx;
   for(BBx=o->BB.xmin;BBx<o->BB.xmax;BBx+=increment)
    {
    visq[6] = BBx;
    x=(BBx- bitmapset->realx)/bitmapset->pace;  
    double BBy;
    for(BBy=o->BB.ymin;BBy<o->BB.ymax&&x>0&&x<bitmap->nx;BBy+=increment)
     {
     visq[7]  = BBy;
     y=(BBy- bitmapset->realy)/bitmapset->pace;  
     double BBz;
     for(BBz=o->BB.zmin;BBz<o->BB.zmax&&y>0&&y<bitmap->ny;BBz+=increment)
      {
      visq[8]  = BBz;
      z=(BBz - bitmapset->realz)/bitmapset->pace;  
      
     

      if(x>0&&x<bitmap->nx&&y>0&&y<bitmap->ny&&z>0&&z<bitmap->nz)
       {
        ////bitmapset->bitmap[bt_type]->data[x][y][z].val = 0;  //no obstacle

        p3d_set_and_update_this_robot_conf(bitmapset->visball, visq);
       int kcd_with_report=0;
       int res = p3d_col_test_robot(bitmapset->visball,kcd_with_report);
       if(res>0) ////if(p3d_col_test_robot(bitmapset->visball,FALSE))
        {
         //////// printf(" Populating ACTUAL obstacle for x, y, z = %d %d %d\n",x, y, z);
       
       bitmapset->bitmap[bt_type]->data[x][y][z].val = -1;  //Exact obstacle
        
        
        
       int i=0; 
        for(i=x-expansion;i<=x+expansion;i++)
         {
            //printf(" for i = %d\n",i);
         if(i>=0&&i<bitmap->nx)
          {
             int j=0;
             for(j=y-expansion;j<=y+expansion;j++)
           {
              //printf(" for i, j = %d %d\n",i, j);
              if(j>=0&&j<bitmap->ny)
            {
               int k=0;
                for(k=z-expansion;k<=z+expansion;k++)
             {
              //printf(" for i, j, k = %d %d %\n",i, j, k);
              if(k>=0&&k<bitmap->nz)
              {
                // printf(" Populating %d, %d, %d \n",i,j,k);
                 if(bitmapset->bitmap[bt_type]->data[i][j][k].val == -1) //Already exact obstacle
                 {
                 }
                 else
                 bitmapset->bitmap[bt_type]->data[i][j][k].val = -2;  //Near to the obstacle
              }
             } 
            }
           }
          } 
         } 
        }
       else
        {
        
      /* 
       bitmapset->bitmap[bt_type]->data[x][y][z].val = 0;  //No obstacle
        
      
       int i=0; 
        for(i=x-expansion;i<=x+expansion;i++)
         {
            //printf(" for i = %d\n",i);
         if(i>=0&&i<bitmap->nx)
          {
             int j=0;
             for(j=y-expansion;j<=y+expansion;j++)
           {
              //printf(" for i, j = %d %d\n",i, j);
              if(j>=0&&j<bitmap->ny)
            {
               int k=0;
                for(k=z-expansion;k<=z+expansion;k++)
             {
              //printf(" for i, j, k = %d %d %\n",i, j, k);
              if(k>=0&&k<bitmap->nz)
              {
                // printf(" Populating %d, %d, %d \n",i,j,k);
                 bitmapset->bitmap[bt_type]->data[i][j][k].val = 0;  //No obstacle
              }
             } 
            }
           }
          } 
         } 
         */
       
        }
       }
      }
     }
    }
   }
   p3d_col_activate_rob_rob(bitmapset->visball,r);
   p3d_set_and_update_this_robot_conf(r, cur_rob_pos);
  ////*****END making the previously occupied cells as free cells  

  ////*****Now creating exact obstacle at new place of robot
  ////printf(" r->name = %s ,r->no=%d \n",r->name,r->no);
  r_o_ctr=0; 
  for(r_o_ctr=0;r_o_ctr<r->no;r_o_ctr++)
   {
   o = r->o[r_o_ctr];
   ////printf(" o->name = %s \n",o->name);
   double BBx;
   for(BBx=o->BB.xmin;BBx<o->BB.xmax;BBx+=increment)
    {
    visq[6] = BBx;
    x=(BBx- bitmapset->realx)/bitmapset->pace;  
    double BBy;
    for(BBy=o->BB.ymin;BBy<o->BB.ymax&&x>0&&x<bitmap->nx;BBy+=increment)
     {
     visq[7]  = BBy;
     y=(BBy- bitmapset->realy)/bitmapset->pace;  
     double BBz;
     for(BBz=o->BB.zmin;BBz<o->BB.zmax&&y>0&&y<bitmap->ny;BBz+=increment)
      {
      visq[8]  = BBz;
      z=(BBz - bitmapset->realz)/bitmapset->pace;  
      
     

      if(x>0&&x<bitmap->nx&&y>0&&y<bitmap->ny&&z>0&&z<bitmap->nz)
       {
        p3d_set_and_update_this_robot_conf(bitmapset->visball, visq);
       int kcd_with_report=0;
       int res = p3d_col_test_robot(bitmapset->visball,kcd_with_report);
       if(res>0) ////if(p3d_col_test_robot(bitmapset->visball,FALSE))
        {
         //////// printf(" Populating ACTUAL obstacle for x, y, z = %d %d %d\n",x, y, z);
       
       bitmapset->bitmap[bt_type]->data[x][y][z].val = -1;  //Exact obstacle
       
      
       //else
        //{
        //bitmapset->bitmap[bt_type]->data[x][y][surf_z].is_horizontal_surface=0;//Does not belong to a horizontal surface of table
       // }
          
       int i=0; 
        for(i=x-expansion;i<=x+expansion;i++)
         {
            //printf(" for i = %d\n",i);
         if(i>=0&&i<bitmap->nx)
          {
             int j=0;
             for(j=y-expansion;j<=y+expansion;j++)
           {
              //printf(" for i, j = %d %d\n",i, j);
              if(j>=0&&j<bitmap->ny)
            {
               int k=0;
                for(k=z-expansion;k<=z+expansion;k++)
             {
              //printf(" for i, j, k = %d %d %\n",i, j, k);
              if(k>=0&&k<bitmap->nz)
              {
                // printf(" Populating %d, %d, %d \n",i,j,k);
                 if(bitmapset->bitmap[bt_type]->data[i][j][k].val == -1) //Already exact obstacle
                 {
                 }
                 else
                 bitmapset->bitmap[bt_type]->data[i][j][k].val = -2;  //Near to the obstacle
              }
             } 
            }
           }
          } 
         } 
        }
       ////else
       ////{
       ////printf("Inside BB but no collison\n");
       ////exit(0);
       ////}
       }
      }   
     }
    }
    }
 
   robots_status_for_Mightability_Maps[r_ctr].has_moved=0;
   ////p3d_set_and_update_this_robot_conf(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config,r->ROBOT_POS);
  ////robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[6]=r->ROBOT_POS[6];
  ////robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[7]=r->ROBOT_POS[7];
  ////robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[8]=r->ROBOT_POS[8];
  p3d_copy_config_into(r,r->ROBOT_POS, &(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config));
  ////////robots_status_for_Mightability_Maps[r_ctr].prev_BB=r->BB;
   }
   
  }
  

  return 1;
}


int update_3D_grid_for_Mightability_Maps_new(hri_bitmapset * bitmapset, int expansion, int bt_type)
{
  configPt visq;
  configPt cur_rob_pos;
  hri_bitmap * bitmap;
  double increment=3.0/4.0*bitmapset->pace;
  visq = p3d_get_robot_config(bitmapset->visball);
  bitmap = bitmapset->bitmap[bt_type];
   envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
  
   int x,y,z;

  int no = envPt->no;
  int nr = envPt->nr;
  p3d_obj *o;
  p3d_rob *r;
  int r_ctr=0;
  ////printf(" ***************************\n");
  for(r_ctr=0;r_ctr<nr;r_ctr++)
  {
   //// printf(" **** [%d] : %s \n",r_ctr,envPt->robot[r_ctr]->name);
   if(robots_status_for_Mightability_Maps[r_ctr].has_moved==1)
   {
  r = envPt->robot[r_ctr];

  cur_rob_pos=MY_ALLOC(double,r->nb_dof); 
  p3d_get_robot_config_into(r,&cur_rob_pos);
  
  printf(" Moved Object name = %s \n",r->name);
   
   NEED_HUMAN_VISIBILITY_UPDATE=1;
   #ifdef HRI_JIDO
    NEED_JIDO_VISIBILITY_UPDATE=1;
   #elif defined(HRI_HRP2)
   NEED_HRP2_VISIBILITY_UPDATE=1;
   #endif
  
 ////////// printf(" inside update_3D_grid_for_Mightability_Maps_new(), NEED_HUMAN_VISIBILITY_UPDATE=%d\n",NEED_HUMAN_VISIBILITY_UPDATE);
    
  ////**** First making previously occupied cells as free cells  
  p3d_set_and_update_this_robot_conf(r, robots_status_for_Mightability_Maps[r_ctr].rob_prev_config);
  ////////p3d_col_deactivate_rob_rob(bitmapset->visball,r);//To make a cell occupied only if there is collision of visball with other objects not with robot
  int r_o_ctr=0; 
  int cell_ctr_tmp=0;
  for(r_o_ctr=0;r_o_ctr<r->no;r_o_ctr++)
   {
   o = r->o[r_o_ctr];
   ////printf(" o->name = %s \n",o->name);
   double BBx;
   for(BBx=o->BB.xmin;BBx<o->BB.xmax;BBx+=increment)
    {
    visq[6] = BBx;
    x=(BBx- bitmapset->realx)/bitmapset->pace;  
    double BBy;
    for(BBy=o->BB.ymin;BBy<o->BB.ymax&&x>0&&x<bitmap->nx;BBy+=increment)
     {
     visq[7]  = BBy;
     y=(BBy- bitmapset->realy)/bitmapset->pace;  
     double BBz;
     for(BBz=o->BB.zmin;BBz<o->BB.zmax&&y>0&&y<bitmap->ny;BBz+=increment)
      {
      visq[8]  = BBz;
      z=(BBz - bitmapset->realz)/bitmapset->pace;  
      
     

      if(x>0&&x<bitmap->nx&&y>0&&y<bitmap->ny&&z>0&&z<bitmap->nz)
       {
        ////bitmapset->bitmap[bt_type]->data[x][y][z].val = 0;  //no obstacle
 
        /*if(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects==1)
        {
         printf(" Cell belongs to bottle only\n");
        }*/

        cell_ctr_tmp++;
         //////////printf(" cell_ctr=%d: %d, %d\n",cell_ctr_tmp,bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr],bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects);
       //////////// if(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.is_horizontal_surface==0)//NOT the cell just above the  horizontal surface of table
        {
        //////////printf(" Not horizontal surface\n");
        ////printf(" cell_ctr=%d, %d, %d\n",cell_ctr_tmp,bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr],bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects);

        ////////////if((bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]==1&&   bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects==1&&bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects==0)||(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==1&&   bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects==1&&bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects==0)||(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==1&&   bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects==1&&bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects==1&&bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]==1))
         if(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]==1)
          {  
           bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]=0;  
           ////bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects--;

          if(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects==1)
           {
            bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects=0; 

           if(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects==0)
            {
             //////////printf(" Cell belongs to the displaced object only and no. of other close object=0\n");
             bitmapset->bitmap[bt_type]->data[x][y][z].val = 0;  //no obstacle
            
            }
            else//No of close object is >=1
            {
             if(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects==1)//No. of close obj =1 
             {
              if(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==1)//The close object is this object itself, so remove the object from the close as well as from the belonging object list
              { 
               //////////printf(" Cell belongs to the displaced object only\n");
               bitmapset->bitmap[bt_type]->data[x][y][z].val = 0;  //no obstacle

               bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=0;   bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects=0;
              }
              else//There is one close object but it is not this object, so just remove this object from the close object list
              {
             
              bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=0;   bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects--;
              }
             }
             else//No. of close obj >1, 
             {
              if(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==1)//One of the close object is this object itself, so remove the object from the close object list
              { 
              
               bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=0;   bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects--;
              }
             
             } 
            
            } 
           }
           else
           {
          //bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]=0;  
          bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects--;
           } 
          }//End if the cell belongs to this object
          else //The cell does not belong to this object
          {
           if(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==1)//The cell close to this object, so remove from the list of close objects
           {
            bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=0;   ////bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects--;
            
            if(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects==1)//This object is the only object close to the cell, so make the cell free
            {
             bitmapset->bitmap[bt_type]->data[x][y][z].val = 0;  //no obstacle
             bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects=0;
            }
            else
            {
             bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects--;
            }
           }
            

          }// End else of if the cell belongs to this object
         }//End if NOT the cell just above the  horizontal surface of table
        
        
        
       int i=0; 
        for(i=x-expansion;i<=x+expansion;i++)
         {
            //printf(" for i = %d\n",i);
         if(i>=0&&i<bitmap->nx)
          {
             int j=0;
             for(j=y-expansion;j<=y+expansion;j++)
           {
              //printf(" for i, j = %d %d\n",i, j);
              if(j>=0&&j<bitmap->ny)
            {
               int k=0;
                for(k=z-expansion;k<=z+expansion;k++)
             {
              //printf(" for i, j, k = %d %d %\n",i, j, k);
              if(k>=0&&k<bitmap->nz)
              {
                // printf(" Populating %d, %d, %d \n",i,j,k);
                 //if((bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]==1&&   bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects==1)||(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==1&&   bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects==1))
               ////////////if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.is_horizontal_surface==0)//NOT the cell just above the horizontal surface of table
               {
                if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]==1)
                {  
                bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]=0;  
                ////bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_belong_to_objects--;

                if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_belongs_to_objects==1)
                 {
                 bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_belongs_to_objects=0; 

                 if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects==0)
                  {
                  //////////printf(" Cell belongs to the displaced object only\n");
                  bitmapset->bitmap[bt_type]->data[i][j][k].val = 0;  //no obstacle
            
                  }
                  else//No of close object is >=1
                  {
                  if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects==1)//No. of close obj =1 
                   {
                    if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==1)//The close object is this object itself, so remove the object from the close as well as from the belonging object list
                    { 
                     //////////printf(" Cell belongs to the displaced object only\n");
                     bitmapset->bitmap[bt_type]->data[i][j][k].val = 0;  //no obstacle

                     bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=0;   bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects=0;
                    }
                    else//There is one close object but it is not this object, so just remove this object from the close object list
                    {
             
                     bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=0;   bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects--;
                    }
                   }
                   else//No. of close obj >1, 
                   {
                   if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==1)//One of the close object is this object itself, so remove the object from the close object list
                    { 
              
                     bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=0;   bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects--;
                    }
                    
                   } 
            
                  } 
                 }
                 else
                 {
                  bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_belongs_to_objects--;
                 }
                }//End if the cell belongs to this object
                else //The cell does not belong to this object
                {
                if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==1)//The cell close to this object, so remove from the list of close objects
                 {
                 bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=0;   ////bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects--;
            
                 if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects==1)//This object is the only object close to the cell, so make the cell free
                  {
                   bitmapset->bitmap[bt_type]->data[i][j][k].val = 0;  //no obstacle
                   bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects=0;
                  }
                 else
                  {
                   bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects--;
                  }
                 }
                }// End else of if the cell belongs to this object 
               }
              }
             } 
            }
           }
          } 
         } 
       
       
        }
       }
      }
     }
    }
   
   ////////p3d_col_activate_rob_rob(bitmapset->visball,r);
   p3d_set_and_update_this_robot_conf(r, cur_rob_pos);
   MY_FREE(cur_rob_pos, double,r->nb_dof); 
  ////*****END making the previously occupied cells as free cells  

  ////*****Now creating exact obstacle at new place of robot

  ////printf(" r->name = %s ,r->no=%d \n",r->name,r->no);
  r_o_ctr=0; 
  for(r_o_ctr=0;r_o_ctr<r->no;r_o_ctr++)
   {
   o = r->o[r_o_ctr];
   ////printf(" o->name = %s \n",o->name);
   double BBx;
   for(BBx=o->BB.xmin;BBx<o->BB.xmax;BBx+=increment)
    {
    visq[6] = BBx;
    x=(BBx- bitmapset->realx)/bitmapset->pace;  
    double BBy;
    for(BBy=o->BB.ymin;BBy<o->BB.ymax&&x>0&&x<bitmap->nx;BBy+=increment)
     {
     visq[7]  = BBy;
     y=(BBy- bitmapset->realy)/bitmapset->pace;  
     double BBz;
     for(BBz=o->BB.zmin;BBz<o->BB.zmax&&y>0&&y<bitmap->ny;BBz+=increment)
      {
      visq[8]  = BBz;
      z=(BBz - bitmapset->realz)/bitmapset->pace;  
      
     

      if(x>0&&x<bitmap->nx&&y>0&&y<bitmap->ny&&z>0&&z<bitmap->nz)
       {
        p3d_set_and_update_this_robot_conf(bitmapset->visball, visq);
       int kcd_with_report=0;
       int res = p3d_col_test_robot(bitmapset->visball,kcd_with_report);
       if(res>0) ////if(p3d_col_test_robot(bitmapset->visball,FALSE))
        {
         //////// printf(" Populating ACTUAL obstacle for x, y, z = %d %d %d\n",x, y, z);
       
       bitmapset->bitmap[bt_type]->data[x][y][z].val = -1;  //Exact obstacle
       
       if(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]==0)//the cell does not already belong to this robot index
         { 
       bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]=1;
       bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects++;
         }
         /////////bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]=1;
       //else
        //{
        //bitmapset->bitmap[bt_type]->data[x][y][surf_z].is_horizontal_surface=0;//Does not belong to a horizontal surface of table
       // }
          
       int i=0; 
        for(i=x-expansion;i<=x+expansion;i++)
         {
            //printf(" for i = %d\n",i);
         if(i>=0&&i<bitmap->nx)
          {
             int j=0;
             for(j=y-expansion;j<=y+expansion;j++)
           {
              //printf(" for i, j = %d %d\n",i, j);
              if(j>=0&&j<bitmap->ny)
            {
               int k=0;
                for(k=z-expansion;k<=z+expansion;k++)
             {
              //printf(" for i, j, k = %d %d %\n",i, j, k);
              if(k>=0&&k<bitmap->nz)
              {
                 
                // printf(" Populating %d, %d, %d \n",i,j,k);
                 if(bitmapset->bitmap[bt_type]->data[i][j][k].val == -1) //Already exact obstacle
                 {
                   
                 if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==0)//the cell does not already close to this robot index
                  { 
                 bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=1;
                 bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects++;
                  }
                 }
                 else
                 {
                 bitmapset->bitmap[bt_type]->data[i][j][k].val = -2;  //Near to the obstacle
                
                 if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==0)//the cell does not already close to this robot index
                  { 
                 bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=1;
                 bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects++;
                  }
                 }
                bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=1;
              }
             } 
            }
           }
          } 
         } 
        }
       ////else
       ////{
       ////printf("Inside BB but no collison\n");
       ////exit(0);
       ////}
       }
      }   
     }
    }
    }
   ////printf(" Setting has moved = 0\n");
   robots_status_for_Mightability_Maps[r_ctr].has_moved=0;
   ////p3d_set_and_update_this_robot_conf(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config,r->ROBOT_POS);
  ////robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[6]=r->ROBOT_POS[6];
  ////robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[7]=r->ROBOT_POS[7];
  ////robots_status_for_Mightability_Maps[r_ctr].rob_prev_config[8]=r->ROBOT_POS[8];
  ////////////////p3d_copy_config_into(r,r->ROBOT_POS, &(robots_status_for_Mightability_Maps[r_ctr].rob_prev_config));
  ////////robots_status_for_Mightability_Maps[r_ctr].prev_BB=r->BB;

  p3d_get_robot_config_into ( r,&robots_status_for_Mightability_Maps[r_ctr].rob_prev_config ); 
   }
   
  }
  

  return 1;
}


int create_exact_obstacles_for_HRP2_GIK_manip_fast_new ( hri_bitmapset * bitmapset, int expansion, int bt_type )
{
ChronoOff();
ChronoOn();

	if ( bt_type==HRP2_GIK_MANIP )
	{
//                  configPt visq = MY_ALLOC(double,ACBTSET->visball->nb_dof); /* Allocation of temporary robot configuration */
//                  p3d_get_robot_config_into(ACBTSET->robot,&visq); 
// 		
// 		hri_bitmap * bitmap;
// 		double increment=3.0/4.0*bitmapset->pace;
		 	configPt visq;
  			hri_bitmap * bitmap;
  			double increment=3.0/4.0*bitmapset->pace;
  			visq = p3d_get_robot_config(bitmapset->visball);

		////visq = p3d_get_robot_config(bitmapset->visball);

		bitmap = bitmapset->bitmap[bt_type];
		envPt = ( p3d_env * ) p3d_get_desc_curid ( P3D_ENV );
		int no = envPt->no;
		int nr = envPt->nr;

		int x,y,z;
		for ( x=0; x<bitmap->nx; x++ )
		{
			for ( y=0; y<bitmap->ny; y++ )
			{
				for ( z=0; z<bitmap->nz; z++ )
				{
					bitmapset->bitmap[bt_type]->data[x][y][z].val = 0;
					int nr_ctr=0;
					for ( ;nr_ctr<nr;nr_ctr++ )
					{
						bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[nr_ctr]=0;//Initially does not belong to any cell

						bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.close_to_objects_indx[nr_ctr]=0;//Initially does not belong to any cell
					}
					bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.is_horizontal_surface=0;
					bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.near_horizontal_surface=0;


					bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects=0;
					bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects=0;
				}
			}
		}


		p3d_obj *o;
		p3d_rob *r;
		int r_ctr=0;
		for ( r_ctr=0;r_ctr<nr;r_ctr++ )
		{
			
			r = envPt->robot[r_ctr];
                        if ( strcasestr ( r->name,"visball" ) )
			{
                        continue;
			}
				robots_status_for_Mightability_Maps[r_ctr].rob_prev_config=MY_ALLOC ( double,r->nb_dof );
				p3d_get_robot_config_into ( r,&robots_status_for_Mightability_Maps[r_ctr].rob_prev_config );
				////robots_status_for_Mightability_Maps[r_ctr].prev_BB=r->BB;
				robots_status_for_Mightability_Maps[r_ctr].has_moved=0;

				//////////printf(" Robot name = %s \n",r->name);
				int surf_z;
				int is_tabel=0;
				if ( strcasestr ( r->name,"table" ) )
				{
					//int surf_z=z;
					//////////printf(" Cell belongs to table %s \n",r->name);

					surf_z= ( ( r->BB.zmax - bitmapset->realz ) /bitmapset->pace ) +1;//One cell above the surface
					is_tabel=1;
					////////// printf(" surf_z = %d \n",surf_z);
				}
				////printf(" r->name = %s ,r->no=%d \n",r->name,r->no);
				int r_o_ctr=0;
				for ( r_o_ctr=0;r_o_ctr<r->no;r_o_ctr++ )
				{
					o = r->o[r_o_ctr];
					////printf(" o->name = %s \n",o->name);
					double BBx;
					for ( BBx=o->BB.xmin;BBx<o->BB.xmax;BBx+=increment )
					{
						visq[6]  = BBx;
						x= ( BBx- bitmapset->realx ) /bitmapset->pace;
						double BBy;
						for ( BBy=o->BB.ymin;BBy<o->BB.ymax&&x>0&&x<bitmap->nx;BBy+=increment )
						{
							visq[7]  = BBy;
							y= ( BBy- bitmapset->realy ) /bitmapset->pace;
							double BBz;
							for ( BBz=o->BB.zmin;BBz<o->BB.zmax&&y>0&&y<bitmap->ny;BBz+=increment )
							{
								visq[8]  = BBz;
								z= ( BBz - bitmapset->realz ) /bitmapset->pace;

								if ( x>0&&x<bitmap->nx&&y>0&&y<bitmap->ny&&z>0&&z<bitmap->nz )
								{
									p3d_set_and_update_this_robot_conf ( bitmapset->visball, visq );
									int kcd_with_report=0;
									int res = p3d_col_test_robot ( bitmapset->visball,kcd_with_report );
									if ( res>0 ) ////if(p3d_col_test_robot(bitmapset->visball,FALSE))
									{
										//////// printf(" Populating ACTUAL obstacle for x, y, z = %d %d %d\n",x, y, z);

										bitmapset->bitmap[bt_type]->data[x][y][z].val = -1;  //Exact obstacle

										if ( bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]==0 ) //the cell does not already belong to this robot index
										{
											bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]=1;//cell belong to this robot index
											bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.objects_belonging_to[bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects]=r_ctr;

											bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects++;
										}

										////////////bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]=1;//cell belong to this robot index


										
										if ( is_tabel==1&& ( r->BB.zmax-BBz ) <=bitmapset->pace )
										{
											bitmapset->bitmap[bt_type]->data[x][y][surf_z].Mightability_map_cell_obj_info.is_horizontal_surface=1;//Belongs to a horizontal surface of table
											bitmapset->bitmap[bt_type]->data[x][y][surf_z].val = -2;  //To avoid the plan path close to the  table surface
         										
											/*
											if(surf_z+1<bitmap->nz)
											{
											bitmapset->bitmap[bt_type]->data[x][y][surf_z+1].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
											bitmapset->bitmap[bt_type]->data[x][y][surf_z+1].val = -2;  //To avoid the plan path close to the table surface
											}
											if(x-1>0&&x-1<bitmap->nx&&y+1>0&&y+1<bitmap->ny)
											{
											bitmapset->bitmap[bt_type]->data[x-1][y+1][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
											bitmapset->bitmap[bt_type]->data[x-1][y+1][surf_z].val = -2;  //To avoid the plan path close to the table surface
											}
											if(y+1>0&&y+1<bitmap->ny)
											{
											bitmapset->bitmap[bt_type]->data[x][y+1][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
											bitmapset->bitmap[bt_type]->data[x][y+1][surf_z].val = -2;  //To avoid the plan path close to the table surface
											}
											if(x+1>0&&x+1<bitmap->nx&&y+1>0&&y+1<bitmap->ny)
											{
											bitmapset->bitmap[bt_type]->data[x+1][y+1][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
											bitmapset->bitmap[bt_type]->data[x+1][y+1][surf_z].val = -2;  //To avoid the plan path close to the table surface
											}
											if(x-1>0&&x-1<bitmap->nx)
											{
											bitmapset->bitmap[bt_type]->data[x-1][y][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
											bitmapset->bitmap[bt_type]->data[x-1][y][surf_z].val = -2;  //To avoid the plan path close to the table surface
											}
											if(x+1>0&&x+1<bitmap->nx)
											{
											bitmapset->bitmap[bt_type]->data[x+1][y][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
											bitmapset->bitmap[bt_type]->data[x+1][y][surf_z].val = -2;  //To avoid the plan path close to the table surface
											}
											if(x-1>0&&x-1<bitmap->nx&&y-1>0&&y-1<bitmap->ny)
											{
											bitmapset->bitmap[bt_type]->data[x-1][y-1][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
											bitmapset->bitmap[bt_type]->data[x-1][y-1][surf_z].val = -2;  //To avoid the plan path close to the table surface
											}
											if(y-1>0&&y-1<bitmap->ny)
											{
											bitmapset->bitmap[bt_type]->data[x][y-1][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
											bitmapset->bitmap[bt_type]->data[x][y-1][surf_z].val = -2;  //To avoid the plan path close to the table surface
											}
											if(x+1>0&&x+1<bitmap->nx&&y-1>0&&y-1<bitmap->ny)
											{
											bitmapset->bitmap[bt_type]->data[x+1][y-1][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
											bitmapset->bitmap[bt_type]->data[x+1][y-1][surf_z].val = -2;  //To avoid the plan path close to the table surface
											}
											*/
										}

										//else
										//{
										//bitmapset->bitmap[bt_type]->data[x][y][surf_z].is_horizontal_surface=0;//Does not belong to a horizontal surface of table
										// }

										int i=0;
										for ( i=x-expansion;i<=x+expansion;i++ )
										{
											//printf(" for i = %d\n",i);
											if ( i>=0&&i<bitmap->nx )
											{
												int j=0;
												for ( j=y-expansion;j<=y+expansion;j++ )
												{
													//printf(" for i, j = %d %d\n",i, j);
													if ( j>=0&&j<bitmap->ny )
													{
														int k=0;
														for ( k=z-expansion;k<=z+expansion;k++ )
														{
															//printf(" for i, j, k = %d %d %\n",i, j, k);
															if ( k>=0&&k<bitmap->nz )
															{
																// printf(" Populating %d, %d, %d \n",i,j,k);
																if ( bitmapset->bitmap[bt_type]->data[i][j][k].val == -1 ) //Already exact obstacle
																{

																	if ( bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==0 ) //the cell does not already close to this robot index
																	{
																		bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=1;//cell close to this robot index
																		bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects++;
																	}
																	bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=1;//cell close to this robot index
																}
																else
																{
																	bitmapset->bitmap[bt_type]->data[i][j][k].val = -2;  //Near to the obstacle

																	if ( bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==0 ) //the cell does not already close to this robot index
																	{
																		bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=1;//cell close to this robot index
																		bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects++;
																	}
																	bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=1;//cell close to this robot index
																}
															}
														}
													}
												}
											}
										}
									}
									////else
									////{
									////printf("Inside BB but no collison\n");
									////exit(0);
									////}
								}
							}
						}
					}
				}
			
		}//End for ( r_ctr=0;r_ctr<nr;r_ctr++ )


////Now same thing for onjects (non robots)
		int o_ctr=0;
		for ( o_ctr=0;o_ctr<no;o_ctr++ )
		{

			o = envPt->o[r_ctr];
			//// printf(" o->name = %s, \n",o->name);
			double BBx;
			for ( BBx=o->BB.xmin;BBx<o->BB.xmax;BBx+=increment )
			{
				visq[6]  = BBx;
				x= ( BBx- bitmapset->realx ) /bitmapset->pace;
				double BBy;
				for ( BBy=o->BB.ymin;BBy<o->BB.ymax&&x>0&&x<bitmap->nx;BBy+=increment )
				{
					visq[7]  = BBy;
					y= ( BBy- bitmapset->realy ) /bitmapset->pace;
					double BBz;
					for ( BBz=o->BB.zmin;BBz<o->BB.zmax&&y>0&&y<bitmap->ny;BBz+=increment )
					{
						visq[8]  = BBz;
						z= ( BBz- bitmapset->realz ) /bitmapset->pace;


						if ( z>0&&z<bitmap->nz )
						{
							p3d_set_and_update_this_robot_conf ( bitmapset->visball, visq );
							int kcd_with_report=0;
							int res = p3d_col_test_robot ( bitmapset->visball,kcd_with_report );
							if ( res>0 ) ////if(p3d_col_test_robot(bitmapset->visball,FALSE))
							{
								//printf(" Populating ACTUAL obstacle for x, y, z = %d %d %d\n",x, y, z);

								bitmapset->bitmap[bt_type]->data[x][y][z].val = -2;

								int i=0;
								for ( i=x-expansion;i<=x+expansion;i++ )
								{
									//printf(" for i = %d\n",i);
									if ( i>=0&&i<bitmap->nx )
									{
										int j=0;
										for ( j=y-expansion;j<=y+expansion;j++ )
										{
											//printf(" for i, j = %d %d\n",i, j);
											if ( j>=0&&j<bitmap->ny )
											{
												int k=0;
												for ( k=z-expansion;k<=z+expansion;k++ )
												{
													//printf(" for i, j, k = %d %d %\n",i, j, k);
													if ( k>=0&&k<bitmap->nz )
													{
														// printf(" Populating %d, %d, %d \n",i,j,k);

														bitmapset->bitmap[bt_type]->data[i][j][k].val = -2;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}

			}
		}
		p3d_destroy_config ( bitmapset->visball, visq );
//         MY_FREE ( visq,double,bitmapset->visball->nb_dof );
	}

ChronoPrint(" Time for create obstacle cell fast ");
ChronoOff();
	
	return 1;
}



int create_exact_obstacles_for_HRP2_GIK_manip_fast(hri_bitmapset * bitmapset, int expansion, int bt_type)
{
  if(bt_type==HRP2_GIK_MANIP)
 {
   configPt visq;
  hri_bitmap * bitmap;
  double increment=3.0/4.0*bitmapset->pace;
  visq = p3d_get_robot_config(bitmapset->visball);

  bitmap = bitmapset->bitmap[bt_type];
  envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
  int no = envPt->no;
  int nr = envPt->nr;

   int x,y,z;
  for(x=0; x<bitmap->nx; x++)
  {
     for(y=0; y<bitmap->ny; y++)
   {
       for(z=0; z<bitmap->nz; z++)
    {
  bitmapset->bitmap[bt_type]->data[x][y][z].val = 0;  
  int nr_ctr=0;
    for(;nr_ctr<nr;nr_ctr++)
     {
     bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[nr_ctr]=0;//Initially does not belong to any cell
     
     bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.close_to_objects_indx[nr_ctr]=0;//Initially does not belong to any cell
     }
     bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.is_horizontal_surface=0;
     bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.near_horizontal_surface=0;
     
     
     bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects=0;
     bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects=0;  
    }
   }
  }   
  
  
  p3d_obj *o;
  p3d_rob *r;
  int r_ctr=0;
  for(r_ctr=0;r_ctr<nr;r_ctr++)
  {

  r = envPt->robot[r_ctr];

  robots_status_for_Mightability_Maps[r_ctr].rob_prev_config=MY_ALLOC(double,r->nb_dof); 
  p3d_get_robot_config_into(r,&robots_status_for_Mightability_Maps[r_ctr].rob_prev_config);
  ////robots_status_for_Mightability_Maps[r_ctr].prev_BB=r->BB;
  robots_status_for_Mightability_Maps[r_ctr].has_moved=0;
 
  //////////printf(" Robot name = %s \n",r->name);
  int surf_z;
  int is_tabel=0;
   if (strcasestr(r->name,"table"))
   {
        //int surf_z=z;
   //////////printf(" Cell belongs to table %s \n",r->name);

   surf_z=((r->BB.zmax - bitmapset->realz)/bitmapset->pace)+1;//One cell above the surface
   is_tabel=1;
  ////////// printf(" surf_z = %d \n",surf_z);
   }
  ////printf(" r->name = %s ,r->no=%d \n",r->name,r->no);
  int r_o_ctr=0; 
  for(r_o_ctr=0;r_o_ctr<r->no;r_o_ctr++)
   {
   o = r->o[r_o_ctr];
   ////printf(" o->name = %s \n",o->name);
   double BBx;
   for(BBx=o->BB.xmin;BBx<o->BB.xmax;BBx+=increment)
    {
    visq[6]  = BBx;
    x=(BBx- bitmapset->realx)/bitmapset->pace;  
    double BBy;
    for(BBy=o->BB.ymin;BBy<o->BB.ymax&&x>0&&x<bitmap->nx;BBy+=increment)
     {
     visq[7]  = BBy;
     y=(BBy- bitmapset->realy)/bitmapset->pace;  
     double BBz;
     for(BBz=o->BB.zmin;BBz<o->BB.zmax&&y>0&&y<bitmap->ny;BBz+=increment)
      {
      visq[8]  = BBz;
      z=(BBz - bitmapset->realz)/bitmapset->pace;  
      
     

      if(x>0&&x<bitmap->nx&&y>0&&y<bitmap->ny&&z>0&&z<bitmap->nz)
       {
        p3d_set_and_update_this_robot_conf(bitmapset->visball, visq);
       int kcd_with_report=0;
       int res = p3d_col_test_robot(bitmapset->visball,kcd_with_report);
       if(res>0) ////if(p3d_col_test_robot(bitmapset->visball,FALSE))
        {
         //////// printf(" Populating ACTUAL obstacle for x, y, z = %d %d %d\n",x, y, z);
       
       bitmapset->bitmap[bt_type]->data[x][y][z].val = -1;  //Exact obstacle
      
       if(bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]==0)//the cell does not already belong to this robot index
        { 
       bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]=1;//cell belong to this robot index 
      bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.objects_belonging_to[bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects]=r_ctr;

       bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects++;
        }

          ////////////bitmapset->bitmap[bt_type]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[r_ctr]=1;//cell belong to this robot index 

   
      
        if(is_tabel==1&&(r->BB.zmax-BBz)<=bitmapset->pace)
        {
        bitmapset->bitmap[bt_type]->data[x][y][surf_z].Mightability_map_cell_obj_info.is_horizontal_surface=1;//Belongs to a horizontal surface of table
        bitmapset->bitmap[bt_type]->data[x][y][surf_z].val = -2;  //To avoid the plan path close to the table surface
         /*
        if(surf_z+1<bitmap->nz)
         {
        bitmapset->bitmap[bt_type]->data[x][y][surf_z+1].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
        bitmapset->bitmap[bt_type]->data[x][y][surf_z+1].val = -2;  //To avoid the plan path close to the table surface
         }
        if(x-1>0&&x-1<bitmap->nx&&y+1>0&&y+1<bitmap->ny)
         {
        bitmapset->bitmap[bt_type]->data[x-1][y+1][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
        bitmapset->bitmap[bt_type]->data[x-1][y+1][surf_z].val = -2;  //To avoid the plan path close to the table surface
         }
        if(y+1>0&&y+1<bitmap->ny)
         {
        bitmapset->bitmap[bt_type]->data[x][y+1][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
        bitmapset->bitmap[bt_type]->data[x][y+1][surf_z].val = -2;  //To avoid the plan path close to the table surface
         }
        if(x+1>0&&x+1<bitmap->nx&&y+1>0&&y+1<bitmap->ny)
         {
        bitmapset->bitmap[bt_type]->data[x+1][y+1][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
        bitmapset->bitmap[bt_type]->data[x+1][y+1][surf_z].val = -2;  //To avoid the plan path close to the table surface
         }
        if(x-1>0&&x-1<bitmap->nx)
         {
        bitmapset->bitmap[bt_type]->data[x-1][y][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
        bitmapset->bitmap[bt_type]->data[x-1][y][surf_z].val = -2;  //To avoid the plan path close to the table surface
         }
        if(x+1>0&&x+1<bitmap->nx)
         {
         bitmapset->bitmap[bt_type]->data[x+1][y][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
         bitmapset->bitmap[bt_type]->data[x+1][y][surf_z].val = -2;  //To avoid the plan path close to the table surface
         }
        if(x-1>0&&x-1<bitmap->nx&&y-1>0&&y-1<bitmap->ny)
         {
        bitmapset->bitmap[bt_type]->data[x-1][y-1][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
        bitmapset->bitmap[bt_type]->data[x-1][y-1][surf_z].val = -2;  //To avoid the plan path close to the table surface
         } 
        if(y-1>0&&y-1<bitmap->ny) 
         {
        bitmapset->bitmap[bt_type]->data[x][y-1][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
        bitmapset->bitmap[bt_type]->data[x][y-1][surf_z].val = -2;  //To avoid the plan path close to the table surface
         }
        if(x+1>0&&x+1<bitmap->nx&&y-1>0&&y-1<bitmap->ny)
         {
        bitmapset->bitmap[bt_type]->data[x+1][y-1][surf_z].Mightability_map_cell_obj_info.near_horizontal_surface=1;//very close to a horizontal surface of table
        bitmapset->bitmap[bt_type]->data[x+1][y-1][surf_z].val = -2;  //To avoid the plan path close to the table surface
         }
         */
        }
        
       //else
        //{
        //bitmapset->bitmap[bt_type]->data[x][y][surf_z].is_horizontal_surface=0;//Does not belong to a horizontal surface of table
       // }
          
       int i=0; 
        for(i=x-expansion;i<=x+expansion;i++)
         {
            //printf(" for i = %d\n",i);
         if(i>=0&&i<bitmap->nx)
          {
             int j=0;
             for(j=y-expansion;j<=y+expansion;j++)
           {
              //printf(" for i, j = %d %d\n",i, j);
              if(j>=0&&j<bitmap->ny)
            {
               int k=0;
                for(k=z-expansion;k<=z+expansion;k++)
             {
              //printf(" for i, j, k = %d %d %\n",i, j, k);
              if(k>=0&&k<bitmap->nz)
              {
                // printf(" Populating %d, %d, %d \n",i,j,k);
                 if(bitmapset->bitmap[bt_type]->data[i][j][k].val == -1) //Already exact obstacle
                 {
                  
                  if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==0)//the cell does not already close to this robot index
                  { 
                  bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=1;//cell close to this robot index  
                  bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects++;
                  }
                    bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=1;//cell close to this robot index   
                 }
                 else
                 {
                 bitmapset->bitmap[bt_type]->data[i][j][k].val = -2;  //Near to the obstacle
                 
                  if(bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]==0)//the cell does not already close to this robot index
                  { 
                  bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=1;//cell close to this robot index  
                  bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.no_close_to_objects++;
                  }
                bitmapset->bitmap[bt_type]->data[i][j][k].Mightability_map_cell_obj_info.close_to_objects_indx[r_ctr]=1;//cell close to this robot index   
                 }
              }
             } 
            }
           }
          } 
         } 
        }
       ////else
       ////{
       ////printf("Inside BB but no collison\n");
       ////exit(0);
       ////}
       }
      }   
     }
    }
   }
  }
  

  int o_ctr=0;
  for(o_ctr=0;o_ctr<no;o_ctr++)
  {
  
   o = envPt->o[r_ctr];
  //// printf(" o->name = %s, \n",o->name);
   double BBx;
   for(BBx=o->BB.xmin;BBx<o->BB.xmax;BBx+=increment)
    {
    visq[6]  = BBx;
    x=(BBx- bitmapset->realx)/bitmapset->pace;  
    double BBy;
    for(BBy=o->BB.ymin;BBy<o->BB.ymax&&x>0&&x<bitmap->nx;BBy+=increment)
     {
     visq[7]  = BBy;
     y=(BBy- bitmapset->realy)/bitmapset->pace;  
     double BBz;
     for(BBz=o->BB.zmin;BBz<o->BB.zmax&&y>0&&y<bitmap->ny;BBz+=increment)
      {
      visq[8]  = BBz;
      z=(BBz- bitmapset->realz)/bitmapset->pace;  
 
       
      if(z>0&&z<bitmap->nz)
       {
       p3d_set_and_update_this_robot_conf(bitmapset->visball, visq); 
       int kcd_with_report=0;
       int res = p3d_col_test_robot(bitmapset->visball,kcd_with_report);
       if(res>0) ////if(p3d_col_test_robot(bitmapset->visball,FALSE))
        {
           //printf(" Populating ACTUAL obstacle for x, y, z = %d %d %d\n",x, y, z);
       
       bitmapset->bitmap[bt_type]->data[x][y][z].val = -2;  
          
       int i=0; 
        for(i=x-expansion;i<=x+expansion;i++)
         {
            //printf(" for i = %d\n",i);
         if(i>=0&&i<bitmap->nx)
          {
             int j=0;
             for(j=y-expansion;j<=y+expansion;j++)
           {
              //printf(" for i, j = %d %d\n",i, j);
              if(j>=0&&j<bitmap->ny)
            {
               int k=0;
                for(k=z-expansion;k<=z+expansion;k++)
             {
              //printf(" for i, j, k = %d %d %\n",i, j, k);
              if(k>=0&&k<bitmap->nz)
              {
                // printf(" Populating %d, %d, %d \n",i,j,k);
                 
                 bitmapset->bitmap[bt_type]->data[i][j][k].val = -2;  
              }
             }  
            }
           }
          } 
         } 
        }
       }
      }   
     }
    
   }
  }
  p3d_destroy_config(bitmapset->visball, visq);
 } 
return 1;
}

int create_exact_obstacles_for_HRP2_GIK_manip(hri_bitmapset * bitmapset, int expansion, int bt_type)
{
  configPt visq;
  hri_bitmap * bitmap;
  int x,y,z;
  if(bt_type==-1) //Need to create for all bitmap types
  {
  

  visq = p3d_get_robot_config(bitmapset->visball);

  //p3d_col_deactivate_rob_rob(bitmapset->visball,bitmapset->human[bitmapset->actual_human]->HumanPt);
  

  for(x=0; x<bitmap->nx; x++)
   {
     for(y=0; y<bitmap->ny; y++)
    {
       for(z=0; z<bitmap->nz; z++)
     {
      int btm_ctr=0;
      
      for(btm_ctr=0;btm_ctr<bitmapset->n;btm_ctr++)
      {
       ////printf(" x,y,z = (%d,%d,%d), btm_ctr=%d\n",x,y,z,btm_ctr);
       bitmap = bitmapset->bitmap[btm_ctr]; 
       if(bitmap->type==HRP2_GIK_MANIP)
       p3d_col_deactivate_rob_rob(bitmapset->visball,bitmapset->robot);
       
	 visq[6]  = x*bitmapset->pace+bitmapset->realx;
	 visq[7]  = y*bitmapset->pace+bitmapset->realy;
	 visq[8]  = z*bitmapset->pace+bitmapset->realz;
	 p3d_set_and_update_this_robot_conf(bitmapset->visball, visq);
	 //if(p3d_col_test_robot_statics(bitmapset->visball,FALSE))
         if(p3d_col_test_robot(bitmapset->visball,FALSE))
         {
           //printf(" Populating ACTUAL obstacle for x, y, z = %d %d %d\n",x, y, z);
           bitmapset->bitmap[btm_ctr]->data[x][y][z].val = -2;  
          
           int i=0; 
           for(i=x-expansion;i<=x+expansion;i++)
           {
            //printf(" for i = %d\n",i);
            if(i>=0&&i<bitmap->nx)
            {
             int j=0;
             for(j=y-expansion;j<=y+expansion;j++)
             {
              //printf(" for i, j = %d %d\n",i, j);
              if(j>=0&&j<bitmap->ny)
              {
               int k=0;
                for(k=z-expansion;k<=z+expansion;k++)
               {
              //printf(" for i, j, k = %d %d %\n",i, j, k);
              if(k>=0&&k<bitmap->nz)
                {
                // printf(" Populating %d, %d, %d \n",i,j,k);
                 bitmapset->bitmap[btm_ctr]->data[i][j][k].val = -2;  
                }
               } 
              }
             }
            } 
           } 
          }
	 else
          {
	   bitmapset->bitmap[btm_ctr]->data[x][y][z].val = 0;
          }
         if(bitmap->type==HRP2_GIK_MANIP)
         p3d_col_activate_rob_rob(bitmapset->visball,bitmapset->robot); 
        }
       }
     }
  }
  p3d_destroy_config(bitmapset->visball, visq);


//p3d_col_activate_rob_rob(ACBTSET->visball,ACBTSET->human[ACBTSET->actual_human]->HumanPt);

  return TRUE;
    
   
  }
  else //need to create for the specific bitmap type provided
  {
  bitmap = bitmapset->bitmap[bt_type];

  visq = p3d_get_robot_config(bitmapset->visball);

  //p3d_col_deactivate_rob_rob(bitmapset->visball,bitmapset->human[bitmapset->actual_human]->HumanPt);
 
  ////AKP NOTE : Below is the old version in which the collision test has been disactivated with HRP2 for the purpose of letting the A* planner to find the collision free path from the hand to the goal. But now instead of this now using a function to make the cell values 0 around the hand to be used for manipulation
  ////if(bt_type==HRP2_GIK_MANIP)
  ////p3d_col_deactivate_rob_rob(bitmapset->visball,bitmapset->robot);

  for(x=0; x<bitmap->nx; x++){
     for(y=0; y<bitmap->ny; y++){
       for(z=0; z<bitmap->nz; z++){
	 visq[6]  = x*bitmapset->pace+bitmapset->realx;
	 visq[7]  = y*bitmapset->pace+bitmapset->realy;
	 visq[8]  = z*bitmapset->pace+bitmapset->realz;
	 p3d_set_and_update_this_robot_conf(bitmapset->visball, visq);
	 //if(p3d_col_test_robot_statics(bitmapset->visball,FALSE))
         if(p3d_col_test_robot(bitmapset->visball,FALSE))
         {
           //printf(" Populating ACTUAL obstacle for x, y, z = %d %d %d\n",x, y, z);
           bitmapset->bitmap[bt_type]->data[x][y][z].val = -2;  
          
           int i=0; 
           for(i=x-expansion;i<=x+expansion;i++)
           {
            //printf(" for i = %d\n",i);
            if(i>=0&&i<bitmap->nx)
            {
             int j=0;
             for(j=y-expansion;j<=y+expansion;j++)
             {
              //printf(" for i, j = %d %d\n",i, j);
              if(j>=0&&j<bitmap->ny)
              {
               int k=0;
                for(k=z-expansion;k<=z+expansion;k++)
               {
              //printf(" for i, j, k = %d %d %\n",i, j, k);
              if(k>=0&&k<bitmap->nz)
                {
                // printf(" Populating %d, %d, %d \n",i,j,k);
                 bitmapset->bitmap[bt_type]->data[i][j][k].val = -2;  
                }
               } 
              }
             }
            } 
           } 
          }
	 else
          {
	   bitmapset->bitmap[bt_type]->data[x][y][z].val = 0;
          } 
       }
     }
  }
  p3d_destroy_config(bitmapset->visball, visq);

////if(bt_type==HRP2_GIK_MANIP)
 //// p3d_col_activate_rob_rob(bitmapset->visball,bitmapset->robot);
//p3d_col_activate_rob_rob(ACBTSET->visball,ACBTSET->human[ACBTSET->actual_human]->HumanPt);

  return TRUE;
 }
}

int create_3d_grid_for_HRP2_GIK()
{
 configPt rob_cur_pos = MY_ALLOC(double,ACBTSET->robot->nb_dof); /* Allocation of temporary robot configuration */
 p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos);
 
 double BB_half_length=1.5; // along x axis
 double BB_half_width=1.5; // along y axis
 double BB_half_height=2.0; // along z axis
/*
 double BB_half_length=0.5; // along x axis
 double BB_half_width=0.5; // along y axis
 double BB_half_height=2; // along z axis
*/
 //Bounding box coordinates are in global frame
 grid_around_HRP2.grid_BB.min_x=rob_cur_pos[6]-BB_half_length+0.5;
 grid_around_HRP2.grid_BB.max_x=rob_cur_pos[6]+BB_half_length+0.5;
 grid_around_HRP2.grid_BB.min_y=rob_cur_pos[7]-BB_half_width;
 grid_around_HRP2.grid_BB.max_y=rob_cur_pos[7]+BB_half_width;
 grid_around_HRP2.grid_BB.min_z=rob_cur_pos[8]-0.05;//-BB_length;//No need to go underground :)
 grid_around_HRP2.grid_BB.max_z=rob_cur_pos[8]+BB_half_height;
   
  double BB_length=grid_around_HRP2.grid_BB.max_x-grid_around_HRP2.grid_BB.min_x;
  double BB_width=grid_around_HRP2.grid_BB.max_y-grid_around_HRP2.grid_BB.min_y;
  double BB_height=grid_around_HRP2.grid_BB.max_z-grid_around_HRP2.grid_BB.min_z;

 if(grid_around_HRP2.GRID_SET != NULL)
 hri_bt_destroy_bitmapset(grid_around_HRP2.GRID_SET);

 double sampling_rate=0.05;

 grid_around_HRP2.GRID_SET=create_3D_grid(BB_length,BB_width,BB_height,sampling_rate);
 
 hri_bt_change_bitmap_position(grid_around_HRP2.GRID_SET,grid_around_HRP2.grid_BB.min_x,grid_around_HRP2.grid_BB.min_y,grid_around_HRP2.grid_BB.min_z);

 int expansion=1; //To populate the number of cells, around a cell having obstacle, with obstacles also

 //***** AKP : To populate bitmap based on the exact obstacles' position, not based on their bounding Boxes..
/* 
 ChronoPrint("<<<<<<<<Calling create_exact_obstacles_for_HRP2_GIK_manip for HRP2_GIK_MANIP bitmap");
 create_exact_obstacles_for_HRP2_GIK_manip(grid_around_HRP2.GRID_SET,expansion,HRP2_GIK_MANIP); 
 ChronoPrint("<<<<<<<<Calling create_exact_obstacles_for_HRP2_GIK_manip for BT_AFFORDANCE_VISIBILITY bitmap");
 create_exact_obstacles_for_HRP2_GIK_manip(grid_around_HRP2.GRID_SET,expansion,BT_AFFORDANCE_VISIBILITY); 
 ChronoPrint("<<<<<<<<Returned from create_exact_obstacles_for_HRP2_GIK_manip ");
 */
 //***** AKP: Uncomment below to populate the bitmap based on the bounding box of the objects' and robots'
 ////create_obstacles_for_HRP2_GIK_manip(grid_around_HRP2.GRID_SET); 

 ////////////ChronoPrint("<<<<<<<<Calling create_exact_obstacles_for_HRP2_GIK_manip");
//// create_exact_obstacles_for_HRP2_GIK_manip(grid_around_HRP2.GRID_SET,expansion,HRP2_GIK_MANIP); //-1 is for populating every bitmap type 
 ////create_exact_obstacles_for_HRP2_GIK_manip_fast(grid_around_HRP2.GRID_SET,expansion,HRP2_GIK_MANIP); //-1 is for populating every bitmap type 
 create_exact_obstacles_for_HRP2_GIK_manip_fast_new(grid_around_HRP2.GRID_SET,expansion,HRP2_GIK_MANIP); //-1 is for populating every bitmap type 
 ////////////ChronoPrint("<<<<<<<<Returned from create_exact_obstacles_for_HRP2_GIK_manip");

 MY_FREE(rob_cur_pos, double,ACBTSET->robot->nb_dof); 
return 1;
}

int show_3D_workspace_Bounding_Box()
{
//Line id A onwards in alphabetical order. See notebook for fig
g3d_drawOneLine(
        grid_around_HRP2.grid_BB.min_x, grid_around_HRP2.grid_BB.min_y, grid_around_HRP2.grid_BB.min_z,
        grid_around_HRP2.grid_BB.max_x, grid_around_HRP2.grid_BB.min_y, grid_around_HRP2.grid_BB.min_z, Red, NULL);

g3d_drawOneLine(
        grid_around_HRP2.grid_BB.max_x, grid_around_HRP2.grid_BB.min_y, grid_around_HRP2.grid_BB.min_z,
        grid_around_HRP2.grid_BB.max_x, grid_around_HRP2.grid_BB.max_y, grid_around_HRP2.grid_BB.min_z, Red, NULL);

g3d_drawOneLine(
        grid_around_HRP2.grid_BB.max_x, grid_around_HRP2.grid_BB.max_y, grid_around_HRP2.grid_BB.min_z,
        grid_around_HRP2.grid_BB.min_x, grid_around_HRP2.grid_BB.max_y, grid_around_HRP2.grid_BB.min_z, Red, NULL);

g3d_drawOneLine(
        grid_around_HRP2.grid_BB.min_x, grid_around_HRP2.grid_BB.max_y, grid_around_HRP2.grid_BB.min_z,
        grid_around_HRP2.grid_BB.min_x, grid_around_HRP2.grid_BB.min_y, grid_around_HRP2.grid_BB.min_z, Red, NULL);

g3d_drawOneLine(
        grid_around_HRP2.grid_BB.min_x, grid_around_HRP2.grid_BB.min_y, grid_around_HRP2.grid_BB.max_z,
        grid_around_HRP2.grid_BB.max_x, grid_around_HRP2.grid_BB.min_y, grid_around_HRP2.grid_BB.max_z, Red, NULL);

g3d_drawOneLine(
        grid_around_HRP2.grid_BB.max_x, grid_around_HRP2.grid_BB.min_y, grid_around_HRP2.grid_BB.max_z,
        grid_around_HRP2.grid_BB.max_x, grid_around_HRP2.grid_BB.max_y, grid_around_HRP2.grid_BB.max_z, Red, NULL);

g3d_drawOneLine(
        grid_around_HRP2.grid_BB.max_x, grid_around_HRP2.grid_BB.max_y, grid_around_HRP2.grid_BB.max_z,
        grid_around_HRP2.grid_BB.min_x, grid_around_HRP2.grid_BB.max_y, grid_around_HRP2.grid_BB.max_z, Red, NULL);

g3d_drawOneLine(
        grid_around_HRP2.grid_BB.min_x, grid_around_HRP2.grid_BB.max_y, grid_around_HRP2.grid_BB.max_z,
        grid_around_HRP2.grid_BB.min_x, grid_around_HRP2.grid_BB.min_y, grid_around_HRP2.grid_BB.max_z, Red, NULL);

g3d_drawOneLine(
        grid_around_HRP2.grid_BB.max_x, grid_around_HRP2.grid_BB.min_y, grid_around_HRP2.grid_BB.min_z,
        grid_around_HRP2.grid_BB.max_x, grid_around_HRP2.grid_BB.min_y, grid_around_HRP2.grid_BB.max_z, Red, NULL);

g3d_drawOneLine(
        grid_around_HRP2.grid_BB.max_x, grid_around_HRP2.grid_BB.max_y, grid_around_HRP2.grid_BB.min_z,
        grid_around_HRP2.grid_BB.max_x, grid_around_HRP2.grid_BB.max_y, grid_around_HRP2.grid_BB.max_z, Red, NULL);

g3d_drawOneLine(
        grid_around_HRP2.grid_BB.min_x, grid_around_HRP2.grid_BB.min_y, grid_around_HRP2.grid_BB.min_z,
        grid_around_HRP2.grid_BB.min_x, grid_around_HRP2.grid_BB.min_y, grid_around_HRP2.grid_BB.max_z, Red, NULL);

g3d_drawOneLine(
        grid_around_HRP2.grid_BB.min_x, grid_around_HRP2.grid_BB.max_y, grid_around_HRP2.grid_BB.min_z,
        grid_around_HRP2.grid_BB.min_x, grid_around_HRP2.grid_BB.max_y, grid_around_HRP2.grid_BB.max_z, Red, NULL);

return 1;
}

#ifdef USE_SYM_GEO_PLAN
int show_candidate_points_for_curr_geo_node()
{
     int i=0;
     for(i=0;i<curr_geo_plan.GP_node[CURR_GEO_NODE].candidate_points.no_points;i++)
     { 
     
     g3d_drawDisc(curr_geo_plan.GP_node[CURR_GEO_NODE].candidate_points.point[i].x,curr_geo_plan.GP_node[CURR_GEO_NODE].candidate_points.point[i].y,curr_geo_plan.GP_node[CURR_GEO_NODE].candidate_points.point[i].z,  grid_around_HRP2.GRID_SET->pace/4.0, 4, NULL);
     
   
     }      

 return 1;
}
#endif

int show_weighted_candidate_points_for_putinto_obj(int show_weight)
{
  ////printf("Inside show_weighted_candidate_points_for_putinto_obj, current_candidate_points_to_putinto.no_points=%d\n",current_candidate_points_to_putinto.no_points);
  int i=0;
  for(i=0;i<current_candidate_points_to_putinto.no_points;i++)
  {
   ////////g3d_draw_vertical_cylinder(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y, candidate_points_to_put.point[i].z, .02, candidate_points_to_put.weight[i]/2.0, 0.005, 4, NULL);
   g3d_drawDisc(current_candidate_points_to_putinto.point[i].x, current_candidate_points_to_putinto.point[i].y, current_candidate_points_to_putinto.point[i].z, .02, Green, NULL);
   ////////g3d_drawOneLine(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y,candidate_points_to_put.point[i].z,ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[0][3], ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[1][3], ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[2][3], Red, NULL);
  if(show_weight==1)
   {
  g3d_drawOneLine(current_candidate_points_to_putinto.point[i].x, current_candidate_points_to_putinto.point[i].y,current_candidate_points_to_putinto.point[i].z, current_candidate_points_to_putinto.point[i].x, current_candidate_points_to_putinto.point[i].y, current_candidate_points_to_putinto.point[i].z+current_candidate_points_to_putinto.weight[i]/2.0, Green, NULL);
   }
  }
   /*
  i=0;
  for(i=0;i<candidate_points_to_putinto_blue_trashbin_by_jido.no_points;i++)
  {
   ////////g3d_draw_vertical_cylinder(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y, candidate_points_to_put.point[i].z, .02, candidate_points_to_put.weight[i]/2.0, 0.005, 4, NULL);
   g3d_drawDisc(candidate_points_to_putinto_blue_trashbin_by_jido.point[i].x, candidate_points_to_putinto_blue_trashbin_by_jido.point[i].y, candidate_points_to_putinto_blue_trashbin_by_jido.point[i].z, .02, Green, NULL);
   ////////g3d_drawOneLine(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y,candidate_points_to_put.point[i].z,ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[0][3], ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[1][3], ACBTSET
  
  }
 

  i=0;
  for(i=0;i<candidate_points_to_putinto_by_jido.no_points;i++)
  {
   ////////g3d_draw_vertical_cylinder(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y, candidate_points_to_put.point[i].z, .02, candidate_points_to_put.weight[i]/2.0, 0.005, 4, NULL);
   g3d_drawDisc(candidate_points_to_putinto_by_jido.point[i].x, candidate_points_to_putinto_by_jido.point[i].y, candidate_points_to_putinto_by_jido.point[i].z, .02, Green, NULL);
   ////////g3d_drawOneLine(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y,candidate_points_to_put.point[i].z,ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[0][3], ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[1][3], ACBTSET
  
  }
 */
return 1;
}


int show_weighted_candidate_points_to_put_obj(int show_weight)
{
  int i=0;
  for(i=0;i<candidate_points_to_put.no_points;i++)
  {
   ////////g3d_draw_vertical_cylinder(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y, candidate_points_to_put.point[i].z, .02, candidate_points_to_put.weight[i]/2.0, 0.005, 4, NULL);
   g3d_drawDisc(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y, candidate_points_to_put.point[i].z, .02, 4, NULL);
   ////////g3d_drawOneLine(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y,candidate_points_to_put.point[i].z,ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[0][3], ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[1][3], ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[2][3], Red, NULL);
  if(show_weight==1)
   {
  g3d_drawOneLine(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y,candidate_points_to_put.point[i].z, candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y, candidate_points_to_put.point[i].z+candidate_points_to_put.weight[i]/2.0, Green, NULL);
   }
  }
return 1;
}

int show_weighted_candidate_points_to_show_obj()
{
  int i=0;
  for(i=0;i<candidate_points_to_show.no_points;i++)
  {
   ////////g3d_draw_vertical_cylinder(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y, candidate_points_to_put.point[i].z, .02, candidate_points_to_put.weight[i]/2.0, 0.005, 4, NULL);
   g3d_drawDisc(candidate_points_to_show.point[i].x, candidate_points_to_show.point[i].y, candidate_points_to_show.point[i].z, .02, 4, NULL);
   ////////g3d_drawOneLine(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y,candidate_points_to_put.point[i].z,ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[0][3], ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[1][3], ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[2][3], Red, NULL);
  g3d_drawOneLine(candidate_points_to_show.point[i].x, candidate_points_to_show.point[i].y,candidate_points_to_show.point[i].z, candidate_points_to_show.point[i].x, candidate_points_to_show.point[i].y, candidate_points_to_show.point[i].z+candidate_points_to_show.weight[i]/2.0, Green, NULL);
  }
return 1;
}


int show_weighted_candidate_points_to_hide_obj()
{
  int i=0;
  for(i=0;i<candidate_points_to_hide.no_points;i++)
  {
   ////////g3d_draw_vertical_cylinder(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y, candidate_points_to_put.point[i].z, .02, candidate_points_to_put.weight[i]/2.0, 0.005, 4, NULL);
   g3d_drawDisc(candidate_points_to_hide.point[i].x, candidate_points_to_hide.point[i].y, candidate_points_to_hide.point[i].z, .02, 4, NULL);
   ////////g3d_drawOneLine(candidate_points_to_put.point[i].x, candidate_points_to_put.point[i].y,candidate_points_to_put.point[i].z,ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[0][3], ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[1][3], ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[2][3], Red, NULL);
  
   g3d_drawOneLine(candidate_points_to_hide.point[i].x, candidate_points_to_hide.point[i].y,candidate_points_to_hide.point[i].z, candidate_points_to_hide.point[i].x, candidate_points_to_hide.point[i].y, candidate_points_to_hide.point[i].z+candidate_points_to_hide.weight[i]/2.0, Green, NULL);
  }
return 1;
}


int reverse_sort_weighted_candidate_points_to_putinto_obj()
{
  ////point_co_ordi sorted_points[1000];
int i=0;
 for(i=0;i<current_candidate_points_to_putinto.no_points;i++)  

  //////////printf("Inside sort_weighted_candidate_points_to_putinto_obj(), current_candidate_points_to_putinto.no_points=%d\n",current_candidate_points_to_putinto.no_points);

  //////////printf(" Before sorting \n");
  for(int i=0;i<current_candidate_points_to_putinto.no_points;i++)
  {
  //////////printf(" >>>>>>>>> current_candidate_points_to_putinto.weight[%d]=%lf\n",i, current_candidate_points_to_putinto.weight[i]); 
  }  

  for (int i = 0; i < current_candidate_points_to_putinto.no_points; i++) 
  {
   int curmax = i;
   for (int j = i+1; j < current_candidate_points_to_putinto.no_points; j++) 
   {
    if(current_candidate_points_to_putinto.weight[j]>current_candidate_points_to_putinto.weight[curmax])
    {
     curmax=j;
    } 
   }
   if (curmax != i)
   {
    double tmp_wt=current_candidate_points_to_putinto.weight[i];
    double tmp_x=current_candidate_points_to_putinto.point[i].x;
    double tmp_y=current_candidate_points_to_putinto.point[i].y;
    double tmp_z=current_candidate_points_to_putinto.point[i].z;
    current_candidate_points_to_putinto.weight[i]=current_candidate_points_to_putinto.weight[curmax];
    current_candidate_points_to_putinto.point[i].x=current_candidate_points_to_putinto.point[curmax].x;
    current_candidate_points_to_putinto.point[i].y=current_candidate_points_to_putinto.point[curmax].y;
    current_candidate_points_to_putinto.point[i].z=current_candidate_points_to_putinto.point[curmax].z;
 
    current_candidate_points_to_putinto.weight[curmax]=tmp_wt;
    current_candidate_points_to_putinto.point[curmax].x=tmp_x;
    current_candidate_points_to_putinto.point[curmax].y=tmp_y;
    current_candidate_points_to_putinto.point[curmax].z=tmp_z;

   }
  }
   
  //////////printf(" After sorting \n");
  for(int i=0;i<current_candidate_points_to_putinto.no_points;i++)
  {
  //////////printf(" <<<<<<<< current_candidate_points_to_putinto.weight[%d]=%lf\n",i, current_candidate_points_to_putinto.weight[i]); 
  }  
 return 1;
}


int reverse_sort_weighted_candidate_points_to_put_obj()
{
  ////point_co_ordi sorted_points[1000];
  
  //////////printf("Inside sort_weighted_candidate_points_to_put_obj(), candidate_points_to_put.no_points=%d\n",candidate_points_to_put.no_points);

  //////////printf(" Before sorting \n");
  for(int i=0;i<candidate_points_to_put.no_points;i++)
  {
  //////////printf(" candidate_points_to_put.weight[%d]=%lf\n",i, candidate_points_to_put.weight[i]); 
  }  

  for (int i = 0; i < candidate_points_to_put.no_points; i++) 
  {
   int curmax = i;
   for (int j = i+1; j < candidate_points_to_put.no_points; j++) 
   {
    if(candidate_points_to_put.weight[j]>candidate_points_to_put.weight[curmax])
    {
     curmax=j;
    } 
   }
   if (curmax != i)
   {
    double tmp_wt=candidate_points_to_put.weight[i];
    double tmp_x=candidate_points_to_put.point[i].x;
    double tmp_y=candidate_points_to_put.point[i].y;
    double tmp_z=candidate_points_to_put.point[i].z;
    candidate_points_to_put.weight[i]=candidate_points_to_put.weight[curmax];
    candidate_points_to_put.point[i].x=candidate_points_to_put.point[curmax].x;
    candidate_points_to_put.point[i].y=candidate_points_to_put.point[curmax].y;
    candidate_points_to_put.point[i].z=candidate_points_to_put.point[curmax].z;
 
    candidate_points_to_put.weight[curmax]=tmp_wt;
    candidate_points_to_put.point[curmax].x=tmp_x;
    candidate_points_to_put.point[curmax].y=tmp_y;
    candidate_points_to_put.point[curmax].z=tmp_z;

   }
  }
   
  //////////printf(" After sorting \n");
  for(int i=0;i<candidate_points_to_put.no_points;i++)
  {
  //////////printf(" candidate_points_to_put.weight[%d]=%lf\n",i, candidate_points_to_put.weight[i]); 
  }  
 return 1;
}

int reverse_sort_weighted_candidate_points_to_show_obj()
{
  ////point_co_ordi sorted_points[1000];
  
  //////////printf("Inside reverse_sort_weighted_candidate_points_to_show_obj(), candidate_points_to_show.no_points=%d\n",candidate_points_to_show.no_points);

  //////////printf(" Before sorting \n");
  for(int i=0;i<candidate_points_to_show.no_points;i++)
  {
  //////////printf(" candidate_points_to_show.weight[%d]=%lf\n",i, candidate_points_to_show.weight[i]); 
  }  

  for (int i = 0; i < candidate_points_to_show.no_points; i++) 
  {
   int curmax = i;
   for (int j = i+1; j < candidate_points_to_show.no_points; j++) 
   {
    if(candidate_points_to_show.weight[j]>candidate_points_to_show.weight[curmax])
    {
     curmax=j;
    } 
   }
   if (curmax != i)
   {
    double tmp_wt=candidate_points_to_show.weight[i];
    double tmp_x=candidate_points_to_show.point[i].x;
    double tmp_y=candidate_points_to_show.point[i].y;
    double tmp_z=candidate_points_to_show.point[i].z;
    candidate_points_to_show.weight[i]=candidate_points_to_show.weight[curmax];
    candidate_points_to_show.point[i].x=candidate_points_to_show.point[curmax].x;
    candidate_points_to_show.point[i].y=candidate_points_to_show.point[curmax].y;
    candidate_points_to_show.point[i].z=candidate_points_to_show.point[curmax].z;
 
    candidate_points_to_show.weight[curmax]=tmp_wt;
    candidate_points_to_show.point[curmax].x=tmp_x;
    candidate_points_to_show.point[curmax].y=tmp_y;
    candidate_points_to_show.point[curmax].z=tmp_z;

   }
  }
   
  //////////printf(" After sorting \n");
  for(int i=0;i<candidate_points_to_show.no_points;i++)
  {
  //////////printf(" candidate_points_to_show.weight[%d]=%lf\n",i, candidate_points_to_show.weight[i]); 
  }  
 return 1;
}

int reverse_sort_weighted_candidate_points_to_hide_obj()
{
  ////point_co_ordi sorted_points[1000];
  
  //////////printf("Inside reverse_sort_weighted_candidate_points_to_hide_obj(), candidate_points_to_hide.no_points=%d\n",candidate_points_to_hide.no_points);

 ////////// printf(" Before sorting \n");
  for(int i=0;i<candidate_points_to_hide.no_points;i++)
  {
  //////////printf(" candidate_points_to_hide.weight[%d]=%lf\n",i, candidate_points_to_hide.weight[i]); 
  }  

  for (int i = 0; i < candidate_points_to_hide.no_points; i++) 
  {
   int curmax = i;
   for (int j = i+1; j < candidate_points_to_hide.no_points; j++) 
   {
    if(candidate_points_to_hide.weight[j]>candidate_points_to_hide.weight[curmax])
    {
     curmax=j;
    } 
   }
   if (curmax != i)
   {
    double tmp_wt=candidate_points_to_hide.weight[i];
    double tmp_x=candidate_points_to_hide.point[i].x;
    double tmp_y=candidate_points_to_hide.point[i].y;
    double tmp_z=candidate_points_to_hide.point[i].z;
    candidate_points_to_hide.weight[i]=candidate_points_to_hide.weight[curmax];
    candidate_points_to_hide.point[i].x=candidate_points_to_hide.point[curmax].x;
    candidate_points_to_hide.point[i].y=candidate_points_to_hide.point[curmax].y;
    candidate_points_to_hide.point[i].z=candidate_points_to_hide.point[curmax].z;
 
    candidate_points_to_hide.weight[curmax]=tmp_wt;
    candidate_points_to_hide.point[curmax].x=tmp_x;
    candidate_points_to_hide.point[curmax].y=tmp_y;
    candidate_points_to_hide.point[curmax].z=tmp_z;

   }
  }
   
  //////////printf(" After sorting \n");
  for(int i=0;i<candidate_points_to_hide.no_points;i++)
  {
  //////////printf(" candidate_points_to_hide.weight[%d]=%lf\n",i, candidate_points_to_hide.weight[i]); 
  }  
 return 1;
}


int assign_weights_on_candidte_points_to_put_obj()
{
  p3d_vector4 point_in_global_frame;
  p3d_vector4 point_in_human_frame;
  p3d_matrix4 hum_pos_inverse;
  double relative_yaw, relative_pitch; 
  
  p3d_matInvertXform(ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos, hum_pos_inverse);

   
  int i=0;
  double Amplitude=1;
  double yaw_mean=0;
  double pitch_mean=0;
  double sig_yaw=M_PI;

  for(i=0;i<candidate_points_to_put.no_points;i++)
  {
  point_in_global_frame[0] = candidate_points_to_put.point[i].x;
  point_in_global_frame[1] = candidate_points_to_put.point[i].y;
  point_in_global_frame[2] = candidate_points_to_put.point[i].z;
  point_in_global_frame[3] = 1;
  
  
  ////Assigning weight wrt human axis
  p3d_matvec4Mult(hum_pos_inverse, point_in_global_frame, point_in_human_frame);
  
  p3d_psp_cartesian2spherical(point_in_human_frame[0],point_in_human_frame[1],point_in_human_frame[2],0,0,0,&relative_yaw,&relative_pitch);
  
  
  ////////candidate_points_to_put.weight[i]+=(1.0-(fabs(relative_yaw))/M_PI);

  ////////candidate_points_to_put.weight[i]=Amplitude*exp(-(((yaw_mean-relative_yaw)*(yaw_mean-relative_yaw)/2.0*sig_x*sig_x)+((pitch_mean-relative_yaw)*(pitch_mean-relative_yaw)/2.0*sig_y*sig_y))));

  candidate_points_to_put.weight[i]=Amplitude*exp(-(((yaw_mean-relative_yaw)*(yaw_mean-relative_yaw)/2.0*sig_yaw*sig_yaw)));

  //////////printf(" relative_yaw=%lf, weight for candidate point %d to put with pos (%lf,%lf,%lf) is %lf\n",relative_yaw,i,point_in_global_frame[0],point_in_global_frame[1],point_in_global_frame[2],candidate_points_to_put.weight[i]);

  
  ////Assigning weight based on the closeness to the point which is at dist 0.3 m from the human 
  double human_pos[3];
  double sig_hum_dist=0.7;
  double mean_dist=0.3;
  human_pos[0]=ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[0][3];
  human_pos[1]=ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[1][3];
  human_pos[2]=ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[2][3];
  ////printf("bottle pos from ACBTSET->object = (%lf, %lf, %lf)\n",bottle_pos[0],bottle_pos[1],bottle_pos[2]); 
  double point_to_human_dist=sqrt((human_pos[0]-point_in_global_frame[0])*(human_pos[0]-point_in_global_frame[0])+(human_pos[1]-point_in_global_frame[1])*(human_pos[1]-point_in_global_frame[1])+(human_pos[2]-point_in_global_frame[2])*(human_pos[2]-point_in_global_frame[2]));
  //////////printf(" point to human dist = %lf \n", point_to_human_dist);
  candidate_points_to_put.weight[i]+=Amplitude/2.0*exp(-(((mean_dist-point_to_human_dist)*(mean_dist-point_to_human_dist)/2.0*sig_hum_dist*sig_hum_dist)));
  

  ////Assigning weight based on the closeness to the bottle current position
  double bottle_pos[3];
  double sig_dist=2.0;
  bottle_pos[0]=ACBTSET->object->joints[1]->abs_pos[0][3];
  bottle_pos[1]=ACBTSET->object->joints[1]->abs_pos[1][3];
  bottle_pos[2]=ACBTSET->object->joints[1]->abs_pos[2][3];
  ////printf("bottle pos from ACBTSET->object = (%lf, %lf, %lf)\n",bottle_pos[0],bottle_pos[1],bottle_pos[2]); 
  double point_to_bottle_dist=sqrt((bottle_pos[0]-point_in_global_frame[0])*(bottle_pos[0]-point_in_global_frame[0])+(bottle_pos[1]-point_in_global_frame[1])*(bottle_pos[1]-point_in_global_frame[1])+(bottle_pos[2]-point_in_global_frame[2])*(bottle_pos[2]-point_in_global_frame[2]));
  
  candidate_points_to_put.weight[i]+=Amplitude/1.5*exp(-(((point_to_bottle_dist)*(point_to_bottle_dist)/2.0*sig_dist*sig_dist)));

  /*if(fabs(point_to_bottle_dist)<0.00001)
  candidate_points_to_put.weight[i]+=1;
  else
  candidate_points_to_put.weight[i]+=(1.0/point_to_bottle_dist);
  */
  } 
  
 
  
return 1;
  
}

int assign_weights_on_candidte_points_to_show_obj()
{
  
  p3d_vector4 point_in_global_frame;
  p3d_vector4 point_in_human_frame;
  p3d_matrix4 hum_pos_inverse;
  double relative_yaw, relative_pitch; 
  
  p3d_matInvertXform(ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_NECK_PAN]->abs_pos, hum_pos_inverse);

  configPt hum_cur_pos = MY_ALLOC(double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof); /* Allocation of temporary robot configuration */

 p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);


////double yaw=M_PI/3.0;

////double orig_pan=hum_cur_pos[HUMANq_PAN]; 
  int i=0;
  double Amplitude=1.0;
  double yaw_mean=hum_cur_pos[HUMANq_PAN];
  double pitch_mean=hum_cur_pos[HUMANq_TILT];
  double sig_yaw=M_PI/2.0;
  double sig_pitch=M_PI/2.0;
  double max_weight=0;

  for(i=0;i<candidate_points_to_show.no_points;i++)
  {
  point_in_global_frame[0] = candidate_points_to_show.point[i].x;
  point_in_global_frame[1] = candidate_points_to_show.point[i].y;
  point_in_global_frame[2] = candidate_points_to_show.point[i].z;
  point_in_global_frame[3] = 1;
  
  
  ////Assigning weight wrt human axis
  p3d_matvec4Mult(hum_pos_inverse, point_in_global_frame, point_in_human_frame);
  
  p3d_psp_cartesian2spherical(point_in_human_frame[0],point_in_human_frame[1],point_in_human_frame[2],0,0,0,&relative_yaw,&relative_pitch);
  
  
  ////////candidate_points_to_put.weight[i]+=(1.0-(fabs(relative_yaw))/M_PI);

  ////printf(" For point %d \n",i);
    candidate_points_to_show.weight[i]=Amplitude*exp(-(((yaw_mean-relative_yaw)*(yaw_mean-relative_yaw)/2.0*sig_yaw*sig_yaw)+((pitch_mean-relative_pitch)*(pitch_mean-relative_pitch)/2.0*sig_pitch*sig_pitch)));

   if(max_weight<candidate_points_to_show.weight[i])
   max_weight=candidate_points_to_show.weight[i];
  ////////candidate_points_to_show.weight[i]=Amplitude*exp(-(((yaw_mean-relative_yaw)*(yaw_mean-relative_yaw)/2.0*sig_yaw*sig_yaw)));
  
  //////////printf(" relative_yaw=%lf, relative_pitch=%lf, weight for candidate point %d to put with pos (%lf,%lf,%lf) is %lf\n",relative_yaw,relative_pitch, i,point_in_global_frame[0],point_in_global_frame[1],point_in_global_frame[2],candidate_points_to_show.weight[i]);

  /*
  ////Assigning weight based on the closeness to the point which is at dist 0.3 m from the human 
  double human_pos[3];
  double sig_hum_dist=0.7;
  double mean_dist=0.3;
  human_pos[0]=ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[0][3];
  human_pos[1]=ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[1][3];
  human_pos[2]=ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[1]->abs_pos[2][3];
  ////printf("bottle pos from ACBTSET->object = (%lf, %lf, %lf)\n",bottle_pos[0],bottle_pos[1],bottle_pos[2]); 
  double point_to_human_dist=sqrt((human_pos[0]-point_in_global_frame[0])*(human_pos[0]-point_in_global_frame[0])+(human_pos[1]-point_in_global_frame[1])*(human_pos[1]-point_in_global_frame[1])+(human_pos[2]-point_in_global_frame[2])*(human_pos[2]-point_in_global_frame[2]));
  printf(" point to human dist = %lf \n", point_to_human_dist);
  candidate_points_to_put.weight[i]+=Amplitude/2.0*exp(-(((mean_dist-point_to_human_dist)*(mean_dist-point_to_human_dist)/2.0*sig_hum_dist*sig_hum_dist)));
  */

  /*
  ////Assigning weight based on the closeness to the bottle current position
  double bottle_pos[3];
  double sig_dist=2.0;
  bottle_pos[0]=ACBTSET->object->joints[1]->abs_pos[0][3];
  bottle_pos[1]=ACBTSET->object->joints[1]->abs_pos[1][3];
  bottle_pos[2]=ACBTSET->object->joints[1]->abs_pos[2][3];
  ////printf("bottle pos from ACBTSET->object = (%lf, %lf, %lf)\n",bottle_pos[0],bottle_pos[1],bottle_pos[2]); 
  double point_to_bottle_dist=sqrt((bottle_pos[0]-point_in_global_frame[0])*(bottle_pos[0]-point_in_global_frame[0])+(bottle_pos[1]-point_in_global_frame[1])*(bottle_pos[1]-point_in_global_frame[1])+(bottle_pos[2]-point_in_global_frame[2])*(bottle_pos[2]-point_in_global_frame[2]));
  
  candidate_points_to_show.weight[i]+=max_weight*exp(-(((point_to_bottle_dist)*(point_to_bottle_dist)/2.0*sig_dist*sig_dist)));
  */
  /*if(fabs(point_to_bottle_dist)<0.00001)
  candidate_points_to_put.weight[i]+=1;
  else
  candidate_points_to_put.weight[i]+=(1.0/point_to_bottle_dist);
  */
  } 
  
  //////////printf(" max_weight=%lf\n",max_weight);
  for(i=0;i<candidate_points_to_show.no_points;i++)
  {
  point_in_global_frame[0] = candidate_points_to_show.point[i].x;
  point_in_global_frame[1] = candidate_points_to_show.point[i].y;
  point_in_global_frame[2] = candidate_points_to_show.point[i].z;
  point_in_global_frame[3] = 1;
  ////Assigning weight based on the closeness to the bottle current position
  double bottle_pos[3];
  double sig_dist=2.0;
  bottle_pos[0]=ACBTSET->object->joints[1]->abs_pos[0][3];
  bottle_pos[1]=ACBTSET->object->joints[1]->abs_pos[1][3];
  bottle_pos[2]=ACBTSET->object->joints[1]->abs_pos[2][3];
  ////printf("bottle pos from ACBTSET->object = (%lf, %lf, %lf)\n",bottle_pos[0],bottle_pos[1],bottle_pos[2]); 
  double point_to_bottle_dist=sqrt((bottle_pos[0]-point_in_global_frame[0])*(bottle_pos[0]-point_in_global_frame[0])+(bottle_pos[1]-point_in_global_frame[1])*(bottle_pos[1]-point_in_global_frame[1])+(bottle_pos[2]-point_in_global_frame[2])*(bottle_pos[2]-point_in_global_frame[2]));
  
  //////////printf(" Old weight for candidate point %d to put with pos (%lf,%lf,%lf) is %lf\n", i,point_in_global_frame[0],point_in_global_frame[1],point_in_global_frame[2],candidate_points_to_show.weight[i]);
  candidate_points_to_show.weight[i]+=2.0*max_weight*exp(-(((point_to_bottle_dist)*(point_to_bottle_dist)/2.0*sig_dist*sig_dist)));
  
  //////////printf(" New weight for candidate point %d to put with pos (%lf,%lf,%lf) is %lf\n", i,point_in_global_frame[0],point_in_global_frame[1],point_in_global_frame[2],candidate_points_to_show.weight[i]);
  }
  
MY_FREE(hum_cur_pos,double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof);

  return 1;
}


int assign_weights_on_candidte_points_to_hide_obj()
{
  
  p3d_vector4 point_in_global_frame;
  p3d_vector4 point_in_human_frame;
  p3d_matrix4 hum_pos_inverse;
  double relative_yaw, relative_pitch; 
  
  p3d_matInvertXform(ACBTSET->human[ACBTSET->actual_human]->HumanPt->joints[HUMANj_NECK_PAN]->abs_pos, hum_pos_inverse);

  configPt hum_cur_pos = MY_ALLOC(double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof); /* Allocation of temporary robot configuration */

 p3d_get_robot_config_into(ACBTSET->human[ACBTSET->actual_human]->HumanPt,&hum_cur_pos);


////double yaw=M_PI/3.0;

////double orig_pan=hum_cur_pos[HUMANq_PAN]; 
  int i=0;
  double Amplitude=1.0;
  double yaw_mean=hum_cur_pos[HUMANq_PAN];
  double pitch_mean=hum_cur_pos[HUMANq_TILT];
  double sig_yaw=M_PI/2.0;
  double sig_pitch=M_PI/2.0;
  double max_weight=0;
////int i=0;
  double bottle_pos[3];
  double sig_dist=2.0;
  bottle_pos[0]=ACBTSET->object->joints[1]->abs_pos[0][3];
  bottle_pos[1]=ACBTSET->object->joints[1]->abs_pos[1][3];
  bottle_pos[2]=ACBTSET->object->joints[1]->abs_pos[2][3];

  for(i=0;i<candidate_points_to_hide.no_points;i++)
  {
  point_in_global_frame[0] = candidate_points_to_hide.point[i].x;
  point_in_global_frame[1] = candidate_points_to_hide.point[i].y;
  point_in_global_frame[2] = candidate_points_to_hide.point[i].z;
  point_in_global_frame[3] = 1;
  ////Assigning weight based on the closeness to the bottle current position
  
  ////printf("bottle pos from ACBTSET->object = (%lf, %lf, %lf)\n",bottle_pos[0],bottle_pos[1],bottle_pos[2]); 
  double point_to_bottle_dist=sqrt((bottle_pos[0]-point_in_global_frame[0])*(bottle_pos[0]-point_in_global_frame[0])+(bottle_pos[1]-point_in_global_frame[1])*(bottle_pos[1]-point_in_global_frame[1])+(bottle_pos[2]-point_in_global_frame[2])*(bottle_pos[2]-point_in_global_frame[2]));
  
  ////printf(" Old weight for candidate point %d to put with pos (%lf,%lf,%lf) is %lf\n", i,point_in_global_frame[0],point_in_global_frame[1],point_in_global_frame[2],candidate_points_to_show.weight[i]);
  candidate_points_to_hide.weight[i]=Amplitude*exp(-(((point_to_bottle_dist)*(point_to_bottle_dist)/2.0*sig_dist*sig_dist)));
  
  //////////printf(" New weight for candidate point %d to hide with pos (%lf,%lf,%lf) is %lf\n", i,point_in_global_frame[0],point_in_global_frame[1],point_in_global_frame[2],candidate_points_to_hide.weight[i]);
  }
  
MY_FREE(hum_cur_pos,double,ACBTSET->human[ACBTSET->actual_human]->HumanPt->nb_dof);

  return 1;
}


int find_candidate_points_to_hide_obj_new()
{
 //////////printf(" Inside find_candidate_points_to_hide_obj_new()\n");
////no_candidate_points_to_show=0;
candidate_points_to_hide.no_points=0;

 int reachable_by_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_HRP2_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_bending=0;
  int reachable_by_turning_around=0;
  int visible_by_human=0;
  int visible_by_HRP2=0;

int x=0;
 for(x=0;x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx;x++)
 {
 int y=0;
 for(y=0;y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny;y++)
  {
  int z=0;
  for(z=0;z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz;z++)
   {
    ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].val<0)
    ////{
       ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.visible_by_human=0;
    if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_map_cell_obj_info.is_horizontal_surface==1)
    {
   reachable_by_hand=0;
   reachable_by_HRP2_hand=0;
   visible_by_human=0;
   visible_by_HRP2=0;
   reachable_by_turning_around=0;
   reachable_by_bending=0;

       double cell_x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double cell_y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double cell_z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
     
    
     ////////////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==0)
     if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==0&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human_straight_head_orientation==0&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human_neck_turn==0)
     {
     ////////// printf("* Not Visible by human\n");
      ////////////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.visible_by_HRP2==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.visible_by_HRP2_neck_turn==1)
      ////////////{
       ////////////printf("** Visible by HRP2 neck turn\n");
       ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.reachable_by_human_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.reachable_by_human_RHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.reachable_by_LHand_by_bending==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.reachable_by_RHand_by_bending==1)
       ////{
       //// printf("*** Bending reachable by by human \n");

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)
        {
       //////////  printf("**** Reachable by HRP2 \n");
         candidate_points_to_hide.point[candidate_points_to_hide.no_points].x=cell_x_world;
         candidate_points_to_hide.point[candidate_points_to_hide.no_points].y=cell_y_world;
         candidate_points_to_hide.point[candidate_points_to_hide.no_points].z=cell_z_world;
         candidate_points_to_hide.weight[candidate_points_to_hide.no_points]=0.0;
         candidate_points_to_hide.no_points++;

        } 
       ////} 
      ////////////} 
     } 
    }   
   }  
  }
 }
      
//////////printf(" candidate_points_to_hide.no_points=%d\n",candidate_points_to_hide.no_points);
 return candidate_points_to_hide.no_points;

}


int find_candidate_points_to_show_obj_new()
{
 //////////printf(" Inside find_candidate_points_to_show_obj_new()\n");
////no_candidate_points_to_show=0;
candidate_points_to_show.no_points=0;

 int reachable_by_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_HRP2_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_bending=0;
  int reachable_by_turning_around=0;
  int visible_by_human=0;
  int visible_by_HRP2=0;

int x=0;
 for(x=0;x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx;x++)
 {
 int y=0;
 for(y=0;y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny;y++)
  {
  int z=0;
  for(z=0;z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz;z++)
   {
    ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].val<0)
    ////{
       ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.visible_by_human=0;
    ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].is_horizontal_surface==1)
    {
   reachable_by_hand=0;
   reachable_by_HRP2_hand=0;
   visible_by_human=0;
   visible_by_HRP2=0;
   reachable_by_turning_around=0;
   reachable_by_bending=0;

       double cell_x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double cell_y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double cell_z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
     
    
     if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==1)
     {
      //////////printf("* Visible by human\n");
      if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_HRP2==1)
      {
      ////////// printf("** Visible by HRP2\n");
       ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.reachable_by_human_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.reachable_by_human_RHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.reachable_by_LHand_by_bending==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.reachable_by_RHand_by_bending==1)
       {
       //// printf("*** Bending reachable by by human \n");

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)
        {
        ////////// printf("**** Reachable by HRP2 \n");
         candidate_points_to_show.point[candidate_points_to_show.no_points].x=cell_x_world;
         candidate_points_to_show.point[candidate_points_to_show.no_points].y=cell_y_world;
         candidate_points_to_show.point[candidate_points_to_show.no_points].z=cell_z_world;
         candidate_points_to_show.weight[candidate_points_to_show.no_points]=0;
         candidate_points_to_show.no_points++;

        } 
       } 
      } 
     } 
    }   
   }  
  }
 }
      
//////////printf(" candidate_points_to_show.no_points=%d\n",candidate_points_to_show.no_points);
 return candidate_points_to_show.no_points;

}


int find_candidate_points_on_plane_to_put_obj_new()//(int type)//type=1 means only directly reachable places by human, 2 mean also bending reachable places by human
{
 //////////printf(" \n****Inside find_candidate_points_on_plane_to_put_obj_new()\n");
no_candidate_points_to_put=0;
candidate_points_to_put.no_points=0;

 int reachable_by_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_HRP2_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_bending=0;
  int reachable_by_turning_around=0;
  int visible_by_human=0;
  int visible_by_HRP2=0;

int x=0;
 for(x=0;x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx;x++)
 {
 int y=0;
 for(y=0;y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny;y++)
  {
  int z=0;
  for(z=0;z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz;z++)
   {
    ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].val<0)
    ////{
       ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.visible_by_human=0;
    if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_map_cell_obj_info.is_horizontal_surface==1)
    {
   reachable_by_hand=0;
   reachable_by_HRP2_hand=0;
   visible_by_human=0;
   visible_by_HRP2=0;
   reachable_by_turning_around=0;
   reachable_by_bending=0;

       double cell_x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double cell_y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double cell_z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
     
    
     if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human_straight_head_orientation==1)
     {
      ////printf("* Visible by human\n");
      ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.visible_by_HRP2==1)
      {
       ////printf("** Visible by HRP2\n");
       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1)
       {
        ////printf("*** Reachable by by human \n");
        #ifdef HRI_HRP2
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)
        {
         ////printf("**** Reachable by HRP2 \n");
         candidate_points_to_put.point[candidate_points_to_put.no_points].x=cell_x_world;
         candidate_points_to_put.point[candidate_points_to_put.no_points].y=cell_y_world;
         candidate_points_to_put.point[candidate_points_to_put.no_points].z=cell_z_world;
         candidate_points_to_put.weight[candidate_points_to_put.no_points]=0;
         candidate_points_to_put.no_points++;

        } 
        #endif
        #ifdef HRI_JIDO
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_JIDO_Hand==1)
        {
         ////printf("**** Reachable by HRP2 \n");
         candidate_points_to_put.point[candidate_points_to_put.no_points].x=cell_x_world;
         candidate_points_to_put.point[candidate_points_to_put.no_points].y=cell_y_world;
         candidate_points_to_put.point[candidate_points_to_put.no_points].z=cell_z_world;
         candidate_points_to_put.weight[candidate_points_to_put.no_points]=0;
         candidate_points_to_put.no_points++;

        } 
        #endif
       } 
      } 
     } 
    }   
   }  
  }
 }
      
//////////printf(" candidate_points_to_put.no_points=%d\n",candidate_points_to_put.no_points);
 return candidate_points_to_put.no_points;

}

int JIDO_find_candidate_points_on_plane_to_put_obj()
{
////////// printf(" \n****Inside JIDO_find_candidate_points_on_plane_to_put_obj()\n");
no_candidate_points_to_put=0;
candidate_points_to_put.no_points=0;

 int reachable_by_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_JIDO_hand=0; 
  int reachable_by_bending=0;
  int reachable_by_turning_around=0;
  int visible_by_human=0;
  int visible_by_JIDO=0;

int x=0;
 for(x=0;x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx;x++)
 {
 int y=0;
 for(y=0;y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny;y++)
  {
  int z=0;
  for(z=0;z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz;z++)
   {
    ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].val<0)
    ////{
       ////grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.visible_by_human=0;
    if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_map_cell_obj_info.is_horizontal_surface==1)
    {
   reachable_by_hand=0;
   reachable_by_JIDO_hand=0;
   visible_by_human=0;
   visible_by_JIDO=0;
   reachable_by_turning_around=0;
   reachable_by_bending=0;

       double cell_x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double cell_y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double cell_z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
     
    
     if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.visible_by_human_straight_head_orientation==1)
     {
     ////////// printf("* Visible by human\n");
      ////if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].affordances.visible_by_HRP2==1)
      {
       ////printf("** Visible by HRP2\n");
       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1)
       {
        //////////printf("*** Reachable by by human \n");

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_JIDO_Hand==1)
        {
        ////////// printf("**** Reachable by HRP2 \n");
         candidate_points_to_put.point[candidate_points_to_put.no_points].x=cell_x_world;
         candidate_points_to_put.point[candidate_points_to_put.no_points].y=cell_y_world;
         candidate_points_to_put.point[candidate_points_to_put.no_points].z=cell_z_world;
         candidate_points_to_put.weight[candidate_points_to_put.no_points]=0;
         candidate_points_to_put.no_points++;

        } 
       } 
      } 
     } 
    }   
   }  
  }
 }
      
//////////printf(" candidate_points_to_put.no_points=%d\n",candidate_points_to_put.no_points);
 return candidate_points_to_put.no_points;

}


int find_candidate_points_on_plane_to_put_obj()
{
no_candidate_points_to_put=0;
candidate_points_to_put.no_points=0;

int current_surface_index=0;
for(;current_surface_index<curr_surfaces_in_env.total_no_of_surfaces;current_surface_index++)
{

 int i=0;
 for(;i<curr_surfaces_in_env.flat_surf[current_surface_index].grid_i_max;i++)
 {
  int j=0;
  int show_cell=0;
  int reachable_by_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_HRP2_hand=0; //0 Means not reachable by any hand, 1 means reachable by right hand only, 2 means reachable by left hand only, 3 means reachable by both hands;
  int reachable_by_bending=0;
  int reachable_by_turning_around=0;
  int visible_by_human=0;
  int visible_by_HRP2=0;
  //printf("\n");
  for(;j<curr_surfaces_in_env.flat_surf[current_surface_index].grid_j_max;j++)
  {
   show_cell=0;
   reachable_by_hand=0;
   reachable_by_HRP2_hand=0;
   visible_by_human=0;
   visible_by_HRP2=0;
   reachable_by_turning_around=0;
   reachable_by_bending=0;

   double x=i*surf_grid_samp_rate;
   double y=j*surf_grid_samp_rate;
   x+=curr_surfaces_in_env.flat_surf[current_surface_index].BR_x_min;
   y+=curr_surfaces_in_env.flat_surf[current_surface_index].BR_y_min;
   double z=curr_surfaces_in_env.flat_surf[current_surface_index].BR_z;

   double cur_z_st=z;
   double height=0.1;//0.3;
   double cur_z_end=z;
   //double cur_z_end=;
   //printf("%d ", curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].visible);
   ////printf("SHOW_VISIBLE_PLACE=%d\n",SHOW_VISIBLE_PLACE);
    if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].visible==1)
    {
 
   //g3d_draw_line_with_width(x,y,z,x,y,z+0.2, 6, 3, NULL);
   //cur_z_st+=cur_z_end;
   //cur_z_end+=cur_z_st;
    show_cell=1;
    visible_by_human=1;

    }
    if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].visible_by_HRP2==1)
    {
 
   //g3d_draw_line_with_width(x,y,z,x,y,z+0.2, 6, 3, NULL);
   //cur_z_st+=cur_z_end;
   //cur_z_end+=cur_z_st;
    show_cell=1;
    visible_by_HRP2=1;
    }

    if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
     show_cell=1;
     reachable_by_hand=1;
    }
    if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
     show_cell=1;
     if(reachable_by_hand==1)//Already reachable by right hand
     reachable_by_hand=3; //Reachable by both hands
     else
     reachable_by_hand=2; //Reachable by left hand only
  
    }
     if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand_by_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
     show_cell=1;
      reachable_by_bending=1;
    //// g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
    
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand_by_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
     show_cell=1;
     if(reachable_by_bending==1)//Already reachable by right hand
     reachable_by_bending=3; //Reachable by both hands
     else
     reachable_by_bending=2; //Reachable by left hand only
    }
     if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_RHand_by_turning_around_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
    show_cell=1;
  reachable_by_turning_around=1;
 ////   g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
    
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_LHand_by_turning_around_bending==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
    show_cell=1;

      if(reachable_by_turning_around==1)//Already reachable by right hand
     reachable_by_turning_around=3; //Reachable by both hands
     else
     reachable_by_turning_around=2; //Reachable by left hand only
    }

   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_HRP2_RHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.3, 4, 4, NULL);
     show_cell=1;
     reachable_by_HRP2_hand=1;
    //// g3d_drawDisc(x, y, z+0.02, surf_grid_samp_rate/2.0-0.0075, 4, NULL);
   
    }
   if(curr_surfaces_in_env.flat_surf[current_surface_index].surf_grid[i][j].reachable_by_HRP2_LHand==1)
    {
    //g3d_draw_line_with_width(x,y,z,x,y,z+0.4, 2, 6, NULL);
     show_cell=1;
     if(reachable_by_HRP2_hand==1)//Already reachable by right hand
     reachable_by_HRP2_hand=3; //Reachable by both hands
     else
     reachable_by_HRP2_hand=2; //Reachable by left hand only
    }
    
  
   
    ////if(visible_by_HRP2==1&&visible_by_human==1)
    if(visible_by_human==1) 
     {
    if(reachable_by_HRP2_hand==3&&(reachable_by_hand==3||reachable_by_bending==3||reachable_by_turning_around==3)) //Reachable by both hands of both human and HRP2
      {
     ////g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 4, NULL);
     
     candidate_points_to_put.point[candidate_points_to_put.no_points].x=x;
     candidate_points_to_put.point[candidate_points_to_put.no_points].y=y;
     candidate_points_to_put.point[candidate_points_to_put.no_points].z=z;
     candidate_points_to_put.weight[candidate_points_to_put.no_points]=0;
     candidate_points_to_put.no_points++;
     ////candidate_points_to_put[no_candidate_points_to_put].x=x;
     ////candidate_points_to_put[no_candidate_points_to_put].y=y;
     ////candidate_points_to_put[no_candidate_points_to_put].z=z;
     ////no_candidate_points_to_put++;
    
     
      }
    if((reachable_by_HRP2_hand==1||reachable_by_HRP2_hand==2)&&(reachable_by_hand==1||reachable_by_bending==1||reachable_by_turning_around==1||reachable_by_hand==2||reachable_by_bending==2||reachable_by_turning_around==2))
      {
     ////g3d_drawDisc(x, y, cur_z_end, surf_grid_samp_rate/2.0, 2, NULL);
     candidate_points_to_put.point[candidate_points_to_put.no_points].x=x;
     candidate_points_to_put.point[candidate_points_to_put.no_points].y=y;
     candidate_points_to_put.point[candidate_points_to_put.no_points].z=z;
     candidate_points_to_put.weight[candidate_points_to_put.no_points]=0;
     candidate_points_to_put.no_points++;

     ////candidate_points_to_put.point[no_candidate_points_to_put].x=x;
     ////candidate_points_to_put.point[no_candidate_points_to_put].y=y;
     ////candidate_points_to_put.point[no_candidate_points_to_put].z=z;
     ////no_candidate_points_to_put++;
      }
     } 
    }
   }
  }
 return 1;
}

int get_index_of_robot_by_name(char *rob_name)
{
  p3d_env *envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
  int nr,i;
 
  nr = envPt->nr;
	
  for(i=0;i<nr;i++)
	{
		
		if (strcmp(envPt->robot[i]->name,rob_name)==0)
		{
			return i;
		}
	}
    return NULL;
}
  
int make_cells_around_point_obstacle_free(double hand_pos[3], int expansion)
{ 
 //////////printf(" Inside  make_cells_around_point_obstacle_free=(%lf,%lf,%lf)\n",hand_pos[0],hand_pos[1],hand_pos[2]);
hri_bitmap_cell* current_cell=NULL;
 current_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP], hand_pos[0], hand_pos[1], hand_pos[2]);

       if(current_cell!=NULL)
        { 
      //////////  printf(" Inside  make_cells_around_point_obstacle_free, cell=(%d,%d,%d)\n",current_cell->x,current_cell->y,current_cell->z);
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[current_cell->x][current_cell->y][current_cell->z].val=0;
        

         
           int i=0; 
           for(i=current_cell->x-expansion;i<=current_cell->x+expansion;i++)
           {
            //printf(" for i = %d\n",i);
            if(i>=0&&i<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
            {
             int j=0;
             for(j=current_cell->y-expansion;j<=current_cell->y+expansion;j++)
             {
              //printf(" for i, j = %d %d\n",i, j);
              if(j>=0&&j<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
              {
               int k=0;
                for(k=current_cell->z-expansion;k<=current_cell->z+expansion;k++)
               {
              //printf(" for i, j, k = %d %d %\n",i, j, k);
              if(k>=0&&k<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
                {
                 ////printf(" Populating cell (%d, %d, %d) with 0\n",i,j,k);
                 grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[i][j][k].val=0;
                 
        
                }
               } 
              }
             }
            } 
           }
     } 
    else
     {
     //////////printf(" Can't get cell corresponding to the hand\n");
     } 
return 1;
}

int make_cells_around_point_as_near_to_obstacle(double hand_pos[3], int expansion)
{ 
 //////////printf(" Inside  make_cells_around_point_obstacle=(%lf,%lf,%lf)\n",hand_pos[0],hand_pos[1],hand_pos[2]);
hri_bitmap_cell* current_cell=NULL;
 current_cell  =  hri_bt_get_closest_cell(grid_around_HRP2.GRID_SET, grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP], hand_pos[0], hand_pos[1], hand_pos[2]);

       if(current_cell!=NULL)
        { 
      //////////  printf(" Inside  make_cells_around_point_obstacle_free, cell=(%d,%d,%d)\n",current_cell->x,current_cell->y,current_cell->z);
        grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[current_cell->x][current_cell->y][current_cell->z].val=0;
        
           int i=0; 
           for(i=current_cell->x-expansion;i<=current_cell->x+expansion;i++)
           {
            //printf(" for i = %d\n",i);
            if(i>=0&&i<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
            {
             int j=0;
             for(j=current_cell->y-expansion;j<=current_cell->y+expansion;j++)
             {
              //printf(" for i, j = %d %d\n",i, j);
              if(j>=0&&j<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
              {
               int k=0;
                for(k=current_cell->z-expansion;k<=current_cell->z+expansion;k++)
               {
              //printf(" for i, j, k = %d %d %\n",i, j, k);
              if(k>=0&&k<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
                {
                 ////printf(" Populating cell (%d, %d, %d) with 0\n",i,j,k);
                 grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[i][j][k].val=-2;
                 
        
                }
               } 
              }
             }
            } 
           }
     } 
    else
     {
  //////////   printf(" Can't get cell corresponding to the hand\n");
     } 
return 1;
}

/*int store_cur_GIK_sol_for_real_HRP2()
{
int i=0;
for(i=0;i<HRP2_GIK_sol.no_configs;i++)
 {
  
 }
 
}*/





int make_cells_around_point_as_obstacle(hri_bitmapset *btset, int bt_type, point_co_ordi point, int extension)
{
hri_bitmap* bitmap;

  bitmap = btset->bitmap[bt_type];

int x,y,z;

x=(point.x-btset->realx)/btset->pace;  
y=(point.y-btset->realy)/btset->pace;
z=(point.z-btset->realz)/btset->pace;
bitmap->data[x][y][z].val =-3;  //-3 to indicate that it has been marked as obstacle due to collision during path planning using GIK, it is not actual obstacle

 if((x>0&&x<bitmap->nx)&&(y>0&&y<bitmap->ny)&&(z>0&&z<bitmap->nz))
 {
  int x1=x-extension;
  
  for(x1=x-extension; x1<x+extension; x1++)
  {
   int y1=y-extension; 
   for(y1=y-extension;y1<y+extension;y1++) 
   {
   int z1=z-extension; 
   for(z1=z-extension;z1<z+extension;z1++) 
   
        ////printf(" Making cell (%d,%d,%d) as value 0 \n",x,y,z); 
   bitmap->data[x1][y1][z1].val =-3;  //-3 to indicate that it has been marked as obstacle due to collision during path planning using GIK, it is not actual obstacle
   } 
  }
 }
return 1;
}

int find_symbolic_MM_visibility()
{
 hri_bitmap* bitmap=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP];
 envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
 int no = envPt->no;
 int nr = envPt->nr;

 int nr_ctr=0;
    for(;nr_ctr<nr;nr_ctr++)
     {
     object_MM.object[nr_ctr].geo_MM.visible_by_human=0;
     object_MM.object[nr_ctr].geo_MM.visible_by_human_straight_head_orientation=0;
     object_MM.object[nr_ctr].geo_MM.visible_by_human_neck_turn=0;
     object_MM.object[nr_ctr].geo_MM.visible_by_standing_human=0;
     object_MM.object[nr_ctr].geo_MM.visible_by_standing_human_neck_turn=0;
     object_MM.object[nr_ctr].geo_MM.visible_by_human_straight_head_orientation_standing=0;
    
     object_MM.object[nr_ctr].geo_MM.visible_by_HRP2=0;
     object_MM.object[nr_ctr].geo_MM.visible_by_HRP2_straight_head_orientation=0;
     object_MM.object[nr_ctr].geo_MM.visible_by_HRP2_neck_turn=0;
    
     object_MM.object[nr_ctr].geo_MM.visible_by_JIDO=0;
     object_MM.object[nr_ctr].geo_MM.visible_by_JIDO_straight_head_orientation=0;
     object_MM.object[nr_ctr].geo_MM.visible_by_JIDO_neck_turn=0;
    
     }  

 int x,y,z;
  for(x=0; x<bitmap->nx; x++)
  {
     for(y=0; y<bitmap->ny; y++)
   {
       for(z=0; z<bitmap->nz; z++)
    {
  
  int nr_ctr=0;
    for(;nr_ctr<nr;nr_ctr++)
     {
    
     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[nr_ctr]==1)
      {

/////// FOR HUMAN

     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human==1)
       {
       object_MM.object[nr_ctr].geo_MM.visible_by_human=1;
       }
     
     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human_straight_head_orientation==1)
       {
       object_MM.object[nr_ctr].geo_MM.visible_by_human_straight_head_orientation=1;
       }
     
      if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human_neck_turn==1)
       {
       object_MM.object[nr_ctr].geo_MM.visible_by_human_neck_turn=1;
       }

      if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_standing_human==1)
       {
       object_MM.object[nr_ctr].geo_MM.visible_by_standing_human=1;
       }

      if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_standing_human_neck_turn==1)
       {
       object_MM.object[nr_ctr].geo_MM.visible_by_standing_human_neck_turn=1;
       }

      if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human_straight_head_orientation_standing==1)
       {
       object_MM.object[nr_ctr].geo_MM.visible_by_human_straight_head_orientation_standing=1;
       }



/////// FOR HRP2
     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2==1)
       {
       object_MM.object[nr_ctr].geo_MM.visible_by_HRP2=1;
       }

     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2_straight_head_orientation==1)
       {
       object_MM.object[nr_ctr].geo_MM.visible_by_HRP2_straight_head_orientation=1;
       }

     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_HRP2_neck_turn==1)
       {
       object_MM.object[nr_ctr].geo_MM.visible_by_HRP2_neck_turn=1;
       }

      
     
////////// FOR JIDO
     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO==1)
       {
       object_MM.object[nr_ctr].geo_MM.visible_by_JIDO=1;
       }  
  
     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO_straight_head_orientation==1)
       {
       object_MM.object[nr_ctr].geo_MM.visible_by_JIDO_straight_head_orientation=1;
       }

     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_JIDO_neck_turn==1)
       {
       object_MM.object[nr_ctr].geo_MM.visible_by_JIDO_neck_turn=1;
       }

   

      }
     }
    }
   }
  }  
return 1;     
}

int show_first_non_visible_cells(int obj_index)
{
hri_bitmap* bitmap=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP];
 int x,y,z;
  for(x=0; x<bitmap->nx; x++)
  {
     for(y=0; y<bitmap->ny; y++)
   {
       for(z=0; z<bitmap->nz; z++)
    {
  
  //int nr_ctr=0;
    //for(;nr_ctr<nr;nr_ctr++)
     //{
    
     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[obj_index]==1)
      {

/////// FOR HUMAN

     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human==1)
       {
       double x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
 
    
       g3d_drawDisc(x_world, y_world, z_world, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);
     
       }
     
     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human_straight_head_orientation==1)
       {
      double x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
 
    
       g3d_drawDisc(x_world, y_world, z_world, grid_around_HRP2.GRID_SET->pace/2.0, Yellow, NULL);
       }
  
   
      if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human_neck_turn==1)
       {
      double x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
 
    
       g3d_drawDisc(x_world, y_world, z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
       }

     }
    }
   }
  }

}

int get_object_mightabilities()
{
////printf(" Inside get_object_mightabilities()\n");
 
 Mightabilities_for_obj.total_no_obj=0;
  envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
 int no = envPt->no;
 int nr = envPt->nr;

 int nr_ctr=0;
    for(;nr_ctr<nr;nr_ctr++)
     {
     strcpy( Mightabilities_for_obj.object[Mightabilities_for_obj.total_no_obj].object_name, envPt->robot[nr_ctr]->name);
     Mightabilities_for_obj.object[Mightabilities_for_obj.total_no_obj].object_index=nr_ctr;

     if(object_MM.object[nr_ctr].geo_MM.reachable_by_human_LHand==1||object_MM.object[nr_ctr].geo_MM.reachable_by_human_RHand==1||object_MM.object[nr_ctr].geo_MM.reachable_by_LHand_by_bending==1||object_MM.object[nr_ctr].geo_MM.reachable_by_RHand_by_bending==1)
      {
              
       Mightabilities_for_obj.object[Mightabilities_for_obj.total_no_obj].obj_mightability.is_reachable_by_human=1;
       
      }
      else
      {
       Mightabilities_for_obj.object[Mightabilities_for_obj.total_no_obj].obj_mightability.is_reachable_by_human=0;
       
      }
     
      #ifdef HRI_JIDO
      if(object_MM.object[nr_ctr].geo_MM.reachable_by_JIDO_Hand==1)
      {

       Mightabilities_for_obj.object[Mightabilities_for_obj.total_no_obj].obj_mightability.is_reachable_by_robot=1;

      }
      else
      {
       Mightabilities_for_obj.object[Mightabilities_for_obj.total_no_obj].obj_mightability.is_reachable_by_robot=0;
       
      }
      #endif

      if(object_MM.object[nr_ctr].geo_MM.visible_by_human==1||object_MM.object[nr_ctr].geo_MM.visible_by_human_neck_turn==1||object_MM.object[nr_ctr].geo_MM.visible_by_human_straight_head_orientation==1)
      {
       Mightabilities_for_obj.object[Mightabilities_for_obj.total_no_obj].obj_mightability.is_visible_by_human=1;
      }
      else
      {
       Mightabilities_for_obj.object[Mightabilities_for_obj.total_no_obj].obj_mightability.is_visible_by_human=0;
      }
     
      #ifdef HRI_JIDO
      if(object_MM.object[nr_ctr].geo_MM.visible_by_JIDO==1||object_MM.object[nr_ctr].geo_MM.visible_by_JIDO_neck_turn==1||object_MM.object[nr_ctr].geo_MM.visible_by_JIDO_straight_head_orientation==1)
      {
       Mightabilities_for_obj.object[Mightabilities_for_obj.total_no_obj].obj_mightability.is_visible_by_robot=1;
      }
      else
      {       
       Mightabilities_for_obj.object[Mightabilities_for_obj.total_no_obj].obj_mightability.is_visible_by_robot=0;
      }
      #endif


     Mightabilities_for_obj.total_no_obj++;
     }
return 1;
}

int show_object_Mightabilities()
{
 ////printf(" Inside show_object_Mightabilities()\n");
 int i=0;
 double x,y,z;
 double radius=0.015;
 
 for(i=0;i<Mightabilities_for_obj.total_no_obj;i++)
 {
  int obj_index=Mightabilities_for_obj.object[i].object_index;
   
   if(strcasestr(envPt->robot[obj_index]->name,"visball"))
   {
   continue;
   }
  
  
  x=(envPt->robot[obj_index]->BB.xmin+envPt->robot[obj_index]->BB.xmax)/2.0;
  y=(envPt->robot[obj_index]->BB.ymin+envPt->robot[obj_index]->BB.ymax)/2.0;
  z=envPt->robot[obj_index]->BB.zmax;

  if(SHOW_MM_BASED_OBJECT_REACHABLE==1)
  {
  if( Mightabilities_for_obj.object[obj_index].obj_mightability.is_reachable_by_human==1)
    {
   ////g3d_drawDisc(envPt->robot[obj_index]->BB.xmin, envPt->robot[obj_index]->BB.ymin, envPt->robot[obj_index]->BB.zmax, 0.01, Red, NULL);
   g3d_drawDisc(x-radius, y-radius, z, radius, Red, NULL);
   
   ////Draw_Bounding_Box(envPt->robot[obj_index]->BB.xmin,envPt->robot[obj_index]->BB.ymin,envPt->robot[obj_index]->BB.zmin,envPt->robot[obj_index]->BB.xmax,envPt->robot[obj_index]->BB.ymax,envPt->robot[obj_index]->BB.zmax, Green);
   
    }  
  
  if( Mightabilities_for_obj.object[obj_index].obj_mightability.is_reachable_by_robot==1)
    {
   ////g3d_drawDisc(envPt->robot[obj_index]->BB.xmax, envPt->robot[obj_index]->BB.ymin, envPt->robot[obj_index]->BB.zmax, 0.01, Green, NULL);
   g3d_drawDisc(x-radius, y+radius, z, radius, Green, NULL);
   ////Draw_Bounding_Box(envPt->robot[obj_index]->BB.xmin,envPt->robot[obj_index]->BB.ymin,envPt->robot[obj_index]->BB.zmin,envPt->robot[obj_index]->BB.xmax,envPt->robot[obj_index]->BB.ymax,envPt->robot[obj_index]->BB.zmax, Green);
   
    }
   } 

  if(SHOW_MM_BASED_OBJECT_VISIBLE==1)
   {
  if( Mightabilities_for_obj.object[obj_index].obj_mightability.is_visible_by_human==1)
    {
   ////g3d_drawDisc(envPt->robot[obj_index]->BB.xmin, envPt->robot[obj_index]->BB.ymax, envPt->robot[obj_index]->BB.zmax, 0.01, Blue, NULL);
   g3d_drawDisc(x+radius, y-radius, z, radius, Blue, NULL);
   ////Draw_Bounding_Box(envPt->robot[obj_index]->BB.xmin,envPt->robot[obj_index]->BB.ymin,envPt->robot[obj_index]->BB.zmin,envPt->robot[obj_index]->BB.xmax,envPt->robot[obj_index]->BB.ymax,envPt->robot[obj_index]->BB.zmax, Green);
   
    } 

  if( Mightabilities_for_obj.object[obj_index].obj_mightability.is_visible_by_robot==1)
    {
   ////g3d_drawDisc(envPt->robot[obj_index]->BB.xmax, envPt->robot[obj_index]->BB.ymax, envPt->robot[obj_index]->BB.zmax, 0.01, Yellow, NULL);
   g3d_drawDisc(x+radius, y+radius, z, radius, Yellow, NULL);
   ////Draw_Bounding_Box(envPt->robot[obj_index]->BB.xmin,envPt->robot[obj_index]->BB.ymin,envPt->robot[obj_index]->BB.zmin,envPt->robot[obj_index]->BB.xmax,envPt->robot[obj_index]->BB.ymax,envPt->robot[obj_index]->BB.zmax, Green);
   
    } 
   }
 }

 return 1;

}


int find_symbolic_Mightability_Map_new()
{

 find_symbolic_MM_visibility();

 hri_bitmap* bitmap=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP];
 envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
 int no = envPt->no;
 int nr = envPt->nr;

 int nr_ctr=0;
    for(;nr_ctr<nr;nr_ctr++)
     {
      object_MM.object[nr_ctr].geo_MM.reachable_by_human_LHand=0;
      object_MM.object[nr_ctr].geo_MM.reachable_by_human_RHand=0;
      object_MM.object[nr_ctr].geo_MM.reachable_by_LHand_by_bending=0;
      object_MM.object[nr_ctr].geo_MM.reachable_by_RHand_by_bending=0;
      object_MM.object[nr_ctr].geo_MM.reachable_by_LHand_by_turning_around_bending=0;
      object_MM.object[nr_ctr].geo_MM.reachable_by_RHand_by_turning_around_bending=0;

      object_MM.object[nr_ctr].geo_MM.reachable_by_HRP2_LHand=0;
      object_MM.object[nr_ctr].geo_MM.reachable_by_HRP2_RHand=0;

      object_MM.object[nr_ctr].geo_MM.reachable_by_JIDO_Hand=0;

      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.is_true=0;
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.is_true=0;
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_HRP2.is_true=0;
 
      
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_HRP2.no_points=0;
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.no_points=0;
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.no_points=0;
      ////object_MM.object[nr_ctr].visible_by_human=0;
     }  

 int x,y,z;
 double xc=0, yc=0, zc=0;
  
  
  nr_ctr=0;
    for(;nr_ctr<nr;nr_ctr++)
     {
     
     xc=(envPt->robot[nr_ctr]->BB.xmax+envPt->robot[nr_ctr]->BB.xmin)/2.0;
     yc=(envPt->robot[nr_ctr]->BB.ymax+envPt->robot[nr_ctr]->BB.ymin)/2.0;
     zc=(envPt->robot[nr_ctr]->BB.zmax+envPt->robot[nr_ctr]->BB.zmin)/2.0;
    
     
     x=(xc- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;
     
  
      if(x>=0&&x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
      {
      
      y=(yc- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace;
      
      if(y>=0&&y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
      {
      
      z=(zc- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace;
 
       if(z>=0&&z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
       {
       ////curr_cell=

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_human_LHand=1;
       }
       
       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_human_RHand=1;
       }

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_LHand_by_bending=1;
       }  

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_RHand_by_bending=1;
       }  

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_turning_around_bending==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_LHand_by_turning_around_bending=1;
       } 

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_turning_around_bending==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_RHand_by_turning_around_bending=1;
       }

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_LHand==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_HRP2_LHand=1;
       }

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_HRP2_RHand==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_HRP2_RHand=1;
       }


      if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_JIDO_Hand==1)
       {
       ////printf(" %s is reachable by JIDO \n",envPt->robot[nr_ctr]->name);
       object_MM.object[nr_ctr].geo_MM.reachable_by_JIDO_Hand=1;
       }
      ////Object based visibility
    /*if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human==1&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[nr_ctr]==1)
       {
       object_MM.object[nr_ctr].visible_by_human=1;
       }
      */     
      }
     }
    }
   

     //// For calculating that something can be put inside the object by the agent
    if(strcasestr(envPt->robot[nr_ctr]->name,"ROBOT")||strcasestr(envPt->robot[nr_ctr]->name,"HUMAN")||strcasestr(envPt->robot[nr_ctr]->name,"TABLE"))
    {
    //No need to calculate PUTABILITY :-)
    }
    else
    {
    
   
     double increment=3.0/4.0*grid_around_HRP2.GRID_SET->pace;
     double top_z=envPt->robot[nr_ctr]->BB.zmax;
     int cell_top_z=(top_z- grid_around_HRP2.GRID_SET->realz)/grid_around_HRP2.GRID_SET->pace;
     
     double BB_x_ctr;
     double BB_y_ctr;

    if(cell_top_z>=0&&cell_top_z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz)
    { 
     for(BB_x_ctr=envPt->robot[nr_ctr]->BB.xmin;BB_x_ctr<envPt->robot[nr_ctr]->BB.xmax;BB_x_ctr+=increment)
     {
      int cell_x=(BB_x_ctr- grid_around_HRP2.GRID_SET->realx)/grid_around_HRP2.GRID_SET->pace;  

      if(cell_x>=0&&cell_x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx)
      {
      for(BB_y_ctr=envPt->robot[nr_ctr]->BB.ymin;BB_y_ctr<envPt->robot[nr_ctr]->BB.ymax;BB_y_ctr+=increment)
       {
      
       int cell_y=(BB_y_ctr- grid_around_HRP2.GRID_SET->realy)/grid_around_HRP2.GRID_SET->pace;  
 
       if(cell_y>=0&&cell_y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny)
        {

      if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_top_z].val>=0&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_top_z-1].val>=0)//No obstacle
         {

        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_top_z].Mightability_Map.reachable_by_JIDO_Hand==1)
          {  
        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.is_true=1;
        
        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.no_points].x=BB_x_ctr; 
        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.no_points].y=BB_y_ctr;
        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.no_points].z=top_z;
 
        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.no_points++;
          }
       
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_top_z].Mightability_Map.reachable_by_human_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_top_z].Mightability_Map.reachable_by_human_RHand==1)
          {  
        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.is_true=1;
         object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.point[
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.no_points].x=BB_x_ctr; 
        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.point[
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.no_points].y=BB_y_ctr;
        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.point[
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.no_points].z=top_z;
 
        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.no_points++;
          }  
  
        if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_top_z].Mightability_Map.reachable_by_HRP2_LHand==1||grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_top_z].Mightability_Map.reachable_by_HRP2_RHand==1)
          {  
        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_HRP2.is_true=1;

         object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_HRP2.point[
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_HRP2.no_points].x=BB_x_ctr; 

        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_HRP2.point[
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_HRP2.no_points].y=BB_y_ctr;

        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_HRP2.point[
      object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_HRP2.no_points].z=top_z;
 
        object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_HRP2.no_points++;
          }

         }
        }
       } 
      }
     }
    }
   }
      /*
         //, so assume that the object corresponding to this obstacle is visible
          
          int i=0;
          for(i=0;i<nr;i++)
          {
           
       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[cell_x][cell_y][cell_z].Mightability_map_cell_obj_info.belongs_to_objects_indx[i]==1)
   
   /*int is_visible=psp_is_object_visible(ACBTSET->human[ACBTSET->actual_human]->HumanPt, envPt->robot[nr_ctr], 50, FALSE); 
   if(is_visible==1)
    {
     object_MM.object[nr_ctr].visible_by_human=1;
    }*/
  }   
return 1;
}

 
int find_symbolic_Mightability_Map()
{
 hri_bitmap* bitmap=grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP];
 envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
 int no = envPt->no;
 int nr = envPt->nr;

 int nr_ctr=0;
    for(;nr_ctr<nr;nr_ctr++)
     {
      object_MM.object[nr_ctr].geo_MM.reachable_by_human_LHand=0;
      object_MM.object[nr_ctr].geo_MM.reachable_by_human_RHand=0;
      object_MM.object[nr_ctr].geo_MM.reachable_by_LHand_by_bending=0;
      object_MM.object[nr_ctr].geo_MM.reachable_by_RHand_by_bending=0;
      object_MM.object[nr_ctr].geo_MM.reachable_by_LHand_by_turning_around_bending=0;
      object_MM.object[nr_ctr].geo_MM.reachable_by_RHand_by_turning_around_bending=0;
     }  

 int x,y,z;
  for(x=0; x<bitmap->nx; x++)
  {
     for(y=0; y<bitmap->ny; y++)
   {
       for(z=0; z<bitmap->nz; z++)
    {
  
  int nr_ctr=0;
    for(;nr_ctr<nr;nr_ctr++)
     {
     if(bitmap->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[nr_ctr]==1)
      {
       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_LHand==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_human_LHand=1;
       }
       
       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_human_RHand==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_human_RHand=1;
       }

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_bending==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_LHand_by_bending=1;
       }  

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_bending==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_RHand_by_bending=1;
       }  

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_LHand_by_turning_around_bending==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_LHand_by_turning_around_bending=1;
       } 

       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_Map.reachable_by_RHand_by_turning_around_bending==1)
       {
       object_MM.object[nr_ctr].geo_MM.reachable_by_RHand_by_turning_around_bending=1;
       }
 
      
      }  
     //bitmap->data[x][y][z].Mightability_map_cell_obj_info.close_to_objects_indx[nr_ctr]=0;//Initially does not belong to any cell
     //bitmap->data[x][y][z].Mightability_map_cell_obj_info.is_horizontal_surface=0;
     //bitmap->data[x][y][z].Mightability_map_cell_obj_info.near_horizontal_surface=0;
     }
     //bitmap->data[x][y][z].Mightability_map_cell_obj_info.no_close_to_objects=0;
     //bitmap->data[x][y][z].Mightability_map_cell_obj_info.no_belongs_to_objects=0;  
    }
   }
  }   
return 1;
}

int Draw_Bounding_Box(double min_x, double min_y, double min_z, double max_x, double max_y, double max_z, int color)
{
 g3d_drawOneLine(
        min_x, min_y, min_z,
        max_x, min_y, min_z, color, NULL);

g3d_drawOneLine(
        max_x, min_y, min_z,
        max_x, max_y, min_z, color, NULL);

g3d_drawOneLine(
        max_x, max_y, min_z,
        min_x, max_y, min_z, color, NULL);

g3d_drawOneLine(
        min_x, max_y, min_z,
        min_x, min_y, min_z, color, NULL);

g3d_drawOneLine(
        min_x, min_y, max_z,
        max_x, min_y, max_z, color, NULL);

g3d_drawOneLine(
        max_x, min_y, max_z,
        max_x, max_y, max_z, color, NULL);

g3d_drawOneLine(
        max_x, max_y, max_z,
        min_x, max_y, max_z, color, NULL);

g3d_drawOneLine(
        min_x, max_y, max_z,
        min_x, min_y, max_z, color, NULL);

g3d_drawOneLine(
        max_x, min_y, min_z,
        max_x, min_y, max_z, color, NULL);

g3d_drawOneLine(
        max_x, max_y, min_z,
        max_x, max_y, max_z, color, NULL);

g3d_drawOneLine(
        min_x, min_y, min_z,
        min_x, min_y, max_z, color, NULL);

g3d_drawOneLine(
        min_x, max_y, min_z,
        min_x, max_y, max_z, color, NULL);

return 1;
}

int show_symbolic_Mightability_Map_Relations()
{
 
 envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
 int no = envPt->no;
 int nr = envPt->nr;
 int x,y,z;
  
 int show_visibility=0;
 int show_reachability=0;
 int show_reachability_to_put_inside=0;
  
  int nr_ctr=0;
    for(;nr_ctr<nr;nr_ctr++)
     {
    
      if(show_reachability==1)
      {
       int for_jido=1;
       int for_human=0;

       if(for_human==1)
       {
       if(object_MM.object[nr_ctr].geo_MM.reachable_by_human_LHand==1||object_MM.object[nr_ctr].geo_MM.reachable_by_human_RHand==1)
        {
        Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Green);
        ////envPt->robot[i]->BB.xmin
        }
       
      
       if(object_MM.object[nr_ctr].geo_MM.reachable_by_LHand_by_bending==1||object_MM.object[nr_ctr].geo_MM.reachable_by_RHand_by_bending==1)
        {
        Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Blue);
        }  
/*
       if(object_MM.object[nr_ctr].geo_MM.reachable_by_LHand_by_turning_around_bending==1||object_MM.object[nr_ctr].geo_MM.reachable_by_RHand_by_turning_around_bending==1)
       {
       Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Red);
       } 
*/
       }
  
       if(for_jido==1)
       {  
       if(object_MM.object[nr_ctr].geo_MM.reachable_by_JIDO_Hand==1)
        {
        Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Yellow);
        }  
       }
      }

      if(show_visibility==1)
      { 
 
       if(object_MM.object[nr_ctr].geo_MM.visible_by_human==1)
       {
        Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Green);
       
       }
/*       
       if(object_MM.object[nr_ctr].geo_MM.visible_by_human_neck_turn==1)
       {
        Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Red);
       
       }

       if(object_MM.object[nr_ctr].geo_MM.visible_by_human_straight_head_orientation==1)
       {
        Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Blue);
       
       }
*/
/*
       if(object_MM.object[nr_ctr].visible_by_HRP2==1)
       {
        Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Yellow);
       }
       
       if(object_MM.object[nr_ctr].visible_by_HRP2_straight_head_orientation==1)
       {
        Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Yellow);
       }

         
       if(object_MM.object[nr_ctr].visible_by_HRP2_neck_turn==1)
       {
        Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Yellow);
       }

*/
       if(object_MM.object[nr_ctr].geo_MM.visible_by_JIDO==1)
       {
        Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Yellow);
       }
/*       
       if(object_MM.object[nr_ctr].visible_by_JIDO_straight_head_orientation==1)
       {
        Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Yellow);
       }

         
       if(object_MM.object[nr_ctr].visible_by_JIDO_neck_turn==1)
       {
        Draw_Bounding_Box(envPt->robot[nr_ctr]->BB.xmin,envPt->robot[nr_ctr]->BB.ymin,envPt->robot[nr_ctr]->BB.zmin,envPt->robot[nr_ctr]->BB.xmax,envPt->robot[nr_ctr]->BB.ymax,envPt->robot[nr_ctr]->BB.zmax, Yellow);
       }
*/

      } 

     if(show_reachability_to_put_inside==1)
      {
       int i=0;
       
       for(i=0;i<object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.no_points;i++)
       {
          g3d_drawDisc(object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[i].x, object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[i].y, object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[i].z, grid_around_HRP2.GRID_SET->pace/2.0, Red, NULL);

       }
       
       for(i=0;i<object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.no_points;i++)
       {
          g3d_drawDisc(object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.point[i].x, object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.point[i].y, object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.point[i].z, grid_around_HRP2.GRID_SET->pace/2.0, Green, NULL);

      
       } 
      }
     /* if(object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_human.no_points>0)
      {
          g3d_drawDisc(object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[0].x, object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[0].y, object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[0].z, grid_around_HRP2.GRID_SET->pace/2.0, Red, NULL);

       } 
     */
     }

         
  return 1;
}

int show_obstacle_cells_belonging_to(int object_index)
{
  //////////printf(" Showing obstacle cells belonging to index %d\n",object_index);
  int x,y,z;
  for(x=0; x<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nx; x++)
  {
     for(y=0; y<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->ny; y++)
   {
       for(z=0; z<grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->nz; z++)
    {
  
  
  if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[object_index]==1)
      {
       
       
      ////////// printf("cell %d,%d, %d,\n",x,y,z);
 
       
       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human==1)
       {
        //////////printf("This is first non visible cell for human also \n");          

        double x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
 
    
       g3d_drawDisc(x_world, y_world, z_world, grid_around_HRP2.GRID_SET->pace/2.0, Blue, NULL);
       }
/*
       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_map_cell_obj_info.first_non_visible_by_human_straight_head_orientation==1)
       {
       ////////// printf("This is first_non_visible_by_human_straight_head_orientation also \n");          

        double x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
 
    
       g3d_drawDisc(x_world, y_world, z_world, grid_around_HRP2.GRID_SET->pace/2.0, Red, NULL);
       }
*/

       double x_world = grid_around_HRP2.GRID_SET->realx + (x * grid_around_HRP2.GRID_SET->pace);
       double y_world = grid_around_HRP2.GRID_SET->realy + (y * grid_around_HRP2.GRID_SET->pace);
       double z_world = grid_around_HRP2.GRID_SET->realz + (z * grid_around_HRP2.GRID_SET->pace);
 
    
       g3d_drawDisc(x_world, y_world, z_world, grid_around_HRP2.GRID_SET->pace/4.0, Green, NULL);
    
       envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
 
       int nr = envPt->nr;
       int i=0; 
       for(i=0;i<nr;i++)
       {
        if(i!=object_index&&grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_map_cell_obj_info.belongs_to_objects_indx[i]==1)
        {
        ////////// printf("This cell also belongs to %s\n",envPt->robot[i]->name);
        }
        
       }
 
       if(grid_around_HRP2.GRID_SET->bitmap[HRP2_GIK_MANIP]->data[x][y][z].Mightability_map_cell_obj_info.is_horizontal_surface==1)//the cell just above the  horizontal surface of table
       {
        //////////printf(" This cell also belongs to horizontal surface\n");
       }
      }
 
    }
   }
  }
return 1;
}



//////////// For mhp requests
int MM_is_object_visible(HRI_AGENT *agent, p3d_rob *object)
{
 int i=0;
 for(i=0;i<Mightabilities_for_obj.total_no_obj;i++)
 {
 if(strcmp(object->name,Mightabilities_for_obj.object[i].object_name)==0)
  {
  if(agent->type==HRI_JIDO1)
   {
  if( Mightabilities_for_obj.object[i].obj_mightability.is_visible_by_robot==1)
    {
    return 1;
    }
   }	
  if(agent->type==HRI_ACHILE)
   {
  if( Mightabilities_for_obj.object[i].obj_mightability.is_visible_by_human==1)
    {
    return 1;
    }
   }	
  }
 }
}

int MM_is_object_reachable(HRI_AGENT *agent, p3d_rob *object)
{
 int i=0;
 for(i=0;i<Mightabilities_for_obj.total_no_obj;i++)
 {
 if(strcmp(object->name,Mightabilities_for_obj.object[i].object_name)==0)
  {
  if(agent->type==HRI_JIDO1)
   {
  if( Mightabilities_for_obj.object[i].obj_mightability.is_reachable_by_robot==1)
    {
    return 1;
    }
   }	
  if(agent->type==HRI_ACHILE)
   {
  if( Mightabilities_for_obj.object[i].obj_mightability.is_reachable_by_human==1)
    {
    return 1;
    }
   }	
  }
 }
}





/////////// For CHRIS demo
int plan_JIDO_arm_path_to_grasp(char* obj_to_grasp, MANIPULATION_TASK_TYPE_STR type);
configPt show_all_grasps ( p3d_rob *robot, p3d_rob *object, std::list<gpGrasp> &graspList, gpArm_type arm_type, configPt qbase, gpGrasp &grasp, gpHand_properties &hand );

//////// START Graspability of object test

static int init_Manipulation_params() 
{
  if (manipulation == NULL) {
       ////////// printf(" Initializing for robot %s \n",GP_ROBOT_NAME);
        
	p3d_rob * robotPt= p3d_get_robot_by_name(GP_ROBOT_NAME);
        //////////printf(" robotPt->nbCcCntrts = %d \n",robotPt->nbCcCntrts);
	manipulation= new Manipulation_JIDO(robotPt, GP_GRIPPER);
  }
  return 1;
}



int find_candidate_points_for_putinto(int by_agent, int container_index)
{
 if(by_agent==3)//For JIDO
 { 
 envPt = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
 
 point_co_ordi center_of_opening;
 center_of_opening.x=(envPt->robot[container_index]->BB.xmin+envPt->robot[container_index]->BB.xmax)/2.0;
 center_of_opening.y=(envPt->robot[container_index]->BB.ymin+envPt->robot[container_index]->BB.ymax)/2.0;
 center_of_opening.z=(envPt->robot[container_index]->BB.zmin+envPt->robot[container_index]->BB.zmax)/2.0;

 double Amplitude=1.0;
 double sig_dist=4.0;

 candidate_points_to_putinto_by_jido.no_points=0;
 int i=0;
 for ( i=0;i<object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.no_points;i++ )
  {
  candidate_points_to_putinto_by_jido.point[candidate_points_to_putinto_by_jido.no_points].x=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i].x;
  candidate_points_to_putinto_by_jido.point[candidate_points_to_putinto_by_jido.no_points].y=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i].y;
  candidate_points_to_putinto_by_jido.point[candidate_points_to_putinto_by_jido.no_points].z=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i].z;

  //Assigning initial weights based on the distance from the centre of the opening
  double point_to_cnter_dist=sqrt((candidate_points_to_putinto_by_jido.point[candidate_points_to_putinto_by_jido.no_points].x-center_of_opening.x)*(candidate_points_to_putinto_by_jido.point[candidate_points_to_putinto_by_jido.no_points].x-center_of_opening.x)+(candidate_points_to_putinto_by_jido.point[candidate_points_to_putinto_by_jido.no_points].y-center_of_opening.y)*(candidate_points_to_putinto_by_jido.point[candidate_points_to_putinto_by_jido.no_points].y-center_of_opening.y)+(candidate_points_to_putinto_by_jido.point[candidate_points_to_putinto_by_jido.no_points].z-center_of_opening.z)*(candidate_points_to_putinto_by_jido.point[candidate_points_to_putinto_by_jido.no_points].z-center_of_opening.z));
  
  candidate_points_to_putinto_by_jido.weight[candidate_points_to_putinto_by_jido.no_points]=Amplitude*exp(-(((point_to_cnter_dist)*(point_to_cnter_dist)/2.0*sig_dist*sig_dist)));

  
  candidate_points_to_putinto_by_jido.no_points++;
  }  
 }
}

static int traj_play = TRUE;

static int 
default_drawtraj_fct(p3d_rob* robot, p3d_localpath* curLp)
{
  g3d_draw_allwin_active();
  fl_check_forms();
  return(traj_play);
}

int lp[10000];
       Gb_q6 positions[10000];
       int nbPositions = 0;

int traj_ctr=0;
        p3d_traj* current_new_trajPt[20];

int test_jido_grasp_traj()
{
MM_RECORD_MOVIE_FRAMES=0;
char support_name[50]="HRP2TABLE";
	int no_obj=5;

	double distance = 0.1;
	double pre_q1, pre_q2, pre_q3,pre_q4, pre_q5, pre_q6, q1, q2, q3, q4, q5, q6;

	if ( manipulation== NULL )
	{
		////initManipulationGenom(); //AKP : This is decleared static in FORMGenom.c, so below we define our own function
		init_Manipulation_params();
	}
	////manipulation->setObjectToManipulate((char*)OBJECT_NAME);


	manipulation->setArmCartesian ( true );
#define CAMERA_JNT_NAME "Tilt"
#define CAMERA_FOV 80.0

//         manipulation->setCameraJnt((char*)CAMERA_JNT_NAME);
	manipulation->setCameraFOV ( CAMERA_FOV );
	manipulation->setCameraImageSize ( 200, 200 );

 configPt JIDO_actual_pos = MY_ALLOC ( double,ACBTSET->robot->nb_dof ); /* Allocation of temporary robot configuration */
        p3d_get_robot_config_into ( ACBTSET->robot,&JIDO_actual_pos ); 

char obj_to_manipulate[10][50]={"RED_BOTTLE","GREY_TAPE","BLACK_TAPE","YELLOW_BOTTLE","BLUE_BOTTLE"};
  

			int i=0;
			printf ( " Manipulating %s \n",obj_to_manipulate[i] );
			int obj_index=get_index_of_robot_by_name ( obj_to_manipulate[i] );
			g3d_draw_allwin_active();
                        int grasp_path_res=0;
// 			if ( strcasestr ( obj_to_manipulate[i],"TAPE" ) )
//                        return 1;
                        manipulation->setNbGraspsToTestForPickGoto(100);
		        grasp_path_res=plan_JIDO_arm_path_to_grasp ( obj_to_manipulate[i], ARM_PICK_GOTO );
                        printf(" After plan_JIDO_arm_path_to_grasp ( %s, ARM_PICK_GOTO )\n", obj_to_manipulate[i]);
                        if(grasp_path_res==0)
                        {
                        printf(" Not able to plan_JIDO_arm_path_to_grasp for %s \n",obj_to_manipulate[i]);
			return 0;
                        }

			/*
                        //////// Playing the current trajectory
                        ////p3d_traj* new_trajPt;
                        current_new_trajPt[traj_ctr] = p3d_create_traj_by_copy(ACBTSET->robot->t[0]);
                       
                        traj_play = TRUE;
                        g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
                        traj_ctr++;
                        p3d_copy_config_into ( ACBTSET->robot, ACBTSET-BLUE>robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS );
                        p3d_set_and_update_this_robot_conf ( ACBTSET->robot,ACBTSET->robot->ROBOT_POS );
                        */

                        execute_JIDO_trajectory();//******Uncomment to execute configuration by configuration

                        
                        JIDO_put_obj_in_hand_into_trashbin("PINK_TRASHBIN",obj_to_manipulate[i]);
                         
                        if ( manipulation->grabObject ( obj_to_manipulate[i] ) !=0 )
			{
			////FORMGENOM_OBJECTGRABED = 0;
			printf ( " AKP WARNING: CAN NOT GRAB %s to Manipulate\n", obj_to_manipulate[i] );
			return 0;
			}

			/*
                        //////Playing the current trajectory
                        current_new_trajPt[traj_ctr] = p3d_create_traj_by_copy(ACBTSET->robot->t[0]);
                       
                        traj_play = TRUE;
                        g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
                        traj_ctr++;
                        p3d_copy_config_into ( ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS );
                        p3d_set_and_update_this_robot_conf ( ACBTSET->robot,ACBTSET->robot->ROBOT_POS );
			*/
                        JIDO_HOLDING_OBJECT=1;
                        index_of_curr_obj_in_JIDO_hand=obj_index;
                        execute_JIDO_trajectory();

			manipulation->releaseObject();
			
                        JIDO_HOLDING_OBJECT=0;
			
                        simulate_object_falling(obj_to_manipulate[i]);
			robots_status_for_Mightability_Maps[index_of_curr_obj_in_JIDO_hand].has_moved=0;
                        index_of_curr_obj_in_JIDO_hand=-1;

i=1;
printf ( " Manipulating %s \n",obj_to_manipulate[i] );
			obj_index=get_index_of_robot_by_name ( obj_to_manipulate[i] );
			g3d_draw_allwin_active();
                        grasp_path_res=0;
// 			if ( strcasestr ( obj_to_manipulate[i],"TAPE" ) )
//                        return 1;
		        grasp_path_res=plan_JIDO_arm_path_to_grasp ( obj_to_manipulate[i], ARM_PICK_GOTO );
                        printf(" After plan_JIDO_arm_path_to_grasp ( %s, ARM_PICK_GOTO )\n", obj_to_manipulate[i]);
                        if(grasp_path_res==0)
                        {
                        printf(" Not able to plan_JIDO_arm_path_to_grasp for %s \n",obj_to_manipulate[i]);
			return 0;
                        }
 
			/*
                         //////Playing the current trajectory
                        current_new_trajPt[traj_ctr] = p3d_create_traj_by_copy(ACBTSET->robot->t[0]);
                       
                        traj_play = TRUE;
                        g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
                        traj_ctr++;
                        p3d_copy_config_into ( ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS );
                        p3d_set_and_update_this_robot_conf ( ACBTSET->robot,ACBTSET->robot->ROBOT_POS );
                        */
                         
                        execute_JIDO_trajectory();

                        JIDO_put_obj_in_hand_into_trashbin("BLUE_TRASHBIN",obj_to_manipulate[i]);

			 if ( manipulation->grabObject ( obj_to_manipulate[i] ) !=0 )
			{
			////FORMGENOM_OBJECTGRABED = 0;
			printf ( " AKP WARNING: CAN NOT GRAB %s to Manipulate\n", obj_to_manipulate[i] );
			return 0;
			}

                        /*
                         //////Playing the current trajectory
                        current_new_trajPt[traj_ctr] = p3d_create_traj_by_copy(ACBTSET->robot->t[0]);
                       
                        traj_play = TRUE;
                        g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
                        traj_ctr++;
                        p3d_copy_config_into ( ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS );
                        p3d_set_and_update_this_robot_conf ( ACBTSET->robot,ACBTSET->robot->ROBOT_POS );
                        */
                        JIDO_HOLDING_OBJECT=1;
                         index_of_curr_obj_in_JIDO_hand=obj_index;
                        execute_JIDO_trajectory();

			manipulation->releaseObject();
			
			JIDO_HOLDING_OBJECT=0;
                        simulate_object_falling(obj_to_manipulate[i]);
 			robots_status_for_Mightability_Maps[index_of_curr_obj_in_JIDO_hand].has_moved=0;
                        index_of_curr_obj_in_JIDO_hand=-1;
 
                        /* TODO: Debug it  
			p3d_set_and_update_this_robot_conf ( ACBTSET->robot,JIDO_actual_pos );
                        play_all_JIDO_trajectories();
                        */ 

i=3;
printf ( " Manipulating %s \n",obj_to_manipulate[i] );
			obj_index=get_index_of_robot_by_name ( obj_to_manipulate[i] );
			g3d_draw_allwin_active();
                        grasp_path_res=0;
// 			if ( strcasestr ( obj_to_manipulate[i],"TAPE" ) )
//                        return 1;
		        grasp_path_res=plan_JIDO_arm_path_to_grasp ( obj_to_manipulate[i], ARM_PICK_GOTO );
                        printf(" After plan_JIDO_arm_path_to_grasp ( %s, ARM_PICK_GOTO )\n", obj_to_manipulate[i]);
                        if(grasp_path_res==0)
                        {
                        printf(" Not able to plan_JIDO_arm_path_to_grasp for %s \n",obj_to_manipulate[i]);
			return 0;
                        }
 
                        /*
                         //////Playing the current trajectory
                        current_new_trajPt[traj_ctr] = p3d_create_traj_by_copy(ACBTSET->robot->t[0]);
                       
                        traj_play = TRUE;
                        g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
                        traj_ctr++;
                        p3d_copy_config_into ( ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS );
                        p3d_set_and_update_this_robot_conf ( ACBTSET->robot,ACBTSET->robot->ROBOT_POS );
                        */
                         
                        execute_JIDO_trajectory();

                        JIDO_put_obj_in_hand_into_trashbin("PINK_TRASHBIN",obj_to_manipulate[i]);

			 if ( manipulation->grabObject ( obj_to_manipulate[i] ) !=0 )
			{
			////FORMGENOM_OBJECTGRABED = 0;
			printf ( " AKP WARNING: CAN NOT GRAB %s to Manipulate\n", obj_to_manipulate[i] );
			return 0;
			}

                        /*
                        //////Playing the current trajectory
                        current_new_trajPt[traj_ctr] = p3d_create_traj_by_copy(ACBTSET->robot->t[0]);
                       
                        traj_play = TRUE;
                        g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
                        traj_ctr++;
                        p3d_copy_config_into ( ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS );
                        p3d_set_and_update_this_robot_conf ( ACBTSET->robot,ACBTSET->robot->ROBOT_POS );
                        */
                        JIDO_HOLDING_OBJECT=1;
                        index_of_curr_obj_in_JIDO_hand=obj_index; 
                        execute_JIDO_trajectory();
                        

			manipulation->releaseObject();
			
			JIDO_HOLDING_OBJECT=0;
                        simulate_object_falling(obj_to_manipulate[i]);
			robots_status_for_Mightability_Maps[index_of_curr_obj_in_JIDO_hand].has_moved=0;
                        index_of_curr_obj_in_JIDO_hand=-1;

MM_RECORD_MOVIE_FRAMES=0;

}

int play_all_JIDO_trajectories()
{
 int i=0;
 for(i=0;i<traj_ctr;i++)
 {
 ACBTSET->robot->tcur=current_new_trajPt[i];
 
 traj_play = TRUE;
 g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
 p3d_copy_config_into ( ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS );
 p3d_set_and_update_this_robot_conf ( ACBTSET->robot,ACBTSET->robot->ROBOT_POS );
 }
}

int JIDO_put_obj_in_hand_into_trashbin ( char trashbin_name[50], char obj_to_manipulate[50] )
{
  int traj_ctr=0;
  p3d_traj* current_new_trajPt[20];

       	p3d_copy_config_into ( ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS );
	envPt = ( p3d_env * ) p3d_get_desc_curid ( P3D_ENV );

        int obj_index=get_index_of_robot_by_name ( obj_to_manipulate );

	configPt obj_actual_pos = MY_ALLOC ( double,envPt->robot[obj_index]->nb_dof );
	p3d_get_robot_config_into ( envPt->robot[obj_index],&obj_actual_pos );

        
        configPt obj_tmp_pos = MY_ALLOC ( double,envPt->robot[obj_index]->nb_dof );
	p3d_get_robot_config_into ( envPt->robot[obj_index],&obj_tmp_pos );

	int by_agent=3;//For jido
        configPt JIDO_actual_pos = MY_ALLOC ( double,ACBTSET->robot->nb_dof ); /* Allocation of temporary robot configuration */
        p3d_get_robot_config_into ( ACBTSET->robot,&JIDO_actual_pos ); 
	int container_index=get_index_of_robot_by_name ( "BLUE_TRASHBIN" );
	find_candidate_points_for_putinto ( by_agent, container_index );
	int candi_pts_ctr=0;

	for ( candi_pts_ctr=0;candi_pts_ctr<candidate_points_to_putinto_by_jido.no_points;candi_pts_ctr++ )
	{
		candidate_points_to_putinto_blue_trashbin_by_jido.point[candi_pts_ctr].x=candidate_points_to_putinto_by_jido.point[candi_pts_ctr].x;
		candidate_points_to_putinto_blue_trashbin_by_jido.point[candi_pts_ctr].y=candidate_points_to_putinto_by_jido.point[candi_pts_ctr].y;
		candidate_points_to_putinto_blue_trashbin_by_jido.point[candi_pts_ctr].z=candidate_points_to_putinto_by_jido.point[candi_pts_ctr].z;
		candidate_points_to_putinto_blue_trashbin_by_jido.weight[candi_pts_ctr]=candidate_points_to_putinto_by_jido.weight[candi_pts_ctr];
	}
	candidate_points_to_putinto_blue_trashbin_by_jido.no_points=candidate_points_to_putinto_by_jido.no_points;

	container_index=get_index_of_robot_by_name ( "PINK_TRASHBIN" );
	find_candidate_points_for_putinto ( by_agent, container_index );
	candi_pts_ctr=0;

	for ( candi_pts_ctr=0;candi_pts_ctr<candidate_points_to_putinto_by_jido.no_points;candi_pts_ctr++ )
	{
		candidate_points_to_putinto_pink_trashbin_by_jido.point[candi_pts_ctr].x=candidate_points_to_putinto_by_jido.point[candi_pts_ctr].x;
		candidate_points_to_putinto_pink_trashbin_by_jido.point[candi_pts_ctr].y=candidate_points_to_putinto_by_jido.point[candi_pts_ctr].y;
		candidate_points_to_putinto_pink_trashbin_by_jido.point[candi_pts_ctr].z=candidate_points_to_putinto_by_jido.point[candi_pts_ctr].z;
		candidate_points_to_putinto_pink_trashbin_by_jido.weight[candi_pts_ctr]=candidate_points_to_putinto_by_jido.weight[candi_pts_ctr];
	}
	candidate_points_to_putinto_pink_trashbin_by_jido.no_points=candidate_points_to_putinto_by_jido.no_points;

	if ( strcasestr ( trashbin_name,"BLUE_TRASHBIN" ) )
	{
		container_index=get_index_of_robot_by_name ( "BLUE_TRASHBIN" );
		int candi_pts_ctr=0;
		for ( candi_pts_ctr=0;candi_pts_ctr<candidate_points_to_putinto_blue_trashbin_by_jido.no_points;candi_pts_ctr++ )
		{
			current_candidate_points_to_putinto.point[candi_pts_ctr].x=candidate_points_to_putinto_blue_trashbin_by_jido.point[candi_pts_ctr].x;
			current_candidate_points_to_putinto.point[candi_pts_ctr].y=candidate_points_to_putinto_blue_trashbin_by_jido.point[candi_pts_ctr].y;
			current_candidate_points_to_putinto.point[candi_pts_ctr].z=candidate_points_to_putinto_blue_trashbin_by_jido.point[candi_pts_ctr].z;
			current_candidate_points_to_putinto.weight[candi_pts_ctr]=candidate_points_to_putinto_blue_trashbin_by_jido.weight[candi_pts_ctr];
		}
		current_candidate_points_to_putinto.no_points=candidate_points_to_putinto_blue_trashbin_by_jido.no_points;
	}


	if ( strcasestr ( trashbin_name,"PINK_TRASHBIN" ) )
	{
		container_index=get_index_of_robot_by_name ( "PINK_TRASHBIN" );
		int candi_pts_ctr=0;
		for ( candi_pts_ctr=0;candi_pts_ctr<candidate_points_to_putinto_blue_trashbin_by_jido.no_points;candi_pts_ctr++ )
		{
			current_candidate_points_to_putinto.point[candi_pts_ctr].x=candidate_points_to_putinto_pink_trashbin_by_jido.point[candi_pts_ctr].x;
			current_candidate_points_to_putinto.point[candi_pts_ctr].y=candidate_points_to_putinto_pink_trashbin_by_jido.point[candi_pts_ctr].y;
			current_candidate_points_to_putinto.point[candi_pts_ctr].z=candidate_points_to_putinto_pink_trashbin_by_jido.point[candi_pts_ctr].z;
			current_candidate_points_to_putinto.weight[candi_pts_ctr]=candidate_points_to_putinto_pink_trashbin_by_jido.weight[candi_pts_ctr];
		}
		current_candidate_points_to_putinto.no_points=candidate_points_to_putinto_pink_trashbin_by_jido.no_points;
	}


	reverse_sort_weighted_candidate_points_to_putinto_obj();
	//////////printf ( " >>>> No. of point to putinto =%d for the object %s \n", object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.no_points,obj_to_manipulate[i] );

	printf ( " >>>> No. of point to putinto =%d, for %s \n", current_candidate_points_to_putinto.no_points,obj_to_manipulate );

	double x,y,z,rx,ry,rz;
	manipulation->getArmX ( &x, &y, &z, &rx, &ry, &rz );

	p3d_get_robot_config_into ( ACBTSET->robot,&JIDO_actual_pos );

	point_co_ordi goal_pos;
	int put_into_success=0;

	int i1=0;
	for ( i1=0;i1<current_candidate_points_to_putinto.no_points;i1++ )
	{
		printf ( " **** Testing for %d th place to put into the trashbin for %s\n", i1, obj_to_manipulate );
		////g3d_drawDisc(object_MM.object[rob_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i].x, object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[i].y, object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[i].z, grid_around_HRP2.GRID_SET->pace/2.0, Red, NULL);

		/*rob_tmp_pos[6]=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i1].x;
		rob_tmp_pos[7]=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i1].y;
		rob_tmp_pos[8]=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i1].z+0.15;
		 
		g3d_drawDisc(rob_tmp_pos[6],rob_tmp_pos[7],rob_tmp_pos[8],0.1, Green, NULL);

		point_to_put.x=rob_tmp_pos[6];
		point_to_put.y=rob_tmp_pos[7];
		point_to_put.z=rob_tmp_pos[8];
		*/

		/*point_to_put.x=x+0.1;
		point_to_put.y=y+0.1;
		point_to_put.z=z+0.1;
		*/

		/*goal_pos.x=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i1].x;
		goal_pos.y=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i1].y;
		goal_pos.z=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i1].z+0.15;
		                */

		goal_pos.x=current_candidate_points_to_putinto.point[i1].x;
		goal_pos.y=current_candidate_points_to_putinto.point[i1].y;
		goal_pos.z=current_candidate_points_to_putinto.point[i1].z+0.15;

		point_to_put.x=goal_pos.x;
		point_to_put.y=goal_pos.y;
		point_to_put.z=goal_pos.z;

		obj_tmp_pos[6]=point_to_put.x;
		obj_tmp_pos[7]=point_to_put.y;
		obj_tmp_pos[8]=point_to_put.z-0.25;
		p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_tmp_pos );
		g3d_draw_allwin_active();
		p3d_col_deactivate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );

		int kcd_with_report=0;
		int res = p3d_col_test_robot ( envPt->robot[obj_index],kcd_with_report );
		if ( res>0 )
		{
			printf ( " There is collision at %d th place to put %s , res=%d \n", i1, obj_to_manipulate, res );
			pqp_print_colliding_pair();
			continue;
			//return 0;
		}
		p3d_col_activate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );

		p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_actual_pos );

		////manipulation->setArmX(rob_tmp_pos[6],rob_tmp_pos[7], rob_tmp_pos[8], rx, ry, rz);
		////int arm_set_res=manipulation->setArmX(rob_tmp_pos[6],rob_tmp_pos[7], rob_tmp_pos[8], 500);
		int maxi_no_rand_samples=50;
		double rx_tmp, ry_tmp, rz_tmp;
		int setArmX_ctr=0;
		for ( setArmX_ctr=0; setArmX_ctr<maxi_no_rand_samples; setArmX_ctr++ )
		{
			rx_tmp= p3d_random ( -M_PI, M_PI );
			ry_tmp= p3d_random ( -M_PI, M_PI );
			rz_tmp= p3d_random ( -M_PI, M_PI );

			int arm_set_res= manipulation->setArmX ( goal_pos.x, goal_pos.y, goal_pos.z, rx_tmp, ry_tmp, rz_tmp );

			if ( arm_set_res==0 && !p3d_col_test_robot ( ACBTSET->robot, 0 ) )
			{
				////p3d_destroy_config(_robotPt, qcur);
				////return 0;
			}
			else
			{
				printf ( " AKP Warning : Fail to setArmX\n" );
				p3d_set_and_update_this_robot_conf ( ACBTSET->robot,JIDO_actual_pos );
				continue;
			}

			////////////int arm_set_res=manipulation->setArmX(goal_pos.x,goal_pos.y, goal_pos.z, 5000);

			g3d_draw_allwin_active();
			/* if(arm_set_res==1)//Fail
			 {
			  printf(" AKP Warning : Fail to setArmX\n");
			  p3d_set_and_update_this_robot_conf(ACBTSET->robot,JIDO_actual_pos);
			  ////return 1;
			  continue;
			 }

			  g3d_draw_allwin_active();
			 */

			p3d_get_robot_config_into ( ACBTSET->robot,&ACBTSET->robot->ROBOT_GOTO );
			p3d_set_and_update_this_robot_conf ( ACBTSET->robot,JIDO_actual_pos );
			p3d_get_robot_config_into ( ACBTSET->robot,&ACBTSET->robot->ROBOT_POS );
			////////p3d_set_and_update_this_robot_conf(envPt->robot[rob_index],rob_tmp_pos);

			g3d_draw_allwin_active();
			int path_res=0;
			if ( manipulation->grabObject ( obj_to_manipulate ) !=0 )
			{
				////FORMGENOM_OBJECTGRABED = 0;
				printf ( " AKP WARNING: CAN NOT GRAB THE OBJECT\n" );
				return 0;
			}

			//// }
			path_res=plan_JIDO_arm_path_to_grasp ( obj_to_manipulate, ARM_PICK_TAKE_TO_FREE );
			////////pqp_print_colliding_pair();

			if ( path_res==1 )
			{

				configPt* curr_traj_configs;
				int no_configs=0;
				////curr_traj_configs= GP_GetAllTrajectoriesAsOne ( ACBTSET->robot,  no_configs );

				curr_traj_configs= GP_GetTrajectory ( ACBTSET->robot,  ACBTSET->robot->t[0], no_configs );

				printf ( " <<<< no_configs for putinto %d >>>>\n",no_configs );
				/*int j=0;
				for ( j=0;j<no_configs;j++ )
				{
					p3d_set_and_update_this_robot_conf ( ACBTSET->robot,curr_traj_configs[j] );
					g3d_draw_allwin_active();
				}*/
				p3d_set_and_update_this_robot_conf ( ACBTSET->robot,curr_traj_configs[no_configs-1] );
				g3d_draw_allwin_active();
				p3d_get_robot_config_into ( envPt->robot[obj_index],&obj_tmp_pos );

				p3d_col_deactivate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );

//                                                 //****TO SHOW ARM MOTION
//
//                                                 for(j=0;j< nbPositions;j++)
//                                                 {
//                                                 manipulation->setArmQ(positions[j].q1, positions[j].q2, positions[j].q3, positions[j].q4, positions[j].q5, positions[j].q6);
// 						g3d_draw_allwin_active();
//                                                  fl_check_forms();
//                                                 }
//                                                 manipulation->setArmQ(positions[0].q1, positions[0].q2, positions[0].q3, positions[0].q4, positions[0].q5, positions[0].q6);
//                                                 //p3d_set_and_update_this_robot_conf ( ACBTSET->robot,ACBTSET->robot->ROBOT_POS );
// 						////p3d_get_robot_config_into ( ACBTSET->robot,&ACBTSET->robot->ROBOT_POS );
//                                                 g3d_draw_allwin_active();
// 					        //****END TO SHOW ARM MOTION

				////manipulation->releaseObject();
				////Just to test of falling of the object after releasing
				int falling_collides=0;
				////////p3d_get_robot_config_into ( envPt->robot[obj_index],&obj_tmp_pos );

				////////p3d_col_deactivate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );

				manipulation->releaseObject();

				obj_tmp_pos[8]-=0.15;
				p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_tmp_pos );
				g3d_draw_allwin_active();
				kcd_with_report=0;
				res = p3d_col_test_robot ( envPt->robot[obj_index],kcd_with_report );
				if ( res>0 )
				{
					printf ( " If freefall, There will be collision at %d th place to put %s , res=%d \n", i1, envPt->robot[obj_index]->name, res );
					pqp_print_colliding_pair();

					p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_actual_pos );
					falling_collides=1;
					p3d_col_activate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );

					if ( manipulation->grabObject ( obj_to_manipulate ) !=0 )
					{
						////FORMGENOM_OBJECTGRABED = 0;
						printf ( " **** There was free fall collosion. AKP WARNING: CAN NOT GRAB THE OBJECT\n" );
						return 0;
					}
					p3d_set_and_update_this_robot_conf ( ACBTSET->robot,JIDO_actual_pos );
					g3d_draw_allwin_active();
					continue;
				}
				else
				{
					p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_actual_pos );
					manipulation->setArmQ ( positions[0].q1, positions[0].q2, positions[0].q3, positions[0].q4, positions[0].q5, positions[0].q6 );

					if ( manipulation->grabObject ( obj_to_manipulate ) !=0 )
					{
						////FORMGENOM_OBJECTGRABED = 0;
						printf ( " AKP WARNING: CAN NOT GRAB THE OBJECT\n" );
						return 0;
					}
					/*
                                        current_new_trajPt[traj_ctr] = p3d_create_traj_by_copy ( ACBTSET->robot->t[0] );
					ACBTSET->robot->tcur=ACBTSET->robot->t[0];
					traj_play = TRUE;
					g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
					traj_ctr++;
					*/	
 					/*
                                        printf(" ***** TOTAL no. of Configs = %d \n",nbPositions);
					int j=0;
					for ( j=0;j< nbPositions;j++ )
					{
						manipulation->setArmQ ( positions[j].q1, positions[j].q2, positions[j].q3, positions[j].q4, positions[j].q5, positions[j].q6 );
						g3d_draw_allwin_active();
						fl_check_forms();
					}*/
					//manipulation->releaseObject();
					
					printf ( " Putting the %s into trashbin is successfull \n",obj_to_manipulate );
					//p3d_set_and_update_this_robot_conf ( ACBTSET->robot,JIDO_actual_pos );
					/*
					p3d_get_robot_config_into ( ACBTSET->robot,&JIDO_actual_pos );
					p3d_get_robot_config_into ( ACBTSET->robot,&ACBTSET->robot->ROBOT_POS );
					obj_tmp_pos[8]+=0.15;
					p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_tmp_pos );
					//////p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_actual_pos );
					p3d_col_activate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );
					g3d_draw_allwin_active();
					*/
// 						if ( manipulation->grabObject ( obj_to_manipulate[i] ) !=0 )
// 						{
// 					////FORMGENOM_OBJECTGRABED = 0;
// 						printf ( " AKP WARNING: CAN NOT GRAB THE OBJECT\n" );
// 						return 0;
// 						}
//
// 				                 for(j=0;j< nbPositions;j++)
//                                                 {
//                                                 manipulation->setArmQ(positions[j].q1, positions[j].q2, positions[j].q3, positions[j].q4, positions[j].q5, positions[j].q6);
// 						g3d_draw_allwin_active();
//                                                  fl_check_forms();
//                                                 }
					//manipulation->releaseObject();
					/*current_new_trajPt[traj_ctr] = p3d_create_traj_by_copy ( ACBTSET->robot->t[0] );
					ACBTSET->robot->tcur=ACBTSET->robot->t[0];
					traj_play = TRUE;
					g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
					traj_ctr++;
                                        
					p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_tmp_pos );
                                        */ 
					////current_new_trajPt = p3d_create_traj_by_copy(ACBTSET->robot->t[0]);
					////CB_showtraj_obj(NULL,0);

					put_into_success=1;
				}
				////continue;
				//////////p3d_col_activate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );

				////////if ( falling_collides==1 )
				////////continue;

				//return 0;

				/*
				int obj_pos_ctr=0;
				for ( obj_pos_ctr=0;obj_pos_ctr<15;obj_pos_ctr++ )
				{
				printf(" Setting %s, as (x,y,z) =(%lf, %lf, %lf)\n", envPt->robot[obj_index]->name,obj_tmp_pos[6], obj_tmp_pos[7], obj_tmp_pos[8]);
				obj_tmp_pos[8]-=0.02;
				p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_tmp_pos );
				g3d_draw_allwin_active();
				        g3d_draw_allwin_active();
				////kcd_with_report=0;
				////res = p3d_col_test_robot(envPt->robot[obj_index],kcd_with_report);
				////if(res>0)
				////{
				////printf(" While falling There is collision at %d th place to put %s , res=%d \n", i1, envPt->robot[obj_index]->name, res);
				////pqp_print_colliding_pair();
				////p3d_set_and_update_this_robot_conf(envPt->robot[obj_index],obj_actual_pos);
				////falling_collides=1;
				////continue;
				////break;
				//return 0;
				////}

				}
				*/
				////p3d_col_activate_rob_rob(envPt->robot[obj_index],ACBTSET->robot);

				////if(falling_collides==1)
				////continue;
				////else
				////break;
				break; //It will break the for ( setArmX_ctr=0) loop

			}

		}//end for ( setArmX_ctr=0



		/*
		p3d_copy_config_into(ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS);

		plan_JIDO_arm_path_to_grasp(obj_to_manipulate[i], ARM_PICK_TAKE_TO_FREE);
		pqp_print_colliding_pair();

		////curr_traj_configs= GP_GetAllTrajectoriesAsOne ( ACBTSET->robot,  no_configs );
		curr_traj_configs= GP_GetTrajectory ( ACBTSET->robot,  ACBTSET->robot->t[0], no_configs );

		printf(" <<<< no_configs %d >>>>\n",no_configs);
		j=0;
		for(j=0;j<no_configs;j++)
		 {
		 p3d_set_and_update_this_robot_conf(ACBTSET->robot,curr_traj_configs[j]);
		 g3d_draw_allwin_active();
		 }
		////p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos);
		p3d_copy_config_into(ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS);
		*/
		////return 1;
		if ( put_into_success==1 )
		{
//                         int j1=0;
// 			 for(j1=0;j1< nbPositions;j1++)
//                                                 {
//                                                 manipulation->setArmQ(positions[j1].q1, positions[j1].q2, positions[j1].q3, positions[j1].q4, positions[j1].q5, positions[j1].q6);
// 						g3d_draw_allwin_active();
//                                                 }
			manipulation->releaseObject();

			/*p3d_set_and_update_this_robot_conf(ACBTSET->robot,rob_cur_pos);
			traj_play = TRUE;
			ACBTSET->robot->tcur=current_new_trajPt;
			   g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
			 */
			/*
			            if ( manipulation->grabObject ( obj_to_manipulate[i] ) !=0 )
					{
					////FORMGENOM_OBJECTGRABED = 0;
					printf ( " AKP WARNING: CAN NOT GRAB THE OBJECT\n" );
					return 0;
					}


			manipulation->releaseObject();
			            */
			
			
			
			printf ( " %d th place to putinto trashbin is final place to release %s\n", i1, envPt->robot[obj_index]->name );
			g3d_draw_allwin_active();
			////return 1;
			break;
		}
	}//End for(i1)


}

int simulate_object_falling(char obj_name[50])
{
   int obj_index=get_index_of_robot_by_name ( obj_name );

// 	configPt obj_actual_pos = MY_ALLOC ( double,envPt->robot[obj_index]->nb_dof );
// 	p3d_get_robot_config_into ( envPt->robot[obj_index],&obj_actual_pos );

        
        configPt obj_tmp_pos = MY_ALLOC ( double,envPt->robot[obj_index]->nb_dof );
	p3d_get_robot_config_into ( envPt->robot[obj_index],&obj_tmp_pos );

			int obj_pos_ctr=0;
                        p3d_get_robot_config_into ( envPt->robot[obj_index],&obj_tmp_pos );
			for ( obj_pos_ctr=0;obj_pos_ctr<15;obj_pos_ctr++ )
			{
				////printf(" Setting %s, as (x,y,z) =(%lf, %lf, %lf)\n", envPt->robot[obj_index]->name,obj_tmp_pos[6], obj_tmp_pos[7], obj_tmp_pos[8]);
				obj_tmp_pos[8]-=0.02;
				p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_tmp_pos );
				g3d_draw_allwin_active();

				int kcd_with_report=0;
				int res  = p3d_col_test_robot ( envPt->robot[obj_index],kcd_with_report );
				if ( res>0 )
				{
					printf ( " While falling There is collision for %s , res=%d \n",  envPt->robot[obj_index]->name, res );
					pqp_print_colliding_pair();
					////p3d_set_and_update_this_robot_conf(envPt->robot[obj_index],obj_actual_pos);
					////falling_collides=1;
					////continue;
					////break;
					//return 0;
				}

			}
      p3d_get_robot_config_into ( envPt->robot[obj_index],&envPt->robot[obj_index]->ROBOT_POS );
      MY_FREE (obj_tmp_pos, double,envPt->robot[obj_index]->nb_dof );

}

int JIDO_put_obj_in_hand_into_trashbin_old ( char trashbin_name[50], char obj_to_manipulate[50] )
{
  int traj_ctr=0;
  p3d_traj* current_new_trajPt[20];

       	p3d_copy_config_into ( ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS );
	envPt = ( p3d_env * ) p3d_get_desc_curid ( P3D_ENV );

        int obj_index=get_index_of_robot_by_name ( obj_to_manipulate );

	configPt obj_actual_pos = MY_ALLOC ( double,envPt->robot[obj_index]->nb_dof );
	p3d_get_robot_config_into ( envPt->robot[obj_index],&obj_actual_pos );

        
        configPt obj_tmp_pos = MY_ALLOC ( double,envPt->robot[obj_index]->nb_dof );
	p3d_get_robot_config_into ( envPt->robot[obj_index],&obj_tmp_pos );

	int by_agent=3;//For jido
        configPt JIDO_actual_pos = MY_ALLOC ( double,ACBTSET->robot->nb_dof ); /* Allocation of temporary robot configuration */
        p3d_get_robot_config_into ( ACBTSET->robot,&JIDO_actual_pos ); 
	int container_index=get_index_of_robot_by_name ( "BLUE_TRASHBIN" );
	find_candidate_points_for_putinto ( by_agent, container_index );
	int candi_pts_ctr=0;

	for ( candi_pts_ctr=0;candi_pts_ctr<candidate_points_to_putinto_by_jido.no_points;candi_pts_ctr++ )
	{
		candidate_points_to_putinto_blue_trashbin_by_jido.point[candi_pts_ctr].x=candidate_points_to_putinto_by_jido.point[candi_pts_ctr].x;
		candidate_points_to_putinto_blue_trashbin_by_jido.point[candi_pts_ctr].y=candidate_points_to_putinto_by_jido.point[candi_pts_ctr].y;
		candidate_points_to_putinto_blue_trashbin_by_jido.point[candi_pts_ctr].z=candidate_points_to_putinto_by_jido.point[candi_pts_ctr].z;
		candidate_points_to_putinto_blue_trashbin_by_jido.weight[candi_pts_ctr]=candidate_points_to_putinto_by_jido.weight[candi_pts_ctr];
	}
	candidate_points_to_putinto_blue_trashbin_by_jido.no_points=candidate_points_to_putinto_by_jido.no_points;

	container_index=get_index_of_robot_by_name ( "PINK_TRASHBIN" );
	find_candidate_points_for_putinto ( by_agent, container_index );
	candi_pts_ctr=0;

	for ( candi_pts_ctr=0;candi_pts_ctr<candidate_points_to_putinto_by_jido.no_points;candi_pts_ctr++ )
	{
		candidate_points_to_putinto_pink_trashbin_by_jido.point[candi_pts_ctr].x=candidate_points_to_putinto_by_jido.point[candi_pts_ctr].x;
		candidate_points_to_putinto_pink_trashbin_by_jido.point[candi_pts_ctr].y=candidate_points_to_putinto_by_jido.point[candi_pts_ctr].y;
		candidate_points_to_putinto_pink_trashbin_by_jido.point[candi_pts_ctr].z=candidate_points_to_putinto_by_jido.point[candi_pts_ctr].z;
		candidate_points_to_putinto_pink_trashbin_by_jido.weight[candi_pts_ctr]=candidate_points_to_putinto_by_jido.weight[candi_pts_ctr];
	}
	candidate_points_to_putinto_pink_trashbin_by_jido.no_points=candidate_points_to_putinto_by_jido.no_points;

	if ( strcasestr ( trashbin_name,"BLUE_TRASHBIN" ) )
	{
		container_index=get_index_of_robot_by_name ( "BLUE_TRASHBIN" );
		int candi_pts_ctr=0;
		for ( candi_pts_ctr=0;candi_pts_ctr<candidate_points_to_putinto_blue_trashbin_by_jido.no_points;candi_pts_ctr++ )
		{
			current_candidate_points_to_putinto.point[candi_pts_ctr].x=candidate_points_to_putinto_blue_trashbin_by_jido.point[candi_pts_ctr].x;
			current_candidate_points_to_putinto.point[candi_pts_ctr].y=candidate_points_to_putinto_blue_trashbin_by_jido.point[candi_pts_ctr].y;
			current_candidate_points_to_putinto.point[candi_pts_ctr].z=candidate_points_to_putinto_blue_trashbin_by_jido.point[candi_pts_ctr].z;
			current_candidate_points_to_putinto.weight[candi_pts_ctr]=candidate_points_to_putinto_blue_trashbin_by_jido.weight[candi_pts_ctr];
		}
		current_candidate_points_to_putinto.no_points=candidate_points_to_putinto_blue_trashbin_by_jido.no_points;
	}


	if ( strcasestr ( trashbin_name,"PINK_TRASHBIN" ) )
	{
		container_index=get_index_of_robot_by_name ( "PINK_TRASHBIN" );
		int candi_pts_ctr=0;
		for ( candi_pts_ctr=0;candi_pts_ctr<candidate_points_to_putinto_blue_trashbin_by_jido.no_points;candi_pts_ctr++ )
		{
			current_candidate_points_to_putinto.point[candi_pts_ctr].x=candidate_points_to_putinto_pink_trashbin_by_jido.point[candi_pts_ctr].x;
			current_candidate_points_to_putinto.point[candi_pts_ctr].y=candidate_points_to_putinto_pink_trashbin_by_jido.point[candi_pts_ctr].y;
			current_candidate_points_to_putinto.point[candi_pts_ctr].z=candidate_points_to_putinto_pink_trashbin_by_jido.point[candi_pts_ctr].z;
			current_candidate_points_to_putinto.weight[candi_pts_ctr]=candidate_points_to_putinto_pink_trashbin_by_jido.weight[candi_pts_ctr];
		}
		current_candidate_points_to_putinto.no_points=candidate_points_to_putinto_pink_trashbin_by_jido.no_points;
	}


	reverse_sort_weighted_candidate_points_to_putinto_obj();
	//////////printf ( " >>>> No. of point to putinto =%d for the object %s \n", object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.no_points,obj_to_manipulate[i] );

	printf ( " >>>> No. of point to putinto =%d, for %s \n", current_candidate_points_to_putinto.no_points,obj_to_manipulate );

	double x,y,z,rx,ry,rz;
	manipulation->getArmX ( &x, &y, &z, &rx, &ry, &rz );

	p3d_get_robot_config_into ( ACBTSET->robot,&JIDO_actual_pos );

	point_co_ordi goal_pos;
	int put_into_success=0;

	int i1=0;
	for ( i1=0;i1<current_candidate_points_to_putinto.no_points;i1++ )
	{
		printf ( " **** Testing for %d th place to put into the trashbin for %s\n", i1, obj_to_manipulate );
		////g3d_drawDisc(object_MM.object[rob_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i].x, object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[i].y, object_MM.object[nr_ctr].sym_MM.reachable_for_putting_inside_by_JIDO.point[i].z, grid_around_HRP2.GRID_SET->pace/2.0, Red, NULL);

		/*rob_tmp_pos[6]=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i1].x;
		rob_tmp_pos[7]=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i1].y;
		rob_tmp_pos[8]=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i1].z+0.15;
		 
		g3d_drawDisc(rob_tmp_pos[6],rob_tmp_pos[7],rob_tmp_pos[8],0.1, Green, NULL);

		point_to_put.x=rob_tmp_pos[6];
		point_to_put.y=rob_tmp_pos[7];
		point_to_put.z=rob_tmp_pos[8];
		*/

		/*point_to_put.x=x+0.1;
		point_to_put.y=y+0.1;
		point_to_put.z=z+0.1;
		*/

		/*goal_pos.x=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i1].x;
		goal_pos.y=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i1].y;
		goal_pos.z=object_MM.object[container_index].sym_MM.reachable_for_putting_inside_by_JIDO.point[i1].z+0.15;
		                */

		goal_pos.x=current_candidate_points_to_putinto.point[i1].x;
		goal_pos.y=current_candidate_points_to_putinto.point[i1].y;
		goal_pos.z=current_candidate_points_to_putinto.point[i1].z+0.15;

		point_to_put.x=goal_pos.x;
		point_to_put.y=goal_pos.y;
		point_to_put.z=goal_pos.z;

		obj_tmp_pos[6]=point_to_put.x;
		obj_tmp_pos[7]=point_to_put.y;
		obj_tmp_pos[8]=point_to_put.z-0.25;
		p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_tmp_pos );
		g3d_draw_allwin_active();
		p3d_col_deactivate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );

		int kcd_with_report=0;
		int res = p3d_col_test_robot ( envPt->robot[obj_index],kcd_with_report );
		if ( res>0 )
		{
			printf ( " There is collision at %d th place to put %s , res=%d \n", i1, obj_to_manipulate, res );
			pqp_print_colliding_pair();
			continue;
			//return 0;
		}
		p3d_col_activate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );

		p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_actual_pos );

		////manipulation->setArmX(rob_tmp_pos[6],rob_tmp_pos[7], rob_tmp_pos[8], rx, ry, rz);
		////int arm_set_res=manipulation->setArmX(rob_tmp_pos[6],rob_tmp_pos[7], rob_tmp_pos[8], 500);
		int maxi_no_rand_samples=50;
		double rx_tmp, ry_tmp, rz_tmp;
		int setArmX_ctr=0;
		for ( setArmX_ctr=0; setArmX_ctr<maxi_no_rand_samples; setArmX_ctr++ )
		{
			rx_tmp= p3d_random ( -M_PI, M_PI );
			ry_tmp= p3d_random ( -M_PI, M_PI );
			rz_tmp= p3d_random ( -M_PI, M_PI );

			int arm_set_res= manipulation->setArmX ( goal_pos.x, goal_pos.y, goal_pos.z, rx_tmp, ry_tmp, rz_tmp );

			if ( arm_set_res==0 && !p3d_col_test_robot ( ACBTSET->robot, 0 ) )
			{
				////p3d_destroy_config(_robotPt, qcur);
				////return 0;
			}
			else
			{
				printf ( " AKP Warning : Fail to setArmX\n" );
				p3d_set_and_update_this_robot_conf ( ACBTSET->robot,JIDO_actual_pos );
				continue;
			}

			////////////int arm_set_res=manipulation->setArmX(goal_pos.x,goal_pos.y, goal_pos.z, 5000);

			g3d_draw_allwin_active();
			/* if(arm_set_res==1)//Fail
			 {
			  printf(" AKP Warning : Fail to setArmX\n");
			  p3d_set_and_update_this_robot_conf(ACBTSET->robot,JIDO_actual_pos);
			  ////return 1;
			  continue;
			 }

			  g3d_draw_allwin_active();
			 */

			p3d_get_robot_config_into ( ACBTSET->robot,&ACBTSET->robot->ROBOT_GOTO );
			p3d_set_and_update_this_robot_conf ( ACBTSET->robot,JIDO_actual_pos );
			p3d_get_robot_config_into ( ACBTSET->robot,&ACBTSET->robot->ROBOT_POS );
			////////p3d_set_and_update_this_robot_conf(envPt->robot[rob_index],rob_tmp_pos);

			g3d_draw_allwin_active();
			int path_res=0;
			if ( manipulation->grabObject ( obj_to_manipulate ) !=0 )
			{
				////FORMGENOM_OBJECTGRABED = 0;
				printf ( " AKP WARNING: CAN NOT GRAB THE OBJECT\n" );
				return 0;
			}

			//// }
			path_res=plan_JIDO_arm_path_to_grasp ( obj_to_manipulate, ARM_PICK_TAKE_TO_FREE );
			////////pqp_print_colliding_pair();

			if ( path_res==1 )
			{

				configPt* curr_traj_configs;
				int no_configs=0;
				////curr_traj_configs= GP_GetAllTrajectoriesAsOne ( ACBTSET->robot,  no_configs );

				curr_traj_configs= GP_GetTrajectory ( ACBTSET->robot,  ACBTSET->robot->t[0], no_configs );

				printf ( " <<<< no_configs for putinto %d >>>>\n",no_configs );
				/*int j=0;
				for ( j=0;j<no_configs;j++ )
				{
					p3d_set_and_update_this_robot_conf ( ACBTSET->robot,curr_traj_configs[j] );
					g3d_draw_allwin_active();
				}*/
				p3d_set_and_update_this_robot_conf ( ACBTSET->robot,curr_traj_configs[no_configs-1] );
				g3d_draw_allwin_active();
				p3d_get_robot_config_into ( envPt->robot[obj_index],&obj_tmp_pos );

				p3d_col_deactivate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );

//                                                 //****TO SHOW ARM MOTION
//
//                                                 for(j=0;j< nbPositions;j++)
//                                                 {
//                                                 manipulation->setArmQ(positions[j].q1, positions[j].q2, positions[j].q3, positions[j].q4, positions[j].q5, positions[j].q6);
// 						g3d_draw_allwin_active();
//                                                  fl_check_forms();
//                                                 }
//                                                 manipulation->setArmQ(positions[0].q1, positions[0].q2, positions[0].q3, positions[0].q4, positions[0].q5, positions[0].q6);
//                                                 //p3d_set_and_update_this_robot_conf ( ACBTSET->robot,ACBTSET->robot->ROBOT_POS );
// 						////p3d_get_robot_config_into ( ACBTSET->robot,&ACBTSET->robot->ROBOT_POS );
//                                                 g3d_draw_allwin_active();
// 					        //****END TO SHOW ARM MOTION

				////manipulation->releaseObject();
				////Just to test of falling of the object after releasing
				int falling_collides=0;
				////////p3d_get_robot_config_into ( envPt->robot[obj_index],&obj_tmp_pos );

				////////p3d_col_deactivate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );

				manipulation->releaseObject();

				obj_tmp_pos[8]-=0.15;
				p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_tmp_pos );
				g3d_draw_allwin_active();
				kcd_with_report=0;
				res = p3d_col_test_robot ( envPt->robot[obj_index],kcd_with_report );
				if ( res>0 )
				{
					printf ( " If freefall, There will be collision at %d th place to put %s , res=%d \n", i1, envPt->robot[obj_index]->name, res );
					pqp_print_colliding_pair();

					p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_actual_pos );
					falling_collides=1;
					p3d_col_activate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );

					if ( manipulation->grabObject ( obj_to_manipulate ) !=0 )
					{
						////FORMGENOM_OBJECTGRABED = 0;
						printf ( " **** There was free fall collosion. AKP WARNING: CAN NOT GRAB THE OBJECT\n" );
						return 0;
					}
					p3d_set_and_update_this_robot_conf ( ACBTSET->robot,JIDO_actual_pos );
					g3d_draw_allwin_active();
					continue;
				}
				else
				{
					p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_actual_pos );
					manipulation->setArmQ ( positions[0].q1, positions[0].q2, positions[0].q3, positions[0].q4, positions[0].q5, positions[0].q6 );

					if ( manipulation->grabObject ( obj_to_manipulate ) !=0 )
					{
						////FORMGENOM_OBJECTGRABED = 0;
						printf ( " AKP WARNING: CAN NOT GRAB THE OBJECT\n" );
						return 0;
					}
					/*
                                        current_new_trajPt[traj_ctr] = p3d_create_traj_by_copy ( ACBTSET->robot->t[0] );
					ACBTSET->robot->tcur=ACBTSET->robot->t[0];
					traj_play = TRUE;
					g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
					traj_ctr++;
					*/	
 					
                                        printf(" ***** TOTAL no. of Configs = %d \n",nbPositions);
					int j=0;
					for ( j=0;j< nbPositions;j++ )
					{
						manipulation->setArmQ ( positions[j].q1, positions[j].q2, positions[j].q3, positions[j].q4, positions[j].q5, positions[j].q6 );
						g3d_draw_allwin_active();
						fl_check_forms();
					}
					//manipulation->releaseObject();
					
					printf ( " Putting the %s into trashbin is successfull \n",obj_to_manipulate );
					//p3d_set_and_update_this_robot_conf ( ACBTSET->robot,JIDO_actual_pos );
					/*
					p3d_get_robot_config_into ( ACBTSET->robot,&JIDO_actual_pos );
					p3d_get_robot_config_into ( ACBTSET->robot,&ACBTSET->robot->ROBOT_POS );
					obj_tmp_pos[8]+=0.15;
					p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_tmp_pos );
					//////p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_actual_pos );
					p3d_col_activate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );
					g3d_draw_allwin_active();
					*/
// 						if ( manipulation->grabObject ( obj_to_manipulate[i] ) !=0 )
// 						{
// 					////FORMGENOM_OBJECTGRABED = 0;
// 						printf ( " AKP WARNING: CAN NOT GRAB THE OBJECT\n" );
// 						return 0;
// 						}
//
// 				                 for(j=0;j< nbPositions;j++)
//                                                 {
//                                                 manipulation->setArmQ(positions[j].q1, positions[j].q2, positions[j].q3, positions[j].q4, positions[j].q5, positions[j].q6);
// 						g3d_draw_allwin_active();
//                                                  fl_check_forms();
//                                                 }
					//manipulation->releaseObject();
					/*current_new_trajPt[traj_ctr] = p3d_create_traj_by_copy ( ACBTSET->robot->t[0] );
					ACBTSET->robot->tcur=ACBTSET->robot->t[0];
					traj_play = TRUE;
					g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
					traj_ctr++;
                                        
					p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_tmp_pos );
                                        */ 
					////current_new_trajPt = p3d_create_traj_by_copy(ACBTSET->robot->t[0]);
					////CB_showtraj_obj(NULL,0);

					put_into_success=1;
				}
				////continue;
				//////////p3d_col_activate_rob_rob ( envPt->robot[obj_index],ACBTSET->robot );

				////////if ( falling_collides==1 )
				////////continue;

				//return 0;

				/*
				int obj_pos_ctr=0;
				for ( obj_pos_ctr=0;obj_pos_ctr<15;obj_pos_ctr++ )
				{
				printf(" Setting %s, as (x,y,z) =(%lf, %lf, %lf)\n", envPt->robot[obj_index]->name,obj_tmp_pos[6], obj_tmp_pos[7], obj_tmp_pos[8]);
				obj_tmp_pos[8]-=0.02;
				p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_tmp_pos );
				g3d_draw_allwin_active();
				        g3d_draw_allwin_active();
				////kcd_with_report=0;
				////res = p3d_col_test_robot(envPt->robot[obj_index],kcd_with_report);
				////if(res>0)
				////{
				////printf(" While falling There is collision at %d th place to put %s , res=%d \n", i1, envPt->robot[obj_index]->name, res);
				////pqp_print_colliding_pair();
				////p3d_set_and_update_this_robot_conf(envPt->robot[obj_index],obj_actual_pos);
				////falling_collides=1;
				////continue;
				////break;
				//return 0;
				////}

				}
				*/
				////p3d_col_activate_rob_rob(envPt->robot[obj_index],ACBTSET->robot);

				////if(falling_collides==1)
				////continue;
				////else
				////break;
				break; //It will break the for ( setArmX_ctr=0) loop

			}

		}//end for ( setArmX_ctr=0



		/*
		p3d_copy_config_into(ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS);

		plan_JIDO_arm_path_to_grasp(obj_to_manipulate[i], ARM_PICK_TAKE_TO_FREE);
		pqp_print_colliding_pair();

		////curr_traj_configs= GP_GetAllTrajectoriesAsOne ( ACBTSET->robot,  no_configs );
		curr_traj_configs= GP_GetTrajectory ( ACBTSET->robot,  ACBTSET->robot->t[0], no_configs );

		printf(" <<<< no_configs %d >>>>\n",no_configs);
		j=0;
		for(j=0;j<no_configs;j++)
		 {
		 p3d_set_and_update_this_robot_conf(ACBTSET->robot,curr_traj_configs[j]);
		 g3d_draw_allwin_active();
		 }
		////p3d_get_robot_config_into(ACBTSET->robot,&rob_cur_pos);
		p3d_copy_config_into(ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &ACBTSET->robot->ROBOT_POS);
		*/
		////return 1;
		if ( put_into_success==1 )
		{
//                         int j1=0;
// 			 for(j1=0;j1< nbPositions;j1++)
//                                                 {
//                                                 manipulation->setArmQ(positions[j1].q1, positions[j1].q2, positions[j1].q3, positions[j1].q4, positions[j1].q5, positions[j1].q6);
// 						g3d_draw_allwin_active();
//                                                 }
			manipulation->releaseObject();

			/*p3d_set_and_update_this_robot_conf(ACBTSET->robot,rob_cur_pos);
			traj_play = TRUE;
			ACBTSET->robot->tcur=current_new_trajPt;
			   g3d_show_tcur_rob(ACBTSET->robot,default_drawtraj_fct);
			 */
			/*
			            if ( manipulation->grabObject ( obj_to_manipulate[i] ) !=0 )
					{
					////FORMGENOM_OBJECTGRABED = 0;
					printf ( " AKP WARNING: CAN NOT GRAB THE OBJECT\n" );
					return 0;
					}


			manipulation->releaseObject();
			            */
			p3d_set_and_update_this_robot_conf ( ACBTSET->robot,ACBTSET->robot->ROBOT_GOTO );
			p3d_get_robot_config_into ( ACBTSET->robot,&JIDO_actual_pos );

			p3d_copy_config_into ( ACBTSET->robot, JIDO_actual_pos, &ACBTSET->robot->ROBOT_POS );
			image_rate=1;
			int obj_pos_ctr=0;
                        p3d_get_robot_config_into ( envPt->robot[obj_index],&obj_tmp_pos );
			for ( obj_pos_ctr=0;obj_pos_ctr<15;obj_pos_ctr++ )
			{
				////printf(" Setting %s, as (x,y,z) =(%lf, %lf, %lf)\n", envPt->robot[obj_index]->name,obj_tmp_pos[6], obj_tmp_pos[7], obj_tmp_pos[8]);
				obj_tmp_pos[8]-=0.02;
				p3d_set_and_update_this_robot_conf ( envPt->robot[obj_index],obj_tmp_pos );
				g3d_draw_allwin_active();

				kcd_with_report=0;
				res = p3d_col_test_robot ( envPt->robot[obj_index],kcd_with_report );
				if ( res>0 )
				{
					printf ( " While falling There is collision at %d th place to put %s , res=%d \n", i1, envPt->robot[obj_index]->name, res );
					pqp_print_colliding_pair();
					////p3d_set_and_update_this_robot_conf(envPt->robot[obj_index],obj_actual_pos);
					////falling_collides=1;
					////continue;
					////break;
					//return 0;
				}

			}

			image_rate=100;
			printf ( " %d th place to putinto trashbin is final place to release %s\n", i1, envPt->robot[obj_index]->name );
			g3d_draw_allwin_active();
			////return 1;
			break;
		}
	}//End for(i1)


}


int execute_JIDO_trajectory()
{

                                        int skip=10;
                                        printf(" ***** TOTAL no. of Configs = %d \n",nbPositions);
					int j=0;
					for ( j=0;j< nbPositions;j+=skip )
					{
						manipulation->setArmQ ( positions[j].q1, positions[j].q2, positions[j].q3, positions[j].q4, positions[j].q5, positions[j].q6 );
						g3d_draw_allwin_active();
						fl_check_forms();
					}
                                        if(skip>1) //To set the last configuration
                                        {
                                        j=nbPositions-1;
                                      	manipulation->setArmQ ( positions[j].q1, positions[j].q2, positions[j].q3, positions[j].q4, positions[j].q5, positions[j].q6 );
					g3d_draw_allwin_active();
					fl_check_forms();
					}
                         configPt JIDO_actual_pos = MY_ALLOC ( double,ACBTSET->robot->nb_dof ); /* Allocation of temporary robot configuration */
                        p3d_get_robot_config_into ( ACBTSET->robot,&JIDO_actual_pos ); 
                        p3d_set_and_update_this_robot_conf ( ACBTSET->robot,ACBTSET->robot->ROBOT_GOTO );
			p3d_get_robot_config_into ( ACBTSET->robot,&JIDO_actual_pos );

			p3d_copy_config_into ( ACBTSET->robot, JIDO_actual_pos, &ACBTSET->robot->ROBOT_POS );
                          MY_FREE (JIDO_actual_pos, double,ACBTSET->robot->nb_dof );

}

int plan_JIDO_arm_path_to_grasp(char* obj_to_grasp, MANIPULATION_TASK_TYPE_STR type)
{

////UPDATE_MIGHTABILITY_MAP_INFO=0;
////SHOW_MIGHTABILITY_MAP_INFO=0;   

char support_name[50]="HRP2TABLE";
      

      double distance = 0.1;
      double pre_q1, pre_q2, pre_q3,pre_q4, pre_q5, pre_q6, q1, q2, q3, q4, q5, q6;

      if (manipulation== NULL) {
	  ////initManipulationGenom(); //AKP : This is decleared static in FORMGenom.c, so below we define our own function
          init_Manipulation_params();
      }
       ////manipulation->setObjectToManipulate((char*)OBJECT_NAME);
 
       
       manipulation->setArmCartesian(true);
#define CAMERA_JNT_NAME "Tilt"
#define CAMERA_FOV 80.0
       manipulation->setObjectToManipulate(obj_to_grasp);
//         manipulation->setCameraJnt((char*)CAMERA_JNT_NAME);
        manipulation->setCameraFOV(CAMERA_FOV);
        manipulation->setCameraImageSize(200, 200);
       ////if(manipulation->isObjectGraspable((char*)OBJECT_NAME) == false) 
       /*if(manipulation->isObjectGraspable(obj_to_grasp) == false) 
       {
	  ////std::cout << "this object is not graspable " << std::endl;
          printf(" The %s is not graspable \n",obj_to_grasp);
 	  return 0;
       }
       else
       {
         printf(" The %s is graspable \n",obj_to_grasp);
       }
      */
       ////printf(" AKP 1\n");
      // configPt qresult= NULL;
//qresult= gpFind_grasp_from_base_configuration(p3d_rob *robot, p3d_rob *object, std::list<gpGrasp> &graspList, gpArm_type arm_type, configPt qbase, gpGrasp &grasp, gpHand_properties &handProp);
    //   if(qresult==NULL)
//       configPt qPOS= NULL, qGOTO= NULL;
//      qPOS= p3d_alloc_config(ACBTSET->robot);
//      qGOTO= p3d_alloc_config(ACBTSET->robot);
// 
//   p3d_copy_config_into(ACBTSET->robot, ACBTSET->robot->ROBOT_POS, &qPOS);
//   p3d_copy_config_into(ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &qGOTO);

//   p3d_set_and_update_this_robot_conf(_robotPt, qcur);
//   p3d_get_robot_config_into(_robotPt, &_robotPt->ROBOT_POS);

/////////////// Finding different grsaps
/*
   plan_JIDO_arm_path_to_grasp(obj_to_manipulate[i], ARM_PICK_GOTO);
   pqp_print_colliding_pair();

   ////curr_traj_configs= GP_GetAllTrajectoriesAsOne ( ACBTSET->robot,  no_configs );
   curr_traj_configs= GP_GetTrajectory ( ACBTSET->robot,  ACBTSET->robot->t[0], no_configs );

   printf(" <<<< no_configs %d >>>>\n",no_configs);
   int j=0;
   for(j=0;j<no_configs;j++)
    {
    p3d_set_and_update_this_robot_conf(ACBTSET->robot,curr_traj_configs[j]);
    g3d_draw_allwin_active();
    }


std::list<gpGrasp> grasp_list;

int result= gpGet_grasp_list_gripper(obj_to_grasp, grasp_list);

 if(result==GP_ERROR)
  {  
   printf("AKP Warning : Not able to get grasp list\n");
   return 0;  
  }

 static p3d_rob *object= NULL;
  object= p3d_get_robot_by_name(obj_to_grasp);
  configPt qcur= NULL, qgrasp= NULL, qend= NULL;
  qcur= p3d_alloc_config(ACBTSET->robot);
  p3d_get_robot_config_into(ACBTSET->robot, &qcur);
  gpGrasp valid_grasp;   // the current grasp


 gpHand_properties hand_prop;
 hand_prop.initialize(grasp_list.front().hand_type);


show_all_grasps(ACBTSET->robot,object, grasp_list, GP_PA10, qcur, valid_grasp, hand_prop);

p3d_set_and_update_this_robot_conf ( ACBTSET->robot, qcur );
 
g3d_draw_allwin_active();

*/
///////FINDING PATH

       
       MANIPULATION_TASK_MESSAGE message;

      manipulation->setSupport(support_name);
      
      ////message= manipulation->armPlanTask(ARM_PICK_GOTO,manipulation->robotStart(), manipulation->robotGoto(), obj_to_grasp, lp, positions, &nbPositions);
      printf(" After manipulation->setSupport(%s);\n", support_name);
    
      message= manipulation->armPlanTask(type,manipulation->robotStart(), manipulation->robotGoto(), obj_to_grasp, lp, positions, &nbPositions);
      if(message!=MANIPULATION_TASK_OK)
      {
       printf(" ***** AKP WARNING: Can not find jido arm path \n");
       return 0;
      }  

printManipulationMessage(message);
printf("armPlanTask OK\n");
       int i=0;
//        for(i=0;i<nbPositions;i++)
//        {
//        manipulation->setArmQ(positions[i].q1, positions[i].q2, positions[i].q3, positions[i].q4, positions[i].q5, positions[i].q6);
//        
//        g3d_draw_allwin_active();
//        }

     ////  UPDATE_MIGHTABILITY_MAP_INFO=1;
   ////    SHOW_MIGHTABILITY_MAP_INFO=1;   
       ////printf(" 4\n");
      if(nbPositions>0) 
        return 1;
      else
        return 0;
 
}


int is_object_graspable()
{

UPDATE_MIGHTABILITY_MAP_INFO=0;
SHOW_MIGHTABILITY_MAP_INFO=0;   

char obj_to_grasp[50]="RED_BOTTLE";//"GREY_TAPE";//"BLACK_TAPE";//"GREY_TAPE";//"YELLOW_BOTTLE";
char support_name[50]="HRP2TABLE";
      printf(" Checking graspability of %s  \n",obj_to_grasp);

      double distance = 0.1;
      double pre_q1, pre_q2, pre_q3,pre_q4, pre_q5, pre_q6, q1, q2, q3, q4, q5, q6;

      if (manipulation== NULL) {
	  ////initManipulationGenom(); //AKP : This is decleared static in FORMGenom.c, so below we define our own function
          init_Manipulation_params();
      }
       ////manipulation->setObjectToManipulate((char*)OBJECT_NAME);
 
       
       manipulation->setArmCartesian(true);
#define CAMERA_JNT_NAME "Tilt"
#define CAMERA_FOV 80.0
       manipulation->setObjectToManipulate(obj_to_grasp);
//         manipulation->setCameraJnt((char*)CAMERA_JNT_NAME);
        manipulation->setCameraFOV(CAMERA_FOV);
        manipulation->setCameraImageSize(200, 200);
       ////if(manipulation->isObjectGraspable((char*)OBJECT_NAME) == false) 
       /*if(manipulation->isObjectGraspable(obj_to_grasp) == false) 
       {
	  ////std::cout << "this object is not graspable " << std::endl;
          printf(" The %s is not graspable \n",obj_to_grasp);
 	  return 0;
       }
       else
       {
         printf(" The %s is graspable \n",obj_to_grasp);
       }
      */
       printf(" 1\n");
      // configPt qresult= NULL;
//qresult= gpFind_grasp_from_base_configuration(p3d_rob *robot, p3d_rob *object, std::list<gpGrasp> &graspList, gpArm_type arm_type, configPt qbase, gpGrasp &grasp, gpHand_properties &handProp);
    //   if(qresult==NULL)
//       configPt qPOS= NULL, qGOTO= NULL;
//      qPOS= p3d_alloc_config(ACBTSET->robot);
//      qGOTO= p3d_alloc_config(ACBTSET->robot);
// 
//   p3d_copy_config_into(ACBTSET->robot, ACBTSET->robot->ROBOT_POS, &qPOS);
//   p3d_copy_config_into(ACBTSET->robot, ACBTSET->robot->ROBOT_GOTO, &qGOTO);

//   p3d_set_and_update_this_robot_conf(_robotPt, qcur);
//   p3d_get_robot_config_into(_robotPt, &_robotPt->ROBOT_POS);



       int res=manipulation->findPregraspAndGraspConfiguration(distance, &pre_q1, &pre_q2, &pre_q3, &pre_q4, &pre_q5, &pre_q6, &q1, &q2, &q3, &q4, &q5, &q6);
       if(res==1)
       {
        printf(" The %s is not graspable \n",obj_to_grasp);
        
 	return 0;
       }  
       else
       {
        if(res==0) //Success
        {
        printf(" The %s is graspable \n",obj_to_grasp);
        }
       }

//  p3d_set_and_update_this_robot_conf(ACBTSET->robot, qPOS);
//   p3d_copy_config_into(ACBTSET->robot, qPOS, &ACBTSET->robot->ROBOT_POS);
//   p3d_copy_config_into(ACBTSET->robot, qGOTO, &ACBTSET->robot->ROBOT_GOTO);
       ////printf(" 2\n");
       //////////manipulation->setArmQ(q1, q2, q3, q4, q5, q6);
       /////manipulation->setArmQ(pre_q1, pre_q2, pre_q3, pre_q4, pre_q5, pre_q6);
       ////printf(" 3\n");

/////////////// Finding differenr grsaps

std::list<gpGrasp> grasp_list;

int result= gpGet_grasp_list_gripper(obj_to_grasp, grasp_list);

 if(result==GP_ERROR)
  {  
   printf("AKP Warning : Not able to get grasp list\n");
   return 0;  
  }

 static p3d_rob *object= NULL;
  object= p3d_get_robot_by_name(obj_to_grasp);
  configPt qcur= NULL, qgrasp= NULL, qend= NULL;
  qcur= p3d_alloc_config(ACBTSET->robot);
  p3d_get_robot_config_into(ACBTSET->robot, &qcur);
  gpGrasp valid_grasp;   // the current grasp


 gpHand_properties hand_prop;
 hand_prop.initialize(grasp_list.front().hand_type);


show_all_grasps(ACBTSET->robot,object, grasp_list, GP_PA10, qcur, valid_grasp, hand_prop);

p3d_set_and_update_this_robot_conf ( ACBTSET->robot, qcur );
 
g3d_draw_allwin_active();

////////////// END finding different grasps


///////FINDING PATH

       int lp[10000];
       Gb_q6 positions[10000];
       int nbPositions = 0;
       MANIPULATION_TASK_MESSAGE message;

      manipulation->setSupport(support_name);
      message= manipulation->armPlanTask(ARM_PICK_GOTO,manipulation->robotStart(), manipulation->robotGoto(), obj_to_grasp, lp, positions, &nbPositions);
printManipulationMessage(message);
printf("armPlanTask OK\n");
       int i=0;
//        for(i=0;i<nbPositions;i++)
//        {
//        manipulation->setArmQ(positions[i].q1, positions[i].q2, positions[i].q3, positions[i].q4, positions[i].q5, positions[i].q6);
//        
//        g3d_draw_allwin_active();
//        }

       UPDATE_MIGHTABILITY_MAP_INFO=1;
       SHOW_MIGHTABILITY_MAP_INFO=1;   
       ////printf(" 4\n");
       return 1;

}



//! @ingroup graspPlanning
//! Finds, for a given mobile base configuration of the robot, a grasp from the given grasp list, that is
//! reachable by the arm and hand, and computes for the grasp a grasping configuration for the whole arm.
//! \param robot the robot
//! \param object the object to grasp (a freeflyer robot)
//! \param graspList a list of grasps
//! \param arm_type the robot arm type
//! \param qbase a configuration of the robot (only the part corresponding to the mobile base will be used)
//! \param grasp a copy of the grasp that has been found, in case of success
//! \param hand parameters of the hand
//! \return a pointer to the computed grasping configuration of the whole robot in case of success, NULL otherwise
configPt show_all_grasps ( p3d_rob *robot, p3d_rob *object, std::list<gpGrasp> &graspList, gpArm_type arm_type, configPt qbase, gpGrasp &grasp, gpHand_properties &hand )
{
#ifdef GP_DEBUG
	if ( robot==NULL )
	{
		printf ( "%s: %d: gpFind_robot_config_from_grasp(): robot is NULL.\n",__FILE__,__LINE__ );
		return NULL;
	}
	if ( object==NULL )
	{
		printf ( "%s: %d: gpFind_robot_config_from_grasp(): object is NULL.\n",__FILE__,__LINE__ );
		return NULL;
	}
#endif

	std::list<gpGrasp>::iterator igrasp;

	p3d_matrix4 object_frame, base_frame, inv_base_frame, gframe_object, gframe_world, gframe_robot, gframe_robot2;
	p3d_matrix4 T, Thand, Twrist;
	configPt q0= NULL; //pour memoriser la configuration courante du robot
	configPt result= NULL;


	q0= p3d_get_robot_config ( robot );

	//////////XYZ_ENV->cur_robot= robot;

	//On met à jour la configuration du robot pour que sa base soit dans la configuration
	//souhaitée:
	p3d_set_and_update_this_robot_conf ( robot, qbase );
	result= p3d_alloc_config ( robot );

	gpGet_arm_base_frame ( robot, base_frame ); //on récupère le repere de la base du bras
	p3d_matInvertXform ( base_frame, inv_base_frame );

	//pour chaque prise de la liste:
	for ( igrasp=graspList.begin(); igrasp!=graspList.end(); igrasp++ )
	{
		p3d_mat4Copy ( igrasp->frame, gframe_object );
		p3d_get_body_pose ( object, igrasp->body_index, object_frame );

		p3d_mat4Mult ( object_frame, gframe_object, gframe_world ); //passage repere objet -> repere monde
		p3d_mat4Mult ( gframe_world, hand.Tgrasp_frame_hand, Thand );
		p3d_mat4Mult ( Thand, hand.Thand_wrist, Twrist );

		p3d_mat4Mult ( inv_base_frame, gframe_world, gframe_robot ); //passage repere monde -> repere robot
		p3d_mat4Mult ( inv_base_frame, T, Twrist );

//     gpDeactivate_object_fingertips_collisions(robot, object, hand);
		switch ( arm_type )
		{
			case GP_PA10:
				p3d_mat4Mult ( gframe_robot, hand.Tgrasp_frame_hand, gframe_robot2 );
				p3d_mat4Mult ( gframe_robot2, hand.Thand_wrist, gframe_robot );

				p3d_copy_config_into ( robot, qbase, &result );

				if ( gpInverse_geometric_model_PA10 ( robot, gframe_robot, result ) ==GP_OK )
				{
#ifdef LIGHT_PLANNER
// 	   p3d_update_virtual_object_config_for_pa10_6_arm_ik_constraint(robot, result);
//            p3d_set_and_update_this_robot_conf(robot, result);
#endif
					p3d_set_and_update_this_robot_conf ( robot, result );

                                        
					gpSet_grasp_configuration ( robot, *igrasp );

                                        g3d_draw_allwin_active();

//            if(!p3d_col_test()) //if no collision
					if ( !p3d_col_test_robot_statics ( robot, 0 ) && !p3d_col_test_self_collision ( robot, 0 ) ) //if no collision
					{
						p3d_get_robot_config_into ( robot, &result );
						////igrasp->collision_state= COLLISION_FREE;
						grasp= *igrasp;

						//p3d_set_and_update_this_robot_conf ( robot, q0 );
						//p3d_destroy_config ( robot, q0 );

						////return result;
					}
				}
				break;
			default:
				printf ( "%s: %d: gpFind_grasp_from_base_configuration(): undefined or unimplemented arm type.\n",__FILE__,__LINE__ );
				p3d_set_and_update_this_robot_conf ( robot, q0 );
				p3d_destroy_config ( robot, q0 );
				return NULL;
				break;
		}

	}

	p3d_set_and_update_this_robot_conf ( robot, q0 );
	p3d_destroy_config ( robot, q0 );

	return NULL;
}

/////// END Graspability of obejct test
