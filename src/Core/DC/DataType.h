#pragma once

#pragma execution_character_set("utf-8")

//定义变量的别名，用来切换具有统一接口的库，例如
//可以切换使用基类OsgQWidget或者DCScene::SceneView
//作为系统的主要viewWidget

#include "OsgQWidget/OsgQWidget.h"
//DCScene
#include "DCScene/scene/SceneView.h"
#include "DCScene/scene/SceneModel.h"

#include <ONodeManager/DataManager.h>
#include <Manager/Manager.h>

namespace DC
{
	class SceneView;
}

class OsgQWidget;
class DataManager;

//-------------------确定项目使用的viewWidget类-------------------------------
typedef DC::SceneView OSGViewWidget;
//typedef OsgQWidget OSGViewWidget;

//-------------------确定项目使用的节点管理类manager-------------------------------
//typedef Manager UserDataManager;
typedef DataManager UserDataManager;
