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
#include "P3d-pkg.h"

/*
 * Local computation constants and macros
 */
#define LOCAL_COMPUTATION_EPSILON (1e-9)
/* #define MAX(a,b) (((a) > (b)) ? (a) : (b)) */
/* #define MIN(a,b) (((a) < (b)) ? (a) : (b)) */
/* #define SIGN(x) (((x) < 0 ) ? (-1) : (1)) */
/* #define ABS(x) (((x) > 0 ) ? (x) : -(x)) */
/* #define SQR(x) ((x) * (x)) */
/* #define CUBE(x) (SQR(x) * (x)) */


/*
 * Compute atan2
 */

static double 
arm_atan2(double y, double x)
{
  return ((x == 0.) ? ((y == 0. ) ? 0. : SIGN(y) * M_PI / 2.) : atan2(y , x));
}



/*******************************************************************************/

int compute_all_ik_R6_arm(double **q_list, p3d_matrix4 Tgrip, p3d_matrix4 Tbase, double a2, double r4, double r7){
  int no_sol;

  no_sol = TRUE;//There is no sol

  //Sol1
  if(!compute_inverse_kinematics_R6_arm(q_list[0],Tgrip,Tbase,a2,r4,r7,1,1,1)){
    MY_FREE(q_list[0], double, 6);
    q_list[0] = NULL;     /* forward-up-flip */
  }else{
    no_sol = FALSE;//There a sol
  }
  //Sol2
  if(!compute_inverse_kinematics_R6_arm(q_list[1],Tgrip,Tbase,a2,r4,r7,1,1,-1)){
    MY_FREE(q_list[1], double, 6);
    q_list[1] = NULL;     /* forward-up-flop */
  }else{
    no_sol = FALSE;//There a sol
  }
  //Sol3
  if(!compute_inverse_kinematics_R6_arm(q_list[2],Tgrip,Tbase,a2,r4,r7,1,-1,1)){
    MY_FREE(q_list[2], double, 6);
    q_list[2] = NULL;     /* forward-down-flip */
  }else{
    no_sol = FALSE;//There a sol
  }
  //Sol4
  if(!compute_inverse_kinematics_R6_arm(q_list[3],Tgrip,Tbase,a2,r4,r7,1,-1,-1)){
    MY_FREE(q_list[3], double, 6);
    q_list[3] = NULL;     /* forward-down-flop */
  }else{
    no_sol = FALSE;//There a sol
  }
  //Sol5
  if(!compute_inverse_kinematics_R6_arm(q_list[4],Tgrip,Tbase,a2,r4,r7,-1,1,1)){
    MY_FREE(q_list[4], double, 6);
    q_list[4] = NULL;     /* backward-up-flip */
  }else{
    no_sol = FALSE;//There a sol
  }
  //Sol6
  if(!compute_inverse_kinematics_R6_arm(q_list[5],Tgrip,Tbase,a2,r4,r7,-1,1,-1)){
    MY_FREE(q_list[5], double, 6);
    q_list[5] = NULL;     /* backward-up-flop */
  }else{
    no_sol = FALSE;//There a sol
  }
  //Sol7
  if(!compute_inverse_kinematics_R6_arm(q_list[6],Tgrip,Tbase,a2,r4,r7,-1,-1,1)){
    MY_FREE(q_list[6], double, 6);
    q_list[6] = NULL;     /* backward-down-flip */
  }else{
    no_sol = FALSE;//There a sol
  }
  //Sol8
  if(!compute_inverse_kinematics_R6_arm(q_list[7],Tgrip,Tbase,a2,r4,r7,-1,-1,-1)){
    MY_FREE(q_list[7], double, 6);
    q_list[7] = NULL;     /* backward-down-flop */
  }else{
    no_sol = FALSE;//There a sol
  }
  return(!no_sol);
}



/*
 * Compute inverse kinematics of 6R arm
 *
 * ( function adapted from /usr/local/robots/gt6a/kinematics/ )
 *
 * Output: q
 * Inputs: grip, kinematic_params, e1, e3, e5...
 *
 */

