#include <QtCore/QtDebug>

#include <vector>

#include "SceneView.h"
#include "SceneModel.h"

#include "ScreenshotHandler.h"

#include "ExtentsVisitor.h"
#include "SelectionManager.h"
#include "ShaderSelectionDecorator.h"

#include <osgGA/TrackballManipulator>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>

#include <osg/PolygonOffset>

// shadow algorithm
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/StandardShadowMap>
#include <osg/ShapeDrawable>
#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/BlendColor>
#include <osgDB/ReadFile>

#include "OsgMath.h"
#include "OsgData.h"

#include "PickHandler.h"

#include <osgGA/StateSetManipulator>

using namespace DC;

const float compassSize = 100.0f;
const float compassOffset = 20.0f;
const float distanceRatioPivot = 200.0f;

#if WIN32
#undef snprintf
#define snprintf sprintf_s
#endif

// typedefs and definitions
#if   !defined(CLAMP)
#define  CLAMP(x,min,max)   ( (x<min) ? min : ( (x>max) ? max : x ) )
#endif

const int receivesShadowTraversalMask = 0x1;
const int castsShadowTraversalMask = 0x2;

// get a valid scale value (1,2,5,10,20,50,...)

static double getNextVal (double curr)
{
    int OK = FALSE;
    double pas[3] = {1.,2.5,5.};
    int cycle = 0;
    double base = 0.;
    double factor = 1.;

    while (!OK)
    {
        /* la valeur de base est < a la valeur a depasser */
        if (base < curr)
        {
            /* on augmente de la valeur predefini */
            base = pas[cycle] * factor;
            cycle++;

            /* on arrive en bout de course, il faut passer a la decade
             * superieure */
            if (cycle > 2)
            {
                cycle = 0;
                factor *= 10.;
            }
        }
        else
            OK = TRUE;  /* c'est tout bon */
    }

    return(base);
}

// callback for fading node
//---------------------------------------------------------------------------------
class FadingUpdateCallback :
    public osg::NodeCallback
{
public:

    FadingUpdateCallback(osg::BlendColor *blendColor, size_t speed = 1) :
        m_pos(0),
        m_speed(speed),
        m_increment(speed),
        m_blendColor(blendColor)
    {}

    virtual ~FadingUpdateCallback() {}

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        m_pos += m_increment;
        m_blendColor->setConstantColor( osg::Vec4( 1.0,1.0,1.0,1.0 - (m_pos / 255.) ) );
        if (m_pos >= 255)
            node->setNodeMask(0x0);
    }

    void reset()
    {
        m_pos = 0;
        m_blendColor->setConstantColor( osg::Vec4(1.0,1.0,1.0,1.0) );
    }

private:

    size_t m_pos;
    size_t m_speed;
    int m_increment;
    osg::BlendColor *m_blendColor;
};

// compass update

//---------------------------------------------------------------------------------

class UpdateCompassCallback :
    public osg::NodeCallback
{
public:
    UpdateCompassCallback(osg::Camera* sceneCam, osg::MatrixTransform *mat ) :
        cam_(sceneCam),
        mat_(mat)
    {}

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        osg::Vec3f eye,centre,up;
        osg::Vec3f north(0.0f, 1.0f, 0.0f);

        cam_->getViewMatrixAsLookAt(eye,centre,up);

        up.normalize();
        Plane xy_plane(0.0, 0.0, 1.0, 0.0);
        up = xy_plane.ProjectVectorOnPlane(up);
        float angle = OsgMath::AngleBetweenTwoVectorsInRadians(up, north);
        if (up.x() < 0)
            angle *= -1.0f;

        mat_->setMatrix( osg::Matrix::rotate(angle,0.0f,0.0f,1.0f) );
    }

    osg::Camera* cam_;
    osg::MatrixTransform *mat_;
};


