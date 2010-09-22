/*
 *  OpenGLWidget.hpp
 *  BioMove3D
 *
 *  Created by Jim Mainprice on 29/07/10.
 *  Copyright 2010 LAAS/CNRS. All rights reserved.
 *
 */
#ifndef QT_OPENGL_WIDGET_HPP
#define	QT_OPENGL_WIDGET_HPP

extern void qt_canvas_viewing(int mouse_press,int button);
extern void qt_get_win_mouse(int* i, int *j);
extern void qt_get_fl_win();
extern void qt_calc_cam_param();
extern void qt_change_mob_frame(G3D_Window* win,pp3d_matrix4 frame);
extern void qt_reset_mob_frame(G3D_Window* win);
extern G3D_Window * qt_get_cur_g3d_win();

extern p3d_vector4 JimXc;
extern p3d_vector4 JimXw;
extern p3d_vector4 Jimup;

#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>

extern QWaitCondition* waitDrawAllWin;
extern QMutex* lockDrawAllWin;

#include "qtWindow/qtOpenGL/g3dQtConnection.hpp"

extern Move3D2OpenGl* pipe2openGl;

#endif