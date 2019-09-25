#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H
#include "DataManager_global.h"

#include "../../NameSpace.h"
#include <QObject>
#include <QSet>
#include <QMap>
#include <QDir>
#include <QVector>
#include <QString>

class PluginInterface;
class ViewerWidget;
class DataManager;
struct PluginEntry;

QT_BEGIN_NAMESPACE
class QToolBar;
class QMenu;
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace DC
{
	class SceneView;
}

class DATAMANAGER_EXPORT PluginManager : public QObject
{
	Q_OBJECT

public:
	struct PluginGroup {
		QString groupName;
		QToolBar* toolBar;
		QMenu* menu;
	};

public:
	PluginManager(QObject *parent, DataManager* dataManager, DC::SceneView* viewer);
	~PluginManager();
	void registerPluginGroup(const QString& name, QToolBar* toolBar, QMenu* menu);
	void loadPlugins();

public slots:
	void loadContextMenu(QMenu* contextMenu, QTreeWidgetItem* selectedItem);

signals:
    void sendNowInitName(const QString&);

protected:
	PluginEntry* getOrCreatePluginEntry(const QString& pluginName);

	void parseDependency(const QString& fileName, const QDir& pluginsDir);

	void loadPlugin(PluginEntry* pluginEntry);
	void registerPlugin(PluginInterface* plugin);
	PluginInterface* instantiate(QObject* instance);

private:
	QMap<QString, PluginGroup> _pluginGroups;
	QMap<QString, PluginEntry*> _pluginEntries;
	QVector<PluginEntry*> _readyToLoad;
	QVector<PluginInterface*> _loadedPlugins;
	DC::SceneView* _viewerWidget;
	DataManager* _dataManager;
};

#endif // PLUGINMANAGER_H