SceneView::SceneView( QWidget * parent) :
    OsgQWidget(parent),
    m_model(NULL),
    m_gradientBackground(NULL),
    m_bgColors(NULL),
    m_grid(NULL),
    m_axis(NULL),
    m_showGrid(false),
    m_currGridScale(1.0f),
    m_cameraCompass(NULL),
    m_selectionManager(NULL)
{
    m_atm = new PickingHandler();

	///反转鼠标滚轮
	m_atm->setEnabledInverseMouseWheel(true);
    m_view->setCameraManipulator( m_atm.get() );

    m_camera->setSmallFeatureCullingPixelSize( 0 );

    ScreenshotHandler *ssh = new ScreenshotHandler("screen_shot", "jpg");
    connect( ssh,SIGNAL( newScreenshotAvailable(osg::Image *) ),this,SIGNAL( newScreenshotAvailable(osg::Image *) ) );
 //   m_view->addEventHandler( new osgViewer::ScreenCaptureHandler(ssh) );

	//m_statsHandler = new StatisticsHandler();
	//m_view->addEventHandler(m_statsHandler);

	////! 状态切换
	//m_view->addEventHandler(new osgGA::StateSetManipulator(m_camera->getOrCreateStateSet()));


	//// add the record camera path handler
	//m_view->addEventHandler(new osgViewer::RecordCameraPathHandler);
    createSceneEnvironnement();

	//添加坐标捕捉
	osg::ref_ptr<osgText::Text> updatetext = new osgText::Text();
	CreateHUD *hudText= new CreateHUD();

	//添加到场景
	m_scene->addChild(hudText->createHUD(updatetext.get()));

	//添加PICK事件处理器
	m_view->addEventHandler(new CPickHandler(updatetext.get()));

    // by default all stateset are enabled
    setBackfaceEnabled(true);
    setLightingEnabled(true);
    setTextureEnabled(true);

	setMinimumSize( 400, 300 );
}

SceneView::~SceneView(void)
{}

void SceneView::createSceneEnvironnement()
{
    m_scene = new osg::Group(); // root node
    m_scene->setName("sceneGroup");

    //创建背景色节点
    m_clearColor = osg::Vec4f(0.1f,0.1f,0.2f,1.0f);
    m_backdrop = new osg::ClearNode;
    m_backdrop->setName("backdrop");
    m_backdrop->setClearColor(m_clearColor);
    m_scene->addChild(m_backdrop);

    //创建渐变背景节点
    m_gradientBackground = createBackground();
    m_scene->addChild(m_gradientBackground);

    // Attach the switch containing the root nodes
    m_rootNodes = new osgShadow::ShadowedScene();
    m_rootNodes->setReceivesShadowTraversalMask(receivesShadowTraversalMask);
    m_rootNodes->setCastsShadowTraversalMask(castsShadowTraversalMask);
    m_rootNodes->setDataVariance( osg::Object::DYNAMIC );

    m_scene->addChild(m_rootNodes);

    m_grid = makeGrid();
    m_scene->addChild(m_grid);
    m_axis = makeAxis();
    m_scene->addChild(m_axis);

    // hide by default
    m_grid->setNodeMask(0x0);
    m_axis->setNodeMask(0x0);

    // for compass
    //m_cameraCompass = createCompass();
    //m_scene->addChild(m_cameraCompass);

    createPivotManipulator();

    setSceneData(m_scene);
}

void SceneView::createPivotManipulator()
{
    // prepare pivot struct
    osg::Node *node = NULL;

    m_rotatePivot = new osg::AutoTransform();
    osg::Switch *sw = new osg::Switch;
    m_rotatePivot->addChild(sw);

    std::string pivot = osgDB::findDataFile("rotate_pivot.osg");
    node = osgDB::readNodeFile(pivot);
    if (node)
    {
        node->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS), osg::StateAttribute::ON);
        node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        sw->addChild(node);
    }

    std::string scalein = osgDB::findDataFile("scale_in_pivot.osg");
    node = osgDB::readNodeFile(scalein);
    if (node)
    {
        node->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS), osg::StateAttribute::ON);
        node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        sw->addChild(node);
    }

    std::string scaleout = osgDB::findDataFile("scale_out_pivot.osg");
    node = osgDB::readNodeFile(scaleout);
    if (node)
    {
        node->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS), osg::StateAttribute::ON);
        node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        sw->addChild(node);
    }

    std::string drag = osgDB::findDataFile("drag_pivot.osg");
    node = osgDB::readNodeFile(drag);
    if (node)
    {
        node->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS), osg::StateAttribute::ON);
        node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        sw->addChild(node);
    }

    sw->setAllChildrenOff();

    // transparency
    osg::StateSet *stateSet = sw->getOrCreateStateSet();

    osg::BlendFunc *blendFunc = new osg::BlendFunc(osg::BlendFunc::CONSTANT_ALPHA, osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);

    osg::BlendColor *blendColor = new osg::BlendColor ( osg::Vec4(1.0,1.0,1.0,1.0) );

    stateSet->setAttributeAndModes (blendFunc,osg::StateAttribute::ON);
    stateSet->setAttributeAndModes (blendColor,osg::StateAttribute::ON);
    stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    stateSet->setMode(GL_BLEND,   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

    sw->setUpdateCallback( new FadingUpdateCallback(blendColor,10) );

    m_scene->addChild(m_rotatePivot);
}

