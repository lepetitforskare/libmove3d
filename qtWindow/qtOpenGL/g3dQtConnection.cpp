/*
 * g3d_qt_connection.cpp
 *
 *  Created on: Oct 7, 2009
 *      Author: jmainpri
 */

#include "g3dQtConnection.hpp"
#include "../cppToQt.hpp"

#include <iostream>
#include <string>

using namespace std;

QWaitCondition* waitDrawAllWin;
QMutex* lockDrawAllWin;

Move3D2OpenGl::Move3D2OpenGl() : _isWatingForTimer(false)
{
	_timer = new QTimer(this);
}

Move3D2OpenGl::Move3D2OpenGl(GLWidget *glW) : _isWatingForTimer(false)
{
	_glWidget = glW;
	connect(this,SIGNAL(activate_qt_gl_window()),_glWidget,SLOT(updateGL()),Qt::QueuedConnection);
	connect(this,SIGNAL(add_current_image_vector()),_glWidget,SLOT(addCurrentImage()),Qt::QueuedConnection);
	connect(this,SIGNAL(save_image_vector_to_disk()),_glWidget,SLOT(saveImagesToDisk()),Qt::QueuedConnection);

	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(releaseLockIfWating()));

	_isNotTimeControlled = true;
}

void Move3D2OpenGl::setIsNotTimeControlled(bool value)
{
	_isNotTimeControlled = value;
}

void Move3D2OpenGl::releaseLockIfWating()
{
//	cout << "_isWatingForTimer=false" << endl;
	_isWatingForTimer=false;
}

void Move3D2OpenGl::addCurrentImage()
{
	emit
	add_current_image_vector();
}

void Move3D2OpenGl::saveImagesToDisk()
{
	emit
	save_image_vector_to_disk();
}

void Move3D2OpenGl::updatePipe()
{
	// Sends a signal to the worker thread
	// that its got to send the Draw sequence
	string str = "g3d_draw_allwin_active";
	write(qt_fl_pipe[1],str.c_str(),str.length()+1);
}

void Move3D2OpenGl::update()
{
	if((!_isWatingForTimer) || _isNotTimeControlled )
	{
		lockDrawAllWin->lock();
		emit
		activate_qt_gl_window();
		waitDrawAllWin->wait(lockDrawAllWin);
		lockDrawAllWin->unlock();
		_isWatingForTimer=true;
		_timer->start((int)(1000/ENV.getDouble(Env::FPS)));
	}
}

#include "moc_g3dQtConnection.cpp"
