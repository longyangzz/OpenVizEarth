#include "SceneModel.h"
#include "ExtentsVisitor.h"

#include <osg/PolygonOffset>
#include <osg/ShapeDrawable>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgDB/WriteFile>
#include <osg/CullFace>

#include <osgManipulator/CommandManager>
#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TrackballDragger>

#include <osgUtil/Statistics>

//DC
#include "../../DC/OsgLogger.h"
#include "../../DC/LogHandler.h"

// typedefs and definitions
#if   !defined(CLAMP)
#define  CLAMP(x,min,max)   ( (x<min) ? min : ( (x>max) ? max : x ) )
#endif

const int receivesShadowTraversalMask = 0x1;
const int castsShadowTraversalMask = 0x2;

// --------------------------------------------------------------------------------
SceneModel::SceneModel(QObject *parent) :
    QObject(parent),
    m_highlightScene(false),
    m_sceneCenter( osg::Vec3(0.0f,0.0f,0.0f) )
{
    // set the osg log to QT message
    osg::setNotifyLevel(osg::NOTICE);
    OsgLogger *loggerCout = new OsgLogger(std::cout);
    connect(loggerCout, SIGNAL( message(const QString &) ), LogHandler::getInstance(), SLOT( reportDebug(const QString &) ),Qt::QueuedConnection);
    OsgLogger *loggerCerr = new OsgLogger(std::cerr);
    connect(loggerCerr, SIGNAL( message(const QString &) ),  LogHandler::getInstance(), SLOT( reportInfo(const QString &) ),Qt::QueuedConnection);

    //osg::setNotifyLevel(osg::DEBUG_INFO);
    //osg::setNotifyLevel(osg::NOTICE);

    createScene();
}

SceneModel::~SceneModel()
{}

void SceneModel::resetModel()
{
    setData( new osg::Node() );
}

bool SceneModel::saveSceneData(const std::string & file)
{
    return osgDB::writeNodeFile(*m_currentData, file);
}

void SceneModel::setHighlightScene(bool val)
{
    if (m_highlightScene == val)
        return;

    m_highlightScene = val;
    if (m_highlightScene)
    {
        osg::Group *parent = m_NodeScene->getParent(0);
        m_currentHightlight = new osgFX::Scribe();
        m_currentHightlight->setWireframeLineWidth(2.0);
        m_currentHightlight->setWireframeColor( osg::Vec4(1.0,1.0,1.0,1.0) );

        m_currentHightlight->addChild(m_NodeScene);
        parent->replaceChild(m_NodeScene,m_currentHightlight);
    }
    else
    {
        osgFX::Scribe* parentAsScribe = m_currentHightlight;
        osg::Node::ParentList parentList = parentAsScribe->getParents();
        for(osg::Node::ParentList::iterator itr = parentList.begin();
            itr!=parentList.end();
            ++itr)
            (*itr)->replaceChild( parentAsScribe,parentAsScribe->getChild(0) );
    }
}

osg::Node *SceneModel::getScene()
{
    return m_switchRoot;
}

osg::Node *SceneModel::getObject()
{
    return m_currentData;
}

void SceneModel::createScene()
{
    // init
    m_switchRoot = new osg::Switch(); // switch
    m_switchRoot->setName("rootSwitch");

    m_switchRoot->addChild( createSceneLight() );

    // create and attach the scene nodes
    m_transformSpinScene = new osg::MatrixTransform; // spin transform for global scene
    m_transformSpinScene->setName("spinTransformScene");
    m_switchRoot->addChild(m_transformSpinScene);

    m_NodeScene = new osg::Group(); // scene
    m_NodeScene->setName("NodeScene");

    m_transformSpinScene->addChild(m_NodeScene);

    m_currentData = new osg::Node();
    m_currentData->setName("currentData");

    m_NodeScene->addChild( m_currentData.get() );
}

osg::Group * SceneModel::createSceneLight()
{
    osg::Group *grp = new osg::Group;

    // create and attach the light source
    osg::LightSource *lightSource = new osg::LightSource;
    lightSource->setLocalStateSetModes( osg::StateAttribute::ON );

    lightSource->getLight()->setAmbient( osg::Vec4(0.4,0.4,0.4,1.0) );
    lightSource->getLight()->setDiffuse( osg::Vec4(0.6,0.6,0.6,1.0) );
    lightSource->getLight()->setPosition( osg::Vec4(0.f,1000.0f,1000.0f,0.0f) );
    grp->addChild(lightSource);

    return grp;
}

void SceneModel::setData(osg::Node *data, bool resetHome)
{
    osg::Matrixd matrix;
    osg::Vec3d center;

    // reset selection
    emit loadBegin(resetHome);

    m_sceneCenter = osg::Vec3(0.0f,0.0f,0.0f);

    //m_NodeScene->removeChild( m_currentData.get() );
    m_currentData = data;

    if (!data)
        return;

    m_currentData->setName("currentData");

    m_NodeScene->addChild( m_currentData.get() );

    // translate all the underlay layers to bottom
    ExtentsVisitor ext;
    m_currentData->accept(ext);

    m_sceneCenter = ext.GetBound().center();

    if (m_highlightScene)
    {
        setHighlightScene(false);
        setHighlightScene(true);
    }


	//! 加载数据完成，则调用sceneview的resetHome方法。
    emit loadFinished();
}
