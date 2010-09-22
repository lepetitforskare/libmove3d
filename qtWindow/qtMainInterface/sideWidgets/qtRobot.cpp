/*
 *  qtRobot.cpp
 *  BioMove3D
 *
 *  Created by Jim Mainprice on 01/04/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "qtRobot.hpp"
#include "ui_qtRobot.h"

#include "Util-pkg.h"
#include "P3d-pkg.h"

#ifdef CXX_PLANNER
#include "util/CppApi/MultiRun.hpp"
#include "util/CppApi/SaveContext.hpp"
#include "util/CppApi/testModel.hpp"
#include "API/project.hpp"
//#include "planner_cxx/Greedy/GridCollisionChecker.h"
#endif

#ifdef HRI_GENERALIZED_IK
#include "Hri_planner-pkg.h"
#endif

#ifdef LIGHT_PLANNER
#include "lightPlannerApi.h"
#endif

using namespace std;
using namespace tr1;

RobotWidget::RobotWidget(QWidget *parent) :
QWidget(parent),
m_ui(new Ui::RobotWidget)
{
	m_ui->setupUi(this);
	
	initModel();
	//initVoxelCollisionChecker();
}

RobotWidget::~RobotWidget()
{
	delete m_ui;
}

//---------------------------------------------------------------------
// Robot
//---------------------------------------------------------------------
void RobotWidget::initRobot()
{
	m_ui->formRobot->initAllForms(m_mainWindow->getOpenGL());	
}

MoveRobot*  RobotWidget::getMoveRobot()
{
	return m_ui->formRobot;
}

//---------------------------------------------------------------------
// TEST MODEL
//---------------------------------------------------------------------
void RobotWidget::initModel()
{
	connect(m_ui->pushButtonCollision,SIGNAL(clicked()),this,SLOT(collisionsTest()));
	connect(m_ui->pushButtonLocalPath,SIGNAL(clicked()),this,SLOT(localpathsTest()));
	connect(m_ui->pushButtonCost,SIGNAL(clicked()),this,SLOT(costTest()));
	connect(m_ui->pushButtonTestAll,SIGNAL(clicked()),this,SLOT(allTests()));
	connect(m_ui->pushButtonSetObjectToCarry,SIGNAL(clicked()),this,SLOT(SetObjectToCarry()));
	
	connect(ENV.getObject(Env::numberOfCollisionPerSec),SIGNAL(valueChanged(QString)),m_ui->labelCollision,SLOT(setText(QString)));
	connect(ENV.getObject(Env::numberOfLocalPathPerSec),SIGNAL(valueChanged(QString)),m_ui->labelLocalPath,SLOT(setText(QString)));
	connect(ENV.getObject(Env::numberOfCostPerSec),SIGNAL(valueChanged(QString)),m_ui->labelTimeCost,SLOT(setText(QString)));
	
	connect(m_ui->pushButtonAttMat,SIGNAL(clicked()),this,SLOT(setAttMatrix()));
	
	QString RobotObjectToCarry("No Object");
	
	ENV.setString(Env::ObjectToCarry,RobotObjectToCarry);
	
	// Grab Object
	for(int i =0;i<XYZ_ENV->nr;i++)
	{
		if(XYZ_ENV->robot[i]->joints[1]->type == P3D_FREEFLYER )
		{
			if( XYZ_ENV->robot[i]->njoints == 1 )
			{
				QString FFname(XYZ_ENV->robot[i]->name);
				m_ui->comboBoxGrabObject->addItem(FFname);
				mFreeFlyers.push_back(FFname);
				//                cout<< " FreeFlyer = "  << XYZ_ENV->robot[i]->name << endl;
			}
		}
	}
	
	m_ui->comboBoxGrabObject->setCurrentIndex(0);
	connect(m_ui->comboBoxGrabObject, SIGNAL(currentIndexChanged(int)),this, SLOT(currentObjectChange(int))/*, Qt::DirectConnection*/);
	
	m_mainWindow->connectCheckBoxToEnv(m_ui->checkBoxIsWeightedRot,			Env::isWeightedRotation);
	m_mainWindow->connectCheckBoxToEnv(m_ui->checkBoxFKSampling,			Env::FKShoot);
	m_mainWindow->connectCheckBoxToEnv(m_ui->checkBoxFKDistance,			Env::FKDistance);
	m_mainWindow->connectCheckBoxToEnv(m_ui->checkBoxDrawBox,				Env::drawBox);
	
	new QtShiva::SpinBoxSliderConnector(
																			this, m_ui->doubleSpinBoxWeightedRot, m_ui->horizontalSliderWeightedRot , Env::RotationWeight );
	
	connect(m_ui->pushButtonGrabObject,SIGNAL(clicked()),this,SLOT(GrabObject()));
	connect(m_ui->pushButtonReleaseObject,SIGNAL(clicked()),this,SLOT(ReleaseObject()));
	
	connect(m_ui->pushButtonComputeLeftArmGIK,SIGNAL(clicked()),this,SLOT(computeHriGikLARM()));
	connect(m_ui->pushButtonComputeRightArmGIK,SIGNAL(clicked()),this,SLOT(computeHriGikRARM()));
	
	connect(m_ui->pushButtonPrintCurrentPos,SIGNAL(clicked()),this,SLOT(printCurrentPos()));
	
