#ifndef DC_OSGQWIDGET_H
#define DC_OSGQWIDGET_H

#include "osgqwidget_global.h"

#include <QtCore/QTimer>

#include <osgViewer/CompositeViewer>
#include <osgGA/StateSetManipulator>
#include <osgQt/GraphicsWindowQt>

class OSGQWIDGET_EXPORT OsgQWidget : public QWidget, public osgViewer::CompositeViewer
{
public:

	OsgQWidget(QWidget *parent = NULL);
	virtual ~OsgQWidget(void) {}

	void setRefreshPeriod(unsigned int period);
	void setIdle(bool val);

	void setSceneData(osg::Node *);

	void takeSnapshot();

protected:

	QWidget* addViewWidget();
	osg::Camera* createCamera( int x, int y, int w, int h );
	virtual void paintEvent( QPaintEvent* /* event */ )  { frame(); }

	QTimer m_timer;
	unsigned int m_refreshPeriod;
	osg::ref_ptr<osg::Camera> m_camera;
	osg::ref_ptr<osgViewer::View> m_view;

	///◊¥Ã¨…Ë÷√
	osg::ref_ptr<osgGA::StateSetManipulator> m_statesetManipulator;
};

#endif // DC_OSGQWIDGET_H
