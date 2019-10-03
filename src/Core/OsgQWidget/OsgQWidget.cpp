#include "OsgQWidget.h"
#include <QtWidgets/QGridLayout>

#include <osgDB/ReadFile>
#include <osg/BlendFunc>
#include <osg/AutoTransform>
#include <osg/PositionAttitudeTransform>
#include <osgDB/WriteFile>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>

const unsigned int defaultRefreshPeriod = 30;
const unsigned int idleRefreshPeriod = 60;
static const int  DEFAULT_FRAME_RATE = 60;
//------------------------------------------------------------------------------------------------------------

OsgQWidget::OsgQWidget(QWidget *parent, osg::Node *scene) :
QWidget(parent),
	m_refreshPeriod(defaultRefreshPeriod)
{
	setMinimumSize(400, 300);
	setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
	
	//! 创建一个view
	m_camera = createCamera(0,0,100,100);
	m_statesetManipulator = new osgGA::StateSetManipulator( m_camera->getOrCreateStateSet() );

	QWidget* widget1 = addViewWidget( );

	if (scene && m_view)
	{
		m_view->setSceneData(scene);
	}
	else
	{
		osg::Node* groupRoot = new osg::Group;
		m_view->setSceneData(groupRoot);
	}

	m_grid = new QGridLayout;

	// maximal area for widget
	m_grid->setContentsMargins(1,1,1,1);
	m_grid->setSpacing(0);
	m_grid->addWidget( widget1, 0, 0 );
	setLayout(m_grid);

	// Playback controlling
	m_frameRate = DEFAULT_FRAME_RATE;
	connect( &m_timer, SIGNAL( timeout() ), this, SLOT( update() ) );
	startRendering();
}

osgViewer::View * OsgQWidget::getMainView()
{
	return m_view;
}

void OsgQWidget::paintEvent(QPaintEvent *event)
{
	frame();
}


osg::ref_ptr<osg::PositionAttitudeTransform>  OsgQWidget::createCameraIndicator()
{
	// Init a transform that always faces camera
	osg::ref_ptr<osg::PositionAttitudeTransform>  cameraIndicator = new osg::PositionAttitudeTransform();
	osg::ref_ptr<osg::AutoTransform>              cameraCenter = new osg::AutoTransform();

	cameraCenter->setAutoScaleToScreen(true);
	cameraCenter->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	osg::StateSet *state = cameraCenter->getOrCreateStateSet();
	state->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	state->setMode(GL_LIGHTING, osg::StateAttribute::OFF &osg::StateAttribute::OVERRIDE);
	state->setRenderBinDetails(99, "RenderBin");

	// Render the image
	osg::ref_ptr<osg::Geode>      geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry>   geom = createTexturedQuadGeometry(osg::Vec3(-20, -20, 0), osg::Vec3(40, 0, 0), osg::Vec3(0, 40, 0));
	osg::ref_ptr<osg::Image>      image = osgDB::readImageFile("resources/icons/center.png");
	osg::ref_ptr<osg::Texture2D>  texture = new osg::Texture2D;
	texture->setTextureSize(100, 100);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	texture->setImage(image);
	geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	geom->getOrCreateStateSet()->setAttributeAndModes(
		new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA));

	// Attach to the main camera
	geode->addDrawable(geom);
	cameraCenter->addChild(geode);
	cameraIndicator->addChild(cameraCenter);
	cameraIndicator->setNodeMask(0);

	m_view->getCamera()->addChild(cameraIndicator);

	return cameraIndicator;
}

void  OsgQWidget::stopRendering()
{
	m_timer.stop();
	stopThreading();
}

void  OsgQWidget::startRendering()
{
	startThreading();
	m_timer.start(1000 / m_frameRate);
}

void  OsgQWidget::removeView(osgViewer::View *view)
{
	stopRendering();
	((QGridLayout *)layout())->removeWidget(m_widgets[view]);
	m_widgets.remove(view);
	osgViewer::CompositeViewer::removeView(view);
	startRendering();
}


void  OsgQWidget::setFrameRate(int FPS)
{
	stopRendering();
	m_frameRate = FPS;
	startRendering();
}

void  OsgQWidget::setWidgetInLayout(QWidget *widget, int row, int column, bool visible /*= true*/)
{
	m_grid->addWidget(widget, row, column);
	widget->setVisible(visible);
}

QWidget * OsgQWidget::createViewWidget(osgQt::GraphicsWindowQt *gw, osg::Node *scene)
{
	osg::ref_ptr<osgViewer::View>       view = new osgViewer::View;
	osg::Camera                        *camera = view->getCamera();
	const osg::GraphicsContext::Traits *traits = gw->getTraits();

	// Connect and align the camera with the given graphics window
	camera->setGraphicsContext(gw);
	camera->setClearColor(osg::Vec4(0.95, 0.95, 0.95, 1.0));
	//camera->setClearColor(osg::Vec4(1, 0.1, 0.2, 1.0));
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setSmallFeatureCullingPixelSize(-1.0f);

	// Init the scene
	//osgEarth::GLUtils::setGlobalDefaults(camera->getOrCreateStateSet());

	view->setSceneData(scene);
	view->addEventHandler(new osgViewer::StatsHandler);
	view->addEventHandler(new osgViewer::ThreadingHandler);
	view->addEventHandler(new osgViewer::WindowSizeHandler);
	view->addEventHandler(new osgViewer::LODScaleHandler);
	// add the help handler
	view->addEventHandler(new osgViewer::HelpHandler);
	// view->addEventHandler(new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet()));

	// Tell the database pager to not modify the unref settings
	view->getDatabasePager()->setUnrefImageDataAfterApplyPolicy(true, false);

	// We have to pause all threads before a view will be added to the composite viewer
	stopThreading();
	addView(view);
	startThreading();

	m_widgets.insert(view, gw->getGLWidget());

	return gw->getGLWidget();
}

