//qt
#include "MainWindowAction.h"
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

MainWindowAction::MainWindowAction(QWidget *parent, Qt::WindowFlags flags)
	: MainWindow(parent, flags)
{
	
}

MainWindowAction::~MainWindowAction()
{
	
	
}



void  MainWindowAction::adjustDockWidget(NXDockWidget *dockWidget)
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


void  MainWindowAction::dockWidgetUnpinned(NXDockWidget *dockWidget)
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

void MainWindowAction::NodeSelected(const QModelIndex &index)
{
	if (CurrentSceneView())
	{
		return;
	}

	if (index.isValid())
	{
		CurrentSceneView()->highlight((osg::Node *)index.internalPointer());

		// display stats
		m_propertyWidget->displayProperties((osg::Node *)index.internalPointer());
	}
}

void MainWindowAction::NewLoadedFile(osg::Node *node, QString type)
{
	// reset the cursor
	QApplication::restoreOverrideCursor();

	if (!node)
	{
		return;
	}

	//! 在当前窗口中添加一个实体
	if (type == "LOAD")
	{
		//! 创建一个view，视窗与view共享场景根节点
		DC::SceneView* pNewViewer = CreateNewSceneViewer();
		if (pNewViewer)
		{
			//! 更新场景数据
			pNewViewer->getModel()->setData(node);

			pNewViewer->resetHome();
		}
	}
	else if (type == "ADD")
	{
		DC::SceneView* pNewViewer = CurrentSceneView();
		if (pNewViewer)
		{
			//! 更新场景数据
			pNewViewer->getModel()->setData(node);

			pNewViewer->resetHome();
		}
	}




	//! 更新NodeTreeModel数据,管理的数据结构是 osg::node
	//m_nodeTreeModel->setNode(node);

	//! 更新标题
	setWindowTitle(windowTitle() + " - " + QFileInfo(m_currentFile).fileName());

	//! 增加一个最近打开文件
	AddRecentlyOpenedFile(m_currentFile, m_recentFiles);

	// 重置光标
	QApplication::restoreOverrideCursor();

	//! 更新actions状态
	EnableActions(true);

}

void  MainWindowAction::dockWidgetPinned(NXDockWidget *dockWidget)
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


QRect  MainWindowAction::getDockWidgetsAreaRect()
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

void MainWindowAction::InitDockWidget()
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
		AddDockWidget(Qt::RightDockWidgetArea, controlPanel);
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
		AddDockWidget(Qt::RightDockWidgetArea, attributePanel);
	}
}

void  MainWindowAction::hideDockWidget(NXDockWidget *dockWidget)
{
	if ((dockWidget == nullptr) || (dockWidget->isHidden()))
	{
		return;
	}

	_dockWidget = nullptr;

	dockWidget->hide();
}

void  MainWindowAction::showDockWidget(NXDockWidget *dockWidget)
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


void  MainWindowAction::dockWidgetDocked(NXDockWidget *dockWidget)
{
	if (dockWidget == nullptr)
	{
		return;
	}
}

void  MainWindowAction::dockWidgetUndocked(NXDockWidget *dockWidget)
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


void  MainWindowAction::createDockWidgetBar(Qt::DockWidgetArea area)
{
	if (_dockWidgetBar.find(area) != _dockWidgetBar.end())
	{
		return;
	}

	NXDockWidgetTabBar *dockWidgetBar = new NXDockWidgetTabBar(area);
	dockWidgetBar->setWindowTitle("DockTool");
	_dockWidgetBar[area] = dockWidgetBar;
	connect(dockWidgetBar, &NXDockWidgetTabBar::signal_dockWidgetButton_clicked, this, &MainWindowAction::showDockWidget);

	addToolBar(dockAreaToToolBarArea(area), dockWidgetBar);
}


void MainWindowAction::on_actionExit_triggered()
{
	close();
}


void MainWindowAction::on_menuFile_aboutToShow()
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

void MainWindowAction::on_RecentlyOpenedFiles_triggered(QAction *action)
{
	if ( !action->text().isEmpty() )
	{
		LoadFile( action->text() , "LOAD");
	}
}

void MainWindowAction::on_actionFullScreen_triggered(bool val)
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

