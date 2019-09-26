#include "UIFacade.h"

#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include <QCoreApplication>

#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QDateTime>
#include <QProcess>
#include <QMenu>
#include <QTreeWidgetItem>
#include <QToolBar>


#include <osg/Notify>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/TexGen>
#include <osg/TexEnv>

#include <osgSim/OverlayNode>
#include <osgDB/FileUtils>

#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/Registry>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/LogarithmicDepthBuffer>
#include <osgEarthUtil/ExampleResources>

#include <gdal_priv.h>

#include <ONodeManager/DataManager.h>

//DCScene
#include "DCScene/scene/SceneView.h"
#include "DCScene/scene/SceneModel.h"

#include <DC/MouseEventHandler.h>
#include <DC/SettingsManager.h>
#include <ONodeManager/MPluginManager.h>


class LogFileHandler : public osg::NotifyHandler
{
	const std::string  severityTag[osg::DEBUG_FP + 1] = {
		"ALWAYS",
		"FATAL",
		"WARN",
		"NOTICE",
		"INFO",
		"DEBUG_INFO",
		"DEBUG_FP"
	};

public:

	LogFileHandler(std::ofstream *outStream) :
		_log(outStream)
	{
	}

	virtual void  notify(osg::NotifySeverity severity, const char *msg)
	{
		if (_log)
		{
			(*_log) << "[osg]    " << "[" << severityTag[severity] << "]    " << msg;
			_log->flush();
		}
	}

	~LogFileHandler()
	{
	}

protected:
	std::ofstream *_log = NULL;
};

UIFacade::UIFacade(QWidget *parent, Qt::WindowFlags flags):
	MainWindow(parent, flags)
{
	// Some global environment settings
	QCoreApplication::setOrganizationName("DCLW");
	QCoreApplication::setApplicationName("OpenViz");

	GDALAllRegister();
	CPLSetConfigOption("GDAL_DATA", ".\\resources\\GDAL_data");

	osg::DisplaySettings::instance()->setNumOfHttpDatabaseThreadsHint(8);
	osg::DisplaySettings::instance()->setNumOfDatabaseThreadsHint(2);
}

UIFacade::~UIFacade()
{
	osg::setNotifyLevel(osg::FATAL);
	osg::setNotifyHandler(nullptr);
	osgEarth::setNotifyHandler(nullptr);

	cout << "Program closed: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str() << endl;
	_log->close();
	delete _log;
}

void UIFacade::initDCUIVar()
{
	emit  sendNowInitName(tr("Initializing DCCore"));

	_root = new osg::Group;
	_root->setName("Root");

	_settingsManager = new SettingsManager(this);
	SetSettingManager(_settingsManager);
	_dataManager = new DataManager(this);


	// thread-safe initialization of the OSG wrapper manager. Calling this here
	// prevents the "unsupported wrapper" messages from OSG
	osgDB::Registry::instance()->getObjectWrapperManager()->findWrapper("osg::Image");

	_pluginManager = new MPluginManager(this, _dataManager, m_pCurrentNewViewer);

	//! 通过外部传入插件组、插件根toolbar、插件根menu
	QToolBar* dataToolBar = new QToolBar(this);
	dataToolBar->setWindowTitle("Data Manager");
	addToolBar(dataToolBar);
	_pluginManager->registerPluginGroup("Data", dataToolBar,  nullptr);

	connect(_dataManager, &DataManager::requestContextMenu, _pluginManager, &MPluginManager::loadContextMenu);
	connect(_pluginManager, &MPluginManager::sendNowInitName, this, &UIFacade::sendNowInitName);
}

void  UIFacade::initAll()
{
	collectInitInfo();

	emit  sendNowInitName(tr("Initializing log"));
	initLog();

	//! 初始化dc库中的基础base变量
	initDCUIVar();

	emit  sendNowInitName(tr("Initializing UI"));

	//! 生成界面，创建datamanager
	setupUi();

	//初始化一个 view,绑定空node，必须在initDCUIVar前
	initView();
	

	//！ 初始化加载一个场景数据，作为根节点，传递给dataManager
	initDataManagerAndScene();

	emit  sendNowInitName(tr("Initializing camera"));
	resetCamera();

	initPlugins();

	emit  sendNowInitName(tr("Stylizing UI"));
	initUiStyles();
}

void  UIFacade::setupUi()
{
	bool initState = ConfigInit(this);

	if (initState)
	{
		//！ 中心三维视窗
		SetCentralWidget();

		//！信号槽
		Init();

		//！ 初始化docketwidget
		//InitManager();
		if (_dataManager)
		{
			//! 初始化node管理面板
			_dataManager->setupUi(this);
		}

		LoadSettings();
	}

	ConfigFinish(this);
}