int compute_inverse_kinematics_R6_arm (double *q,
				       p3d_matrix4 Tgrip,  // wTg
				       p3d_matrix4 Tbase,  // wTb
				       // p3d_R6_arm_kin_param *kinematic_params,
				       double a2, double r4, double r7,
				       int e1, int e3, int e5)			    
{
  p3d_matrix4 bTg,Tinv;
  p3d_vector3 v,vxgrip,vygrip,vzgrip; 
  p3d_vector3 posgrip;
  double d[12];
  double c1, c2, c3, c4, c5, c6, c23;
  double s1, s2, s3, s4, s5, s6, s23;
  double px, py, pz;
/*   double r4 = kinematic_params->r[4 - 1]; */
/*   double a2 = kinematic_params->a[2]; */
/*   double r7 = kinematic_params->grip_to_q6_dis; */

  /* transformation from base to grip */
  /* wTg = wTb * bTg  ->  bTg = inv(wTb) * wTg */
  p3d_matInvertXform(Tbase, Tinv);
  p3d_mat4Mult(Tinv,Tgrip,bTg);

  /* vectors */
  v[0] = bTg[0][0];
  v[1] = bTg[1][0];
  v[2] = bTg[2][0];
  p3d_vectNormalize(v,vxgrip);
  v[0] = bTg[0][1];
  v[1] = bTg[1][1];
  v[2] = bTg[2][1];
  p3d_vectNormalize(v,vygrip);
  v[0] = bTg[0][2];
  v[1] = bTg[1][2];
  v[2] = bTg[2][2];
  p3d_vectNormalize(v,vzgrip);
  posgrip[0] = bTg[0][3];
  posgrip[1] = bTg[1][3];
  posgrip[2] = bTg[2][3];

  /* compute position of the fourth joint */
  px = posgrip[0] - r7 * vzgrip[0];
  py = posgrip[1] - r7 * vzgrip[1];
  pz = posgrip[2] - r7 * vzgrip[2];

  /* filter */
  if(ABS(px) < LOCAL_COMPUTATION_EPSILON) px = 0.0;
  if(ABS(py) < LOCAL_COMPUTATION_EPSILON) py = 0.0;
  if(ABS(pz) < LOCAL_COMPUTATION_EPSILON) pz = 0.0;



  d[11] = e1 * sqrt(SQR(px) + SQR(py));
  
  if (ABS(d[11]) < LOCAL_COMPUTATION_EPSILON) {
/*     printf("->arm_kinematic_computation: Singularity!\n"); */
    return 0;
  } else {
    c1 = px / d[11];
    s1 = py / d[11];
    q[0] = arm_atan2 (s1, c1);
    s3 = (SQR(d[11]) + SQR(pz) - SQR(r4) - SQR(a2)) / 2. / a2 / r4;
    c3 = e3 * sqrt(1 - SQR(s3));
    
    if (s3 > 1) {
/*       printf("->arm_kinematic_computation: position is not accessible!\n"); */
      return 0;
    } else {
      q[2] = arm_atan2(s3, c3);
      s2 = ( r4 * c3 * d[11] + (r4 * s3 + a2) * pz) / (SQR(d[11]) + SQR(pz));
      c2 = (-r4 * c3 * pz + (r4 * s3 + a2) * d[11]) / (SQR(d[11]) + SQR(pz));
      q[1] = arm_atan2(s2, c2);
      s23 = (d[11] - a2 * c2) / r4;
      c23 = (-pz + a2 * s2) / r4;
      d[10] = c1 * vzgrip[0] + s1 * vzgrip[1];
      d[8] = s1 * vzgrip[0] - c1 * vzgrip[1];
      d[9] = c1 * vxgrip[0] + s1 * vxgrip[1];
      d[7] = s1 * vxgrip[0] - c1 * vxgrip[1];
      d[6] = c23 * d[10] + s23 * vzgrip[2];
      c5 = s23 * d[10] - c23 * vzgrip[2];
      d[5] = c23 * d[9] + s23 * vxgrip[2];
      d[4] = s23 * d[9] - c23  * vxgrip[2];
      s5 = e5 * sqrt(SQR(d[6]) + SQR(d[8]));
      
      q[4] = arm_atan2(s5, c5);
	
      if (ABS(s5) < LOCAL_COMPUTATION_EPSILON) {
/* 	printf("->arm_kinematic_computation: Singularity!\n"); */
	return 0;
      } else {
	c4 = d[6] / s5;
	s4 = d[8] / s5;
	q[3] = arm_atan2(s4, c4);
	d[3] = c4 * d[5] + s4 * d[7];
	s6 = - s4 * d[5] + c4 * d[7];
	c6 = c5 * d[3] - s5 * d[4];
	q[5] = arm_atan2(s6, c6);

	/* due to the non-correspondence of models */
	/* we need to modify values */
	q[1] -= M_PI/2;
	q[2] -= M_PI/2;
	// modif Juan (for model 6R_arm)
	if(((q[3] > 0.0) && (q[5] > 0.0)) ||
	   ((q[3] < 0.0) && (q[5] < 0.0)) ||
	   ((q[3] == 0.0) && (q[5] == M_PI)) ||
	   ((q[3] == M_PI) && (q[5] == 0.0))) {
	  q[5] += M_PI;
	  if(q[5] > M_PI)
	    q[5] -= 2*M_PI;
	}

	/* Ok */
	return 1;
      }
    }
  }
}


/*******************************************************************************/
/* CT Bot                                                                      */
/*******************************************************************************/