//-------------------------------------------------------------------------
// Pivot Functions

void SceneView::recenterPivotPoint(double x,double y,double z)
{
    m_rotatePivot->setPosition( osg::Vec3d(x,y,z) );
}

void SceneView::showPivot()
{
    double distance = getTrackballManipulator()->getDistance();
    m_rotatePivot->setScale(distance / distanceRatioPivot);
    osg::Switch *sw = dynamic_cast<osg::Switch *>( m_rotatePivot->getChild(0) );
    if (sw)
    {
        sw->setNodeMask(0xffffffff);
        sw->setSingleChildOn(PIVOT);
        FadingUpdateCallback *fu = dynamic_cast<FadingUpdateCallback *>( sw->getUpdateCallback() );
        if (fu)
            fu->reset();
    }
}

void SceneView::showZoomIn()
{
    double distance = getTrackballManipulator()->getDistance();
    m_rotatePivot->setScale(distance / distanceRatioPivot);
    osg::Switch *sw = dynamic_cast<osg::Switch *>( m_rotatePivot->getChild(0) );
    if (sw)
    {
        sw->setNodeMask(0xffffffff);
        sw->setSingleChildOn(ZOOMIN);
        FadingUpdateCallback *fu = dynamic_cast<FadingUpdateCallback *>( sw->getUpdateCallback() );
        if (fu)
            fu->reset();
    }
}

void SceneView::showZoomOut()
{
    double distance = getTrackballManipulator()->getDistance();
    m_rotatePivot->setScale(distance / distanceRatioPivot);
    osg::Switch *sw = dynamic_cast<osg::Switch *>( m_rotatePivot->getChild(0) );
    if (sw)
    {
        sw->setNodeMask(0xffffffff);
        sw->setSingleChildOn(ZOOMOUT);
        FadingUpdateCallback *fu = dynamic_cast<FadingUpdateCallback *>( sw->getUpdateCallback() );
        if (fu)
            fu->reset();
    }
}

void SceneView::showDrag()
{
    osg::Vec3d center = getTrackballManipulator()->getCenter();
    recenterPivotPoint( center.x(),center.y(),center.z() );
    double distance = getTrackballManipulator()->getDistance();
    m_rotatePivot->setScale(distance / distanceRatioPivot);

    osg::Switch *sw = dynamic_cast<osg::Switch *>( m_rotatePivot->getChild(0) );
    if (sw)
    {
        sw->setNodeMask(0xffffffff);
        sw->setSingleChildOn(DRAG);
        FadingUpdateCallback *fu = dynamic_cast<FadingUpdateCallback *>( sw->getUpdateCallback() );
        if (fu)
            fu->reset();
    }
}

void SceneView::pickGeometry(osg::Drawable *d)
{
    resetSelection();
}


SceneModel* SceneView::getModel()
{
	return m_model;
}