void MainWindowAction::on_actionLight_triggered(bool val)
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
		action->setIcon(QIcon(":/MainWindowAction/Resources/tool/64/view_lightOn.png"));
	}
	else
	{
		action->setIcon(QIcon(":/MainWindowAction/Resources/tool/64/view_lightOff.png"));
	}

	if (CurrentSceneView())
	{
		CurrentSceneView()->setLightingEnabled(val);
	}
	
}


void MainWindowAction::on_actionColorGradient_triggered(bool val)
{
	if (CurrentSceneView())
	{
		CurrentSceneView()->setFlatBackgroundColor(!val);
	}
	
}

void  MainWindowAction::AddDockWidget(Qt::DockWidgetArea area, NXDockWidget *dockWidget, Qt::Orientation orientation)
{
	if (dockWidget == nullptr)
	{
		return;
	}

	connect(dockWidget, &NXDockWidget::signal_pinned, this, &MainWindowAction::dockWidgetPinned);
	connect(dockWidget, &NXDockWidget::signal_unpinned, this, &MainWindowAction::dockWidgetUnpinned);
	connect(dockWidget, &NXDockWidget::signal_docked, this, &MainWindowAction::dockWidgetDocked);
	connect(dockWidget, &NXDockWidget::signal_undocked, this, &MainWindowAction::dockWidgetUndocked);

	_dockWidgets.push_back(dockWidget);

	QMainWindow::addDockWidget(area, dockWidget, orientation);
}

void  MainWindowAction::AddDockWidget(Qt::DockWidgetArea area, NXDockWidget *dockWidget)
{
	AddDockWidget(area, dockWidget, Qt::Vertical);
}

void MainWindowAction::on_actionBGColor_triggered()
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

void  MainWindowAction::removeDockWidget(NXDockWidget *dockWidget)
{
	if (dockWidget == nullptr)
	{
		return;
	}

	if (_dockWidgets.indexOf(dockWidget) < 0)
	{
		return;
	}

	_dockWidgets.removeOne(dockWidget);

	if (dockWidget->isMinimized())
	{
		dockWidgetPinned(dockWidget);
	}

	QMainWindow::removeDockWidget(dockWidget);

	dockWidget->setParent(nullptr);
}

void MainWindowAction::on_actionOnline_Update_triggered()
{
	//执行软件安装路径下的维护工具exe即可
	QString toolFileName = QString("%1/%2").arg(QApplication::applicationDirPath()).arg("maintenancetool.exe");

	//启动tool
	QProcess::startDetached(toolFileName, QStringList(toolFileName));
}

void MainWindowAction::SwitchMode()
{
	QString mode = getOrAddSetting("Base mode", "geocentric").toString();
	if (mode == "geocentric")
		setOrAddSetting("Base mode", "projected");
	else
		setOrAddSetting("Base mode", "geocentric");
	qApp->quit();
	QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void MainWindowAction::on_actionAbout_triggered()
{
	QString strAbout = tr("版权所有!\n"
		"版本:%1\n"
		"公司:鼎创力维\n").arg(m_version);
	QMessageBox::about(this, tr("About"), strAbout);
}

void MainWindowAction::on_actionQss3_triggered()
{
	DoStyleSheet(":/Qss/Resources/qss/qss3.qss");
}

void MainWindowAction::on_actionQssDefault_triggered()
{
	DoStyleSheet(":/Qss/Resources/qss/stylesheet.qss");
}

void MainWindowAction::on_actionQss2_triggered()
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

void MainWindowAction::on_actionQss1_triggered()
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

void MainWindowAction::DoStyleSheet(QString qssFilename)
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

void MainWindowAction::on_actionOpen_triggered()
{
	QString filters = "Files All files (*.*);;(*.ply *.osg *.obj *.txt *.3ds *.stl *.s3c *.osgb)";

	QString file = QFileDialog::getOpenFileName(
		this,
		"选择打开文件",
		m_lastDirectory,
		filters);

	LoadFile(file, "LOAD");

	

}

//void MainWindowAction::on_actionAdd_triggered()
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

void MainWindowAction::on_actionSplitFiles_triggered()
{
	
}