#if defined(LIGHT_PLANNER)
	connect(m_ui->pushButtonSwitchFKIK,SIGNAL(clicked()),this,SLOT(switchFKIK()));
#endif
	
}

void RobotWidget::costTest()
{
	if(ENV.getBool(Env::isCostSpace))
	{
#ifdef CXX_PLANNER
		TestModel tests;
		tests.nbOfCostPerSeconds();
#endif
	}
}

void RobotWidget::collisionsTest()
{
#ifdef CXX_PLANNER
	TestModel tests;
	tests.nbOfColisionsPerSeconds();
#endif
}

void RobotWidget::localpathsTest()
{
#ifdef CXX_PLANNER	
	TestModel tests;
	tests.nbOfLocalPathsPerSeconds();
#endif
}

void RobotWidget::allTests()
{
#ifdef CXX_PLANNER		
	TestModel tests;
	tests.runAllTests();
#endif
}

void RobotWidget::setAttMatrix()
{
#ifdef LIGHT_PLANNER
	p3d_rob *robotPt = (p3d_rob*) p3d_get_desc_curid(P3D_ROBOT);
	//  p3d_compute_attached_matrix_from_virt_obj(robotPt->ccCntrts[0]);
	for(int i = 0; i < robotPt->nbCcCntrts; i++)
	{
		//p3d_compute_Tatt(robotPt->ccCntrts[i]);
		
		cout << "Tatt = " << endl;
		for (i = 0; i < 4; i++)
		{
			cout << robotPt->ccCntrts[0]->Tatt[i][0] << " " <<
			" " << robotPt->ccCntrts[0]->Tatt[i][1] <<
			" " << robotPt->ccCntrts[0]->Tatt[i][2] <<
			" " << robotPt->ccCntrts[0]->Tatt[i][3] << endl;
		}
		cout << endl;
	}
#endif
}

#if defined (HRI_GENERALIZED_IK)
void RobotWidget::computeHriGik(bool leftArm)
{	
	int i=0;
	for(i=0; i<XYZ_ENV->nr; i++){
		if( strcasestr(XYZ_ENV->robot[i]->name,"VISBALL") )
			break;
	}
	if(i==XYZ_ENV->nr){
		printf("No visball in the environment\n");
		return;
	}
	
	// 1 - Select Goto point
	p3d_vector3 Tcoord;
	
	Tcoord[0] = XYZ_ENV->robot[i]->joints[1]->abs_pos[0][3];
	Tcoord[1] = XYZ_ENV->robot[i]->joints[1]->abs_pos[1][3];
	Tcoord[2] = XYZ_ENV->robot[i]->joints[1]->abs_pos[2][3];
	
	
	// 2 - Select Task
	HRI_GIK_TASK_TYPE task;
	
	if (leftArm == true) 
	{
		task = GIK_LAREACH; // Left Arm GIK
	}
	else 
	{
		task = GIK_RAREACH; // Left Arm GIK
	}
	
	// 3 - Select Agent
	HRI_AGENTS * agents = hri_create_agents();
	
	configPt q;
	
	double distance_tolerance = 1.0;
	
	if(	agents->humans_no > 0 ) // Humans
	{
		q = p3d_get_robot_config(agents->humans[0]->robotPt);
		hri_agent_single_task_manip_move(agents->humans[0], task, &Tcoord, distance_tolerance, &q);
		p3d_set_and_update_this_robot_conf(agents->humans[0]->robotPt,q);
	}
	else if ( agents->robots_no > 0) // Robots
	{
		q = p3d_get_robot_config(agents->robots[0]->robotPt);
		hri_agent_single_task_manip_move(agents->robots[0], task, &Tcoord, distance_tolerance, &q);
		p3d_set_and_update_this_robot_conf(agents->robots[0]->robotPt,q);
	}
	else {
		cout << "Warning: No Agent for GIK" << endl;
	}
	
	
	//delete_config(robotPt,q);	
	m_mainWindow->drawAllWinActive();
}
#endif

