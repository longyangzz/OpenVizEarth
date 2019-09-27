#ifndef DC_OSGQWIDGET_H
#define DC_OSGQWIDGET_H

#include "osgqwidget_global.h"

#include <QtCore/QTimer>

#include <osgViewer/CompositeViewer>
#include <osgGA/StateSetManipulator>
#include <osgQt/GraphicsWindowQt>


class QGridLayout;
class OSGQWIDGET_EXPORT OsgQWidget : public QWidget, public osgViewer::CompositeViewer
{
public:

	OsgQWidget(QWidget *parent = NULL);
	virtual ~OsgQWidget(void) {}

	// Get the main view of the viewer, by default the viewer at position (0, 0)
	osgViewer::View* getMainView();

	// Add a widget to the viewer layout at specified position
	void setWidgetInLayout(QWidget* widget, int row, int column, bool visible = true);

	// Create a qt widget containing the given node
	QWidget* createViewWidget(osgQt::GraphicsWindowQt* gw, osg::Node* scene);

	// Create a qt graphics widget with osg support
	osgQt::GraphicsWindowQt* createGraphicsWindow(
		int x, int y, int w, int h, const std::string& name = "", bool shareMainContext = false, bool windowDecoration = false);

	// Create a legend that's rendered above the whole view
	static osg::ref_ptr<osg::Camera> createLegendHud(const QString& titleString, QVector<osg::Vec4> colorVec, QVector<QString> txtVec);

	void setRefreshPeriod(unsigned int period);
	void setIdle(bool val);

	void setSceneData(osg::Node *);

	void takeSnapshot();

	void removeView(osgViewer::View* view);



public slots:
	void stopRendering();
	void startRendering();
	void setFrameRate(int FPS);

protected:

	QWidget* addViewWidget();
	osg::Camera* createCamera( int x, int y, int w, int h );
	virtual void paintEvent(QPaintEvent* /* event */);

	QTimer m_timer;
	unsigned int m_refreshPeriod;
	osg::ref_ptr<osg::Camera> m_camera;
	osg::ref_ptr<osgViewer::View> m_view;
	osgQt::GraphicsWindowQt* m_mainContext;
	int m_frameRate;

	QMap<osgViewer::View*, QWidget*> m_widgets;

	QGridLayout* m_grid;

	///◊¥Ã¨…Ë÷√
	osg::ref_ptr<osgGA::StateSetManipulator> m_statesetManipulator;
};

#endif // DC_OSGQWIDGET_H
