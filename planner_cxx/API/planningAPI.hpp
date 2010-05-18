//#ifndef PLANNING_API_CPP_
//#define PLANNING_API_CPP_

/**
 * C++ basic headers (they all have name spaces)
 */
#include <iostream>
#include <iomanip>
#include <iosfwd>
#include <sstream>
#include <fstream>
#include <string>
//#include <vector>
#include <set>
#include <map>
#include <list>
#include <utility>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <tr1/memory>

#include "P3d-pkg.h"

/**
 * Environment has to be included before anything (weird)
 */
#include "../p3d/env.hpp"

/**
 * The CPP API so that
 * Robot is first and Graph is last (kind of tricky because its backwards)
 */

#include <Eigen/Core>
#define EIGEN_USE_NEW_STDVECTOR
#include <Eigen/StdVector>
#include <Eigen/Geometry> 

#include "Search/AStar/AStar.h"
#include "Search/Dijkstra/dijkstra.hpp"

//#include "Trajectory/CostOptimization.hpp"
//#include "Trajectory/Smoothing.hpp"
#include "Trajectory/trajectory.hpp"

#include "Roadmap/graph.hpp"
#include "Roadmap2/graph2.hpp"
#include "Roadmap/compco.h"
#include "Roadmap/edge.hpp"
#include "Roadmap/node.hpp"
#include "ConfigSpace/localPathValidTest.h"
#include "ConfigSpace/localpath.hpp"
#include "ConfigSpace/configuration.hpp"
#include "Device/robot.hpp" 
#include "Device/joint.h" 

#include "scene.h"
#include "project.h"

//#endif

