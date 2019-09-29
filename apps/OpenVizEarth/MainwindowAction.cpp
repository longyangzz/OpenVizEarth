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
#include "QProgressBar"
#include "QStatusBar"


#include <assert.h>

//osg
#include "osg/Node"

//subself
//DC
#include "DC/ThreadPool.h"
#include "DC/SettingsManager.h"

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

const int maxRecentlyOpenedFileNum = 10;

MainWindowAction::MainWindowAction(QWidget *parent , Qt::WindowFlags flags )
	: IWindow(parent, flags)
	, m_SettingsManager(nullptr)
	, m_appName(PACKAGE_NAME)
	, m_version(PACKAGE_VERSION)
	, m_pMdiArea(new QMdiArea(this))
	, m_bgLoader(nullptr)
	, m_pCurrentNewViewer(nullptr)
	, m_pProgressBar(nullptr)
{
	
}

MainWindowAction::~MainWindowAction()
{
	
	
}


void  MainWindowAction::loadingDone()
{
	statusBar()->removeWidget(m_pProgressBar);
	// _mousePicker->updateDrawOffset();
	delete m_pProgressBar;
	m_pProgressBar = NULL;
}

void  MainWindowAction::loadingProgress(int percent)
{
	if (m_pProgressBar == NULL)
	{
		m_pProgressBar = new QProgressBar;
		m_pProgressBar->setMaximumWidth(400);
		m_pProgressBar->setMaximumHeight(22);
		m_pProgressBar->setRange(0, 100);
		m_pProgressBar->setValue(0);
		statusBar()->addPermanentWidget(m_pProgressBar);
		m_pProgressBar->setValue(percent);
	}
	else
	{
		m_pProgressBar->setValue(percent);
	}
}

void MainWindowAction::SetSettingManager(SettingsManager* sManager)
{
	m_SettingsManager = sManager;
}

DC::SceneView* MainWindowAction::CurrentSceneView()
{
	DC::SceneView* pViewer = static_cast<DC::SceneView*>(ActiveMdiChild());

	m_pCurrentNewViewer = pViewer;

	return pViewer;
}

//! 返回当前激活的窗口
QWidget* MainWindowAction::ActiveMdiChild()
{
	if (QMdiSubWindow *activeSubWindow = m_pMdiArea->activeSubWindow())
		return qobject_cast<QWidget *>(activeSubWindow->widget());

	return 0;
}

bool MainWindowAction::LoadFile(const QString &file, QString type)
{
	if (file.isEmpty() || !QFileInfo(file).exists())
		return false;

	//saveIfNeeded();
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	m_currentFile = file;
	m_lastDirectory = QFileInfo(file).absolutePath();

	// enable actions
	//enableActions(false);

	emit NewFileToLoad(file, type);

	return true;
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

DC::SceneView* MainWindowAction::CreateNewSceneViewer()
{
	DC::SceneView* sceneView = new DC::SceneView(this);
	sceneView->setModel(new SceneModel(this));
	QMdiSubWindow* subWindow = m_pMdiArea->addSubWindow(sceneView);

	subWindow->showMaximized();

	return sceneView;
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

void MainWindowAction::AddRecentlyOpenedFile(const QString &filename, QStringList &filelist)
{
	QFileInfo fi(filename);

	if (filelist.contains(fi.absoluteFilePath()))
		return;

	if (filelist.count() >= maxRecentlyOpenedFileNum)
		filelist.removeLast();

	filelist.prepend(fi.absoluteFilePath());
}


void MainWindowAction::EnableActions(const bool isEnable)
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


void MainWindowAction::on_actionOnline_Update_triggered()
{
	//执行软件安装路径下的维护工具exe即可
	QString toolFileName = QString("%1/%2").arg(QApplication::applicationDirPath()).arg("maintenancetool.exe");

	//启动tool
	QProcess::startDetached(toolFileName, QStringList(toolFileName));
}

void MainWindowAction::SwitchMode()
{
	if (m_SettingsManager)
	{
		QString mode = m_SettingsManager->getOrAddSetting("Base mode", "geocentric").toString();
		if (mode == "geocentric")
			m_SettingsManager->setOrAddSetting("Base mode", "projected");
		else
			m_SettingsManager->setOrAddSetting("Base mode", "geocentric");
		qApp->quit();
		QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
	}

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

