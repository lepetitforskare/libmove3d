#ifndef ROBOT_HPP
#define ROBOT_HPP

#include <Eigen/Core>
#define EIGEN_USE_NEW_STDVECTOR
#include <Eigen/StdVector>
#include <Eigen/Geometry> 

#include "Device/joint.h"
#include "ConfigSpace/configuration.hpp"

#ifndef _DEVICE_H
typedef struct rob;
typedef struct jnt;
#endif

#ifndef _TRAJ_H
typedef struct traj;
#endif

/**
  * @ingroup CPP_API
  * @defgroup ROBOT Device
  * @brief The robot class
  */

/*!
 @ingroup ROBOT
 @brief This class holds a the robot 
 represented by a kinematic chain
 */
class Robot{

public:
  //constructor and destructor
    /**
     * Constructeur de la classe
     * @param R le p3d_rob pour lequel l'objet Robot est créé
     */
    Robot(rob* R , bool copy = false );

    /**
     * Destructeur de la classe
     */
    ~Robot();

    rob* copyRobotStruct(rob* robotPt);

  //Accessor
    /**
     * obtient la structure p3d_rob de la classe
     * @return la structure p3d_rob
     */
    rob* getRobotStruct();

    /**
     * obtient le nom du Robot
     * @return le nom du Robot
     */
    std::string getName();
	
	/**
     * Gets traj associated with Robot
     * @return pointer to structure p3d_traj
     */
    traj* getTrajStruct();
	
	/**
	 * Get the number of Joints
	 * @return the Number of Joints
	 */
	unsigned int getNumberOfJoints();
	
	/**
	 * Gets the ith joint structure
	 * @return ith joint structure
	 */
	Joint* getJoint(unsigned int i);
	
	/**
	 * Returns the Object
	 * Box
	 */
	std::vector<Eigen::Vector3d> getObjectBox();
	
	/**
	 * Initializes the box in which the FF 
	 * Will be sampled
	 */
	void initObjectBox();
	
    /**
     * tire une Configuration aléatoire pour le Robot
     * @param samplePassive (default = TRUE) indique si l'on tire les joints passif ou non (ie. FALSE dans le cas de ML-RRT)
     * @return la Configuration tirée
     */
    std::tr1::shared_ptr<Configuration> shoot(bool samplePassive = false);
    /**
     * obtient une Configuration-Direction aléatoire pour le Robot
     * @param samplePassive (default = true) indique si l'on tire les joints passif ou non (ie. FALSE dans le cas de ML-RRT)
     * @return la Configuration tirée
     */
    std::tr1::shared_ptr<Configuration> shootDir(bool samplePassive = false);

    /**
      * shoots the active free flyer inside a box
      */
    std::tr1::shared_ptr<Configuration> shootFreeFlyer(double* box);

	
    /**
     * place le Robot dans une Configuration
     * @param q la Configuration dans laquelle le Robot sera placé
     * @return la Configuration est atteignable cinématiquement
     */
    int setAndUpdate(Configuration& q);
	
	/**
     * place le Robot dans une Configuration
     * @param q la Configuration dans laquelle le Robot sera placé
     * @return la Configuration est atteignable cinématiquement
     */
	bool setAndUpdateMultiSol(Configuration& q);

    /**
     * place le Robot dans une Configuration, without checking the cinematic constraints.
     * @param q la Configuration dans laquelle le Robot sera placé
     */
     void setAndUpdateWithoutConstraints(Configuration& q);

    /**
     * obtient la Configuration current du Robot
     * @return la Configuration current du Robot
     */
    std::tr1::shared_ptr<Configuration> getInitialPosition();
	
	/**
	 * Sets the Initial Position of the Robot
	 */
	void setInitialPosition(Configuration& conf);
	
    /**
     * obtient la Configuration GoTo du Robot
     * @return la Configuration GoTo du Robot
     */
    std::tr1::shared_ptr<Configuration> getGoTo();
	
	/**
	 * Sets the Goto Position of the Robot
	 */
	void setGoTo(Configuration& conf);

    /**
     * Returns the Robot current Configuration
     */
    std::tr1::shared_ptr<Configuration> getCurrentPos();
	
    /**
      * Returns a new configuration
      */
    std::tr1::shared_ptr<Configuration> getNewConfig();

	/**
	 * Get the Robot joint AbsPos
	 */
	Eigen::Matrix4d getJointAbsPos(int id);
	
    /**
     * Get the Robot joint Position
     */
    Eigen::Vector3d getJointPos(int id);

#ifdef LIGHT_PLANNER
    /**
      * Returns the Virtual object dof
      */
    int getObjectDof();
	
	/**
	 * Activate Constraint
	 */
	void activateCcConstraint();
	
	/**
	 * Deactivate Constraint
	 */
	void deactivateCcConstraint();
	
	/**
	 * Returns the base Joint
 	 */
	jnt* getBaseJnt();
	
	/**
	 * Shoots the base Joint of the robot
	 */
	std::tr1::shared_ptr<Configuration> shootBase();
	
	/**
	 * Shoots the base Joint of the robot
	 */
	std::tr1::shared_ptr<Configuration> shootAllExceptBase();
	
	/**
	 * Update but not base
	 */ 
	bool setAndUpdateAllExceptBase(Configuration& Conf);
	
	/**
	 * shoots the robot by shooting the 
	 * FreeFlyer inside the accecibility box
	 */
	void shootObjectJoint(Configuration& Conf);
#endif

private:
    rob* _Robot; /*!< une structure de p3d_rob contenant les données sur le Robot*/
    std::string _Name; /*!< le nom du Robot*/
	bool _copy; /*!< Is true if the p3d_jnt copies and not only points to the structure */
	
	std::vector<Joint*> m_Joints;
	
	Eigen::Vector3d m_ObjectBoxCenter;
	Eigen::Vector3d m_ObjectBoxDimentions;
};

extern Robot* API_activeRobot;

#endif
