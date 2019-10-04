#pragma execution_character_set("utf-8")
#include "MeshMode.h"

#include <QAction>
#include <QMenu>
#include <QToolBar>

#include <osg/PolygonMode>

#include <ONodeManager/FindNode.hpp>

MeshMode::MeshMode()
	: _mode(0)
{
	_pluginName = tr("Mesh Mode");
	_pluginCategory = tr("渲染");;
}

MeshMode::~MeshMode()
{
}

void MeshMode::setupUi(QToolBar * toolBar, QMenu * menu)
{
	_action = new QAction(_mainWindow);
	_action->setObjectName(QStringLiteral("meshModeAction"));
	QIcon icon8;
	icon8.addFile(QStringLiteral("resources/icons/triangulation.png"), QSize(), QIcon::Normal, QIcon::Off);
	_action->setIcon(icon8);

	_action->setText(tr("网格模式"));
	_action->setToolTip(tr("切换网格渲染模式"));

	connect(_action, SIGNAL(triggered()), this, SLOT(trigger()));
	registerMutexAction(_action);

	toolBar->addAction(_action);
	menu->addAction(_action);
}

void MeshMode::trigger()
{
	osg::PolygonMode * polygonMode = new osg::PolygonMode;

	switch (_mode)
	{
	case(0):
    {
        auto skyNode = findNodeInNode("Sky", _root);
        if (skyNode)
            skyNode->setNodeMask(0);
        polygonMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
        _mode++;
        break;
    }
	case(1):
    {
        polygonMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::POINT);
        _mode++;
        break;
    }
	case(2):
    {
        auto skyNode = findNodeInNode("Sky", _root);
        if (skyNode)
            skyNode->setNodeMask(1);
        polygonMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
        _mode = 0;
        break;
    }
    default:
        return;
	}
	_root->getStateSet()->setAttributeAndModes(polygonMode, osg::StateAttribute::ON);
}
