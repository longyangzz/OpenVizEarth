#ifndef MYCLASS_H
#define MYCLASS_H

#include "QModelIndex"
#include <QSettings>

#include "MainWindowAction.h"

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

class DataManager;

namespace osg
{
	class Node;
}

namespace DC
{
	class SceneView;
}

class MainWindow : public MainWindowAction
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~MainWindow();

	void setOrAddSetting(const QString& key, const QVariant& value);
	QVariant getOrAddSetting(const QString& key, const QVariant& defaultValue);

public:

	//! 解析外部传入的参数
	void PraseArgs(QVector<QString > args);

	void CreateConnection();

	void ResetViews(bool allClear);

	

	void Init();

	void initViewWidget();

	void SetCentralWidget();

	//! 添加一个dockwidget，用来放置QtreView,用来管理节点
	//! 老版本
	void InitManager();

	//! 添加一个dockwidget，用来放置QtreView,用来管理节点
	//! 新版本
	

	//! 保存相关设置参数
	void SaveSettings();

	//! 加载记录的设置
	void LoadSettings();
protected:
	//virtual bool event(QEvent* event) override;



	
public:

	
};

#endif // MYCLASS_H