void SceneView::setModel(SceneModel *model)
{
    m_model = model;

    // clean previous elements
    for (int i = m_rootNodes->getNumChildren() - 1; i >= 0; i--)
        m_rootNodes->removeChild(i);

    if (m_model == NULL)
        return;

    connect( m_model, SIGNAL( loadBegin(bool) ), this, SLOT( resetView(bool) ) );
    connect( m_model, SIGNAL( loadFinished() ), this, SLOT( resetHome() ) );

    if (NULL != m_atm)
    {
        connect( m_atm.get(), SIGNAL( picked(osg::Drawable*) ),this, SIGNAL( picked(osg::Drawable*) ) );
        connect( m_atm.get(), SIGNAL( recenterViewTo(double,double,double) ),this, SLOT( recenterPivotPoint(double,double,double) ) );

        connect( m_atm.get(), SIGNAL( zoomViewIn() ),this, SLOT( showZoomIn() ) );
        connect( m_atm.get(), SIGNAL( zoomViewOut() ),this, SLOT( showZoomOut() ) );
        connect( m_atm.get(), SIGNAL( dragView() ),this, SLOT( showDrag() ) );
        connect( m_atm.get(), SIGNAL( rotateView() ),this, SLOT( showPivot() ) );
    }

    m_rootNodes->addChild( m_model->getScene() );
}

void SceneView::resetView(bool reset)
{
    m_resetHome = reset;
    if (!m_resetHome)
    {
        // get the previous values;
        m_matrix = getTrackballManipulator()->getMatrix();
        m_center = getTrackballManipulator()->getCenter();
        m_distance = getTrackballManipulator()->getDistance();
    }
}

void SceneView::resetHome()
{
    //m_view->getCameraManipulator()->setAutoComputeHomePosition(false);

    m_rootNodes->dirtyBound();

    // save the current bbox;
    ExtentsVisitor ext;
    m_rootNodes->accept(ext);

    osg::BoundingSphere bound = m_rootNodes->getBound();

    /*m_view->getCameraManipulator()->setHomePosition( bound.center() + osg::Vec3( 1.5f * bound.radius(),-3.0f * bound.radius(),1.5f * bound.radius() ),
                                                     bound.center(),
                                                     osg::Vec3(0.0f,0.0f,1.0f) );*/

	//! 初始化坐标系
	m_view->getCameraManipulator()->setHomePosition( bound.center() + osg::Vec3( 0,0,3.5f * bound.radius() ),
													bound.center(),
													osg::Vec3(0.0f,1.0f,0.0f) );
    
	//更新_Node
	m_view->getCameraManipulator()->setNode(m_rootNodes);
	
	// compute the new scale of grid if enable
    if (m_showGrid)
        setGridEnabled(true);

    if (m_resetHome)
    {
        home();
    }
    else
    {
        // restore the view point
        getTrackballManipulator()->setDistance(m_distance);
        getTrackballManipulator()->setByMatrix(m_matrix);
        getTrackballManipulator()->setCenter(m_center);
    }
}

void SceneView::home()
{
    osg::Vec3d eye;
    osg::Vec3d center;
    osg::Vec3d up;

    // reset the pivot center point
    m_view->getCameraManipulator()->getHomePosition(eye, center, up);
    recenterPivotPoint( center.x(),center.y(),center.z() );

    m_view->home();
}

bool SceneView::highlight(osg::Node* node)
{
    if (!m_selectionManager)
    {
        m_selectionManager = new SelectionManager;
        m_selectionManager->setSelectionDecorator(new ShaderSelectionDecorator);
    }

    resetSelection();
    m_selectionManager->select(node);
    return true;
}

void SceneView::setLODFactor(double val)
{
    m_camera->setLODScale(val);
}

void SceneView::setStatsEnabled(bool val)
{
    m_view->getEventQueue()->keyPress('s');
}

void SceneView::requestScreenShot()
{
    m_view->getEventQueue()->keyRelease('c');
    qDebug() << "requestScreenShot";
}

void SceneView::resizeEvent(QResizeEvent * event)
{
	if (m_statsHandler)
	    m_statsHandler->setWindowSize(width(), height());

    if (m_cameraCompass)
    {
        m_cameraCompass->setProjectionMatrix( osg::Matrix::ortho2D( 0.0,width(),0.0,height() ) );
        osg::MatrixTransform *mat = dynamic_cast<osg::MatrixTransform *>( m_cameraCompass->getChild(0) );
        mat->setMatrix( osg::Matrix::translate(width() - compassSize / 2.0f - compassOffset,compassSize / 2.0f + compassOffset,0.0f) );
    }

    // inform that aspect rastio has changed
    emit newAspectRatio( QSize( width(), height() ) );
    QWidget::resizeEvent(event);
}

