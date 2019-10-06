#ifndef PXDATAMANAGER_H
#define PXDATAMANAGER_H

#include "manager_global.h"
#include "../../NameSpace.h"
#include "QVector"

#include <QObject>

#include <osg/Vec3>
#include <osg/Node>
#include "QItemSelection"

#include <QMap>
#include <QVector>
#include <QPair>

QT_BEGIN_NAMESPACE
class QProgressBar;
class QMenu;
class QAction;
class QTreeWidgetItem;
class QTreeView;
QT_END_NAMESPACE

namespace osgSim {
	class OverlayNode;
}

namespace osgEarth {
	class MapNode;
	class ModelLayer;
	class Layer;
	class SpatialReference;
	class GeoExtent;
	class Map;
}
namespace osgText{
	class Font;
}
namespace osg {
	class Group;
	class OverlayNode;
}

class SettingsManager;
class DataRecord;
class DataTree; 

class SkyBox;

class FeatureStyleSettingDlg;
class ModelLayerManager;
class ColorVisitor;
class FontVisitor;
class IconSymbolVisitor;

class QMainWindow;

class CompleteLineEdit;
class ToolBoxTreeModel;

class NodeTreeModel;
class NodePropertyWidget;

typedef QPair<QString, QString> attrib;
typedef QVector<QString> feature;

class MANAGER_EXPORT Manager : public QObject
{
	Q_OBJECT

public:
	Manager(QObject* parent);
	~Manager();

	void reset();

	// Settings and registration
	void registerDataRoots(osg::Group* root);

	// Node visibility management
	void setWindowMask(const QString& nodeName, int mask);
	int getMask(const QString& nodeName);

	// Getters
	const osgEarth::GeoExtent* getExtent(const QString& name);
	QList<QTreeWidgetItem*> getSelectedItems();

	
	void setupUi(QMainWindow* mw);
public slots:
	// Data management
	void newProject();
	void recordData(osg::Node* node, const QString& name, const QString& parent, bool hidden = false);
	void recordData(osgEarth::Layer* layer, const QString& name, const QString& parent, osgEarth::GeoExtent* = NULL, bool hidden = false);
	void removeData(const QString& nodeName);
	void switchData(const QString& nodeName, bool checked);

	// Style management
	//void colorChangeSlot(PrimType type, osg::Vec4 color);
	//void fontChangeSlot(osgText::Font* font, int size);
	//void sizeChangeSlot(PrimType type, int size);
	//osgEarth::ModelLayer* changeLayerStyle(
	//	std::string path, const QString& gemtype, FileType addType, std::string iconPath, float layerHeight);
	//void changeLayerStyleSlot();

	// Data manipulation
	void showDataTreeContextMenu(const QPoint &pos);
	void doubleClickTreeSlot(QTreeWidgetItem* item, int column);

	
	void showMetaDataSlot();
	//void showAttributeTableSlot();

	//! nodeLayers图层节点管理
	void CreateLayerContainerNode(QString layersName);

	//! modelSelet发生变化
	void ChangeSelection(const QItemSelection & selected, const QItemSelection & deselected);

	void updateAttributeList(const QString& name, const QVector<attrib>& attribs);
	void updateFeatureTable(const QString& name, const QVector<feature>& features);
	void updateFeatureFieldList(const QString& name, const QStringList& featureList);

	void NodeSelected(const QModelIndex &index);
signals:
	void moveToNode(const osg::Node*, double);
	void moveToBounding(const osg::BoundingSphere*, double);
	void loadingProgress(int);
	void loadingDone();
	void requestContextMenu(QMenu*, QTreeWidgetItem*);
	void resetCamera();

private:
	void InitDockWidget();
	void initDataTree();
	void initToolBox();

private:
	// Program structure reference
	SettingsManager* _settingsManager;

	// Tree structures
	DataTree* _nodeTree;
	QMenu* _treeWidgetMenu;

	// Actions
	QAction* showAllChildrenAction;
	QAction* hideAllChildrenAction;
	QAction* saveNodeAction;
	QAction* deleteNodeAction;
	//QAction* rotateModelAction;
	//QAction* moveModelAction;
	//QAction* showMetatDataAction;
	//QAction* showNodeLabelAction;
	//QAction* changeLayerStyleAction;
	//QAction* showAttributeTableAction;


	// Data loading
	//ModelLayerManager* _modellyermanager;
	char* _wktInfo;
	int _countLoadingData;

	// Style visitors
	//ColorVisitor* _colorvisitor;
	//FontVisitor* _fontvisitor;
	//IconSymbolVisitor* _iconsymbolvisitor;
	//FeatureStyleSettingDlg *_featureStyleDlg;
	CompleteLineEdit* m_searchEdit;
	//QTreeVIew
	QTreeView* m_toolBoxTreeView;
	//场景model
	ToolBoxTreeModel* m_pToolBoxTreeModel;

	QMainWindow* m_mainWindow;

	//! 节点管理模型视图
	NodeTreeModel* m_nodeTreeModel;
	NodePropertyWidget* m_propertyWidget;
};

#endif
