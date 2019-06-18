#include "OsgQWidget.h"
#include <QtWidgets/QGridLayout>

#include <osgDB/WriteFile>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>

const unsigned int defaultRefreshPeriod = 30;
const unsigned int idleRefreshPeriod = 60;

//------------------------------------------------------------------------------------------------------------

OsgQWidget::OsgQWidget(QWidget *parent) :
QWidget(parent),
	m_refreshPeriod(defaultRefreshPeriod)
{
	setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
	m_camera = createCamera(0,0,100,100);
	m_statesetManipulator = new osgGA::StateSetManipulator( m_camera->getOrCreateStateSet() );

	QWidget* widget1 = addViewWidget( );

	QGridLayout* grid = new QGridLayout;

	// maximal area for widget
	grid->setContentsMargins(1,1,1,1);
	grid->setSpacing(0);
	grid->addWidget( widget1, 0, 0 );
	setLayout( grid );

	connect( &m_timer, SIGNAL( timeout() ), this, SLOT( update() ) );
	m_timer.start( m_refreshPeriod );
}

QWidget* OsgQWidget::addViewWidget( )
{
	m_view = new osgViewer::View;
	m_view->setCamera( m_camera );
	addView( m_view );

	//m_view->addEventHandler( new osgViewer::StatsHandler );
	m_view->setCameraManipulator( new osgGA::TrackballManipulator );
	//! ×´Ì¬ÇÐ»»
	m_view->addEventHandler(m_statesetManipulator);

	osgQt::GraphicsWindowQt* gw = dynamic_cast<osgQt::GraphicsWindowQt*>( m_camera->getGraphicsContext() );
	return gw ? gw->getGLWidget() : NULL;
}

osg::Camera* OsgQWidget::createCamera( int x, int y, int w, int h )
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	osg::GraphicsContext::Traits* traits = new osg::GraphicsContext::Traits;
	//traits->windowName = "SceneView";
	traits->windowDecoration = false;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = ds->getNumMultiSamples();

	osg::Camera * camera = new osg::Camera;
	camera->setGraphicsContext( new osgQt::GraphicsWindowQt( traits ) );

	camera->setClearColor( osg::Vec4(1, 0.1, 0.2, 1.0) );
	camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
	camera->setProjectionMatrixAsPerspective(
		30.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0f, 10000.0f );
	return camera;
}

void OsgQWidget::setRefreshPeriod(unsigned int period)
{
	m_refreshPeriod = period;
}

void OsgQWidget::setIdle(bool val)
{
	if (val)
		m_timer.start(idleRefreshPeriod);
	else
		m_timer.start(defaultRefreshPeriod);
}

void OsgQWidget::setSceneData(osg::Node *node)
{
	m_view->setSceneData( node );
}

void OsgQWidget::takeSnapshot()
{

}
