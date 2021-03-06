/*
 *  p3d_random.c
 *  BioMove3D
 *
 *  Created by Jim Mainprice on 03/06/10.
 *  Copyright 2010 LAAS/CNRS. All rights reserved.
 *
 */
#include "P3d-pkg.h"

#include <cassert>
#include "MTRand.hpp"

#ifdef USE_GSL
#include <gsl/gsl_randist.h>
gsl_rng * _gsl_seed;
#endif

MTRand mersenne_twister_rng = MTRand(time(NULL));

/***********************************************/
/* Fonction initialisant le generateur de      */
/* nombres aleatoires par un nombre choisi     */
/* In : l'initialisation                       */
/* Out :                                       */
/***********************************************/
void p3d_init_random_seed(int seed)
{
  // SHOULD THAT BE IN THERE ?? (TRO deadline 2016)
  std::srand((unsigned int) (seed)); // C library implementation
	mersenne_twister_rng.seed(seed);
#ifdef USE_GSL
	_gsl_seed = gsl_rng_alloc (gsl_rng_taus);
#endif
  printf("Used Seed (set user) = %u\n", seed);
}

/***********************************************/
/* Fonction initialisant le generateur de      */
/* nombres aleatoires avec l'horloge           */
/* In :                                        */
/* Out :                                       */
/***********************************************/
uint p3d_init_random(void)
{
	// srand(time(NULL)); // C library implementation
  uint t = (uint)time(NULL);
  printf("Used Seed = %u\n", t);
  mersenne_twister_rng.seed(t);
#ifdef USE_GSL
	_gsl_seed = gsl_rng_alloc (gsl_rng_taus);
#endif
  return t;
}

/*******************************************/
/* Fonction generant un nombre aleatoire   */
/* entre a et b                            */
/* Int : a et b                            */
/* Out : le nombre genere                  */
/*******************************************/
double p3d_random(double a, double b)
{double v;
	
	// C library implementation
	// v =  rand()/((double)RAND_MAX+1); /* nombre aleatoire [0.,1.] */
	v = mersenne_twister_rng.rand();
	v = (b-a)*v + a;
	return(v);
}

/*!
  Uniform sampling of an integer in the range [a, b].
  Precondition: a <= b
 */
int p3d_random_integer(int a, int b)
{
  assert(a <= b);
  return(mersenne_twister_rng.randInt(b - a) + a);
}

/***************************************************/
/* by Boor 7-12-1999                               */
/* Gaussian random number generator                */
/* Given a double Sigma_d_a, NormalRand            */
/* returns a double chosen from a normal           */
/* distribution using sigma Sigma_d_a              */
/***************************************************/
double NormalRand( double Sigma_d_a )
{
	double a,b;
	double result;
	
	a = p3d_random(.0,1.0);
	// following test necessary only when rand() can return 0!!!
	while(a == 0.0){
		a = p3d_random(.0,1.) ;
	}
	b = p3d_random(.0,1.0);
	result = Sigma_d_a*cos(2.0*M_PI*b)*sqrt(-2.0*log(a));
	return(result);
}

/***************************************************/
/* Gaussian random number generator                */
/***************************************************/
double p3d_gaussian_random( const double& mean, const double& variance , const double& min, const double& max)
{
	double result;
	
    result = p3d_gaussian_random2(mean,variance);
	
	if (min<min){
		result = min;
	}
	if (result>= max) {
		result = max;
	}
	
	return(result);
}

double p3d_gaussian_random2( const double& mean, const double& variance )
{
    double result;

    result = mersenne_twister_rng.randNorm(mean,variance);
    return(result);
}
