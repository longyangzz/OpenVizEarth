#include "MPluginManager.h"

#include <QDir>
#include <QPluginLoader>
#include <QJsonValue>
#include <QJsonArray>
#include <QApplication>
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QMainWindow>


#include <osgViewer/View>

#include <DC/MPluginInterface.h>
#include <ONodeManager/DataManager.h>

//DCScene
#include "DCScene/scene/SceneView.h"
#include "DCScene/scene/SceneModel.h"

struct PluginEntry
{
  QString        name;
  unsigned       dependsToResolve = 0;
  QString        path;
  QSet<QString>  children;
};

MPluginManager::MPluginManager(QObject *parent, DataManager *dataManager, DC::SceneView* viewer):
  QObject(parent),
  _dataManager(dataManager),
  _viewerWidget(viewer)
{
  qRegisterMetaType<osgEarth::Viewpoint>("osgEarth::Viewpoint");
}

MPluginManager::~MPluginManager()
{
}

void  MPluginManager::registerPlugin(MPluginInterface *plugin)
{
	//_viewerWidget->getMainView()->addEventHandler(plugin);

  connect(plugin, SIGNAL(recordData(osg::Node *,QString,QString,bool)),
          _dataManager, SLOT(recordData(osg::Node *,QString,QString,bool)));

	connect(plugin, SIGNAL(recordData(osgEarth::Layer*, QString, QString, osgEarth::GeoExtent*, bool)),
		_dataManager, SLOT(recordData(osgEarth::Layer*, QString, QString, osgEarth::GeoExtent*, bool)));

	connect(plugin, &MPluginInterface::removeData, _dataManager, &DataManager::removeData);
  connect(plugin, &MPluginInterface::switchData, _dataManager, &DataManager::switchData);

	connect(plugin, &MPluginInterface::loadingProgress, _dataManager, &DataManager::loadingProgress);
	connect(plugin, &MPluginInterface::loadingDone, _dataManager, &DataManager::loadingDone);

	//! 为插件类创建一个图层容器节点
	connect(plugin, SIGNAL(addLayersNode(QString)),
		_dataManager, SLOT(CreateLayerContainerNode(QString)));

    //osg::ref_ptr<MapController> controller = 
    //    dynamic_cast<MapController*>(_viewerWidget->getMainView()->getCameraManipulator());
    //if (controller.valid())
    //    connect(plugin, &MPluginInterface::setViewPoint, controller, &MapController::setViewPoint);

	_loadedPlugins.push_back(plugin);
}

void  MPluginManager::loadPlugins()
{
  QDir  pluginsDir(qApp->applicationDirPath());

#if defined (Q_OS_MAC)

  if (pluginsDir.dirName() == "MacOS")
  {
		pluginsDir.cdUp();
		pluginsDir.cdUp();
		pluginsDir.cdUp();
	}

#endif
	pluginsDir.cd("Mplugins");

	// Parsing plugin dependencies
  foreach(const QString& fileName, pluginsDir.entryList(QDir::Files))
  {
    if ((fileName.split('.').back() == "so") || (fileName.split('.').back() == "dll"))
    {
      parseDependency(fileName, pluginsDir);
    }
	}

	// Load plugins based on denpendency tree
	while (!_readyToLoad.isEmpty())
	{
    PluginEntry *pluginEntry = _readyToLoad.front();
		_readyToLoad.pop_front();
		loadPlugin(pluginEntry);
	}

	// Notify the user about the remaining plugins
  for (auto failedPlugin : _pluginEntries)
	{
		qInfo() << "Plugin dependency cannot be resolved: " << failedPlugin->name;
	}
}

MPluginInterface * MPluginManager::instantiate(QObject *instance)
{
  MPluginInterface *plugin = qobject_cast<MPluginInterface *>(instance);

	if (plugin)
	{
		//! 根据插件的名称，生成菜单和工具栏组
		registerPluginGroup(plugin->getPluginGroup());

		if (_pluginGroups.contains(plugin->getPluginGroup()))
		{
			registerPlugin(plugin);
			plugin->init();

      auto &pluginGroup = _pluginGroups[plugin->getPluginGroup()];
			plugin->setupUi(pluginGroup.toolBar, pluginGroup.menu);
			qInfo() << "Plugin loaded: " << plugin->getPluginName();

			return plugin;
		}
		else
		{
			qWarning() << "Plugin group not defined: " << plugin->getPluginName();
		}
	}

	return nullptr;
}

