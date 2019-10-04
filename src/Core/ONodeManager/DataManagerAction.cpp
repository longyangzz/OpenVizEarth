#pragma execution_character_set("utf-8")
//qt
#include "DataManagerAction.h"
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
#include "QMainWindow"

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

const int maxRecentlyOpenedFileNum = 10;

DataManagerAction::DataManagerAction(QWidget *parent, Qt::WindowFlags flags)
	: QObject(parent)
	, m_mainWindow(nullptr)
	, _dockWidget(nullptr)
{
	
}

DataManagerAction::~DataManagerAction()
{
	
	
}

QList<NXDockWidget *>  DataManagerAction::getDockWidgetListAtArea(Qt::DockWidgetArea area)
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


NXDockWidgetTabBar * DataManagerAction::getDockWidgetBar(Qt::DockWidgetArea area)
{
	assert(_dockWidgetBar.find(area) != _dockWidgetBar.end());

	auto  it = _dockWidgetBar.find(area);

	if (it != _dockWidgetBar.end())
	{
		return *it;
	}

	return nullptr;
}


void  DataManagerAction::adjustDockWidget(NXDockWidget *dockWidget)
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


void  DataManagerAction::dockWidgetUnpinned(NXDockWidget *dockWidget)
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

	QList<QDockWidget *>  dockWidgetList = m_mainWindow->tabifiedDockWidgets(dockWidget);
	dockWidgetList.push_back(dockWidget);

	for (QDockWidget *qDockWidget : dockWidgetList)
	{
		NXDockWidget *dockWidget = static_cast<NXDockWidget *>(qDockWidget);

		dockWidget->setState(NXDockWidget::DockWidgetState::Hidden);

		if (!dockWidget->isHidden())
		{
			dockWidgetBar->addDockWidget(dockWidget);

			dockWidget->setTabifiedDocks(dockWidgetList);

			m_mainWindow->removeDockWidget(dockWidget);
		}
	}

	if (dockWidget->getArea() == Qt::LeftDockWidgetArea)
	{
		getDockWidgetBar(Qt::TopDockWidgetArea)->insertSpacing();
		getDockWidgetBar(Qt::BottomDockWidgetArea)->insertSpacing();
	}
}



void  DataManagerAction::dockWidgetPinned(NXDockWidget *dockWidget)
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
				m_mainWindow->addDockWidget(dockWidget->getArea(), dockWidget);
			}
			else
			{
				m_mainWindow->tabifyDockWidget(prevDockWidget, dockWidget);
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


QRect  DataManagerAction::getDockWidgetsAreaRect()
{
	int  left = m_mainWindow->centralWidget()->x();

	QList<NXDockWidget *>  leftAreaDockWidgets = getDockWidgetListAtArea(Qt::LeftDockWidgetArea);

	for (const NXDockWidget *dockWidget : leftAreaDockWidgets)
	{
		if ((dockWidget->x() >= 0) && (dockWidget->width() > 0))
		{
			left = std::min(left, dockWidget->x());
		}
	}

	int                    top = m_mainWindow->centralWidget()->y();
	QList<NXDockWidget *>  topAreaDockWidgets = getDockWidgetListAtArea(Qt::TopDockWidgetArea);

	for (const NXDockWidget *dockWidget : topAreaDockWidgets)
	{
		if ((dockWidget->y() >= 0) && (dockWidget->height() > 0))
		{
			top = std::min(top, dockWidget->y());
		}
	}

	int                    right = m_mainWindow->centralWidget()->x() + m_mainWindow->centralWidget()->width();
	QList<NXDockWidget *>  rightAreaDockWidgets = getDockWidgetListAtArea(Qt::RightDockWidgetArea);

	for (const NXDockWidget *dockWidget : rightAreaDockWidgets)
	{
		if ((dockWidget->x() >= 0) && (dockWidget->width() > 0))
		{
			right = std::max(right, dockWidget->x() + dockWidget->width());
		}
	}

	int                    bottom = m_mainWindow->centralWidget()->y() + m_mainWindow->centralWidget()->height();
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

void DataManagerAction::InitOrtherDockWidget()
{
	//! 创建dockwidger的工具栏容器
	createDockWidgetBar(Qt::LeftDockWidgetArea);
	createDockWidgetBar(Qt::RightDockWidgetArea);
	createDockWidgetBar(Qt::TopDockWidgetArea);
	createDockWidgetBar(Qt::BottomDockWidgetArea);


	// Control panel
	{
		NXDockWidget *controlPanel = new NXDockWidget(tr("控制面板"), m_mainWindow);
		
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
		NXDockWidget *attributePanel = new NXDockWidget(tr("属性"), m_mainWindow);
		
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

void  DataManagerAction::hideDockWidget(NXDockWidget *dockWidget)
{
	if ((dockWidget == nullptr) || (dockWidget->isHidden()))
	{
		return;
	}

	_dockWidget = nullptr;

	dockWidget->hide();
}

void  DataManagerAction::showDockWidget(NXDockWidget *dockWidget)
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
			m_mainWindow->addDockWidget(dockWidget->getArea(), dockWidget);
			dockWidget->setFloating(false);

			m_mainWindow->removeDockWidget(dockWidget);
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

void  DataManagerAction::dockWidgetDocked(NXDockWidget *dockWidget)
{
	if (dockWidget == nullptr)
	{
		return;
	}
}

void  DataManagerAction::dockWidgetUndocked(NXDockWidget *dockWidget)
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
			m_mainWindow->addDockWidget(dockWidget->getArea(), dockWidget);
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


void  DataManagerAction::createDockWidgetBar(Qt::DockWidgetArea area)
{
	if (_dockWidgetBar.find(area) != _dockWidgetBar.end())
	{
		return;
	}

	NXDockWidgetTabBar *dockWidgetBar = new NXDockWidgetTabBar(area);
	dockWidgetBar->setWindowTitle("DockTool");
	_dockWidgetBar[area] = dockWidgetBar;
	connect(dockWidgetBar, &NXDockWidgetTabBar::signal_dockWidgetButton_clicked, this, &DataManagerAction::showDockWidget);

	m_mainWindow->addToolBar(dockAreaToToolBarArea(area), dockWidgetBar);
}




void  DataManagerAction::AddDockWidget(Qt::DockWidgetArea area, NXDockWidget *dockWidget, Qt::Orientation orientation)
{
	if (dockWidget == nullptr)
	{
		return;
	}

	connect(dockWidget, &NXDockWidget::signal_pinned, this, &DataManagerAction::dockWidgetPinned);
	connect(dockWidget, &NXDockWidget::signal_unpinned, this, &DataManagerAction::dockWidgetUnpinned);
	connect(dockWidget, &NXDockWidget::signal_docked, this, &DataManagerAction::dockWidgetDocked);
	connect(dockWidget, &NXDockWidget::signal_undocked, this, &DataManagerAction::dockWidgetUndocked);

	_dockWidgets.push_back(dockWidget);

	m_mainWindow->addDockWidget(area, dockWidget, orientation);
}

void  DataManagerAction::AddDockWidget(Qt::DockWidgetArea area, NXDockWidget *dockWidget)
{
	AddDockWidget(area, dockWidget, Qt::Vertical);
}


void  DataManagerAction::RemoveDock(NXDockWidget *dockWidget)
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

	m_mainWindow->removeDockWidget(dockWidget);

	dockWidget->setParent(nullptr);
}




