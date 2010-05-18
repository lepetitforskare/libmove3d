/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.

 */
#ifndef __CEXTRACT__

extern void p3d_mat4Add ( p3d_matrix4 a, p3d_matrix4 b, p3d_matrix4 c );
extern void p3d_mat4Sub ( p3d_matrix4 a, p3d_matrix4 b, p3d_matrix4 c );
extern void p3d_mat3Sub ( p3d_matrix3 a, p3d_matrix3 b, p3d_matrix3 c );
extern void p3d_mat3Add ( p3d_matrix3 a, p3d_matrix3 b, p3d_matrix3 c );
extern void p3d_mat4Mult ( p3d_matrix4 a, p3d_matrix4 b, p3d_matrix4 c );
extern void p3d_matvec4Mult ( p3d_matrix4 a, p3d_vector4 v, p3d_vector4 c );
extern void p3d_vec3Mat4Mult ( double a[3], p3d_matrix4 b, p3d_matrix_type c[3] );
extern void p3d_mat3Mult ( p3d_matrix3 a, p3d_matrix3 b, p3d_matrix3 c );
extern void p3d_mat4Copy ( p3d_matrix4 source, p3d_matrix4 dest );
extern void p3d_mat3Copy ( p3d_matrix3 source, p3d_matrix3 dest );
extern void p3d_mat4Transpose ( p3d_matrix4 source, p3d_matrix4 dest );
extern void p3d_mat3Transpose ( p3d_matrix3 source, p3d_matrix3 dest );
extern void p3d_mat4Print ( p3d_matrix4 M, const char *name );
extern void p3d_mat3Print ( p3d_matrix3 M, char *name );
extern int p3d_mat4IsEqual ( p3d_matrix4 M1, p3d_matrix4 M2 );
extern void p3d_matMultXform ( p3d_matrix4 a, p3d_matrix4 b, p3d_matrix4 c );
extern void p3d_matInvertXform ( p3d_matrix4 M, p3d_matrix4 inv );
extern int p3d_matInvertArbitraryXform ( p3d_matrix4 mat, p3d_matrix4 invmat );
extern void p3d_mat4Trans ( p3d_matrix4 M, p3d_vector3 vect );
extern void p3d_mat4Rot ( p3d_matrix4 M, p3d_vector3 axe, double t );
extern void p3d_mat4Pos ( p3d_matrix4 M, double Tx, double Ty, double Tz, double Rx, double Ry, double Rz );
extern void p3d_mat4PosReverseOrder ( p3d_matrix4 M, double Tx, double Ty, double Tz, double Rx, double Ry, double Rz );
extern void p3d_mat4ExtractPosReverseOrder ( p3d_matrix4 M, double * Tx, double * Ty, double * Tz, double * Rx, double * Ry, double * Rz );
extern void p3d_mat4ExtractRot ( p3d_matrix4 M, p3d_vector3 axis, double *angle );
extern void p3d_vectDeltaRot ( p3d_vector3 axe, double * t, p3d_vector3 axe1, p3d_vector3 axe2 );
extern void p3d_mat4DeltaRot ( p3d_matrix4 M, p3d_vector3 axe1, double t1, p3d_vector3 axe2, double t2 );
extern void p3d_matBuildXform ( char *axes, p3d_matrix_type angles[], p3d_matrix_type dx, p3d_matrix_type dy, p3d_matrix_type dz, p3d_matrix4 M );
extern void p3d_xformPoint ( p3d_matrix4 M, p3d_vector3 p, p3d_vector3 p2 );
extern void p3d_xformVect ( p3d_matrix4 M, p3d_vector3 v, p3d_vector3 v2 );
extern void p3d_xform4 ( p3d_matrix4 M, p3d_vector4 x, p3d_vector4 x2 );
extern void p3d_xform3 ( p3d_matrix3 M, p3d_vector3 x, p3d_vector3 x2 );
extern void p3d_vectCopy ( p3d_vector3 src, p3d_vector3 dest );
extern void p3d_vectAdd ( p3d_vector3 a, p3d_vector3 b, p3d_vector3 c );
extern void p3d_vectSub ( p3d_vector3 a, p3d_vector3 b, p3d_vector3 c );
extern void p3d_vectNeg ( p3d_vector3 src, p3d_vector3 dest );
extern void p3d_vectScale ( p3d_vector3 src, p3d_vector3 dest, p3d_matrix_type k );
extern void p3d_vectNormalize ( p3d_vector3 src, p3d_vector3 dest );
extern void p3d_vect4Normalize ( p3d_vector3 src, p3d_vector3 dest );
extern p3d_matrix_type p3d_vectNorm ( p3d_vector3 v );
extern p3d_matrix_type p3d_vect4Norm ( p3d_vector4 v );
extern p3d_matrix_type p3d_vectDistance ( p3d_vector3 a,  p3d_vector3 b);
extern p3d_matrix_type p3d_square_of_vectNorm ( p3d_vector3 v );
extern int p3d_vectEqual ( p3d_vector3 a, p3d_vector3 b );
extern p3d_matrix_type p3d_vectDotProd ( p3d_vector3 a, p3d_vector3 b );
extern void p3d_vectXprod ( p3d_vector3 a, p3d_vector3 b, p3d_vector3 c );
extern p3d_matrix_type p3d_planeDist ( p3d_vector4 plane, p3d_vector3 point );
extern void p3d_displacePoint ( p3d_vector3 point, p3d_vector3 vect, p3d_matrix_type lambda, p3d_vector3 result );
extern double p3d_mat3Det ( p3d_matrix3 mat );
extern double p3d_mat4Det ( p3d_matrix4 m );
extern int p3d_mat3Invert ( p3d_matrix3 mat, p3d_matrix3 invmat );
extern void p3d_vec3Mat3Mult ( p3d_matrix3 M, p3d_vector3 a, p3d_vector3 b );
extern int p3d_isTransfMat ( p3d_matrix4 M );
extern int p3d_extractScale ( p3d_matrix4 M, double *scale );
extern int p3d_mat2Invert ( p3d_matrix2 mat, p3d_matrix2 invmat );
extern void p3d_mat3Rot ( p3d_matrix3 M, p3d_vector3 axis, double t );
extern void p3d_mat4TransRot( p3d_matrix4 M, double tx, double ty, double tz, p3d_vector3 axis, double angle);
extern void p3d_mat4ExtractTrans ( p3d_matrix4 M, p3d_vector3 v);
extern void p3d_mat4ExtractRotMatrix ( p3d_matrix4 M, p3d_matrix3 R );
extern void p3d_mat4SetRotMatrix (p3d_matrix3 R, p3d_matrix4 M );
extern void p3d_mat4ExtractPosReverseOrder2(p3d_matrix4 M,
				    double * tx, double * ty, double * tz,
				    double * ax, double * ay, double * az);
extern void p3d_orthogonal_vector(p3d_vector3 v, p3d_vector3 result);
extern void p3d_orthonormal_basis(p3d_vector3 u, p3d_vector3 v, p3d_vector3 w);

extern void p3d_quaternion_to_matrix3(p3d_vector4 q, p3d_matrix3 R);
extern void p3d_quaternion_to_matrix4(p3d_vector4 q0, p3d_matrix4 T);
extern void p3d_matrix3_to_quaternion(p3d_matrix3 R, p3d_vector4 q);
extern double p3d_mat4Distance(p3d_matrix4 M1, p3d_matrix4 M2, double weightR, double weightT);

extern void p3d_mat4ExtractColumnX(p3d_matrix4 M, p3d_vector3 v);
extern void p3d_mat4ExtractColumnY(p3d_matrix4 M, p3d_vector3 v);
extern void p3d_mat4ExtractColumnZ(p3d_matrix4 M, p3d_vector3 v);


extern void p3d_random_quaternion(p3d_vector4 q);

#include "Graphic-pkg.h"
extern void p3d_to_gl_matrix(p3d_matrix4 T, GLfloat mat[16]);

#endif