void SceneView::resetSelection()
{
    if (m_selectionManager)
        m_selectionManager->clearSelection();
}

void SceneView::setViewPoint(double distance, const osg::Vec3 & center, const osg::Matrixd& matrixEnd)
{
    getTrackballManipulator()->setByMatrix(matrixEnd);
    getTrackballManipulator()->setDistance(distance);
    getTrackballManipulator()->setCenter(center);

    getUpdateVisitor()->setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
}

osg::Matrix SceneView::matrixListtoSingle(const osg::MatrixList &tmplist)
{
    osg::Matrix tmp;

    if (tmplist.size() > 0)
    {
        unsigned int i;
        for (i = 1, tmp = tmplist[0]; i < tmplist.size(); i++)
            tmp *= tmplist[0];
        tmp = tmplist[0];
    }
    return (tmp);
}

//-----------------------------------------------------------------------------
// Background color settings
//-----------------------------------------------------------------------------

void SceneView::setFlatBackgroundColor(bool val)
{
    if (val)
        m_gradientBackground->setNodeMask(0x0);
    else
        m_gradientBackground->setNodeMask(0xffffffff);
}

void SceneView::setBgColor(const QColor &color)
{
    float red, green, blue;

    red = color.red() / 255.0f;
    green = color.green() / 255.0f;
    blue = color.blue() / 255.0f;

    m_clearColor = osg::Vec4f(red,green,blue,1.0f);
    
	///将背景色更新到背景节点
	m_backdrop->setClearColor(m_clearColor);

	///根据clearcolor生成渐变色
    setGradientBgColor();
}

void SceneView::setGradientBgColor()
{
    m_bgColors->clear();

    m_bgColors->push_back( osg::Vec4f(CLAMP(m_clearColor[0] - 0.1,0.,1.),
                                      CLAMP(m_clearColor[1] - 0.1,0.,1.),
                                      CLAMP(m_clearColor[2] - 0.1,0.,1.),
                                      1.0f) );
    m_bgColors->push_back( osg::Vec4f(CLAMP(m_clearColor[0] - 0.1,0.,1.),
                                      CLAMP(m_clearColor[1] - 0.1,0.,1.),
                                      CLAMP(m_clearColor[2] - 0.1,0.,1.),
                                      1.0f) );
    m_bgColors->push_back( osg::Vec4f(CLAMP(m_clearColor[0] + 0.2,0.,1.),
                                      CLAMP(m_clearColor[1] + 0.2,0.,1.),
                                      CLAMP(m_clearColor[2] + 0.2,0.,1.),
                                      1.0f) );
    m_bgColors->push_back( osg::Vec4f(CLAMP(m_clearColor[0] + 0.2,0.,1.),
                                      CLAMP(m_clearColor[1] + 0.2,0.,1.),
                                      CLAMP(m_clearColor[2] + 0.2,0.,1.),
                                      1.0f) );
}

QColor SceneView::getBgColor() const
{
    return QColor ( (int)(m_clearColor[0] * 255.0f),(int)(m_clearColor[1] * 255.0f),(int)(m_clearColor[2] * 255.0f) );
}

osg::Vec4f SceneView::getVec4BgColor() const
{
    return m_clearColor;
}

