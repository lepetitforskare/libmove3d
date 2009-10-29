/*
 * trajectory.hpp
 *
 *  Created on: Jun 17, 2009
 *      Author: jmainpri
 */

#ifndef TRAJECTORY_HPP_
#define TRAJECTORY_HPP_

#include "../planningAPI.hpp"

/**
 * @ingroup Trajectory
 * @brief Trajectory witch is a vector of local paths
 */

class Trajectory {

public:

	//---------------------------------------------------------
	// Constructors
	Trajectory();
	Trajectory(Robot* R);
	Trajectory(const Trajectory& T);
	Trajectory(Robot* R,p3d_traj* t);
	Trajectory(std::vector< std::tr1::shared_ptr<Configuration> > C);
	~Trajectory();

	Trajectory& operator= (const Trajectory& f);

	//---------------------------------------------------------
	// Operations
	std::vector< std::tr1::shared_ptr<Configuration> > getTowConfigurationAtParam(
			double param1, double param2 , uint& lp1, uint& lp2);

	std::vector<LocalPath*> extractSubPortion(
			double param1,
			double param2,
			uint& first,
			uint& last);

	Trajectory extractSubTrajectory(
			double param1,
			double param2);

	void replacePortion(
			uint id1,
			uint id2,
			std::vector<LocalPath*> paths);

	void replacePortion(
			double param1,
			double param2,
			std::vector<LocalPath*> paths);

	void cutTrajInSmallLP(unsigned int nLP);
	uint cutPortionInSmallLP(std::vector<LocalPath*>& portion, uint nLP);

	void push_back(std::tr1::shared_ptr<Configuration> q);

	//---------------------------------------------------------
	// Cost
	double computeSubPortionIntergralCost(std::vector<LocalPath*> portion);
	double computeSubPortionCost(std::vector<LocalPath*> portion);
	double computeSubPortionCostVisib( std::vector<LocalPath*> portion );
	double costOfPortion(double param1,double param2);
	double extractCostPortion(double param1, double param2);
	double cost();


	//---------------------------------------------------------
	// Basic
	std::tr1::shared_ptr<Configuration> configAtParam(double param);

	std::vector< std::tr1::shared_ptr<Configuration> > getNConfAtParam(double delta);

	uint 			getIdOfPathAt(double param);
	LocalPath* 		getLocalPathPtrAt(uint id);
	int				getNbPaths();

	bool getValid();

	void 	updateRange();
	double computeSubPortionRange(std::vector<LocalPath*> portion);

	void 	replaceP3dTraj();
	void 	replaceP3dTraj(p3d_traj* trajPt);

	void drawGL(int nbKeyFrame);
	void print();

	//---------------------------------------------------------
	// Getters & Setters

	void setColor(int col) {color=col;}

	uint getHighestCostId(){
		return HighestCostId;
	}

	Robot* getRobot(){
		return mRobot;
	}

	double getRangeMax(){
		return range_param;
	}

	std::tr1::shared_ptr<Configuration> getBegin(){
		return mBegin;
	}

	std::tr1::shared_ptr<Configuration> getEnd(){
		return mEnd;
	}


	//---------------------------------------------------------
	// Members
protected:
	uint HighestCostId;
	bool isHighestCostIdSet;

private:

	std::vector<LocalPath*> 	mCourbe;

	/* name of trajectory */
	std::string name;

	/* Name of the file */
	std::string file;

	/* Robot */
	Robot*		mRobot;

	/* Number of localpath */
	uint		nloc;

	int color;

	/* Maximum range of parameter along the trajectory (length)*/
	double    	range_param;

	/* Start and Goal (should never change) */
	std::tr1::shared_ptr<Configuration> mBegin;
	std::tr1::shared_ptr<Configuration> mEnd;
};

#endif /* TRAJECTORY_HPP_ */
