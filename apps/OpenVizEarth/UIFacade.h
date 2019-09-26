#ifndef ATLAS_H
#define ATLAS_H

#include <MainWindow.h>

#include "../NameSpace.h"

#include <osg/Object>
#include <osg/BoundingSphere>

class DataManager;
class SettingsManager;
class ViewerWidget;
class MPluginManager;
class MouseEventHandler;

namespace osg
{
class Group;
class PositionAttitudeTransform;
}

namespace osgSim
{
	class OverlayNode;
}

namespace osgText
{
class Font;
}

namespace osgEarth
{
class Map;
class MapNode;
}

class UIFacade: public MainWindow
{
	Q_OBJECT

public:
	UIFacade(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

	~UIFacade();

  void  initAll();

private:
  void  initView();

  void initDCUIVar();

  void  initDataManagerAndScene();

  void  initPlugins();

  void  initLog();

  void  setupUi();

  void  collectInitInfo();

  //UI style
  void initUiStyles();

public slots:

	// View related slots
  void  resetCamera();

signals:
	// For splash screen
  void  sendTotalInitSteps(int);

  void  sendNowInitName(const QString&);

private:
  std::ofstream *_log;

  DataManager     *_dataManager;
  SettingsManager *_settingsManager;
  MPluginManager   *_pluginManager;
  osg::ref_ptr<MouseEventHandler>     _mousePicker;

	// Root for all
  osg::ref_ptr<osg::Group>                      _root;
  // Root for all data that can be projected on
  osg::ref_ptr<osgSim::OverlayNode>             _dataOverlay;
  // Node that is projected to the _dataOverlay
  osg::ref_ptr<osg::Group>  _overlaySubgraph;
  // Root for all drawings
  osg::ref_ptr<osg::Group>  _drawRoot;
  // Root for osgEarth maps
  osg::ref_ptr<osg::Group>  _mapRoot;
  // Root for osg format data
  osg::ref_ptr<osg::Group>  _dataRoot;

  osg::ref_ptr<osgEarth::MapNode>  _mapNode[MAX_SUBVIEW];
  osg::ref_ptr<osgEarth::Map>      _mainMap[MAX_SUBVIEW];
};

#endif