osg::Projection*  SceneView::createBackground()
{
    m_bgColors = new osg::Vec4Array;

    osg::Geode* bgGeode = new osg::Geode();
    bgGeode->setName("Background");


    osg::Projection* HUDProjectionMatrix = new osg::Projection();
    HUDProjectionMatrix->setMatrix( osg::Matrix::ortho2D(0,1024,0,1024) );
    osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
    HUDModelViewMatrix->setMatrix( osg::Matrix::identity() );
    HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    HUDProjectionMatrix->addChild(HUDModelViewMatrix);
    HUDModelViewMatrix->addChild( bgGeode );

    osg::Geometry* HUDBackgroundGeometry = new osg::Geometry();
    HUDBackgroundGeometry->setUseDisplayList(false);

    osg::Vec3Array* HUDBackgroundVertices = new osg::Vec3Array;
    HUDBackgroundVertices->push_back( osg::Vec3( 0,    0,-100) );
    HUDBackgroundVertices->push_back( osg::Vec3(1024,  0,-100) );
    HUDBackgroundVertices->push_back( osg::Vec3(1024,1024,-100) );
    HUDBackgroundVertices->push_back( osg::Vec3(   0,1024,-100) );

    osg::DrawElementsUInt* HUDBackgroundIndices = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
    HUDBackgroundIndices->push_back(0);
    HUDBackgroundIndices->push_back(1);
    HUDBackgroundIndices->push_back(2);
    HUDBackgroundIndices->push_back(3);

    setGradientBgColor();

    osg::Vec2Array* texcoords = new osg::Vec2Array(4);
    (*texcoords)[0].set(0.0f,0.0f);
    (*texcoords)[1].set(1.0f,0.0f);
    (*texcoords)[2].set(1.0f,1.0f);
    (*texcoords)[3].set(0.0f,1.0f);

    HUDBackgroundGeometry->setTexCoordArray(0,texcoords);

    osg::Vec3Array* HUDnormals = new osg::Vec3Array;
    HUDnormals->push_back( osg::Vec3(0.0f,0.0f,1.0f) );
    HUDBackgroundGeometry->setNormalArray(HUDnormals);
    HUDBackgroundGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
    HUDBackgroundGeometry->addPrimitiveSet(HUDBackgroundIndices);
    HUDBackgroundGeometry->setVertexArray(HUDBackgroundVertices);
    HUDBackgroundGeometry->setColorArray(m_bgColors);
    HUDBackgroundGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    bgGeode->addDrawable(HUDBackgroundGeometry);

    // Create and set up a state set using the texture from above:
    osg::StateSet* HUDStateSet = new osg::StateSet();
    bgGeode->setStateSet(HUDStateSet);

     // Disable depth testing so geometry is draw regardless of depth values
    // of geometry already draw.
    HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    HUDStateSet->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    // Need to make sure this geometry is draw last. RenderBins are handled
    // in numerical order so set bin number to 11
    HUDStateSet->setRenderBinDetails( -1, "RenderBin");

    return HUDProjectionMatrix;
}

osg::MatrixTransform *SceneView::makeGrid()
{
    // Turn on FSAA, makes the lines look better.
    //osg::DisplaySettings::instance()->setNumMultiSamples( 4 );

    osg::MatrixTransform *transform = new osg::MatrixTransform();
    osg::Geode *geode = new osg::Geode();
    osg::Geometry *geom = new osg::Geometry();
    osg::Vec3Array *vertices = new osg::Vec3Array();
    int i;
    int numLines = 10;
    float radius = 10.;
    vertices->push_back( osg::Vec3(0.0f,-radius,0.0f) );
    vertices->push_back( osg::Vec3(0.0f, radius,0.0f) );
    vertices->push_back( osg::Vec3(-radius,0.0f,0.0f) );
    vertices->push_back( osg::Vec3( radius,0.0f,0.0f) );
    vertices->push_back( osg::Vec3(0.0f, 0.0f,-radius) );
    vertices->push_back( osg::Vec3(0.0f, 0.0f, radius) );
    for (i = 1; i<=numLines; i++)
    {
        vertices->push_back( osg::Vec3(i,-radius,0.0f) );
        vertices->push_back( osg::Vec3(i,radius,0.0f) );
        vertices->push_back( osg::Vec3(-i,-radius,0.0f) );
        vertices->push_back( osg::Vec3(-i,radius,0.0f) );
        vertices->push_back( osg::Vec3(-radius,i,0.0f) );
        vertices->push_back( osg::Vec3(radius,i,0.0f) );
        vertices->push_back( osg::Vec3(-radius,-i,0.0f) );
        vertices->push_back( osg::Vec3(radius,-i,0.0f) );
    }
    geom->setVertexArray(vertices);
    osg::Vec4Array* colors = new osg::Vec4Array();
    colors->push_back( osg::Vec4(.9f,.9f,.9f,1.0f) );
    colors->push_back( osg::Vec4(.9f,.9f,.9f,1.0f) );
    geom->setColorArray(colors);
    geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back( osg::Vec3(0.0f,-1.0f,0.0f) );
    geom->setNormalArray(normals);
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

    geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES,0,6) );
    geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES,6,vertices->size() - 6) );
    geode->addDrawable(geom);
    osg::StateSet *set = new osg::StateSet();
    set->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    set->setAttributeAndModes(new osg::PolygonOffset(1.0f,1.0f),osg::StateAttribute::ON);
    set->setMode(GL_LINE_SMOOTH,osg::StateAttribute::ON);
    geode->setStateSet(set);
    transform->addChild(geode);
    transform->setName("transformGrid");
    return transform;
}

