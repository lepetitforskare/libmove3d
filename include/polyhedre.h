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
#ifndef _POLYHEDRE_H
#define _POLYHEDRE_H

/******************************************************************************************
   
  FICHIER: polyhedre.h

  USAGE: module gerant des objets a facettes, creation par sommet 
         puis definition des faces
         le calcul des plans des faces est effectuee et reste memorisee uniquement 
         a la demande. De meme pour les arretes

******************************************************************************************/
#include <math.h>
#include <stdlib.h>
#include <locale.h>

#include "p3d_matrix.h"

#ifdef _p3d_MATRIX_H

#define  poly_type_de_base p3d_matrix_type
#define poly_vector3 p3d_vector3
#define poly_matrix4 p3d_matrix4

#else
typedef double  poly_type_de_base;

typedef poly_type_de_base  poly_vector3[3];

typedef poly_type_de_base poly_matrix4[4][4];
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef UNKNOWN
#define UNKNOWN 2
#endif


#define poly_error_null_pointer             1
#define poly_error_malloc_failled           2
#define poly_error_unknown_point            3
#define poly_error_impossible_edge          4
#define poly_error_impossible_index         5
#define poly_error_edge_needs_2_points      6
#define poly_error_normal_nulle             7
#define poly_error_plane_needs_3_points     8
#define poly_error_impossible_list_edges    9
#define poly_error_impossible_polyhedre     10


/*************************************************
 cet valeur est nulle quand il n y a pas d erreur
  et les valeurs M3d_error????? sinon
*************************************************/
extern unsigned int poly_error_value;


typedef unsigned int poly_index;
typedef poly_index *poly_index_p ;

typedef unsigned int poly_triangle[3];
#define p3d_triangle poly_triangle

typedef struct  poly_plane
  { poly_vector3      normale;
    poly_type_de_base  d;
  } poly_plane;

typedef struct poly_edge
  { poly_index     point1,point2; //! \warning these indices start from 1
    poly_index     face1,face2; //! \warning these indices start from 1
                                //!  (if there is no adjacent face on one side, the corresponding field is left to 0)
    poly_vector3  u;

     //! edge angle in radians (not computed by default, use p3d_compute_edges_and_face_neighbours).
     //! the angle is 0 if the two adjacent triangles are coplanar. It is > 0 if the angle is convex, < 0 otherwise.
     //! NB: if the edge does not have two adjacent triangles, its angle is undefined and left to 0.
    double angle;

    poly_vector3 normal; /*!< edge normal (not computed by default, use p3d_compute_edges_and_face_neighbours)*/
    poly_vector3 midpoint; /*!< edge middle point (not computed by default, use p3d_compute_edges_and_face_neighbours)*/

  } poly_edge;

typedef struct poly_face
  { poly_plane     *plane;
    unsigned  int  nb_points;
    poly_index     *the_indexs_points; //! \warning these indices start from 1
    unsigned int face_is_convex;

    //! the two following fields are computed with p3d_compute_face_areas_and_centroids():
    p3d_vector3 centroid; //! geometric centroid of the face (not computed by default)
    double area; //! area of the face (must be triangle); (not computed by default)
  
    //! the two following fields are computed with p3d_compute_edges_and_face_neighbours():
    //! array of the indices (starting from zero) of the neighbours faces (all faces must be triangles;  
    //! consequently a face has at most 3 adjacent faces) in the p3d_polyhedre's face array:
    //! NB: if the triangle has no i-th neighbour then neighbours[i] is set to -1 
    //! (not computed by default)
    int neighbours[3];

    //! array of the indices (starting from zero) of the face edges (all faces must be triangles)
    //! in the p3d_polyhedre's edge array:
    //! NB: if the i-th has not been computed yet then edges[i] is set to -1
    //! (not computed by default)
    int edges[3];

#ifdef GRASP_PLANNING
    //! the ID of the part the face belongs to (used by the polyhedron segmentation algorithm):
    unsigned int part;
    double curvature;
#endif

  } poly_face;  


