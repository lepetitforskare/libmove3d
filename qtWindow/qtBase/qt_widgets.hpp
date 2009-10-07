#ifndef QT_WIDGETS_HH
#define QT_WIDGETS_HH

#include "p3d_sys.h"
#include <vector>

/**
 * @ingroup qtWindow
 * @brief Slider for double
 */
class QDoubleSlider: public QSlider
{
	Q_OBJECT

	double doubleValue;
	double doubleMin;
	double doubleMax;

signals:
void valueChanged(double value);

private:
	void setValue(double new_value, bool sync, bool init = false);private slots:
	void synchronizeDouble(int new_value);

public slots:
	void setValue(double new_value);

public:
	QDoubleSlider(double min, double max, double init_value, QWidget *parent =
			0);
	void setRange(double min, double max);
	double value() const;
	double min() const;
	double max() const;
};

/**
 * @ingroup qtWindow
 * @brief Lableled slider for double
 */
class LabeledDoubleSlider: public QWidget
{
	Q_OBJECT

	QDoubleSlider* slider;

	signals:
	void sliderValue(QString);
	void sliderValue(double);
	void valueChanged(double);public slots:
	void doubleToQString(double value);
	void qStringToDouble(QString value);
	void setValue(double new_value);

public:
	LabeledDoubleSlider(double min, double max, double init_value,
			QString text, int row = 0, QGridLayout* layout = 0,
			QWidget *parent = 0);
	double value() const;
};

/**
 * @ingroup qtWindow
 * @brief Lableled slider forinteger
 */
class LabeledSlider: public QWidget
{
	Q_OBJECT

	QSlider* slider;

	signals:
	void sliderValue(QString);
	void sliderValue(int);
	void valueChanged(int);public slots:
	void intToQString(int value);
	void qStringToInt(QString value);
	void setValue(int new_value);

public:
	LabeledSlider(int min, int max, int init_value, QString text, int row = 0,
			QGridLayout* layout = 0, QWidget *parent = 0);
	int value() const;
};

/**
 * @ingroup qtWindow
 * @brief GroupBox class
 */
class QVGroupBox: public QGroupBox
{
Q_OBJECT

public:
	QVGroupBox(const QString& title, QWidget* parent = 0);
	void addWidget(QWidget* w, Qt::Alignment alignment = 0);
};

/**
 * @ingroup qtWindow
 * @brief Open a File
 */
class OpenFile: public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString filename READ filename)
	QLabel _label;
	QString p_filename;signals:
	void fileChanged(QString text);

protected slots:
	void setTextFromList(QStringList list);

public:
	OpenFile(QString button_label, QStringList filters = QStringList(
			"Any files (*)"), QBoxLayout::Direction dir =
			QBoxLayout::LeftToRight);
	QString filename() const;
	QFileDialog dialog;
};

#endif