osg::MatrixTransform *SceneView::makeAxis()
{
    osg::MatrixTransform *transform = new osg::MatrixTransform();
    osg::Geode *geode = new osg::Geode();
    osg::Geometry *geom = new osg::Geometry();
    osg::Vec3Array *vertices = new osg::Vec3Array();

    vertices->push_back( osg::Vec3(0,0,0) );
    vertices->push_back( osg::Vec3(1,0,0) );

    vertices->push_back( osg::Vec3(0,0,0) );
    vertices->push_back( osg::Vec3(0,1,0) );

    vertices->push_back( osg::Vec3(0,0,0) );
    vertices->push_back( osg::Vec3(0,0,1) );

    geom->setVertexArray(vertices);
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back( osg::Vec4(1.0f,0.0f,0.0f,1.0f) );
    colors->push_back( osg::Vec4(0.0f,1.0f,0.0f,1.0f) );
    colors->push_back( osg::Vec4(0.0f,0.0f,1.0f,1.0f) );
    geom->setColorArray(colors);
//    geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);

    geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2) );
    geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES,2,2) );
    geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES,4,2) );
    geode->addDrawable(geom);
    osg::StateSet *set = new osg::StateSet();
    set->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    osg::LineWidth *width = new osg::LineWidth();
    width->setWidth(3.0f);
    set->setAttributeAndModes(width, osg::StateAttribute::ON);
    geode->setStateSet(set);
    transform->addChild(geode);
    transform->setName("transformAxis");
    return transform;
}

void SceneView::setGridEnabled(bool val)
{
    m_showGrid = val;
    if (m_showGrid)
    {
        float scale = m_rootNodes->getBound().radius();
        scale = getNextVal(scale);
        if (scale > 0.0)
        {
            osg::Matrixd mat;
            mat.makeIdentity();
            float val = scale;
            mat.makeScale( osg::Vec3f(val / 8.0f,val / 8.0f,val / 8.0f) );
            m_grid->setMatrix(mat);
            m_axis->setMatrix(mat);
            m_currGridScale = scale;
        }

        m_grid->setNodeMask(0xffffffff);
        m_axis->setNodeMask(0xffffffff);
    }
    else
    {
        m_grid->setNodeMask(0x0);
        m_axis->setNodeMask(0x0);
    }
}

void SceneView::setShadowEnabled(bool val)
{
    if (val)
    {
        int alg = 3;
        const osg::BoundingSphere& bs = m_rootNodes->getBound();

        // test bidon pour modifier l'algo d'ombrage en fonction de la taille de l'objet
        if (alg == 0)
        {
            osg::ref_ptr<osgShadow::ParallelSplitShadowMap> pssm = new osgShadow::ParallelSplitShadowMap(NULL,5);
            pssm->setTextureResolution(2048);
            m_rootNodes->setShadowTechnique( pssm.get() );

            // blindage mais je ne sais pas trop pourquoi ?!?
            pssm->init();
        }
        else if (alg == 2)
        {
            osg::ref_ptr<osgShadow::SoftShadowMap> pssm = new osgShadow::SoftShadowMap();

            pssm->setTextureSize( osg::Vec2s(2048,2048) );

            //pssm->setTextureResolution(2048);
            m_rootNodes->setShadowTechnique( pssm.get() );

            // blindage mais je ne sais pas trop pourquoi ?!?
            pssm->init();
        }
        else if (alg == 3)
        {
            osg::ref_ptr<osgShadow::MinimalShadowMap> pssm = new osgShadow::LightSpacePerspectiveShadowMapDB();

            pssm->setTextureSize( osg::Vec2s(2048,2048) );

            //pssm->setTextureResolution(2048);
            m_rootNodes->setShadowTechnique( pssm.get() );

            // blindage mais je ne sais pas trop pourquoi ?!?
            pssm->init();
        }
    }
    else
    {
        m_rootNodes->setShadowTechnique(NULL);
    }
}