void RobotWidget::currentObjectChange(int i)
{
	if((mFreeFlyers.size() > 0) && (i != 0))
	{
		
		//        cout << "Env::ObjectToCarry  is "<< mFreeFlyers[i-1] << endl;
		ENV.setString(Env::ObjectToCarry,mFreeFlyers[i-1]);
	}
}

void RobotWidget::SetObjectToCarry()
{
#ifdef LIGHT_PLANNER
	if(mFreeFlyers.size() > 0)
	{
		p3d_rob *robotPt = (p3d_rob*) p3d_get_desc_curid(P3D_ROBOT);
		p3d_set_object_to_carry(robotPt,ENV.getString(Env::ObjectToCarry).toStdString().c_str());
		
		// Set the dist of the object to the radius of the carried object
		robotPt->curObjectJnt->dist = robotPt->carriedObject->joints[1]->dist;
		
		double radius = 1.5;
		//take only x and y composantes of the base
		double dof[2][2];
		for(int i = 0; i < 2; i++){
			dof[i][0] = p3d_jnt_get_dof(robotPt->joints[1], i) - radius;
			dof[i][1] = p3d_jnt_get_dof(robotPt->joints[1], i) + radius;
		}
		for(int i = 0; i < 2; i++){
			p3d_jnt_set_dof_rand_bounds(robotPt->curObjectJnt, i, dof[i][0], dof[i][1]);
		}
		
	}
	else
	{
		p3d_rob *robotPt = (p3d_rob*) p3d_get_desc_curid(P3D_ROBOT);
		// Set the dist of the object to the radius of the carried object
		
		cout << "Setting Dist of Joint : " << robotPt->joints[6]->name << endl;
		cout << "To Joint : "  << robotPt->joints[7]->name << endl;
		
		cout << robotPt->joints[7]->dist << "  Takes "  << robotPt->joints[6]->dist << endl;
		
		robotPt->joints[7]->dist = robotPt->joints[6]->dist;
	}
#endif
}

void RobotWidget::GrabObject()
{
	
#if defined( LIGHT_PLANNER ) && defined( PQP )
	p3d_rob *robotPt = (p3d_rob*) p3d_get_desc_curid(P3D_ROBOT);
	cout << "Robot = " << robotPt->name <<  endl;
	p3d_set_object_to_carry(robotPt,ENV.getString(Env::ObjectToCarry).toStdString().c_str());
	p3d_grab_object2(robotPt,0);
	
	//    if(mFreeFlyers.size() > 0)
	//    {
	//        p3d_rob *robotPt = (p3d_rob*) p3d_get_desc_curid(P3D_ROBOT);
	//		//        p3d_rob *carriedObject;
	//		
	//        p3d_set_object_to_carry(robotPt,ENV.getString(Env::ObjectToCarry).toStdString().c_str());
	//		//        p3d_matrix4 saved;
	//		//        p3d_mat4Copy(robotPt->curObjectJnt->abs_pos,saved);
	//        p3d_mat4Copy(robotPt->carriedObject->joints[1]->abs_pos,robotPt->curObjectJnt->abs_pos);
	//        p3d_grab_object(robotPt,0);
	//		//        p3d_mat4Copy(saved,robotPt->curObjectJnt->abs_pos);
	//		//        configPt q = p3d_get_robot_config(robotPt);
	//		
	//		//        robotPt->ROBOT_POS = q;
	//		//        p3d_set_and_update_robot_conf(q);
	//        p3d_mat4Print(robotPt->ccCntrts[0]->Tatt,"curObject Grab");
	//    }
#endif
}