osg::ref_ptr<osg::Camera>  OsgQWidget::createLegendHud(const QString &titleString, QVector<osg::Vec4> colorVec, QVector<QString> txtVec)
{
	// An hud camera
	osg::ref_ptr<osg::Camera>  hudCamera = new osg::Camera;

	hudCamera->setName("hudCamera");
	hudCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1280, 0, 800));
	hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	hudCamera->setViewMatrix(osg::Matrix::identity());
	hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
	hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
	hudCamera->setAllowEventFocus(false);

	// A geode for rendering texts
	osg::ref_ptr<osg::Geode>     pGeode = new osg::Geode;
	osg::ref_ptr<osg::StateSet>  pStateSet = pGeode->getOrCreateStateSet();
	pStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	pStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	// Generate title
	osg::ref_ptr<osgText::Font>  pFont = new osgText::Font;
	pFont = osgText::readFontFile("simhei.ttf");
	osg::ref_ptr<osgText::Text>  titleText = new osgText::Text;
	titleText->setFont(pFont);
	titleText->setText(titleString.toStdString(), osgText::String::ENCODING_UTF8);
	titleText->setPosition(osg::Vec3(10.0f, 752.0f + 20, -1));
	titleText->setCharacterSize(16.0f);
	titleText->setColor(osg::Vec4(199, 77, 15, 1));
	pGeode->addDrawable(titleText);

	// Color contents
	int  length = colorVec.size();

	for (int i = 0; i < length; i++)
	{
		// For every color, generate a squad with that color as filling color
		osg::ref_ptr<osg::Geometry>  pGeo = new osg::Geometry;
		pGeo = osg::createTexturedQuadGeometry(osg::Vec3(10, 750.0f - (17 * i), -1), osg::Vec3(38, 0.0, 0.0), osg::Vec3(0.0, 15.0, 0.0));

		osg::ref_ptr<osg::Vec4Array>  colorArray = new osg::Vec4Array;
		colorArray->push_back(colorVec.at(i));
		pGeo->setColorArray(colorArray.get());
		pGeo->setColorBinding(osg::Geometry::BIND_OVERALL);

		pGeode->addDrawable(pGeo);

		// Generate the associated describing text
		osg::ref_ptr<osgText::Text>  pText = new osgText::Text;
		pText->setFont(pFont);
		pText->setText(txtVec.at(i).toStdString());
		pText->setPosition(osg::Vec3(52.0f, 752.0f - (17 * i), -1));
		pText->setCharacterSize(15.0f);
		pText->setColor(osg::Vec4(199, 77, 15, 1));

		pGeode->addDrawable(pText);
	}

	hudCamera->addChild(pGeode);

	return hudCamera;
}

osgQt::GraphicsWindowQt * OsgQWidget::createGraphicsWindow(int x, int y, int w, int h, const std::string &name /*=""*/, bool shareMainContext /*= false*/,
	bool windowDecoration /*=false */)
{
	// Settings of the rendering context
	osg::DisplaySettings *ds = osg::DisplaySettings::instance().get();

	ds->setNumMultiSamples(4);
	osg::ref_ptr<osg::GraphicsContext::Traits>  traits = new osg::GraphicsContext::Traits;
	traits->windowName = name;
	traits->windowDecoration = windowDecoration;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = ds->getNumMultiSamples();

	// This setting helps to make sure two synced widget live and die together
	if (shareMainContext)
	{
		traits->sharedContext = m_mainContext;
	}

	osg::ref_ptr<osgQt::GraphicsWindowQt>  gw = new osgQt::GraphicsWindowQt(traits.get());

	return gw.release();
}

QWidget* OsgQWidget::addViewWidget( )
{
	m_view = new osgViewer::View;
	m_view->setCamera( m_camera );
	addView( m_view );

	//m_view->addEventHandler( new osgViewer::StatsHandler );
	m_view->setCameraManipulator( new osgGA::TrackballManipulator );
	//! 状态切换
	m_view->addEventHandler(new osgViewer::StatsHandler);
	m_view->addEventHandler(new osgViewer::ThreadingHandler);
	m_view->addEventHandler(new osgViewer::WindowSizeHandler);
	m_view->addEventHandler(new osgViewer::LODScaleHandler);
	// add the help handler
	m_view->addEventHandler(new osgViewer::HelpHandler);

	osgQt::GraphicsWindowQt* gw = dynamic_cast<osgQt::GraphicsWindowQt*>( m_camera->getGraphicsContext() );
	m_mainContext = gw;

	m_widgets.insert(m_view, gw->getGLWidget());
	return gw ? gw->getGLWidget() : NULL;
}

osg::Camera* OsgQWidget::createCamera( int x, int y, int w, int h )
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowName = "SceneView";
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
	camera->setGraphicsContext(new osgQt::GraphicsWindowQt(traits.get()));

	camera->setClearColor(osg::Vec4(1, 0, 0, 1.0));
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setProjectionMatrixAsPerspective(
		30.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0f, 10000.0f);
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
