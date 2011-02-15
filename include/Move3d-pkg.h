/* 
 *  This file includes the prototypes of the functions defined in the
 *  directory move3d and the files defining the structure appearing in 
 *  these prototypes.
 */

#ifndef MOVE3D_PKG_H
#define MOVE3D_PKG_H

/* struct */
#include "move3d.h"

#ifdef WITH_XFORMS
/* globals */
extern Pixmap GetApplicationIcon( );

/* proto */
#include "../move3d/proto/move3d_proto.h"

#else

#include "P3d-pkg.h"
// Functions Without XFORMS
extern int fct_stop(void);
extern void fct_draw(void);
extern int g3d_get_KCD_CHOICE_IS_ACTIVE();
extern int p3d_get_user_drawnjnt(void);
extern void p3d_set_user_drawnjnt(int jnt);
extern p3d_traj *p3d_graph_to_traj ( p3d_rob *robotPt );
extern void g3d_add_traj ( char *name, int i , p3d_rob* rob = NULL , p3d_traj* traj = NULL );
extern void g3d_add_config_to_ui( char *name, p3d_rob *robotPt , configPt q );
#endif

#endif /* #ifndef MOVE3D_PKG_H */
