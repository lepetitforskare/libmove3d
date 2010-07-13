#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "p3d_sys.h"
#include "kcdpropertieswindow.hpp"
#include "p3d/env.hpp"

#include "qtFormRobot/moverobot.hpp"
#ifdef QWT
#include "qtPlot/histoWin.hpp"
#endif
#include "qtBase/qt_widgets.hpp"
#include "qtOpenGL/glwidget.hpp"

#include <vector>

/**
 * @ingroup qtWindow
 * @defgroup qtMainWindow
 * The Qt Main window is fired in a different thread as the worker thread in which is located the xform thread.
 * It then makes an object of the MainWindow class which contains other small widgets like the Side Window.
 \code
    MainWindow::MainWindow(QWidget *parent)
            : QMainWindow(parent), ui(new Ui::MainWindow)
    {
        ui->setupUi(this);
        ui->sidePannel->setMainWindow(this);

        mKCDpropertiesWindow = new KCDpropertiesWindow();

        ui->OpenGL->setWinSize(G3D_WIN->size);
        pipe2openGl = new Move3D2OpenGl(ui->OpenGL);

        QPalette pal(ui->centralWidget->palette()); // copy widget's palette to non const QPalette
        QColor myColor(Qt::darkGray);
        pal.setColor(QPalette::Window,myColor);
        ui->centralWidget->setPalette( pal );        // set the widget's palette

        cout << "pipe2openGl = new Move3D2OpenGl(ui->OpenGL)" << endl;
        ...
  \endcode
  The Ui file edited is setup by ui->setupUi(this);
 */

namespace Ui
{
    class MainWindow;
}

/**
  * @ingroup qtMainWindow
  * @brief Qt Main Window container
  * Tow Widget are derived from other classes The GLWidget widget and the MoveRobot Widget
  \image html Designer.png
  */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
	
	void connectCheckBoxToEnv(QCheckBox* box, Env::boolParameter p);
	
	GLWidget*		getOpenGL();
	
public slots:
	void drawAllWinActive();
	void isPlanning();
    void planningFinished();
	
	void setBoolGhost(bool value);
    void setBoolBb(bool value);
    void setBoolFloor(bool value);
    void setBoolTiles(bool value);
    void setBoolWalls(bool value);
    void setBoolSmooth(bool value);
    void setBoolShadows(bool value);
    void setBoolFilaire(bool value);
	void setBoolJoints(bool value);
	void setBoolContour(bool value);
	void setBoolEnableLight(bool value);
	
	void setJointToDraw(int joint);
	
protected:
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
	
	
private slots:
	
	void initRobotsMenu();
	void setRobotAsCurrent();
	
	void openScenario();
	void saveScenario();
	
	void loadGraph();
	void saveGraph();
	void saveXYZGraphToDot();
	
	void loadTraj();
	void saveTraj();
	
    void changeLightPosX();
    void changeLightPosY();
    void changeLightPosZ();
	void addTrajToDraw();
	void clearTrajToDraw();
	void colorTrajChange(int color);
    void run();
    void stop();
    void reset();
    void ResetGraph();
    void showTraj();
    void restoreView();
	void mobileCamera();

    // Global
//    void setLineEditWithNumber(Env::intParameter p , int val );
    void changeEvent(QEvent *e);

    //Util
    //Greedy
    void greedyPlan();
    void biasPos();

	void test1();
	void test2();
	void test3();

private:
	
    Ui::MainWindow*				m_ui;
	
    KCDpropertiesWindow*		mKCDpropertiesWindow;

    void connectCheckBoxes();

	std::vector<QAction*>		m_RobotsInMenu;
	
    QPushButton* greedy;
	LabeledSlider* createSlider(QString s, Env::intParameter p,int lower, int upper);
    LabeledDoubleSlider* createDoubleSlider(QString s,Env::doubleParameter p, double lower, double upper);

	void initRunButtons();
    void initViewerButtons();
    void initLightSource();
    void initUtil();
    void initOptim();
    void initModel();
	void initTests();
};

/**
 * @ingroup qtWindow
 * @brief Planner thread class 
 */
class Plannerthread: public QThread
{
	Q_OBJECT
	
public:
	Plannerthread(QObject* parent = 0);
	
protected:
	void run();
	
};

/**
 * @ingroup qtWindow
 * @brief Showtraj thread class 
 */
class Showtrajthread: public QThread
{
	Q_OBJECT
	
public:
	Showtrajthread(QObject* parent = 0);
	
protected:
	void run();
	
};


#endif // MAINWINDOW_H
