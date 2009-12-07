#ifndef HRICS_PLANNER_H
#define HRICS_PLANNER_H

/*
 *  HRICS_CostSpace.h
 *  BioMove3D
 *
 *  Created by Jim Mainprice on 04/12/09.
 *  Copyright 2009 mainprice@laas.fr All rights reserved.
 *
 */

#include "../planner.hpp"
#include "../API/planningAPI.hpp"
#include "HRICS_Grid.h"
#include "HRICS_Distance.h"

class HRICS_Planner : public Planner {
	
public :
	
	HRICS_Planner();
	HRICS_Planner(Robot* rob, Graph* graph);

        ~HRICS_Planner();

        void initGrid();
        void deleteGrid();
        bool computeAStarIn3DGrid();
        HriGrid* getGrid() { return _3DGrid; }

        void initDistance();
        HRICS_Distance* getDistance() { return _Distance; }

private:
        std::vector<Robot*>     _Humans;
        HriGrid*                _3DGrid;
        HRICS_Distance*         _Distance;
	
};


extern HRICS_Planner* HRICS_MOPL;

#endif
