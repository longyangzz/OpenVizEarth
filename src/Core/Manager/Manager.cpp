#pragma execution_character_set("utf-8")

#include "Manager.h"

#include <QVector>
#include <QMenu>
#include <QMetaType>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QMainWindow>
#include <QSignalMapper>
#include <QLineEdit>


#include <osgSim/OverlayNode>
#include <osgEarth/GeoData>
#include <osgEarth/MapNode>


//#include "FeatureStyleSettingDlg.h"
//#include "ColorVisitor.h"
//#include "FontVisitor.h"
//#include "ModelManipulator.h"

Manager::Manager(QObject* parent /*= NULL*/)
	: QObject()
	, _countLoadingData(0)
	
	//, _featureStyleDlg(NULL)
{
	//initDataTree();

	//_colorvisitor = new ColorVisitor;
	//_fontvisitor = new FontVisitor;
}

Manager::~Manager()
{
}

void Manager::reset()
{
	
}

void Manager::InitDockWidget()
{
	
	//！ 创建tree dock节点管理面板
	initDataTree();

	//！ 创建工具箱面板
	initToolBox();
}

//！ 建立成员变量存放所有的菜单action，并初始化
void Manager::setupUi(QMainWindow* mainWindow)
{
	//! 记录主窗口
	m_mainWindow = mainWindow;

	//!3. 创建docket组件
	InitDockWidget();

	//！ 初始化信号槽及action
	// Context menu actions
	//showAttributeTableAction = new QAction(mainWindow);
	//showAttributeTableAction->setObjectName(QStringLiteral("showAttributeTableAction"));
	//showAttributeTableAction->setText(tr("Attributes"));
	//showAttributeTableAction->setToolTip(tr("Show attribute table"));

	//showMetatDataAction = new QAction(mainWindow);
	//showMetatDataAction->setObjectName(QStringLiteral("showMetatDataAction"));
	//showMetatDataAction->setText(tr("Meta Data"));
	//showMetatDataAction->setToolTip(tr("Show meta data"));

	showAllChildrenAction = new QAction(mainWindow);
	showAllChildrenAction->setObjectName(QStringLiteral("showAllChildrenAction"));
	showAllChildrenAction->setText(tr("Show"));
	showAllChildrenAction->setToolTip(tr("Show all children"));

	hideAllChildrenAction = new QAction(mainWindow);
	hideAllChildrenAction->setObjectName(QStringLiteral("hideAllChildrenAction"));
	hideAllChildrenAction->setText(tr("Hide"));
	hideAllChildrenAction->setToolTip(tr("Hide all children"));

	saveNodeAction = new QAction(mainWindow);
	saveNodeAction->setObjectName(QStringLiteral("saveNodeAction"));
	saveNodeAction->setText(tr("Save"));
	saveNodeAction->setToolTip(tr("Save node"));

	deleteNodeAction = new QAction(mainWindow);
	deleteNodeAction->setObjectName(QStringLiteral("deleteNodeAction"));
	deleteNodeAction->setText(tr("Delete"));
	deleteNodeAction->setToolTip(tr("Delete node"));

	
	
}

void Manager::initToolBox()
{

	if (!m_mainWindow)
	{
		return;
	}


	
}

void Manager::initDataTree()
{
	if (!m_mainWindow)
	{
		return;
	}

	_treeWidgetMenu = new QMenu(m_mainWindow);

	// Init dock
	
}

