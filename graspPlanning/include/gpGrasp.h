/*
 * Copyright (c) 2001-2014 LAAS/CNRS
 * All rights reserved.
 *
 * Redistribution  and  use  in  source  and binary  forms,  with  or  without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice and this list of conditions.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice and  this list of  conditions in the  documentation and/or
 *      other materials provided with the distribution.
 *
 * THE SOFTWARE  IS PROVIDED "AS IS"  AND THE AUTHOR  DISCLAIMS ALL WARRANTIES
 * WITH  REGARD   TO  THIS  SOFTWARE  INCLUDING  ALL   IMPLIED  WARRANTIES  OF
 * MERCHANTABILITY AND  FITNESS.  IN NO EVENT  SHALL THE AUTHOR  BE LIABLE FOR
 * ANY  SPECIAL, DIRECT,  INDIRECT, OR  CONSEQUENTIAL DAMAGES  OR  ANY DAMAGES
 * WHATSOEVER  RESULTING FROM  LOSS OF  USE, DATA  OR PROFITS,  WHETHER  IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR  OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.                                  
 *
 * Siméon, T., Laumond, J. P., & Lamiraux, F. (2001). 
 * Move3d: A generic platform for path planning. In in 4th Int. Symp.
 * on Assembly and Task Planning.
 */
#ifndef GP_GRASP_H
#define GP_GRASP_H

#include "../graspPlanning/include/graspPlanning.h"
#include "../graspPlanning/include/gpContact.h"
#include "../graspPlanning/include/gpPlacement.h"

// #include "GraspPlanning-pkg.h"


//! @ingroup graspPlanning 
//! This is the class used to describe all the characteristics of a grasp.
class gpGrasp
{
 public:
  bool autoGen; /*!< tells if the grasp was generated automatically or by a user */
  int ID;  /*!< ID number */
  double stability;   /*!< stability score of the grasp (unstable if < 0) */
  
  //!different scores that are used to compute the grasp quality:
  double curvatureScore; /*!< score based on object's surface curvature at contact points */
  double centroidScore; /*!< inverse of the distance from the contact centroid to object's center of mass */

  double IKscore; /*!< IK score of the grasp (optional) */
  double visibility; /*!< visibility score of the grasp (optional) */
  double quality;   /*!< overall quality score of the grasp */
  p3d_matrix4 frame;  /*!< grasp frame */
  std::vector<gpContact> contacts; /*!< vector of contacts of the grasp */
  int handID; /*!< in case there are several hand, this stores the hand used by the grasp. If there is one hand= 0, two hands= 0 and 1 */
  p3d_rob *object;  /*!< the grasped object */
  std::string object_name;  /*!< name of the grasped object */
  double finger_opening;  /*!< gripper opening (distance between the jaws)
                          corresponding to the grasp (for GP_GRIPPER hand) */
  enum gpHand_type hand_type;  /*!< type of the hand realizing the grasp */
  std::vector<double> config; /*!< configuration vector of the hand for the associated grasp */
  std::vector<double> openConfig; /*!< configuration vector of the hand slightly open from its grasp configuration (is used for the hand approach phase) */
  bool tested; /*!< used to mark the grasps that have been tested in some path planning function  */
  
  gpPlacement recomPlacement; /*!< the best way to place the object on a plane support for the current grasp: recommended placement */

  gpGrasp();
  gpGrasp(const gpGrasp &grasp);
  gpGrasp(const gpHand_properties &handProp);
  ~gpGrasp();
  gpGrasp & operator = (const gpGrasp &grasp);
  bool operator == (const gpGrasp &grasp);
  int print();
  int printInFile(const char *filename);
  int draw(double cone_length, int cone_nb_slices= 10);
  int computeStability();
  int computeQuality();
  double configCost();
  int removeContactsTooCloseToEdge(double angleThreshold, double distancethreshold);
  friend double gpGraspDistance(const gpGrasp &grasp1, const gpGrasp &grasp2);
  int contactCentroid(p3d_vector3 centroid);
  int direction(p3d_vector3 direction) const;
  double similarity(const gpGrasp &grasp);
  int computeOpenConfig(p3d_rob *robot, p3d_rob *object, bool environment);
};

bool gpCompareGraspQuality(const gpGrasp &grasp1, const gpGrasp &grasp2); 
bool gpReversedCompareGraspQuality(const gpGrasp &grasp1, const gpGrasp &grasp2); 
bool gpCompareGraspVisibility(const gpGrasp &grasp1, const gpGrasp &grasp2); 
bool gpCompareGraspNumberOfContacts(const gpGrasp &grasp1, const gpGrasp &grasp2); 

//! @ingroup graspPlanning
//! This class is used to describe all the characteristics of a double grasp
//! (the object is grasped simultaneously with the two hands).
class gpDoubleGrasp
{
  public:
  int ID;  /*!< ID number */
  gpGrasp grasp1, grasp2;  /*!< the grasps of each hand */
  double  distanceScore; /*!< distance score between the two hands */

  double stability;   /*!< stability score of the double grasp */
  double quality;   /*!< quality score of the double grasp */

  gpDoubleGrasp();
  gpDoubleGrasp(const gpGrasp &graspA, const gpGrasp &graspB);
  gpDoubleGrasp(const gpDoubleGrasp &dgrasp);
  ~gpDoubleGrasp();
  int setFromSingleGrasps(const gpGrasp &graspA, const gpGrasp &graspB);
  gpDoubleGrasp & operator = (const gpDoubleGrasp &dgrasp);
  int print();
  int draw(double cone_length, int cone_nb_slices= 10);
  int computeStability();
  int computeQuality();
  int computeBestObjectOrientation(p3d_matrix4 torsoPose, p3d_matrix4 objectPose);
};

inline bool operator < (const gpDoubleGrasp &grasp1, const gpDoubleGrasp &grasp2)
{
//    if(this==NULL)
//    {
//        printf("%s: %d: gpDoubleGrasp::operator <: the calling instance is NULL.\n",__FILE__,__LINE__);
//        return GP_ERROR;
//    }
    
    return (grasp1.quality < grasp2.quality) ? true : false;
}

int gpNormalize_distance_score(std::list<gpDoubleGrasp> &list);
int gpNormalize_stability(std::list<gpDoubleGrasp> &list);


#endif