typedef struct poly_polyhedre
  { char *name;
    unsigned int  nb_points;
    unsigned int  nb_faces;
    unsigned int  nb_edges;
    poly_vector3  *the_points;
    poly_face      *the_faces;
    poly_edge      *the_edges;
    poly_matrix4   pos;
    double *curvatures; //! same size as the_points: will contain the curvature of the surface at each vertex position

    //! same size as the_points: surface normals on each vertex (not computed but allocated by default, use p3d_compute_vertex_normals):
    p3d_vector3  *vertex_normals; 
 
    p3d_vector3 centroid; //! centroid of the polyhedron. It is not computed by default; call p3d_compute_poly_centroid()   

    //! boolean to know if the edges and neighbours have been computed
    int areEdgesAndNeighboursUpToDate;

     poly_vector3  *originalPoints;

    #ifdef GRASP_PLANNING 
     //! all the following values are left to zero by default and computed
     //! by calling gpCompute_mass_properties()
     //! polyhedron's center of mass:
     p3d_vector3 cmass;

     //! polyhedron's main inertia axes (in columns):
     p3d_matrix3 inertia_axes;
 
     //! polyhedron's volume
     double volume;
    #endif
  } poly_polyhedre;

/*
void poly_f_2_v3(float x, float y, float z, poly_vector3 *vector);
void poly_d_2_v3(double x, double y, double z, poly_vector3 *vector);
void poly_v3_2_f( poly_vector3 *vector, float *x, float *y, float *z);
void poly_v3_2_d(poly_vector3 *vector, double *x, double *y, double *z);
void poly_plane_2_d(poly_plane *plane, double *a, double *b, double *c, double *d);
void poly_show_error_on_shell(int value);
int poly_get_error_value();
int poly_error();
void poly_init_poly(poly_polyhedre *polyhedre,char *name);
poly_polyhedre *poly_create_poly(char *name);
void poly_destroy_poly(poly_polyhedre *polyhedre);
void poly_destroy_edges(poly_polyhedre *polyhedre);
void poly_destroy_planes(poly_polyhedre *polyhedre);
char *poly_get_name(poly_polyhedre *polyhedre);
unsigned int poly_get_nb_points(poly_polyhedre *polyhedre);
unsigned int poly_get_nb_points_in_face(poly_polyhedre *polyhedre, poly_index face);
unsigned int poly_get_nb_faces(poly_polyhedre *polyhedre);
unsigned int poly_get_nb_edges_in_face(poly_polyhedre *polyhedre, poly_index face);
unsigned int poly_get_nb_edges(poly_polyhedre *polyhedre);
int poly_get_index_point_in_face(poly_polyhedre *polyhedre,poly_index face, poly_index index);
int poly_get_edge_points(poly_polyhedre *polyhedre, poly_index edge, poly_index *p1, poly_index *p2);
int poly_get_edge_faces(poly_polyhedre *polyhedre, poly_index edge, poly_index *f1, poly_index *f2);
int poly_add_point(poly_vector3 point, poly_polyhedre *polyhedre);
int poly_find_point(poly_vector3 point, poly_polyhedre *polyhedre);
int poly_get_point_2_v3(poly_polyhedre *polyhedre, poly_index index, poly_vector3 *vector);
int poly_get_point_2_d(poly_polyhedre *polyhedre, poly_index index, double *x,double *y,double *z);
void poly_get_poly_pos(poly_polyhedre *polyhedre, poly_matrix4 mat);
poly_matrix4 *poly_get_poly_mat(poly_polyhedre *polyhedre);
void poly_set_poly_pos(poly_polyhedre *polyhedre, poly_matrix4 mat);
int poly_find_pos_in_face(poly_polyhedre *polyhedre, poly_index index, poly_index point);
int poly_get_point_in_pos_in_face(poly_polyhedre *polyhedre, poly_index face, poly_index point, double *x, double *y, double *z);
int poly_get_point_in_edge(poly_polyhedre *polyhedre, poly_index edge, poly_index point, double *x, double *y, double *z);
int poly_find_edge(poly_polyhedre *polyhedre, poly_index p1, poly_index p2);
int poly_build_edges(poly_polyhedre *polyhedre);
int poly_build_plane_face(poly_polyhedre *polyhedre, poly_index numero);
int poly_get_plane(poly_polyhedre *polyhedre, poly_index index, poly_plane *plane);
int poly_get_plane_2_d(poly_polyhedre *polyhedre, poly_index index, double *a,double *b,double *c, double *d);
int poly_build_planes(poly_polyhedre *polyhedre);
int poly_build_face(poly_index *the_indexs, unsigned int nombre, poly_polyhedre *polyhedre);
*/

#endif
