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
#include "DC/AppSettings.h"
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

#include "NXDockWidget.h"
#include "NXDockWidgetTabBar.h"
#include "NXDockWidgetTabButton.h"

const int maxRecentlyOpenedFileNum = 10;

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
	: IWindow(parent, flags)
	, m_appName(PACKAGE_NAME)
	, m_version(PACKAGE_VERSION)
	, m_pMdiArea( new QMdiArea( this ) )
	, m_bgLoader(nullptr)
	, _dockWidget(nullptr)
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

DC::SceneView* MainWindow::CreateNewSceneViewer()
{
	DC::SceneView* sceneView = new DC::SceneView(this);
	sceneView->setModel(new SceneModel(this));
	QMdiSubWindow* subWindow = m_pMdiArea->addSubWindow( sceneView );

	subWindow->showMaximized();

	return sceneView;
}

//! 返回当前激活的窗口
QWidget* MainWindow::ActiveMdiChild()
{
	if (QMdiSubWindow *activeSubWindow = m_pMdiArea->activeSubWindow())
		return qobject_cast<QWidget *>(activeSubWindow->widget());

	return 0;
}

DC::SceneView* MainWindow::CurrentSceneView()
{
	DC::SceneView* pViewer = static_cast<DC::SceneView* >(ActiveMdiChild());

	return pViewer;
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

void MainWindow::initViewWidget()
{
	DC::SceneView* pNewViewer = CreateNewSceneViewer();
	if (pNewViewer)
	{
		/*osg::ref_ptr<osgEarth::Map> map = new osgEarth::Map;
		osg::ref_ptr<osgEarth::MapNode> mapNode = new osgEarth::MapNode(map);
		osgEarth::Drivers::BingOptions bing;
		osgEarth::Drivers::GDALOptions gdal;
		osgEarth::Drivers::TMSOptions tms;
		tms.url() = "D:/data/world/tms.xml";
		gdal.url() = "H:\\osgearthSDK\\Data\\world.tif";
		map->addLayer(new osgEarth::ImageLayer("My", gdal));*/
		//osg::ref_ptr < osg::Node > mapNode = osgDB::readNodeFile("Resources\earth_files\geocentric.earth");
		//osg::ref_ptr<osgEarth::MapNode> mapNode = new osgEarth::MapNode(node.get() );


		QString  baseMapPath;
		QString mode = "projected";
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
			//_settingsManager->setOrAddSetting("Base mode", "projected");
			baseMapPath = QStringLiteral("resources/earth_files/projected.earth");
		}
		osg::ref_ptr<osgDB::Options>  myReadOptions = osgEarth::Registry::cloneOrCreateOptions(0);
		osgEarth::Config              c;
		c.add("elevation_smoothing", false);
		osgEarth::TerrainOptions  to(c);
		osgEarth::MapNodeOptions  defMNO;
		defMNO.setTerrainOptions(to);

		myReadOptions->setPluginStringData("osgEarth.defaultOptions", defMNO.getConfig().toJSON());

		osg::Node *baseMap = osgDB::readNodeFile(baseMapPath.toStdString(), myReadOptions);


	   //! 更新场景数据
		pNewViewer->getModel()->setData(baseMap);

		pNewViewer->resetHome();
	}
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

void  MainWindow::adjustDockWidget(NXDockWidget *dockWidget)
{
	if (dockWidget == nullptr)
	{
		return;
	}

	QRect  rect = getDockWidgetsAreaRect();

	switch (dockWidget->getArea())
	{
	case Qt::LeftDockWidgetArea:
		dockWidget->setGeometry(rect.left(), rect.top(), dockWidget->width(), rect.height());
		break;
	case Qt::TopDockWidgetArea:
		dockWidget->setGeometry(rect.left(), rect.top(), rect.width(), dockWidget->height());
		break;
	case Qt::RightDockWidgetArea:
		dockWidget->setGeometry(rect.left() + rect.width() - dockWidget->width(), rect.top(), dockWidget->width(), rect.height());
		break;
	case Qt::BottomDockWidgetArea:
		dockWidget->setGeometry(rect.left(), rect.top() + rect.height() - dockWidget->height(), rect.width(), dockWidget->height());
		break;
	}
}

QList<NXDockWidget *>  MainWindow::getDockWidgetListAtArea(Qt::DockWidgetArea area)
{
	QList<NXDockWidget *>  dockWidgetList;

	for (NXDockWidget *dockWidget : _dockWidgets)
	{
		if ((dockWidget->getArea() == area) && (dockWidget->isDocked()))
		{
			dockWidgetList.push_back(dockWidget);
		}
	}

	return dockWidgetList;
}

void MainWindow::initUiStyles()
{
	for (auto child : children())
	{
		NXDockWidget *dock = dynamic_cast<NXDockWidget *>(child);

		if (dock)
		{
			dockWidgetUnpinned(dock);
			dock->setFixedWidth(250);
		}
	}
}

void  MainWindow::dockWidgetUnpinned(NXDockWidget *dockWidget)
{
	if (dockWidget == nullptr)
	{
		return;
	}

	NXDockWidgetTabBar *dockWidgetBar = getDockWidgetBar(dockWidget->getArea());

	if (dockWidgetBar == nullptr)
	{
		return;
	}

	QList<QDockWidget *>  dockWidgetList = tabifiedDockWidgets(dockWidget);
	dockWidgetList.push_back(dockWidget);

	for (QDockWidget *qDockWidget : dockWidgetList)
	{
		NXDockWidget *dockWidget = static_cast<NXDockWidget *>(qDockWidget);

		dockWidget->setState(NXDockWidget::DockWidgetState::Hidden);

		if (!dockWidget->isHidden())
		{
			dockWidgetBar->addDockWidget(dockWidget);

			dockWidget->setTabifiedDocks(dockWidgetList);

			QMainWindow::removeDockWidget(dockWidget);
		}
	}

	if (dockWidget->getArea() == Qt::LeftDockWidgetArea)
	{
		getDockWidgetBar(Qt::TopDockWidgetArea)->insertSpacing();
		getDockWidgetBar(Qt::BottomDockWidgetArea)->insertSpacing();
	}
}

void  MainWindow::dockWidgetPinned(NXDockWidget *dockWidget)
{
	if (dockWidget == nullptr)
	{
		return;
	}

	NXDockWidgetTabBar *dockWidgetBar = getDockWidgetBar(dockWidget->getArea());

	if (dockWidgetBar == nullptr)
	{
		return;
	}

	_dockWidget = nullptr;

	QList<NXDockWidget *>  dockWidgetList = dockWidget->getTabifiedDocks();
	dockWidgetList.push_back(dockWidget);

	NXDockWidget *prevDockWidget = nullptr;

	for (NXDockWidget *dockWidget : dockWidgetList)
	{
		if (dockWidgetBar->removeDockWidget(dockWidget))
		{
			if (prevDockWidget == nullptr)
			{
				QMainWindow::addDockWidget(dockWidget->getArea(), dockWidget);
			}
			else
			{
				tabifyDockWidget(prevDockWidget, dockWidget);
			}

			prevDockWidget = dockWidget;

			dockWidget->setState(NXDockWidget::DockWidgetState::Docked);

			dockWidget->show();
		}
	}

	dockWidget->raise();

	if ((dockWidget->getArea() == Qt::LeftDockWidgetArea)
		&& dockWidgetBar->isHidden())
	{
		getDockWidgetBar(Qt::TopDockWidgetArea)->removeSpacing();
		getDockWidgetBar(Qt::BottomDockWidgetArea)->removeSpacing();
	}
}

NXDockWidgetTabBar * MainWindow::getDockWidgetBar(Qt::DockWidgetArea area)
{
	assert(_dockWidgetBar.find(area) != _dockWidgetBar.end());

	auto  it = _dockWidgetBar.find(area);

	if (it != _dockWidgetBar.end())
	{
		return *it;
	}

	return nullptr;
}

QRect  MainWindow::getDockWidgetsAreaRect()
{
	int  left = centralWidget()->x();

	QList<NXDockWidget *>  leftAreaDockWidgets = getDockWidgetListAtArea(Qt::LeftDockWidgetArea);

	for (const NXDockWidget *dockWidget : leftAreaDockWidgets)
	{
		if ((dockWidget->x() >= 0) && (dockWidget->width() > 0))
		{
			left = std::min(left, dockWidget->x());
		}
	}

	int                    top = centralWidget()->y();
	QList<NXDockWidget *>  topAreaDockWidgets = getDockWidgetListAtArea(Qt::TopDockWidgetArea);

	for (const NXDockWidget *dockWidget : topAreaDockWidgets)
	{
		if ((dockWidget->y() >= 0) && (dockWidget->height() > 0))
		{
			top = std::min(top, dockWidget->y());
		}
	}

	int                    right = centralWidget()->x() + centralWidget()->width();
	QList<NXDockWidget *>  rightAreaDockWidgets = getDockWidgetListAtArea(Qt::RightDockWidgetArea);

	for (const NXDockWidget *dockWidget : rightAreaDockWidgets)
	{
		if ((dockWidget->x() >= 0) && (dockWidget->width() > 0))
		{
			right = std::max(right, dockWidget->x() + dockWidget->width());
		}
	}

	int                    bottom = centralWidget()->y() + centralWidget()->height();
	QList<NXDockWidget *>  bottomAreaDockWidgets = getDockWidgetListAtArea(Qt::BottomDockWidgetArea);

	for (const NXDockWidget *dockWidget : bottomAreaDockWidgets)
	{
		if ((dockWidget->y() >= 0) && (dockWidget->height() > 0))
		{
			bottom = std::max(bottom, dockWidget->y() + dockWidget->height());
		}
	}

	return QRect(left, top, right - left, bottom - top);
}

void  MainWindow::hideDockWidget(NXDockWidget *dockWidget)
{
	if ((dockWidget == nullptr) || (dockWidget->isHidden()))
	{
		return;
	}

	_dockWidget = nullptr;

	dockWidget->hide();
}

void  MainWindow::showDockWidget(NXDockWidget *dockWidget)
{
	if (dockWidget == nullptr)
	{
		return;
	}

	if (dockWidget->isHidden())
	{
		hideDockWidget(_dockWidget);

		if (dockWidget->isFloating())
		{
			QMainWindow::addDockWidget(dockWidget->getArea(), dockWidget);
			dockWidget->setFloating(false);

			QMainWindow::removeDockWidget(dockWidget);
		}

		adjustDockWidget(dockWidget);

		dockWidget->show();
		dockWidget->raise();

		dockWidget->setFocus();

		_dockWidget = dockWidget;
	}
	else
	{
		hideDockWidget(dockWidget);
	}
}

static Qt::ToolBarArea  dockAreaToToolBarArea(Qt::DockWidgetArea area)
{
	switch (area)
	{
	case Qt::LeftDockWidgetArea:

		return Qt::LeftToolBarArea;
	case Qt::RightDockWidgetArea:

		return Qt::RightToolBarArea;
	case Qt::TopDockWidgetArea:

		return Qt::TopToolBarArea;
	case Qt::BottomDockWidgetArea:

		return Qt::BottomToolBarArea;
	default:

		return Qt::ToolBarArea(0);
	}
}

void  MainWindow::dockWidgetDocked(NXDockWidget *dockWidget)
{
	if (dockWidget == nullptr)
	{
		return;
	}
}

void  MainWindow::dockWidgetUndocked(NXDockWidget *dockWidget)
{
	hideDockWidget(_dockWidget);

	NXDockWidgetTabBar *dockWidgetBar = getDockWidgetBar(dockWidget->getArea());

	if (dockWidgetBar == nullptr)
	{
		return;
	}

	dockWidget->clearTabifiedDocks();

	if (dockWidgetBar->removeDockWidget(dockWidget))
	{
		if (!dockWidget->isFloating())
		{
			QMainWindow::addDockWidget(dockWidget->getArea(), dockWidget);
		}

		if ((dockWidget->getArea() == Qt::LeftDockWidgetArea)
			&& dockWidgetBar->isHidden())
		{
			getDockWidgetBar(Qt::TopDockWidgetArea)->removeSpacing();
			getDockWidgetBar(Qt::BottomDockWidgetArea)->removeSpacing();
		}

		dockWidget->show();
	}
}

void  MainWindow::createDockWidgetBar(Qt::DockWidgetArea area)
{
	if (_dockWidgetBar.find(area) != _dockWidgetBar.end())
	{
		return;
	}

	NXDockWidgetTabBar *dockWidgetBar = new NXDockWidgetTabBar(area);
	dockWidgetBar->setWindowTitle("DockTool");
	_dockWidgetBar[area] = dockWidgetBar;
	connect(dockWidgetBar, &NXDockWidgetTabBar::signal_dockWidgetButton_clicked, this, &MainWindow::showDockWidget);

	addToolBar(dockAreaToToolBarArea(area), dockWidgetBar);
}

void MainWindow::InitDockWidget()
{
	createDockWidgetBar(Qt::LeftDockWidgetArea);
	createDockWidgetBar(Qt::RightDockWidgetArea);
	createDockWidgetBar(Qt::TopDockWidgetArea);
	createDockWidgetBar(Qt::BottomDockWidgetArea);

	// Control panel
	{
		NXDockWidget *controlPanel = new NXDockWidget(tr("Control Panel"), this);
		controlPanel->setObjectName(QStringLiteral("controlPanel"));
		QSizePolicy  sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Minimum);
		sizePolicy1.setHorizontalStretch(0);
		sizePolicy1.setVerticalStretch(0);
		sizePolicy1.setHeightForWidth(controlPanel->sizePolicy().hasHeightForWidth());
		controlPanel->setSizePolicy(sizePolicy1);
		controlPanel->setMinimumSize(QSize(311, 0));
		controlPanel->setMaximumSize(QSize(524287, 100));
		controlPanel->setFloating(false);
		controlPanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		QWidget *dockWidgetContent = new QWidget();
		dockWidgetContent->setObjectName(QStringLiteral("controlPanelContent"));
		sizePolicy1.setHeightForWidth(dockWidgetContent->sizePolicy().hasHeightForWidth());
		dockWidgetContent->setSizePolicy(sizePolicy1);
		dockWidgetContent->setMaximumSize(QSize(16777215, 16777215));
		QVBoxLayout *verticalLayout = new QVBoxLayout(dockWidgetContent);
		verticalLayout->setSpacing(6);
		verticalLayout->setContentsMargins(11, 11, 11, 11);
		verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
		verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
		verticalLayout->setContentsMargins(0, 0, 0, 0);
		QTabWidget *tabWidget = new QTabWidget(dockWidgetContent);
		tabWidget->setObjectName(QStringLiteral("tabWidget"));
		QSizePolicy  sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Minimum);
		sizePolicy2.setHorizontalStretch(0);
		sizePolicy2.setVerticalStretch(0);
		sizePolicy2.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
		tabWidget->setSizePolicy(sizePolicy2);
		tabWidget->setMaximumSize(QSize(16777215, 16777215));
		tabWidget->setTabPosition(QTabWidget::North);

		verticalLayout->addWidget(tabWidget);

		controlPanel->setWidget(dockWidgetContent);
		addDockWidget(Qt::RightDockWidgetArea, controlPanel);
	}

	{
		NXDockWidget *attributePanel = new NXDockWidget(tr("Attributes"), this);
		attributePanel->setObjectName(QStringLiteral("attributePanel"));
		QSizePolicy  sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Expanding);
		sizePolicy3.setHorizontalStretch(0);
		sizePolicy3.setVerticalStretch(0);
		sizePolicy3.setHeightForWidth(attributePanel->sizePolicy().hasHeightForWidth());
		attributePanel->setSizePolicy(sizePolicy3);
		attributePanel->setMinimumSize(QSize(100, 0));
		attributePanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		QWidget *tableDockWidgetContents = new QWidget();
		tableDockWidgetContents->setObjectName(QStringLiteral("tableDockWidgetContents"));
		sizePolicy3.setHeightForWidth(tableDockWidgetContents->sizePolicy().hasHeightForWidth());
		tableDockWidgetContents->setSizePolicy(sizePolicy3);
		QVBoxLayout *verticalLayout_2 = new QVBoxLayout(tableDockWidgetContents);
		verticalLayout_2->setSpacing(6);
		verticalLayout_2->setContentsMargins(11, 11, 11, 11);
		verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
		verticalLayout_2->setSizeConstraint(QLayout::SetMaximumSize);
		verticalLayout_2->setContentsMargins(0, 0, 0, 0);
		QTableWidget *attributeTable = new QTableWidget(tableDockWidgetContents);
		attributeTable->setObjectName(QStringLiteral("attributeTable"));
		attributeTable->setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);
		attributeTable->setAlternatingRowColors(false);
		attributeTable->horizontalHeader()->setVisible(false);
		attributeTable->horizontalHeader()->setMinimumSectionSize(0);
		attributeTable->horizontalHeader()->setStretchLastSection(true);
		attributeTable->verticalHeader()->setVisible(false);
		attributeTable->verticalHeader()->setStretchLastSection(false);

		verticalLayout_2->addWidget(attributeTable);

		attributePanel->setWidget(tableDockWidgetContents);
		addDockWidget(Qt::RightDockWidgetArea, attributePanel);
	}
}

