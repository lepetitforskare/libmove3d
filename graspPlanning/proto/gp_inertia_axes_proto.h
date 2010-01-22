#ifndef GP_INERTIA_AXES_PROTO_H
#define GP_INERTIA_AXES_PROTO_H

extern int jacobi ( float **a, int n, float d[], float **v, int *nrot );
extern void eigsrt ( float d[], float **v, int n );
extern int gpCompute_inertia_axes ( Mass_properties *mass_prop, p3d_matrix3 inertia_axes );
extern int gpInertia_AABB ( p3d_polyhedre *polyhedron, p3d_vector3 cmass, p3d_matrix3 iaxes, double iaabb[6] );
extern void gpDraw_inertia_AABB ( p3d_vector3 cmass, double iaxes[3][3], double iaabb[6] );

#endif
