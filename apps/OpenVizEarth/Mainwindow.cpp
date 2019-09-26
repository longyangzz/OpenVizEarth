//qt
#include "Mainwindow.h"
#include "QGridLayout"
#include "QFileDialog"
#include "QMessageBox"
#include "QProcess"
#include "QColorDialog"
#include "QApplication"
#include <QtWidgets/QDesktopWidget>
#include "QLocale"
#include "QAction"
#include "QMenuBar"
#include "QDockWidget"
#include "QTreeView"
#include "QTreeWidgetItem"
#include "QTreeWidget"
#include "QHeaderView"
#include "QMdiSubWindow"
#include "QMdiArea"
#include "QDebug"
#include "QtGui/QColor"
#include "QTableWidget"

#include <assert.h>

//osg
#include "osg/Node"

//subself
//DC
#include "DC/ThreadPool.h"

//DCDB
#include "DCDb/ObjectLoader.h"
#include <osgDB/ReadFile>

//DCScene
#include "DCScene/scene/SceneView.h"
#include "DCScene/scene/SceneModel.h"

#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgQt/GraphicsWindowQt>
#include <osg/MatrixTransform>
#include <osgManipulator/TabBoxDragger>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>
#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarth/ImageLayer>
#include<osgEarthDrivers/gdal/GDALOptions>

#include<osgEarthDrivers/bing/BingOptions>
#include <osgEarth/Registry>

//Manager
#include "Manager/NodeTreeModel.h"
#include "Manager/NodePropertyWidget.h"

#include "ONodeManager/NXDockWidget.h"
#include "ONodeManager/NXDockWidgetTabBar.h"
#include "ONodeManager/NXDockWidgetTabButton.h"
#include "ONodeManager/DataManager.h"
#include "DC/SettingsManager.h"

const int maxRecentlyOpenedFileNum = 10;

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
	: MainWindowAction(parent, flags)
	
{
	
}

MainWindow::~MainWindow()
{
	ThreadPool::getInstance()->stop();
	SaveSettings();
	if (m_bgLoader)
	{
		delete m_bgLoader;
		m_bgLoader = nullptr;
	}
	
}

void MainWindow::SetCentralWidget()
{
	setCentralWidget(m_pMdiArea);
}

void MainWindow::PraseArgs(QVector<QString > args)
{
	DC::SceneView* pNewViewer = CreateNewSceneViewer();
	
	if (pNewViewer)
	{
		if(args.size() > 1)
		{
			for(int i = 1; i != args.size(); ++i)
			{
				qDebug(args[i].toStdString().c_str());
				osg::Node* node1 = osgDB::readNodeFile( args[i].toStdString() );
				//! 更新场景数据
				pNewViewer->getModel()->setData( node1 );
			}
		}

		pNewViewer->resetHome();
	}
	
}

void MainWindow::CreateConnection()
{
	//file
	//connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(DoActionOpen()));

	//view


	//tool
	//connect(ui.actionQss1, SIGNAL(triggered()), this, SLOT(DoStyleSheet()));
	//connect(ui.actionQss2, SIGNAL(triggered()), this, SLOT(DoStyleSheet()));
	//connect(ui.actionQss3, SIGNAL(triggered()), this, SLOT(DoStyleSheet()));
	//connect(ui.actionQssDefault, SIGNAL(triggered()), this, SLOT(DoStyleSheet()));

	//help
	//connect(ui.actionOnline_Update, SIGNAL(triggered()), this, SLOT(DoOnlineUpdate()));
	
	//connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(DoAbout()));
}

void MainWindow::setOrAddSetting(const QString& key, const QVariant & value)
{
	if (value.isValid())
	{
		m_globalSettings.setValue(key, value);
		m_globalSettings.sync();
	}
}

QVariant MainWindow::getOrAddSetting(const QString& key, const QVariant & defaultValue)
{
	auto  found = m_globalSettings.value(key);

	if (!found.isValid())
	{
		if (defaultValue.isValid())
		{
			m_globalSettings.setValue(key, defaultValue);
			m_globalSettings.sync();
		}

		return defaultValue;
	}

	return found;
}


void MainWindow::initViewWidget()
{
	m_pCurrentNewViewer = CreateNewSceneViewer();
}

