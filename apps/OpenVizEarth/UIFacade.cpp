#pragma execution_character_set("utf-8")

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
#include <QMenuBar>
#include <QTreeWidgetItem>
#include <QToolBar>
#include <QToolButton>
#include <QTranslator>

#include <osg/Notify>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/CullFace>

#include <osgSim/OverlayNode>
#include <osgDB/FileUtils>

#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/Registry>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/LogarithmicDepthBuffer>
#include <osgEarthUtil/ExampleResources>
#include <osgEarth/ViewPoint>
#include <gdal_priv.h>

#include "DC/DataType.h"
#include "DC/LogHandler.h"


#include "ONodeManager/NXDockWidget.h"
#include "ONodeManager/NXDockWidgetTabBar.h"
#include "ONodeManager/NXDockWidgetTabButton.h"

#include <DC/MouseEventHandler.h>
#include <DC/SettingsManager.h>
#include <DC/MapController.h>
#include <DC/MPluginManager.h>


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
	QCoreApplication::setOrganizationName("WLY");
	QCoreApplication::setApplicationName("OpenViz");
	QCoreApplication::setOrganizationDomain("mysoft.com");

	GDALAllRegister();
	CPLSetConfigOption("GDAL_DATA", ".\\resources\\GDAL_data");

	osg::DisplaySettings::instance()->setNumOfHttpDatabaseThreadsHint(8);
	osg::DisplaySettings::instance()->setNumOfDatabaseThreadsHint(2);

	//initAll();
}

UIFacade::~UIFacade()
{
	delete _pluginManager;
	delete _dataManager;
	delete m_SettingsManager;

	osg::setNotifyLevel(osg::FATAL);
	osg::setNotifyHandler(nullptr);
	osgEarth::setNotifyHandler(nullptr);

	cout << "Program closed: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str() << endl;
	_log->close();
	delete _log;
}

void UIFacade::initDCUIVar()
{
	emit  sendNowInitName(tr("初始化 DCCore"));

	m_SettingsManager = new SettingsManager(this);
	


	// thread-safe initialization of the OSG wrapper manager. Calling this here
	// prevents the "unsupported wrapper" messages from OSG
	osgDB::Registry::instance()->getObjectWrapperManager()->findWrapper("osg::Image");
}

//传入待处理数据
void UIFacade::HandlingEntitiesChanged(const QVector<osg::Node*>& entities)
{
	if (_mainMap[0]->isGeocentric())
	{
		osg::ref_ptr<osgEarth::Util::EarthManipulator>  manipulator =
			dynamic_cast<osgEarth::Util::EarthManipulator *>(m_pCurrentNewViewer->getMainView()->getCameraManipulator());

		if (!manipulator.valid())
		{
			manipulator = new osgEarth::Util::EarthManipulator;
			m_pCurrentNewViewer->getMainView()->setCameraManipulator(manipulator);
		}
		else
		{
			if (!entities.isEmpty())
			{
				manipulator->home(0);
				manipulator->setViewpoint(osgEarth::Viewpoint("Home", 101.870, 23.093, 1000, 30.0, -60, 45000), 3);
			}
			else
			{
				manipulator->home(0);
			}
			
		}

		auto  settings = manipulator->getSettings();
		settings->setSingleAxisRotation(true);
		settings->setMinMaxDistance(10000.0, settings->getMaxDistance());
		settings->setMaxOffset(5000.0, 5000.0);
		settings->setMinMaxPitch(-90, 90);
		settings->setTerrainAvoidanceEnabled(true);
		settings->setThrowingEnabled(false);
	}
	else
	{
		MapController *manipulator = dynamic_cast<MapController *>(m_pCurrentNewViewer->getMainView()->getCameraManipulator());

		if (!manipulator)
		{
			// Init a manipulator if not inited yet
			manipulator = new MapController(_dataRoot, _mapRoot, _mainMap[0]->getSRS());
			manipulator->setAutoComputeHomePosition(false);

			if (m_SettingsManager->getOrAddSetting("Camera indicator", false).toBool())
			{
				manipulator->setCenterIndicator(m_pCurrentNewViewer->createCameraIndicator());
			}

			// Nearfar mode and ratio affect scene clipping
			auto  camera = m_pCurrentNewViewer->getMainView()->getCamera();
			camera->setComputeNearFarMode(osg::Camera::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);

			connect(_dataManager, &UserDataManager::moveToNode,
				manipulator, &MapController::fitViewOnNode);
			connect(_dataManager, &UserDataManager::moveToBounding,
				manipulator, &MapController::fitViewOnBounding);

			m_pCurrentNewViewer->getMainView()->setCameraManipulator(manipulator);
			manipulator->registerWithView(m_pCurrentNewViewer->getMainView(), 0);
		}

		if (!entities.isEmpty())
		{
			manipulator->fitViewOnNode(entities[0]);
		}
		else
		{
			manipulator->fitViewOnNode(_mapNode[0]);
		}
		
	}
}


