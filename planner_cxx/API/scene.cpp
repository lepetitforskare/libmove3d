//
// C++ Implementation: environnement
//
// Description: 
//
//
// Author: Florian Pilardeau,B90,6349 <fpilarde@jolimont>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "scene.h"

#include "P3d-pkg.h"

using namespace std;

Scene::Scene(p3d_env* environnement)
{
	m_Scene = environnement;
	
	m_Name = m_Scene->name;
	
	for (int i=0; i<m_Scene->nr; i++) 
	{
		cout << "Add new Robot to Scene : " << m_Scene->robot[i]->name << endl;
		m_Robot.push_back( new Robot( m_Scene->robot[i] ) );
	}
}


Scene::~Scene()
{
}

string Scene::getName()
{
  return m_Name;
}

void Scene::setActiveRobot(string name)
{
	unsigned int id = getRobotId(name);
	p3d_sel_desc_id(P3D_ROBOT,m_Scene->robot[id]);
}

Robot* Scene::getActiveRobot()
{
	return getRobotByName(m_Scene->cur_robot->name);
}

unsigned int Scene::getRobotId(string str)
{
	for (unsigned int i=0; i<m_Robot.size(); i++) 
	{
		if ( m_Robot[i]->getName().compare( str ) == 0 ) 
		{
			return i;
		}
	}
	
	cout << "Error geting robot id in " << __func__ << endl;
	return 0;
}

/**
 * Get robot by name
 */
Robot* Scene::getRobotByName(string str)
{
	unsigned int id = getRobotId(str);
	
	if (id < m_Robot.size() ) 
	{
		return m_Robot[id];
	}

	return 0x00;
}

/**
 * Get robot by name containing
 */
Robot* Scene::getRobotByNameContaining(string str)
{
	for (unsigned int i=0; i<m_Robot.size(); i++) 
	{
		if ( m_Robot[i]->getName().find( str ) != string::npos ) 
		{
			return m_Robot[i];
		}
	}

	return 0x00;
}

void Scene::insertRobot(Robot* R)
{
  m_Robot.push_back(R);
}