void MainWindow::InitManager()
{
	//为manager安装treeWidget和属性widget
	QDockWidget* dokwObjects = new QDockWidget(this);
	dokwObjects->setWindowTitle(QString::fromLocal8Bit("对象") );
	dokwObjects->setObjectName(QString::fromUtf8("dokwObjects"));
	dokwObjects->setEnabled(true);
	dokwObjects->setMinimumSize(QSize(200, 315));
	dokwObjects->setMaximumSize(QSize(400, 524287));
	dokwObjects->setLayoutDirection(Qt::LeftToRight);
	dokwObjects->setStyleSheet(QString::fromUtf8("border-color: rgb(85, 255, 255);\n"
		"gridline-color: rgb(85, 85, 255);"));
	dokwObjects->setFloating(false);
	dokwObjects->setFeatures(QDockWidget::AllDockWidgetFeatures);
	dokwObjects->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);


	QWidget* wgtContext = new QWidget();
	wgtContext->setObjectName(QString::fromUtf8("wgtContext"));
	//创建布局管理器
	QGridLayout* layoutObject = new QGridLayout(wgtContext);
	layoutObject->setSpacing(0);
	layoutObject->setContentsMargins(0, 0, 0, 0);
	layoutObject->setObjectName(QString::fromUtf8("layoutObject"));

	//添加qtreeview
	QTreeView* objTreeView = new QTreeView(wgtContext);
	objTreeView->setObjectName("objTreeView");
	objTreeView->setMinimumSize(QSize(200, 100));

	//为treeView添加模型
	m_nodeTreeModel = new NodeTreeModel();
	objTreeView->setModel(m_nodeTreeModel);

	//信号槽
	connect( objTreeView, SIGNAL( clicked ( const QModelIndex &) ), this, SLOT( NodeSelected(const QModelIndex & ) ) );

	layoutObject->addWidget(objTreeView, 0, 0, 1, 1);
	dokwObjects->setWidget(wgtContext);
	addDockWidget(static_cast<Qt::DockWidgetArea>(1), dokwObjects);


	//属性widget
	//安装属性widget
	QDockWidget* dokwProperties = new QDockWidget(this);
	dokwProperties->setObjectName(QString::fromUtf8("dokwProperties"));
	dokwProperties->setMinimumSize(QSize(200, 193));
	dokwProperties->setMaximumSize(QSize(400, 524287));
	dokwProperties->setBaseSize(QSize(4, 0));
	dokwProperties->setFloating(false);
	dokwProperties->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);

	QWidget* wgtProperty = new QWidget();
	wgtProperty->setObjectName(QString::fromUtf8("wgtProperty"));

	QGridLayout* layoutProperty = new QGridLayout(wgtProperty);
	layoutProperty->setSpacing(0);
	layoutProperty->setContentsMargins(0, 0, 0, 0);
	layoutProperty->setObjectName(QString::fromUtf8("layoutProperty"));

	m_propertyWidget = new NodePropertyWidget(wgtProperty);
	m_propertyWidget->setObjectName(QString::fromUtf8("tblwProperties"));
	m_propertyWidget->setMinimumSize(QSize(200, 100));
	m_propertyWidget->setMaximumSize(QSize(400, 16777215));
	
	layoutProperty->addWidget(m_propertyWidget, 0, 0, 1, 1);
	dokwProperties->setWidget(wgtProperty);
	addDockWidget(static_cast<Qt::DockWidgetArea>(1), dokwProperties);
	dokwProperties->setWindowTitle(QString::fromLocal8Bit("属性"));
}





void MainWindow::LoadSettings()
{
	SettingsManager settings;
	settings.clear();
	//! 主窗口参数
	settings.beginGroup("MainWindow");

	//! Qmainwindow方法
	bool aa = restoreState(settings.value("MainWindowState").toByteArray(), 0);

	//! qwidget方法
	resize( settings.value( "size", QSize(800, 600) ).toSize() );
	move( settings.value( "pos", QPoint(200, 200) ).toPoint() );

	bool fullScreen = settings.value("fullScreen",false).toBool();

	if (fullScreen)
		showFullScreen();

	settings.endGroup();


	////////////////////////////Application相关//////////////////////////////////////////////
	settings.beginGroup("Application");

	//! 最后记录路径
	m_lastDirectory = settings.value("lastDirectory","/home").toString();

	//! 最近打开文件列表
	m_recentFiles = settings.value( "recentlyOpenedFiles").toStringList();
	m_inverseMouseWheel = settings.value("inverseMouseWheel", true).toBool();
	
	if (CurrentSceneView())
	{
		CurrentSceneView()->setEnabledInverseMouseWheel(m_inverseMouseWheel);
	}
	

	m_currentLanguage = settings.value("currentLanguage","").toString();

	if ( m_currentLanguage.isEmpty() )
		m_currentLanguage = QLocale::system().name().left(2);

	//! 背景色
	QColor color = settings.value( "bgcolor",QColor(50,50,50) ).value<QColor>();
	if (CurrentSceneView())
	{
		CurrentSceneView()->setBgColor(color);
	}
	

	settings.endGroup();
}

void MainWindow::SaveSettings()
{
	SettingsManager settings;

	settings.beginGroup("MainWindow");

	settings.setValue( "MainWindowState",saveState(0) );

	settings.setValue( "size", size() );
	settings.setValue( "pos", pos() );

	settings.setValue( "fullScreen", isFullScreen() );

	// save the screen number for the splashsreeen ...
	if ( QApplication::desktop()->isVirtualDesktop() )
		settings.setValue( "screenNumber", QApplication::desktop()->screenNumber( pos() ) );
	else
		settings.setValue( "screenNumber", QApplication::desktop()->screenNumber(this) );

	settings.setValue( "screenNumber", QApplication::desktop()->screenNumber(this) );

	settings.endGroup();

	settings.beginGroup("Application");

	settings.setValue("lastDirectory", m_lastDirectory);

	settings.setValue("currentLanguage", m_currentLanguage);
	settings.setValue("inverseMouseWheel", m_inverseMouseWheel);

	// recent files
	settings.setValue("recentlyOpenedFiles", m_recentFiles);

	// scene background
	if (CurrentSceneView())
	{
		settings.setValue( "bgcolor", CurrentSceneView()->getBgColor() );
	}
	

	settings.endGroup();
}

void MainWindow::Init()
{
	//创建一个后台读写线程
	m_bgLoader = new ObjectLoader();
	m_bgLoader->moveToThread( ThreadPool::getInstance()->getThread() );
	connect( this,SIGNAL( NewFileToLoad(const QString &, QString ) ),m_bgLoader,SLOT( newObjectToLoad(const QString &, QString) ) );
	connect( m_bgLoader,SIGNAL( newObjectToView(osg::Node *, QString) ),this,SLOT( NewLoadedFile(osg::Node *, QString) ) );

	//信号槽连接
	CreateConnection();

	//初始化按钮状态
	EnableActions(false);
}



void MainWindow::ResetViews(bool allClear)
{
	//ui->widgetSceneView->resetSelection();

	// reset 3d view
	if (allClear)
	{
		//m_sceneModel->setData(NULL);
	}
}