void  UIFacade::initAll()
{
	collectInitInfo();
	// 加载语言文件
	LoadLanguages();

	//！ 初始化控制台

	emit  sendNowInitName(tr("初始化日志文件 log"));
	initLog();

	//! 初始化dc库中的基础base变量
	initDCUIVar();

	emit  sendNowInitName(tr("初始化界面 UI"));

	//! 生成界面，创建datamanager
	setupUi();

	//初始化一个 view,绑定空node，必须在initDCUIVar前
	initView();
	

	//！ 初始化加载一个场景数据，作为根节点，传递给dataManager
	initDataManagerAndScene();

	emit  sendNowInitName(tr("初始化相机 camera"));
	resetCamera();

	initPlugins();

	emit  sendNowInitName(tr("更新界面样式 UI"));
	initUiStyles();
}

//! 加载语言文件
void UIFacade::LoadLanguages()
{
	//语言文件根路径(简体中文)
	QString strLanguageDir = QDir::toNativeSeparators(QApplication::applicationDirPath())
		.append("\\Resources\\languages\\zh_cn\\");

	//语言文件路径
	QDir dir(strLanguageDir);

	//如果路径存在，则加载语言
	if (dir.exists())
	{
		//获取符合语言文件格式的文件
		QStringList filters = QStringList() << "*.lng" << "*.qm";
		QStringList lstLanguages = dir.entryList(filters);

		//加载所有语言文件
		for (auto it = lstLanguages.constBegin();
			it != lstLanguages.constEnd(); ++it)
		{
			//创建语言翻译器
			QTranslator* pTranslator = new QTranslator;
			//加载语言文件
			pTranslator->load(strLanguageDir + *it);
			//记录语言翻译器
			//m_translators.push_back(pTranslator);
			//应用程序安装语言翻译器
			QApplication::installTranslator(pTranslator);
		}
	}
}


void UIFacade::initUiStyles()
{
	QList<QToolBar *> toolBars = findChildren<QToolBar *>(QString(), Qt::FindDirectChildrenOnly);

	//！ 设置toolbar顶部和左右放置效果不同
	for (QToolBar *toolBar : toolBars)
	{
		// Set init style
		if (toolBar->orientation() == Qt::Vertical)
		{
			toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
		}
		else
		{
			toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		}

		toolBar->setIconSize(QSize(30, 30));

		for (QAction *action : toolBar->actions())
		{
			action->setStatusTip(action->toolTip());
		}

		for (auto *widget : toolBar->children())
		{
			QToolButton *button = dynamic_cast<QToolButton *>(widget);

			if (button)
			{
				if (toolBar->orientation() == Qt::Vertical)
				{
					button->setToolButtonStyle(Qt::ToolButtonIconOnly);
				}
				else
				{
					button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
				}

				button->setStatusTip(button->toolTip());

				for (QAction *action : button->actions())
				{
					action->setStatusTip(action->toolTip());
				}

				button->setIconSize(QSize(30, 30));

				// QToolButton seems to limit maximum size by default
				button->setMaximumSize(QSize(1000, 1000));
			}
		}

		// When a tool bar is dragged and replaced, change its style accordingly
		connect(toolBar, &QToolBar::orientationChanged, [toolBar](Qt::Orientation orientation)
		{
			if (orientation == Qt::Vertical)
			{
				toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
			}
			else
			{
				toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			}

			for (auto *widget : toolBar->children())
			{
				QToolButton *button = dynamic_cast<QToolButton *>(widget);

				if (button)
				{
					if (orientation == Qt::Vertical)
					{
						button->setToolButtonStyle(Qt::ToolButtonIconOnly);
					}
					else
					{
						button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
					}
				}
			}
		});
	}

	for (auto child : children())
	{
		NXDockWidget *dock = dynamic_cast<NXDockWidget *>(child);

		if (dock)
		{
			//_dataManager->dockWidgetUnpinned(dock);
			dock->setFixedWidth(250);
		}
	}
}