PluginEntry * MPluginManager::getOrCreatePluginEntry(const QString& pluginName)
{
  PluginEntry *pluginEntry;

	if (!_pluginEntries.contains(pluginName))
	{
    pluginEntry                = new PluginEntry;
    pluginEntry->name          = pluginName;
		_pluginEntries[pluginName] = pluginEntry;
	}
	else
  {
    pluginEntry = _pluginEntries[pluginName];
  }

	return pluginEntry;
}

void  MPluginManager::parseDependency(const QString& fileName, const QDir &pluginsDir)
{
  try
  {
		// Get plugin info
    QString        path = pluginsDir.absoluteFilePath(fileName);
    QPluginLoader  pluginLoader(path);
    bool           debug      = pluginLoader.metaData()["debug"].toBool();
    QJsonObject    metaData   = pluginLoader.metaData()["MetaData"].toObject();
    QString        pluginName = metaData["Name"].toString();

		if (debug)
    {
      pluginName += 'd';
    }

		// Get or create a record
    PluginEntry *pluginEntry = getOrCreatePluginEntry(pluginName);
		pluginEntry->path = path;

		// Resolve dependencies
    QJsonValue  deps = metaData["Dependencies"];

		if (deps.isArray() && !deps.toArray().isEmpty())
		{
      for (QJsonValue plugin : deps.toArray())
			{
				// Register info for parent
        QString  dependName = plugin.toObject()["Name"].toString();

				if (debug)
        {
          dependName += 'd';
        }

				pluginEntry->dependsToResolve++;
        PluginEntry *parent = getOrCreatePluginEntry(dependName);
				parent->children.insert(pluginName);
			}
		}
		else
		{
			_readyToLoad.push_back(pluginEntry);
		}
	}
  catch (...)
  {
		qWarning() << "Plugin meta not valid: " << fileName;
	}
}

void  MPluginManager::loadPlugin(PluginEntry *pluginEntry)
{
	emit  sendNowInitName(tr("Init plugin: ") + pluginEntry->name);

	// Mark the plugin as parsed
	_pluginEntries.remove(pluginEntry->name);

	// Try load plugin
	QPluginLoader  pluginLoader(pluginEntry->path);
	QObject       *instance = pluginLoader.instance();

	if (instance)
	{
		// Try init plugin
		MPluginInterface *plugin = instantiate(instance);

			if (!plugin)
		{
		  return;
		}

			// Resolve related dependencies
		for (auto childName : pluginEntry->children)
			{
		  auto  childPlugin = _pluginEntries[childName];
				childPlugin->dependsToResolve--;

				if (childPlugin->dependsToResolve == 0)
				{
					_readyToLoad.push_back(childPlugin);
				}
			}
		}
		else
		{
			qWarning() << "Plugin loading failed: [" << pluginEntry->path
				   << "] " << pluginLoader.errorString();
		}
}

void  MPluginManager::loadContextMenu(QMenu *contextMenu, QTreeWidgetItem *selectedItem)
{
  for (auto plugin : _loadedPlugins)
	{
		plugin->loadContextMenu(contextMenu, selectedItem);
	}
}

void  MPluginManager::registerPluginGroup(const QString& name, QToolBar *toolBar, QMenu *menu)
{
	_pluginGroups[name] = { name, toolBar, menu };
}

void  MPluginManager::registerPluginGroup(const QString& name)
{
	//! 判断当前名字的插件组是否创建，没有则创建，并绑定工具栏及菜单栏，创建过则会共享给子类
	//! 通过外部传入插件组、插件根toolbar、插件根menu
	if (_pluginGroups.contains(name))
	{
		return;
	}

	QToolBar* curToolBar = new QToolBar();
	curToolBar->setObjectName(name + QStringLiteral("ToolBar"));
	curToolBar->setIconSize(QSize(24, 24));
	curToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	static_cast<QMainWindow*>(parent())->addToolBar(Qt::TopToolBarArea, curToolBar);

	QMenuBar* mBar = static_cast<QMainWindow*>(parent())->menuBar();
	QMenu* dataMenu = mBar->addMenu(name);
	dataMenu->setObjectName(QStringLiteral("dataMenu"));
	registerPluginGroup(name, curToolBar, dataMenu);
}