void Manager::ChangeSelection(const QItemSelection & selected, const QItemSelection & deselected)
{
	//! 取消选择
	QModelIndexList deselectedItems = deselected.indexes();
	{
		for (int i = 0; i < deselectedItems.count(); ++i)
		{
			osg::Node* element = static_cast<osg::Node*>(deselectedItems.at(i).internalPointer());
			if (element)
			{
				//element->SetSelected(false);
			}
		}
	}

	//执行选择
	QModelIndexList selectedItems = selected.indexes();
	{
		for (int i = 0; i < selectedItems.count(); ++i)
		{
			osg::Node* element = static_cast<osg::Node*>(selectedItems.at(i).internalPointer());
			if (element)
			{
				//element->SetSelected(true);
				//element->PrepareDisplayForRefresh();
			}
		}
	}


	//! 更新属性
	//UpdateProperty();

	//QVector<osg::Node* > selEntities;
	//QItemSelectionModel* qism = _nodeTree->selectionModel();
	//QModelIndexList selectedIndexes = qism->selectedIndexes();
	//int i, selCount = selectedIndexes.size();

	//for (i = 0; i < selCount; ++i)
	//{
	//	osg::Node* anEntity = static_cast<osg::Node*>(selectedIndexes[i].internalPointer());
	//	if (anEntity)
	//		selEntities.push_back(anEntity);
	//}

	//emit SelectionChanged(selEntities);
}

void Manager::recordData(osg::Node* node, const QString& name, const QString& parent, bool hidden)
{
	//_nodeTree->addRecord(node, name, parent, hidden);
}

void Manager::recordData(osgEarth::Layer* layer, const QString& name, const QString& parent, osgEarth::GeoExtent* extent, bool hidden)
{
	//_nodeTree->addRecord(layer, name, parent, extent, hidden);
}

void Manager::CreateLayerContainerNode(QString layerNodeName)
{
	//DataRecord *parent = _nodeTree->getRecord(layerNodeName);

	//if (!parent)
	//{
	//	parent = new DataRecord(layerNodeName, _nodeTree->GetRootTree());
	//	_nodeTree->GetRootTree()->addChild(parent);
	//	_nodeTree->GetDataRecords().insert(layerNodeName, parent);
	//	parent->setExpanded(true);
	//}
	
}

void Manager::removeData(const QString& nodeName)
{
	//_nodeTree->removeRecord(nodeName);
}

void Manager::switchData(const QString& nodeName, bool checked)
{
	//_nodeTree->switchRecord(nodeName, checked);
}

void Manager::setWindowMask(const QString& nodeName, int mask)
{
	//_nodeTree->setWindowMask(nodeName, mask);
}

int Manager::getMask(const QString& nodeName)
{
	/*auto record = _nodeTree->getRecord(nodeName);
	if (record)
		return record->mask();
	else*/
		return 0x00000000;
}


void Manager::registerDataRoots(osg::Group* root)
{
	/*osg::Group* mapRoot = findNodeInNode("Map Root", root)->asGroup();
  _nodeTree->_overlayNode = static_cast<osgSim::OverlayNode*>(findNodeInNode("Data Overlay", root));
	for (unsigned i = 0; i < MAX_SUBVIEW; i++)
	{
		osg::Node* map = findNodeInNode(QString("Map%1").arg(i).toStdString(), mapRoot);
		if (map)
			_nodeTree->_mainMap[i] = dynamic_cast<osgEarth::MapNode*>(map)->getMap();
		else
			_nodeTree->_mainMap[i] = NULL;
	}*/
}

const osgEarth::GeoExtent* Manager::getExtent(const QString& name)
{
	return  NULL;
}


QList<QTreeWidgetItem*> Manager::getSelectedItems()
{
	QList<QTreeWidgetItem*> list;
	return list;
}

void Manager::newProject()
{
	reset();
}

void Manager::updateAttributeList(const QString& name, const QVector<attrib>& attribs)
{
	//_attributeLists[name] = attribs;
}

void Manager::updateFeatureTable(const QString& name, const QVector<feature>& features)
{
	//_featureTables[name] = features;
}

void Manager::updateFeatureFieldList(const QString& name, const QStringList& featureList)
{
	//_featureFieldList[name] = featureList;
}

void Manager::showMetaDataSlot()
{
	if (m_mainWindow) {
		return;
	}

	
}

void Manager::doubleClickTreeSlot(QTreeWidgetItem* item, int column)
{
	

}

//！ 节点条目点击的右键菜单事件
void Manager::showDataTreeContextMenu(const QPoint &pos)
{
	
}