void  UIFacade::initView()
{
	//! 初始化viewWidget
	emit  sendNowInitName(tr("Initializing ViewWidget"));

	initViewWidget();

	if (m_pCurrentNewViewer)
	{
		m_pCurrentNewViewer->getMainView()->getCamera()->setCullMask(SHOW_IN_WINDOW_1);
	}
	
}

void  UIFacade::initPlugins()
{
	// MouseEventHandler is the shared core of all plugins
	_mousePicker = new MouseEventHandler();
	_mousePicker->registerData(this, _dataManager, m_pCurrentNewViewer, _root, _settingsManager->getGlobalSRS());
	_mousePicker->registerSetting(_settingsManager);
	_mousePicker->setupUi(statusBar());
	m_pCurrentNewViewer->getMainView()->addEventHandler(_mousePicker);

	_pluginManager->loadPlugins();
}

void  UIFacade::initDataManagerAndScene()
{
	emit  sendNowInitName(tr("Initializing DataScene"));

	_mapRoot = new osg::Group;
	_mapRoot->setName("Map Root");

	_drawRoot = new osg::Group;
	_drawRoot->setName("Draw Root");
	// Draw root should not be intersected
	_drawRoot->setNodeMask(0xffffffff & (~INTERSECT_IGNORE));

	_dataRoot = new osg::Group;
	_dataRoot->setName("Data Root");

	// Init osgEarth node using the predefined .earth file
	for (int i = 0; i < MAX_SUBVIEW; i++)
	{
		QString  mode = getOrAddSetting("Base mode", "geocentric").toString();
		QString  baseMapPath;

		if (mode == "projected")
		{
			baseMapPath = QStringLiteral("Resources/earth_files/projected.earth");
		}
		else if (mode == "geocentric")
		{
			baseMapPath = QStringLiteral("Resources/earth_files/geocentric.earth");
		}
		else
		{
			QMessageBox::warning(nullptr, "Warning", "Base map settings corrupted, reset to projected");
			setOrAddSetting("Base mode", "projected");
			baseMapPath = QStringLiteral("Resources/earth_files/projected.earth");
		}

		osg::ref_ptr<osgDB::Options>  myReadOptions = osgEarth::Registry::cloneOrCreateOptions(0);
		osgEarth::Config              c;
		c.add("elevation_smoothing", false);
		osgEarth::TerrainOptions  to(c);
		osgEarth::MapNodeOptions  defMNO;
		defMNO.setTerrainOptions(to);

		myReadOptions->setPluginStringData("osgEarth.defaultOptions", defMNO.getConfig().toJSON());

		osg::Node *baseMap = osgDB::readNodeFile(baseMapPath.toStdString(), myReadOptions);
		_mapNode[i] = osgEarth::MapNode::get(baseMap);
		_mapNode[i]->setName(QString("Map%1").arg(i).toStdString());
		_mapNode[i]->setNodeMask((SHOW_IN_WINDOW_1 << i) | SHOW_IN_NO_WINDOW);
		_mapNode[i]->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		_mainMap[i] = _mapNode[i]->getMap();

		_mapRoot->addChild(_mapNode[i]);
	}

	_settingsManager->setGlobalSRS(_mainMap[0]->getSRS());

	// Init overlayNode with overlayerSubgraph
	// Everything in overlaySubgraph will be projected to its children
	_dataOverlay = new osgSim::OverlayNode(osgSim::OverlayNode::OBJECT_DEPENDENT_WITH_ORTHOGRAPHIC_OVERLAY);
	_dataOverlay->setName("Data Overlay");
	_dataOverlay->getOrCreateStateSet()->setAttributeAndModes(
		new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA));
	_dataOverlay->setOverlayBaseHeight(-1);
	_dataOverlay->setOverlayTextureSizeHint(2048);
	_dataOverlay->setOverlayTextureUnit(3);

	_overlaySubgraph = new osg::Group;
	_dataOverlay->setOverlaySubgraph(_overlaySubgraph);
	_dataOverlay->addChild(_dataRoot);

	_root->addChild(_dataOverlay);
	_root->addChild(_drawRoot);
	_root->addChild(_mapRoot);

	
	//! 更新场景数据
	if (m_pCurrentNewViewer)
	{
		_dataManager->registerDataRoots(_root);

		m_pCurrentNewViewer->getModel()->setData(_root);

		m_pCurrentNewViewer->resetHome();
	}
	
}