void UIFacade::InitManager()
{
	if (!_dataManager)
	{
		_dataManager = new UserDataManager(this);
	}

	if (_dataManager)
	{
		//! 数据加载进度条管理及视窗重置
		connect(_dataManager, &UserDataManager::loadingProgress, this, &UIFacade::loadingProgress);
		connect(_dataManager, &UserDataManager::loadingDone, this, &UIFacade::loadingDone);
		connect(_dataManager, &UserDataManager::resetCamera, this, &UIFacade::resetCamera);

		connect(_dataManager, SIGNAL(SelectionChanged(const QVector<osg::Node*>&)), this, SLOT(HandlingEntitiesChanged(const QVector<osg::Node*>&)));

		//! 初始化node管理面板
		_dataManager->setupUi(this);

		// create the log handler
		connect(LogHandler::getInstance(), SIGNAL(newMessage(const QString &)), this, SLOT(printToLogConsole(const QString &)));
		connect(LogHandler::getInstance(), SIGNAL(newMessages(const QStringList &)), this, SLOT(printToLogConsole(const QStringList &)));
		LogHandler::getInstance()->startEmission(true); // start log emission
	}
}

void UIFacade::printToLogConsole(const QString & mess)
{
	_dataManager->printToLogConsole(mess);
}

void UIFacade::printToLogConsole(const QStringList & mess)
{
	_dataManager->printToLogConsole(mess.join("<br>"));
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
		InitManager();
		

		LoadSettings();
	}

	ConfigFinish(this);
}

void  UIFacade::initView()
{
	//! 初始化viewWidget
	emit  sendNowInitName(tr("初始化图形视窗 ViewWidget"));

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
	_mousePicker->registerData(this, _dataManager, m_pCurrentNewViewer, _root, m_SettingsManager->getGlobalSRS());
	_mousePicker->registerSetting(m_SettingsManager);
	_mousePicker->setupUi(statusBar());
	m_pCurrentNewViewer->getMainView()->addEventHandler(_mousePicker);

	_pluginManager = new MPluginManager(this, _dataManager, m_pCurrentNewViewer);

	//! 插件管理器绑定信号槽

	connect(_dataManager, &UserDataManager::requestContextMenu, _pluginManager, &MPluginManager::loadContextMenu);
	connect(_pluginManager, &MPluginManager::sendNowInitName, this, &UIFacade::sendNowInitName);

	_pluginManager->loadPlugins();
}

