#include "Planner-pkg.h"
#include "Bio-pkg.h"
#include "P3d-pkg.h"

/**
 * p3d_SetDistConfigChoice 
 * Set the current choice of metric used to compute 
 * the distance between
 * two configurations   
 * @param[In] DistConfChoice: the selected choice of 
 * metric to compute the distance.
 */
void p3d_SetDistConfigChoice(int DistConfChoice)
{
	ENV.setInt(Env::DistConfigChoice,DistConfChoice);
}

/**
 * p3d_GetDisConfigChoice 
 * Get the current metric used to compute 
 * the distance between two configurations   
 * @return:  the current metric used to compute 
 * the distance.
 */
int p3d_GetDistConfigChoice(void)
{
  return ENV.getInt(Env::DistConfigChoice);
}

/**
 * SelectedDistConfig
 * Compute the distance between two robot configurations using the
 * current metric defined by  p3d_SetDistConfigChoice
 * @param[in] robotPt: a pointer to the current robot
 * @param[in] Config1: the first configuration
 * @param[in] Config2: the second configuration
 * @return: the distance between the two configurations,
 * and -1. if the computation of the ditances failed
 */
double SelectedDistConfig(p3d_rob* robotPt, configPt Config1, 
			  configPt Config2) {
  double DistConf = P3D_HUGE;

 switch(p3d_GetDistConfigChoice()) {
 case GENERAL_CSPACE_DIST:
   DistConf = p3d_dist_config(robotPt,Config1, Config2);
   break;
 case ACTIVE_CONFIG_DIST:
   DistConf = p3d_ActiveDistConfig(robotPt,Config1, Config2);
   break;
 case LIGAND_PROTEIN_DIST:
#ifdef BIO
   DistConf = bio_compute_ligand_dist(robotPt,Config1, Config2);
#else
   printf("Error: NO BIO Module\n");
#endif
   break;
 case MOBILE_FRAME_DIST:
   PrintInfo(("Warning:  the MOBILE_FRAME_DIST can't be \
directly returned from the configurations"));
   // DistConf = hrm_mob_frame_dist(robotPt, mob_frame_ref,ListNode->N->rel_mob_frame);
   break;
 default:
   /* The default configuration distance used is the
      general Cspace distance method */
   DistConf = p3d_dist_config(robotPt,Config1, Config2);
 }
  return DistConf;
}