/**
 * %  Resolution du modele geometrique du robot a partir d'une
 * %  configuration spatiale F=[FX,FY,FZ] (saisie aiguille) Z=[Zx,Zy,Zz] 
(droite support)
 * %

 * % M.B. 14/10/2003 - LSIIT
 *
	En cas d'impossibilite de calculer le MGI, q et p ne sont pas
	modifi�s.
*/

/* Par : Benjamin Maurin, Doctorant LSIIT/EAVR,
                          B150a, ENSPS, Bd Sebastien Brant
                          67400 ILLKIRCH - FRANCE 
                          Tel: (03 902)44461 Fax: (03 902)44480
                          Email: maurin@eavr.u-strasbg.fr
*/

/* NOTE : Les articulations p1,p5, q14 et q15 sont invers�es 
         (angle orient� dans l'autre sens) par rapport a l'article
*/
 
int CTBot_MGI(p3d_matrix4 Ff, double q[13])
{
  double Fx,Fy,Fz;
  double Zx,Zy,Zz,Znorme;
  double p1,C1,S1,p2b,p2,q3,q4,q8,p7,p7b,p9,C8,S8,p5,C5,S5,X14,q14,S14,C14,q15,q15b,p16,C16,S16;
  double p11,p12,p12p,p13;
  double Sigmap2q34,Sigmap79q8,Cp2q34,Sp2q34;
  double B,C,alpha,beta,gamma;
  p3d_matrix4 F13,F14,F15,F16;
  p3d_matrix4 Tm;
  //p3d_vector3 O13;
  //double F13_1_1;

  // Geometric data (should be measured on the model !!!)
  double LBH	= 0.006+0.001; 
  double LBT	= 0.0758353-0.002; 
  double LPH	= 0.028+0.00;
  double LPT	= 0.065+0.005; 
  double a1	= 0.040; 
  double a2	= 0.067; 
  double a3	= 0.045+0.01; 
  double LBX	= 0.069622-0.002;
  double LBY	= 0.040;
  double LBZ	= 0.006+0.057;
  double LPX	= 0.104; 
  double LPY	= 0.04; 
  double b1	= 0.075; 
  double b2	= 0.060; 
  double b3	= 0.028; 

  // Position du point F
  Fx = Ff[0][3];
  Fy = Ff[1][3];
  Fz = Ff[2][3];
  // Vecteur norm� de descente (2 degres de liberte car ||Z||=1)
  Znorme = sqrt(SQR(Ff[0][2])+SQR(Ff[1][2])+SQR(Ff[2][2]));
  Zx = Ff[0][2]/Znorme;
  Zy = Ff[1][2]/Znorme;
  Zz = Ff[2][2]/Znorme;

  //Patte 1

  //Recherche de p1:
  p1 = atan2(Fx,Fz-LBH);
  C1 = cos(p1);
  S1 = sin(p1);
  
  // Calcul de la somme Sigma(p2+q3+q4):
  Sigmap2q34 = -atan(Zy/(S1*Zx+C1*Zz));
  Cp2q34 = cos(Sigmap2q34);
  Sp2q34 = sin(Sigmap2q34);
  
  // Coefficients temporaires: B et C pour trouver q1:
  B = Fy+LBT-Cp2q34*LPT+Sp2q34*LPH;
  C = S1*Fx+C1*Fz-C1*LBH-Sp2q34*LPT-Cp2q34*LPH-a1;
  q3 = -acos(0.5*(B*B+C*C-a2*a2-a3*a3)/(a2*a3));
  
  // On utilise B et C, d'ou on tire p2:
  //p2a = 2.0*atan((a2+a3*cos(q1)+sqrt(a2*a2+2*a3*a2*cos(q1)+a3*a3-B*B))/(a3*sin(q1)-B));
  // ou:
  p2b = 2.0*atan((a2+a3*cos(q3)-sqrt(a2*a2+2*a3*a2*cos(q3)+a3*a3-B*B))/(a3*sin(q3)-B));
  // On choisit p2b:
  p2 = p2b;
  
  // Enfin on peut trouver q2 grace a Sigma:
  q4 = Sigmap2q34-p2-q3;
  
  //%patte 2
  Sigmap79q8 = Sigmap2q34;
  
  B = Fy-LBT+Cp2q34*LPT+Sp2q34*LPH;
  C = S1*Fx+C1*Fz-C1*LBH+Sp2q34*LPT-Cp2q34*LPH-a1;
  q8 = acos(0.5*(B*B+C*C-a3*a3-a2*a2)/(a3*a2));
  C8 = cos(q8);
  S8 = sin(q8);
  
  p7b = 2*atan((a2+a3*C8-sqrt(a2*a2+2*a3*a2*C8+a3*a3-B*B))/(a3*S8-B));
  p7 = p7b;
  p9 = Sigmap79q8-p7-q8;
  
  //%Angle commun au deux pattes
  p5 = atan2(C1*Zx-S1*Zz, (S1*Zx+C1*Zz)/Cp2q34 );
  C5 = cos(p5);
  S5 = sin(p5);
  
  //%patte 3
  p16 = -atan2(-S1*Sp2q34*(LBX+Fx)-Cp2q34*(-LBY+Fy)-C1*Sp2q34*(-LBZ+Fz)-LPY,
		      Zx*(LBX+Fx)+Zy*(-LBY+Fy)+Zz*(-LBZ+Fz));
  C16 = cos(p16);
  S16 = sin(p16);
	
  alpha = (C1*C5-S1*Cp2q34*S5)*(LBX+Fx)+Sp2q34*S5*(-LBY+Fy)
                  +(-S1*C5-C1*Cp2q34*S5)*(-LBZ+Fz)-LPX;
  beta =  S16*(S1*Sp2q34*(LBX+Fx)+Cp2q34*(-LBY+Fy)+C1*Sp2q34*(-LBZ+Fz)+LPY)
                  +C16*(Zx*(LBX+Fx)+Zy*(-LBY+Fy)+Zz*(-LBZ+Fz))-b3;
  gamma = alpha*alpha+beta*beta;
  
  X14 = -0.5*(b2*b2+b1*b1-gamma)/(b2*b1);
  q14 = acos(X14);
  C14 = X14;
  S14 = sin(q14);
  
  q15b = 2*atan((b1*C14+b2-sqrt(2*b2*b1*C14+b2*b2+b1*b1-alpha*alpha))/(b1*S14-alpha));
  q15  = q15b;

  // ----------------------------------------------------------------------------- 
  // cardan
  // WARNING : operations are not optimized in this part of the function !!!

  p3d_mat4Pos(Tm,-LPX,LPY,0.0,-p16,0.0,0.0);
  p3d_mat4Mult(Ff,Tm,F16);
  p3d_mat4Pos(Tm,0.0,0.0,-b3,0.0,-q15,0.0);
  p3d_mat4Mult(F16,Tm,F15);
  p3d_mat4Pos(Tm,0.0,0.0,-b2,0.0,-q14,0.0);
  p3d_mat4Mult(F15,Tm,F14);
  p3d_mat4Pos(Tm,0.0,0.0,-b1,0.0,0.43633,0.0);  // 0.43633 = 25� !!!
  p3d_mat4Mult(F14,Tm,F13);

  // FOR CHECKING
/*   p3d_mat4Print(Ff,"Ff"); */
/*   p3d_mat4Print(F16,"F16"); */
/*   p3d_mat4Print(F15,"F15"); */
/*   p3d_mat4Print(F14,"F14"); */
/*   p3d_mat4Print(F13,"F13"); */
  // F13 origin position
/*   O13[0] = -LBX; */
/*   O13[1] =  LBY; */
/*   O13[2] =  LBZ; */
/*   printf("O13 = %f , %f , %f\n",O13[0],O13[1],O13[2]); */

  p11 = atan2(F13[1][0],F13[0][0]);
  p13 = atan2(F13[2][1],F13[2][2]);

  p12p = acos(F13[0][0]/cos(p11));

  if(p12p != -asin(F13[2][0]))
    p12p = -p12p;

  // FOR CHECKING
  //F13_1_1 = cos(p11)*cos(p13) + sin(p11)*sin(p12p)*sin(p13);

  p12 = p12p - 0.43633;  // 0.43633 = 25� !!!

  // ----------------------------------------------------------------------------- 
  
  if (isnan(q3)||isnan(q4)||isnan(q8)||isnan(q14)||isnan(q15)||isnan(p1)
      ||isnan(p2)||isnan(p5)||isnan(p7)||isnan(p9)||isnan(p16)
      ||isnan(p11)||isnan(p12)||isnan(p13)) {
    return(0);
  }
  else {
/*     q[0]  = q3; */
/*     q[1]  = q4; */
/*     q[2]  = q8; */
/*     q[3]  = q14; */
/*     q[4]  = q15; */
    
/*     q[5]  = p1; */
/*     q[6]  = p2; */
/*     q[7]  = p5; */
/*     q[8]  = p7; */
/*     q[9]  = p9; */
/*     q[10] = p16; */

// NOTE : p5, p10 and p16 are not used !

    q[0]  = p1;
    q[1]  = p2;
    q[2]  = q3;
    q[3]  = q4;
    q[4]  = p1; // p6 = p1
    q[5]  = p7;
    q[6]  = q8;
    q[7]  = p9;
    //q[8]  = 0;//?;
    //q[9]  = 0;//?;
    //q[10] = 0;//?;
    q[8]  = p11;
    q[9]  = p12;
    q[10] = p13;
    q[11] = q14;
    q[12] = q15;

    return(1);
  }
}