void  UIFacade::initDataManagerAndScene()
{
	_root = new osg::Group;
	_root->setName("Root");

	//_root节点状态设置 Turn off all lights by default
	osg::StateSet *state = _root->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::OFF &osg::StateAttribute::OVERRIDE);
	state->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	osg::ref_ptr<osg::CullFace>  cf = new osg::CullFace;
	cf->setMode(osg::CullFace::BACK);
	state->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
	state->setAttributeAndModes(cf, osg::StateAttribute::ON);

	emit  sendNowInitName(tr("初始化场景树 DataScene"));

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
		QString  mode = m_SettingsManager->getOrAddSetting("Base mode", "geocentric").toString();
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
			m_SettingsManager->setOrAddSetting("Base mode", "projected");
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

	m_SettingsManager->setGlobalSRS(_mainMap[0]->getSRS());

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

	//！ 坐标转换后添加个osg模型，添加到_drawRoot中
	const osgEarth::SpatialReference* geoSRS = _mapNode[0]->getMapSRS()->getGeographicSRS();

	//添加模型
	//{
	//	osg::Node* model = osgDB::readNodeFile("H:\\osg\\OpenSceneGraph-Data-3.4.0\\OpenSceneGraph-Data\\cow.osg");
	//	//osg中光照只会对有法线的模型起作用，而模型经过缩放后法线是不会变得，
	//	//所以需要手动设置属性，让法线随着模型大小变化而变化。GL_NORMALIZE 或 GL_RESCALE_NORMAL
	//	model->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);

	//	osg::Matrix Lmatrix;
	//	geoSRS->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(40.0), osg::DegreesToRadians(116.0), 100000.0, Lmatrix);
	//	//放大一些，方便看到
	//	Lmatrix.preMult(osg::Matrix::scale(osg::Vec3(10000, 10000, 10000)));

	//	osg::MatrixTransform* mt = new osg::MatrixTransform;
	//	mt->setMatrix(Lmatrix);
	//	mt->addChild(model);
	//	_drawRoot->addChild(mt);
	//}

	_root->addChild(_dataOverlay);
	_root->addChild(_drawRoot);
	_root->addChild(_mapRoot);

	
	//! 更新场景数据
	if (m_pCurrentNewViewer)
	{
		_dataManager->registerDataRoots(_root);

		auto isSceneView = m_pCurrentNewViewer->metaObject()->className() == QStringLiteral("DC::SceneView");
		m_pCurrentNewViewer->setSceneData(_root);

	}
	
}

void  UIFacade::resetCamera()
{
	if (_mainMap[0]->isGeocentric())
	{
		osg::ref_ptr<osgEarth::Util::EarthManipulator>  manipulator =
			dynamic_cast<osgEarth::Util::EarthManipulator *>(m_pCurrentNewViewer->getMainView()->getCameraManipulator());

		if (!manipulator.valid())
		{
			manipulator = new osgEarth::Util::EarthManipulator;
			m_pCurrentNewViewer->getMainView()->setCameraManipulator(manipulator);
		}
		else
		{
			manipulator->home(0);
			//视点定位北京地区
			manipulator->setViewpoint(osgEarth::Viewpoint("", 116, 40, 0.0, -2.50, -90.0, 1.5e6));
		}

		auto  settings = manipulator->getSettings();
		settings->setSingleAxisRotation(true);
		settings->setMinMaxDistance(10000.0, settings->getMaxDistance());
		settings->setMaxOffset(5000.0, 5000.0);
		settings->setMinMaxPitch(-90, 90);
		settings->setTerrainAvoidanceEnabled(true);
		settings->setThrowingEnabled(false);
	}
	else
	{
		MapController *manipulator = dynamic_cast<MapController *>(m_pCurrentNewViewer->getMainView()->getCameraManipulator());

		if (!manipulator)
		{
			// Init a manipulator if not inited yet
			manipulator = new MapController(_dataRoot, _mapRoot, _mainMap[0]->getSRS());
			manipulator->setAutoComputeHomePosition(false);

			if (m_SettingsManager->getOrAddSetting("Camera indicator", false).toBool())
			{
				manipulator->setCenterIndicator(m_pCurrentNewViewer->createCameraIndicator());
			}

			// Nearfar mode and ratio affect scene clipping
			auto  camera = m_pCurrentNewViewer->getMainView()->getCamera();
			camera->setComputeNearFarMode(osg::Camera::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);

			connect(_dataManager, &UserDataManager::moveToNode,
			        manipulator, &MapController::fitViewOnNode);
			connect(_dataManager, &UserDataManager::moveToBounding,
			        manipulator, &MapController::fitViewOnBounding);

			m_pCurrentNewViewer->getMainView()->setCameraManipulator(manipulator);
			manipulator->registerWithView(m_pCurrentNewViewer->getMainView(), 0);
		}

		manipulator->fitViewOnNode(_mapNode[0]);
		//视点定位北京地区
		//manipulator->setViewpoint(osgEarth::ViewPoint("", 116, 40, 0.0, -2.50, -90.0, 1.5e6));
	}
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