void SceneView::setCompassEnabled(bool val)
{
    if (val)
        m_cameraCompass->setNodeMask(0xffffffff);
    else
        m_cameraCompass->setNodeMask(0x0);
}

bool SceneView::isCompassEnabled() const
{
    return m_cameraCompass->getNodeMask() != 0x0;
}

osg::Camera* SceneView::createCompass()
{
    osg::Geode *geodeNail = new osg::Geode();
    geodeNail->setName("Nail");
    osg::Geode *geodeCompass = new osg::Geode();
    geodeCompass->setName("Compass");

    // nail !!
    {
        osg::Image* image = osgDB::readImageFile("D:\\osGraphX-1.0.0-code\\data\\nail.png");
        osg::Vec3 pos(-compassSize / 2.0f,-compassSize / 2.0f,0.0f);
        osg::Vec3 width(compassSize,0,0);
        osg::Vec3 height(0.0f,compassSize,0);
        osg::Geometry* geometry = osg::createTexturedQuadGeometry(pos,width,height);
        osg::StateSet* stateset = geometry->getOrCreateStateSet();
        stateset->setTextureAttributeAndModes(0,new osg::Texture2D(image),osg::StateAttribute::ON);
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
        stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        geodeNail->addDrawable(geometry);
    }

    // compass !!
    {
        osg::Image* image = osgDB::readImageFile("D:\\osGraphX-1.0.0-code\\data\\dial.png");
        osg::Vec3 pos(-compassSize / 2.0f,-compassSize / 2.0f,-1.0f);
        osg::Vec3 width(compassSize,0,0);
        osg::Vec3 height(0.0f,compassSize,0);
        osg::Geometry* geometry = osg::createTexturedQuadGeometry(pos,width,height);
        osg::StateSet* stateset = geometry->getOrCreateStateSet();
        stateset->setTextureAttributeAndModes(0,new osg::Texture2D(image),osg::StateAttribute::ON);
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
        stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        geodeCompass->addDrawable(geometry);
    }

    osg::Camera* camera = new osg::Camera;

    // set the projection matrix
    camera->setProjectionMatrix( osg::Matrix::ortho2D( 0,width(),0,height() ) );

    // set the view matrix
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix( osg::Matrix::identity() );

    // only clear the depth buffer
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);

    // draw subgraph after main camera view.
    camera->setRenderOrder(osg::Camera::POST_RENDER);

    osg::MatrixTransform* HUDModelRotateMatrix = new osg::MatrixTransform;
    HUDModelRotateMatrix->addChild(geodeNail);

    osg::MatrixTransform* HUDModelPositionMatrix = new osg::MatrixTransform;
    HUDModelPositionMatrix->addChild(HUDModelRotateMatrix);
    HUDModelPositionMatrix->addChild(geodeCompass);

    // position of compass
    HUDModelPositionMatrix->setMatrix( osg::Matrix::translate(width() - compassSize - compassOffset,compassOffset,0.0f) );
    camera->addChild(HUDModelPositionMatrix);

    geodeNail->setUpdateCallback( new UpdateCompassCallback(m_camera,HUDModelRotateMatrix) );

    return camera;
}

void SceneView::setEnabledTrackbalHelper(bool val)
{
    m_atm->setEnabledTrackballHelper(val);
}

void SceneView::setEnabledInverseMouseWheel(bool val)
{
    m_atm->setEnabledInverseMouseWheel(val);
}

bool SceneView::centerOnNode(osg::Node* node)
{
    if (!node)
        return false;

    const osg::BoundingSphere& bs = node->getBound();
    if (bs.radius() < 0.0) // invalid node
        return false;

    // world matrix transform
    osg::Matrix mat = matrixListtoSingle( node->getWorldMatrices() );
    m_atm->setCenter(bs.center() * mat);
    m_atm->setDistance( 3.0 * bs.radius() );

    return true;
}