void RobotWidget::ReleaseObject()
{
	p3d_rob *robotPt = (p3d_rob*) p3d_get_desc_curid(P3D_ROBOT);
	cout << "Robot = " << robotPt->name <<  endl;
	/*
	 p3d_release_object(robotPt);
	 deactivateCcCntrts(robotPt, -1);
	 //	configPt qi = p3d_alloc_config(robotPt);
	 //	p3d_copy_config_into(robotPt, _robotPt->ROBOT_POS, &qi);
	 x
	 //p3d_update_virtual_object_config_for_pa10_6_arm_ik_constraint(_robotPt, qi);
	 double tx, ty, tz, ax, ay, az;
	 p3d_mat4ExtractPosReverseOrder2(robotPt->CurObjtJnt->abs_pos, &tx, &ty, &tz, &ax, &ay, &az);
	 
	 //	p3d_set_and_update_this_robot_conf(robotPt, qi);
	 //	p3d_destroy_config(_robotPt, qi);
	 
	 qi = p3d_get_robot_config(robotPt);
	 p3d_copy_config_into(robotPt, qi, &_robotPt->ROBOT_POS);
	 p3d_destroy_config(_robotPt, qi);
	 m_ui->mainWindow->drawAllWindowActive();
	 */
	
#if defined ( LIGHT_PLANNER ) && defined( PQP )
	//    m_ui->comboBoxGrabObject-
	//    p3d_rob *robotPt = (p3d_rob*) p3d_get_desc_curid(P3D_ROBOT);
	p3d_release_object(robotPt);
	//    m_ui->comboBoxGrabObject->setCurrentIndex(0);
#endif
};

void RobotWidget::printCurrentPos()
{
	Robot currRobot((p3d_rob*) p3d_get_desc_curid(P3D_ROBOT));
	shared_ptr<Configuration> q = currRobot.getCurrentPos();
	q->print(true );
}

#ifdef LIGHT_PLANNER
void RobotWidget::switchFKIK()
{
	cout << "Switching FK to IK" << endl;
	
	Robot* ptrRob = global_Project->getActiveScene()->getActiveRobot();
	
	if ( ptrRob->isActiveCcConstraint() ) 
	{
		ptrRob->deactivateCcConstraint();
	}
	else 
	{
		ptrRob->activateCcConstraint();
	}
	
	getMoveRobot()->setRobotConstraintedDof(ptrRob);
	
}
#endif

/*void RobotWidget::initVoxelCollisionChecker()
 {
 m_mainWindow->connectCheckBoxToEnv(m_ui->checkBoxDrawVoxelGrid,				Env::drawGrid);
 
 connect(m_ui->pushButtonCreateVoxelCollisionChecker,SIGNAL(clicked()),this,SLOT(createVoxelCC()));
 connect(m_ui->pushButtonDeleteVoxelCollisionChecker,SIGNAL(clicked()),this,SLOT(deleteVoxelCC()));
 
 connect(m_ui->pushButtonVoxelCC,SIGNAL(clicked()),this,SLOT(voxelCCTest()));
 connect(ENV.getObject(Env::numberOfCollisionPerSec),SIGNAL(valueChanged(QString)),m_ui->labelVoxelTime,SLOT(setText(QString)));
 
 connect(m_ui->spinBoxVoxelCCnbCells,SIGNAL(valueChanged(int)),ENV.getObject(Env::nbCells),SLOT(set(int)));
 connect(ENV.getObject(Env::nbCells),SIGNAL(valueChanged(int)),m_ui->spinBoxVoxelCCnbCells,SLOT(setValue(int)));
 }
 
 void RobotWidget::createVoxelCC()
 {
 API_activeGrid = global_GridCollisionChecker = new GridCollisionChecker;
 ENV.setBool(Env::drawGrid, true);
 }
 
 void RobotWidget::deleteVoxelCC()
 {
 ENV.setBool(Env::drawAll,false);
 delete API_activeGrid;
 API_activeGrid = NULL;
 }
 
 void RobotWidget::voxelCCTest()
 {
 #ifdef CXX_PLANNER
 TestModel tests;
 tests.nbOfVoxelCCPerSeconds();
 #endif
 }*/
