#ifndef MPLUGINMANAGER_H
#define MPLUGINMANAGER_H
#include "dc_global.h"

#include "../../NameSpace.h"
#include <QObject>
#include <QSet>
#include <QMap>
#include <QDir>
#include <QVector>
#include <QString>

#include "DC/DataType.h"

class MPluginInterface;

struct PluginEntry;

QT_BEGIN_NAMESPACE
class QToolBar;
class QMenu;
class QTreeWidgetItem;
QT_END_NAMESPACE

#include "DC/DataType.h"

class DC_EXPORT MPluginManager : public QObject
{
	Q_OBJECT

public:
	struct PluginGroup {
		QString groupName;
		QToolBar* toolBar;
		QMenu* menu;
	};

public:
	MPluginManager(QObject *parent, UserDataManager* dataManager, OSGViewWidget* viewer);
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
	OSGViewWidget* _viewerWidget;
	UserDataManager* _dataManager;
};

#endif // PLUGINMANAGER_H
