#ifndef MYMainWindowAction_H
#define MYMainWindowAction_H

#include "QModelIndex"
#include <QSettings>
#include "DCGui/iwindow.h"

#include "Mainwindow.h"

namespace DC
{
	class SceneView;
}

class QMdiArea;

class SceneModel;
class ObjectLoader;
class NodeTreeModel;
class NodePropertyWidget;


class NXDockWidget;
class NXDockWidgetTabBar;

namespace osg
{
	class Node;
}

class MainWindowAction : public MainWindow
{
	Q_OBJECT

public:
	MainWindowAction(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
	~MainWindowAction();

public:

public slots:

	/////////////////////////////////////File/////////////////////////////////////
	/** file
	 *读取文件 */
	void on_actionOpen_triggered();

	//! 加载文件  QString  type ["LOAD" | "ADD"]  打开新窗口和追加
	void NewLoadedFile(osg::Node *, QString type);

	//! 激活一个最近打开的文件
	//void RecentFileActivated(QAction *action);

	//! 初始化最近打开文件菜单
	//void SetupRecentFilesMenu();

	void on_actionExit_triggered();

	void on_RecentlyOpenedFiles_triggered(QAction* );

	void on_menuFile_aboutToShow();

	/*void on_actionAdd_triggered();*/

	void on_actionSplitFiles_triggered();

	void SwitchMode();


	///////////////////////////////////////view///////////////////////////////////

	///背景色
	void on_actionBGColor_triggered();

	///是否应用渐变
	void on_actionColorGradient_triggered(bool);
	
	void on_actionLight_triggered(bool val);

	void on_actionFullScreen_triggered(bool val);

	///tool \n
	///设置qss文件
	void DoStyleSheet(QString qssFilename);
	void on_actionQss1_triggered();
	void on_actionQss2_triggered();
	void on_actionQss3_triggered();
	void on_actionQssDefault_triggered();

	//help
	void on_actionOnline_Update_triggered();
	void on_actionAbout_triggered();

	//！ docket
	// Turn on the AutoHide option 
	void dockWidgetPinned(NXDockWidget* dockWidget);

	// Turn off the AutoHide option 
	void dockWidgetUnpinned(NXDockWidget* dockWidget);

	// DockWidget has been docked
	void dockWidgetDocked(NXDockWidget* dockWidget);

	// DockWidget has been undocked
	void dockWidgetUndocked(NXDockWidget* dockWidget);

	void createDockWidgetBar(Qt::DockWidgetArea area);

	void showDockWidget(NXDockWidget* dockWidget);
	void hideDockWidget(NXDockWidget* dockWidget);

	QRect getDockWidgetsAreaRect();

	void adjustDockWidget(NXDockWidget* dockWidget);

	void removeDockWidget(NXDockWidget* dockWidget);

	// Add an auto-hide dock widget
	void AddDockWidget(Qt::DockWidgetArea area, NXDockWidget* dockWidget, Qt::Orientation orientation);

	// Add an auto-hide dock widget
	void AddDockWidget(Qt::DockWidgetArea area, NXDockWidget* dockWidget);

	void InitDockWidget();
private slots:
	void NodeSelected(const QModelIndex &index);
	
private:

};

#endif // MainWindowAction
