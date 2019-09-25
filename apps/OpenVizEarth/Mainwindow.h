#ifndef MYCLASS_H
#define MYCLASS_H

#include "QModelIndex"
#include <QSettings>
#include "DCGui/iwindow.h"

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

class MainWindow : public IWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~MainWindow();

public:
	bool LoadFile(const QString &file, QString type);

	//! 创建一个窗口
	DC::SceneView* CreateNewSceneViewer();

	//! 返回当前激活的窗口
	QWidget* ActiveMdiChild();

	//! 获取当前激活窗口
	DC::SceneView* CurrentSceneView();


	//! 解析外部传入的参数
	void PraseArgs(QVector<QString > args);

	void CreateConnection();

	void ResetViews(bool allClear);

	///设置动作的可用状态
	void EnableActions(const bool isEnable);

	void Init();

	void initViewWidget();

	void SetCentralWidget();

	//! 添加一个dockwidget，用来放置QtreView,用来管理节点
	//! 老版本
	void InitManager();

	//! 添加一个dockwidget，用来放置QtreView,用来管理节点
	//! 新版本
	void InitDockWidget();

	//! 保存相关设置参数
	void SaveSettings();

	//! 加载记录的设置
	void LoadSettings();

	//! 文件读取成功时候调用，用来记录文件名到当前文件和最近打开列表中
	void AddRecentlyOpenedFile(const QString &filename, QStringList &filelist);


	//! dockWidget悬浮窗口==========================================================
	// Add an auto-hide dock widget
	void AddDockWidget(Qt::DockWidgetArea area, NXDockWidget* dockWidget);

	// Add an auto-hide dock widget
	void AddDockWidget(Qt::DockWidgetArea area, NXDockWidget* dockWidget, Qt::Orientation orientation);

	void removeDockWidget(NXDockWidget* dockWidget);

	

	QList<NXDockWidget*> getDockWidgetListAtArea(Qt::DockWidgetArea area);

	

	

	NXDockWidgetTabBar* getDockWidgetBar(Qt::DockWidgetArea area);

	//UI style
	void initUiStyles();

	void setOrAddSetting(const QString& key, const QVariant& value);
	QVariant getOrAddSetting(const QString& key, const QVariant& defaultValue);
protected:
	virtual bool event(QEvent* event) override;

signals:
	void NewFileToLoad(const QString &, QString type);

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
private slots:
	void NodeSelected(const QModelIndex &index);
	
private:

	//! 浏览器场景相关模型视图
	ObjectLoader *m_bgLoader;

	//! 节点管理模型视图
	NodeTreeModel* m_nodeTreeModel;
	NodePropertyWidget* m_propertyWidget;

	//! 软件系统基本信息
	QString m_lastDirectorySnapshot;
	QString m_lastSnapshotName;
	int m_currentSnapshotNum;

	QString m_appName;
	QString m_version;

	QString m_currentLanguage;

	QMdiArea* m_pMdiArea;


	////////////////////////////////设备相关//////////////////////////////////////////
	bool m_inverseMouseWheel;

	////////////////////////////////文件相关//////////////////////////////////////////
	///最近打开的文件
	QStringList m_recentFiles;

	//! 当前文件
	QString m_currentFile;

	//! 最后打开文件路径
	QString m_lastDirectory;

	//! docket
	// Current active(slide out) dockwidget or null
	NXDockWidget* _dockWidget;

	// List of all created dockwidgets
	QList<NXDockWidget*> _dockWidgets;

	// List of 4 dock tabbars
	QMap<Qt::DockWidgetArea, NXDockWidgetTabBar*> _dockWidgetBar;

	QMenu* _treeWidgetMenu;

	//setting
	QSettings m_globalSettings;
};

#endif // MYCLASS_H
