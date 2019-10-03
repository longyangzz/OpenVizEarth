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

class MPluginInterface;
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
class OsgQWidget;

class DATAMANAGER_EXPORT MPluginManager : public QObject
{
	Q_OBJECT

public:
	struct PluginGroup {
		QString groupName;
		QToolBar* toolBar;
		QMenu* menu;
	};

public:
	MPluginManager(QObject *parent, DataManager* dataManager, OsgQWidget* viewer);
	~MPluginManager();
	void registerPluginGroup(const QString& name, QToolBar* toolBar, QMenu* menu);
	void registerPluginGroup(const QString& name);
	void loadPlugins();

public slots:
	void loadContextMenu(QMenu* contextMenu, QTreeWidgetItem* selectedItem);

signals:
    void sendNowInitName(const QString&);

protected:
	PluginEntry* getOrCreatePluginEntry(const QString& pluginName);

	void parseDependency(const QString& fileName, const QDir& pluginsDir);

	void loadPlugin(PluginEntry* pluginEntry);
	void registerPlugin(MPluginInterface* plugin);
	MPluginInterface* instantiate(QObject* instance);

private:
	QMap<QString, PluginGroup> _pluginGroups;
	QMap<QString, PluginEntry*> _pluginEntries;
	QVector<PluginEntry*> _readyToLoad;
	QVector<MPluginInterface*> _loadedPlugins;
	OsgQWidget* _viewerWidget;
	DataManager* _dataManager;
};

#endif // PLUGINMANAGER_H
