#ifndef MYDataManagerAction_H
#define MYDataManagerAction_H

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

class DataManagerAction : public QObject
{
	Q_OBJECT

public:
	DataManagerAction(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
	~DataManagerAction();

public:
	//! dockWidgetÐü¸¡´°¿Ú==========================================================
	QList<NXDockWidget*> getDockWidgetListAtArea(Qt::DockWidgetArea area);
	NXDockWidgetTabBar* getDockWidgetBar(Qt::DockWidgetArea area);

public slots:
	//£¡ docket
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

	void InitOrtherDockWidget();
private slots:
	
	
public:
	QMainWindow* m_mainWindow;

	//! docket
	// Current active(slide out) dockwidget or null
	NXDockWidget* _dockWidget;

	// List of all created dockwidgets
	QList<NXDockWidget*> _dockWidgets;

	// List of 4 dock tabbars
	QMap<Qt::DockWidgetArea, NXDockWidgetTabBar*> _dockWidgetBar;

};

#endif // MYDataManagerAction_H