void MainWindow::NodeSelected(const QModelIndex &index)
{
	if (CurrentSceneView())
	{
		return;
	}

	if ( index.isValid() )
	{
		CurrentSceneView()->highlight( (osg::Node *)index.internalPointer() );

		// display stats
		m_propertyWidget->displayProperties( (osg::Node *)index.internalPointer() );
	}
}

void MainWindow::on_actionExit_triggered()
{
	close();
}

void MainWindow::LoadSettings()
{
	AppSettings settings;

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
	AppSettings settings;

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

void MainWindow::AddRecentlyOpenedFile(const QString &filename, QStringList &filelist)
{
	QFileInfo fi(filename);

	if ( filelist.contains( fi.absoluteFilePath() ) )
		return;

	if ( filelist.count() >= maxRecentlyOpenedFileNum )
		filelist.removeLast();

	filelist.prepend( fi.absoluteFilePath() );
}

void MainWindow::on_menuFile_aboutToShow()
{
	//! 先清空
	QMenu* recentfiles = menuBar()->findChild<QMenu* >("RecentlyOpenedFiles");
	recentfiles->clear();

	//! 判断是否存在m_recentFiles
	if (m_recentFiles.count() > 0)
	{
		recentfiles->setEnabled(true);
		QStringList::Iterator it = m_recentFiles.begin();

		for (; it != m_recentFiles.end(); ++it)
		{
			recentfiles->addAction(*it);
		}
	}
	else
	{
		recentfiles->setEnabled(false);
	}
}

void MainWindow::on_RecentlyOpenedFiles_triggered(QAction *action)
{
	if ( !action->text().isEmpty() )
	{
		LoadFile( action->text() , "LOAD");
	}
}

void MainWindow::on_actionFullScreen_triggered(bool val)
{
	if (val)
	{
		setWindowState(windowState() | Qt::WindowFullScreen);
		//hideDockWidgets();
	}
	else
	{
		setWindowState(windowState() & ~Qt::WindowFullScreen);
		//showDockWidgets();
	}
}

void MainWindow::on_actionLight_triggered(bool val)
{
	//当前对象
	QObject* object = sender();
	if (!object)
	{
		return;
	}
	//转换为action组件
	QAction* action = static_cast<QAction*>(object);
	
	if (val)
	{
		action->setIcon(QIcon(":/Mainwindow/Resources/tool/64/view_lightOn.png"));
	}
	else
	{
		action->setIcon(QIcon(":/Mainwindow/Resources/tool/64/view_lightOff.png"));
	}

	if (CurrentSceneView())
	{
		CurrentSceneView()->setLightingEnabled(val);
	}
	
}

void MainWindow::EnableActions(const bool isEnable)
{
	QAction* actionLight = GetMenuBar()->findChild<QAction* >("actionLight");
	if (!actionLight)
	{
		return;
	}
	actionLight->setEnabled(isEnable);
	if (isEnable)
	{
		//actionLight->setCheckable(isEnable);
		actionLight->setIcon(QIcon(":/Mainwindow/Resources/tool/64/view_lightOn.png"));
	}
	else
	{
		actionLight->setIcon(QIcon(":/Mainwindow/Resources/tool/64/view_lightOff.png"));
	}
}

void MainWindow::on_actionColorGradient_triggered(bool val)
{
	if (CurrentSceneView())
	{
		CurrentSceneView()->setFlatBackgroundColor(!val);
	}
	
}

void MainWindow::on_actionBGColor_triggered()
{
	if (!CurrentSceneView())
	{
		return;
	}
	QColor currColor = CurrentSceneView()->getBgColor();
	QColor c = QColorDialog::getColor(currColor);

	if ( c.isValid() )
		CurrentSceneView()->setBgColor(c);
}

void MainWindow::on_actionOnline_Update_triggered()
{
	//执行软件安装路径下的维护工具exe即可
	QString toolFileName = QString("%1/%2").arg(QApplication::applicationDirPath()).arg("maintenancetool.exe");

	//启动tool
	QProcess::startDetached(toolFileName, QStringList(toolFileName));
}

void MainWindow::on_actionAbout_triggered()
{
	QString strAbout = tr("版权所有!\n"
		"版本:%1\n"
		"公司:鼎创力维\n").arg(m_version);
	QMessageBox::about(this, tr("About"), strAbout);
}

void MainWindow::on_actionQss3_triggered()
{
	DoStyleSheet(":/Qss/Resources/qss/qss3.qss");
}

void MainWindow::on_actionQssDefault_triggered()
{
	DoStyleSheet(":/Qss/Resources/qss/stylesheet.qss");
}

void MainWindow::on_actionQss2_triggered()
{
	QObject* object = sender();
	if (!object)
	{
		return;
	}
	//转换为action组件
	QAction* qssAction = static_cast<QAction*>(object);

	if (qssAction->objectName() == "actionQss2")
	{
		DoStyleSheet(":/Qss/Resources/qss/qss2.qss");
	}
}

void MainWindow::on_actionQss1_triggered()
{
	QObject* object = sender();
	if (!object)
	{
		return;
	}
	//转换为action组件
	QAction* qssAction = static_cast<QAction*>(object);

	if (qssAction->objectName() == "actionQss1")
	{
		DoStyleSheet(":/Qss/Resources/qss/qss1.qss");
	}
	/*else if (qssAction->objectName() == "actionQss2")
	{
		DoStyleSheet(":/Qss/Resources/qss/qss2.qss");
	}
	else if (qssAction->objectName() == "actionQss3")
	{
		DoStyleSheet(":/Qss/Resources/qss/qss3.qss");
	}
	else
	{
		DoStyleSheet(":/Qss/Resources/qss/stylesheet.qss");
	}*/
}

void MainWindow::DoStyleSheet(QString qssFilename)
{
	//加载样式表
	QFile qssFile(qssFilename);
	qssFile.open(QFile::ReadOnly);

	if(qssFile.isOpen())  

	{  

		QString qss = QLatin1String(qssFile.readAll());  

		qApp->setStyleSheet(qss);  

		qssFile.close();  
	}  
}

void MainWindow::on_actionOpen_triggered()
{
	QString filters = "Files All files (*.*);;(*.ply *.osg *.obj *.txt *.3ds *.stl *.s3c *.osgb)";

	QString file = QFileDialog::getOpenFileName(
		this,
		"选择打开文件",
		m_lastDirectory,
		filters);

	LoadFile(file, "LOAD");

	

}

//void MainWindow::on_actionAdd_triggered()
//{
//	QString filters = "Files (*.ply *.osg *.obj *.txt *.3ds *.stl);;All files (*.*)";
//
//	QString file = QFileDialog::getOpenFileName(
//		this,
//		"选择打开文件",
//		m_lastDirectory,
//		filters);
//
//	LoadFile(file, "ADD");
//}

void MainWindow::on_actionSplitFiles_triggered()
{
	
}


bool MainWindow::LoadFile(const QString &file, QString type)
{
	if ( file.isEmpty() || !QFileInfo(file).exists() )
		return false;

	//saveIfNeeded();
	QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

	m_currentFile = file;
	m_lastDirectory = QFileInfo(file).absolutePath();

	// enable actions
	//enableActions(false);

	emit NewFileToLoad(file, type);

	return true;
}

void MainWindow::NewLoadedFile(osg::Node *node, QString type)
{
	// reset the cursor
	QApplication::restoreOverrideCursor();

	if (!node)
	{
		return;
	}

	//! 在当前窗口中添加一个实体
	if(type == "LOAD")
	{
		//! 创建一个view，视窗与view共享场景根节点
		DC::SceneView* pNewViewer = CreateNewSceneViewer();
		if (pNewViewer)
		{
			//! 更新场景数据
			pNewViewer->getModel()->setData( node );

			pNewViewer->resetHome();
		}
	}
	else if(type == "ADD")
	{
		DC::SceneView* pNewViewer = CurrentSceneView();
		if (pNewViewer)
		{
			//! 更新场景数据
			pNewViewer->getModel()->setData( node );

			pNewViewer->resetHome();
		}
	}
	

	

	//! 更新NodeTreeModel数据,管理的数据结构是 osg::node
	//m_nodeTreeModel->setNode(node);

	//! 更新标题
	setWindowTitle( windowTitle() + " - " + QFileInfo(m_currentFile).fileName() );
	
	//! 增加一个最近打开文件
	AddRecentlyOpenedFile(m_currentFile, m_recentFiles);

	// 重置光标
	QApplication::restoreOverrideCursor();

	//! 更新actions状态
	EnableActions(true);

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