void  UIFacade::resetCamera()
{
	//if (_mainMap[0]->isGeocentric())
	//{
	//	osg::ref_ptr<osgEarth::Util::EarthManipulator>  manipulator =
	//		dynamic_cast<osgEarth::Util::EarthManipulator *>(_mainViewerWidget->getMainView()->getCameraManipulator());

	//	if (!manipulator.valid())
	//	{
	//		manipulator = new osgEarth::Util::EarthManipulator;
	//		_mainViewerWidget->getMainView()->setCameraManipulator(manipulator);
	//	}
	//	else
	//	{
	//		manipulator->home(0);
	//	}

	//	auto  settings = manipulator->getSettings();
	//	settings->setSingleAxisRotation(true);
	//	settings->setMinMaxDistance(10000.0, settings->getMaxDistance());
	//	settings->setMaxOffset(5000.0, 5000.0);
	//	settings->setMinMaxPitch(-90, 90);
	//	settings->setTerrainAvoidanceEnabled(true);
	//	settings->setThrowingEnabled(false);
	//}
	//else
	//{
	//	MapController *manipulator = dynamic_cast<MapController *>(_mainViewerWidget->getMainView()->getCameraManipulator());

	//	if (!manipulator)
	//	{
	//		// Init a manipulator if not inited yet
	//		manipulator = new MapController(_dataRoot, _mapRoot, _mainMap[0]->getSRS());
	//		manipulator->setAutoComputeHomePosition(false);

	//		if (_settingsManager->getOrAddSetting("Camera indicator", false).toBool())
	//		{
	//			manipulator->setCenterIndicator(_mainViewerWidget->createCameraIndicator());
	//		}

	//		// Nearfar mode and ratio affect scene clipping
	//		auto  camera = _mainViewerWidget->getMainView()->getCamera();
	//		camera->setComputeNearFarMode(osg::Camera::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);

	//		connect(_dataManager, &DataManager::moveToNode,
	//		        manipulator, &MapController::fitViewOnNode);
	//		connect(_dataManager, &DataManager::moveToBounding,
	//		        manipulator, &MapController::fitViewOnBounding);

	//		_mainViewerWidget->getMainView()->setCameraManipulator(manipulator);
	//		manipulator->registerWithView(_mainViewerWidget->getMainView(), 0);
	//	}

	//	manipulator->fitViewOnNode(_mapNode[0]);
	//}
}

void  qtLogToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	const string  logTypes[] = { "Debug", "Warning", "Critical", "Fatal", "Info" };

	cout << "[Qt]    [" << logTypes[type] << "]    " << msg.toLocal8Bit().constData();
#ifndef NDEBUG
	cout << "    (" << context.file << ": " << context.line << ", " << context.function;
#endif
	cout << endl;
}

void  UIFacade::initLog()
{
	// Open log file
#ifdef _WIN32
	const char *appData = getenv("APPDATA");
#else
	const char *appData = "/tmp";
#endif

	std::string  logDir = QString("%1/%2/%3")
	                      .arg(appData)
	                      .arg(QApplication::organizationName())
	                      .arg(QApplication::applicationName())
	                      .toStdString();

	if (!osgDB::fileExists(logDir))
	{
		osgDB::makeDirectory(logDir);
	}

	std::string  logPath = logDir + "/OpenVizLog.txt";
	_log = NULL;
	_log = new std::ofstream(logPath.c_str());

	if (!_log)
	{
		return;
	}

	// Redirect std iostream
	std::cout.rdbuf(_log->rdbuf());
	std::cerr.rdbuf(_log->rdbuf());

	// Redirect qt logs to stdout, thus to our log file
	qInstallMessageHandler(qtLogToFile);

	// Redirect OSGEarth
	osgEarth::setNotifyLevel(osg::INFO);
	osgEarth::setNotifyHandler(new LogFileHandler(_log));

	// Redirect OSG
	osg::setNotifyLevel(osg::NOTICE);
	osg::setNotifyHandler(new LogFileHandler(_log));

	cout << "Program started: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str() << endl;
}

void  UIFacade::collectInitInfo()
{
	// TODO: find a better way to collect initialization info
	int   initSteps = 7;
	QDir  pluginsDir(qApp->applicationDirPath());

	pluginsDir.cd("plugins");

	// Parsing plugin dependencies
	foreach(const QString &fileName, pluginsDir.entryList(QDir::Files))
	{
		if (fileName.split('.').back() != "dll")
		{
			continue;
		}

		initSteps++;
	}
	emit  sendTotalInitSteps(initSteps);
}



