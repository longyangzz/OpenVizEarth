// Project
#include "NodeTreeModel.h"

// Qt
#include <QtGui/QIcon>
#include "QTreeView"
// OSG
#include <osg/LOD>
#include <osg/Billboard>
#include <osg/Geode>
#include <osg/Node>
#include <osg/Group>
#include <osg/Switch>

//==============================================================================

NodeTreeModel::NodeTreeModel(QTreeView* treeView)
    : QAbstractItemModel()
	, m_treeView(treeView)
{
    m_hashIcon.insert( "LOD", QIcon(":/treeView/Resources/treeview/lod.png") );
    m_hashIcon.insert( "Switch", QIcon(":/treeView/Resources/treeview/switch.png") );
    m_hashIcon.insert( "Group", QIcon(":/treeView/Resources/treeview/group.png") );
    m_hashIcon.insert( "MatrixTransform", QIcon(":/treeView/Resources/treeview/transform.png") );
    m_hashIcon.insert( "Billboard", QIcon(":/treeView/Resources/treeview/billboard.png") );
    m_hashIcon.insert( "Geode", QIcon(":/treeView/Resources/treeview/geode.png") );
    m_hashIcon.insert( "PointLight", QIcon(":/treeView/Resources/treeview/pointlight.png") );
    m_hashIcon.insert( "SpotLight", QIcon(":/treeView/Resources/treeview/spotlight.png") );
    m_hashIcon.insert( "LightSource", QIcon(":/treeView/Resources/treeview/lightsource.png") );
    m_hashIcon.insert( "Bone", QIcon(":/treeView/Resources/treeview/bone.png") );
    m_hashIcon.insert( "Skeleton", QIcon(":/treeView/Resources/treeview/skeleton.png") );
    m_hashIcon.insert( "Sequence", QIcon(":/treeView/Resources/treeview/sequence.png") );

	m_treeView = (treeView);
	m_treeView->setModel(this);
}

//==============================================================================

NodeTreeModel::~NodeTreeModel()
{}

//==============================================================================

void NodeTreeModel::setNode(osg::Node *node)
{
    //reset();
    m_rootNode = node;
}

//==============================================================================

osg::Node* NodeTreeModel::getNode()
{
    return m_rootNode.get();
}

//==============================================================================

const osg::Node* NodeTreeModel::getNode() const
{
    return m_rootNode.get();
}

//==============================================================================

QModelIndex NodeTreeModel::index(int row, int column,
                                const QModelIndex &parent) const
{
    QModelIndex index;

    if ( !parent.isValid() )
    {
        index = createIndex( row, column, m_rootNode.get() );
    }
    else
    {
        osg::Group *grp = dynamic_cast<osg::Group*>( getPrivateData(parent) );

        if ( grp && row < (int)grp->getNumChildren() )
        {
            index = createIndex( row, column, grp->getChild(row) );
        }
        else
        {
            index = createIndex(row, column);
        }
    }

    return index;
}

//==============================================================================

QModelIndex NodeTreeModel::parent(const QModelIndex &index) const
{
    if ( !index.isValid() || getPrivateData(index) == m_rootNode.get() )
        return QModelIndex();

    if (getPrivateData(index) == NULL)
        return QModelIndex();

    osg::ref_ptr<osg::Node> node = dynamic_cast<osg::Node*>( getPrivateData(index) );

    if (node.valid() && node->getNumParents() > 0)
    {
        osg::ref_ptr<osg::Group> parent = node->getParent(0);
        osg::ref_ptr<osg::Group> pp = parent->getParent(0);

        if ( parent.valid() && pp.valid() )
            return createIndex( pp->getChildIndex( parent.get() ), 0, parent.get() );
    }

    return QModelIndex();
}

//==============================================================================

int NodeTreeModel::rowCount(const QModelIndex &parent) const
{
    if ( !parent.isValid() )
        return 1;

    osg::ref_ptr<osg::Node> node = reinterpret_cast<osg::Node*>( parent.internalPointer() );

    if ( node.valid() )
    {
        osg::Group *group = node->asGroup();

        if (group)
            return group->getNumChildren();
    }

    return 0;
}

//==============================================================================

int NodeTreeModel::columnCount(const QModelIndex & /*parent */) const
{
    return NB_COL;
}

//==============================================================================

QVariant NodeTreeModel::data(const QModelIndex &index, int role) const
{
    if ( !getPrivateData(index) )
        return QVariant();

    QVariant d;

    osg::ref_ptr<osg::Node> node = getPrivateData(index);
    QHash<QString, QIcon>::const_iterator it;

    if ( role == Qt::DisplayRole ) // name
    {
        if (index.column() == COL_NAME)
        {
            if ( node->getName().empty() )
                d = tr("<unnamed>");
            else
                d = QString::fromStdString( node->getName() );
        }
        else if (index.column() == COL_TYPE)
        {
            d = QString::fromStdString( node->className() );
        }
        else if (index.column() == COL_DESCRIPTION)
        {
            QString description;

            for (size_t i = 0; node->getNumDescriptions(); i++)
                description += QString::fromStdString( node->getDescription(i) ) + "\n";
        }
    }
	else if (role == Qt::EditRole)  //编辑角色
	{
		if (index.column() == 0)
		{
			return QString::fromStdString(node->getName());
		}
	}
    else if ( role == Qt::DecorationRole && ( index.column( ) == COL_NAME ) ) // icon
    {
        it = m_hashIcon.find( node->className() );
        if ( it != m_hashIcon.end() )
            d = *it;
    }
    else if ( role == Qt::CheckStateRole && ( index.column( ) == COL_NAME ) )
    {
        if ( node.valid() )
        {
            d = (node->getNodeMask() == 0 ? Qt::Unchecked : Qt::Checked);
        }
    }
    return d;
}

QModelIndex NodeTreeModel::index(osg::Node* object)
{
	if (object == m_rootNode)
		return QModelIndex();

	osg::ref_ptr<osg::Group> parent = nullptr;
	int parentNum = object->getNumParents();
	if (parentNum)
	{
		parent = object->getParent(0);
		osg::ref_ptr<osg::Group> pp = parent->getParent(0);

		if (parent.valid() && pp.valid())
			return createIndex(pp->getChildIndex(parent.get()), 0, parent.get());
	}
	if (!parent)
	{
		return QModelIndex();
	}

	int pos = parent->getChildIndex(object);

	return createIndex(pos, 0, object);
}

void NodeTreeModel::UpdateCheckState(osg::Node* entity, const bool isCheck)
{
	if (entity)
	{
		if (isCheck)
			entity->setNodeMask(0xffffffff);
		else
			entity->setNodeMask(0x0);
	}

	//! 更新当前实体对应index的check
	QModelIndex cIndex = index(entity);
	emit dataChanged(cIndex, cIndex);

	//遍历entity，保证所有的子实体enable属性一致
	//int count = entity->GetChildrenNumber();
	//for (int i = 0; i != count; ++i)
	//{
	//	UpdateCheckState(entity->GetChild(i), isCheck);
	//}
}

bool NodeTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.isValid())
	{
		if (role == Qt::EditRole)
		{
			if (value.toString().isEmpty())
				return false;

			osg::Node* entity = NodeFromIndex(index);
			if (entity)
			{
				entity->setName(value.toString().toStdString());

				emit dataChanged(index, index);
			}

			return true;
		}
		else if (role == Qt::CheckStateRole)
		{
			osg::Node* entity = NodeFromIndex(index);

			UpdateCheckState(entity, value.toBool());

			//重绘
			//entity->PrepareDisplayForRefresh();

			return true;
		}
	}

	return false;
}

osg::Node* NodeTreeModel::NodeFromIndex(const QModelIndex &index) const
{
	if (index.isValid())
	{
		return static_cast<osg::Node *>(index.internalPointer());
	}
	else
	{
		return m_rootNode;
	}
}
//==============================================================================

void NodeTreeModel::setEnableIndex(const QModelIndex &index, bool val)
{
    osg::ref_ptr<osg::Node> node = getPrivateData(index);
    if ( node.valid() )
    {
        if (val)
            node->setNodeMask(0xffffffff);
        else
            node->setNodeMask(0x0);
    }
}

void NodeTreeModel::clear()
{ 
	//reset();
}

//==============================================================================

QVariant NodeTreeModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if ( section == COL_NAME )
            return QString::fromLocal8Bit( "名字" );
        else if ( section == COL_TYPE )
            return QString::fromLocal8Bit( "类型" );
        else if ( section == COL_DESCRIPTION )
            return QString::fromLocal8Bit( "描述" );
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

//==============================================================================

Qt::ItemFlags NodeTreeModel::flags(const QModelIndex &index) const
{


	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

	//通用flags
	defaultFlags |= (Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsTristate);

	return defaultFlags;
}

//==============================================================================

osg::Node* NodeTreeModel::getNode(const QModelIndex &index)
{
    if ( index.isValid() )
    {
        return getPrivateData(index);
    }

    return 0L;
}

//==============================================================================

QModelIndex NodeTreeModel::searchForNode(osg::Node *node, const QModelIndex &parent)
{
    // firstly check parent
    if ( parent.isValid() )
    {
        if ( node == getNode(parent) )
            return parent;
    }

    for (int i = 0; i < rowCount(parent); i++)
    {
        QModelIndex ind = index(i,0,parent);
        QModelIndex result = searchForNode(node,ind);
        if ( result.isValid() )
            return result;
    }
    return QModelIndex();
}

//==============================================================================

QModelIndex NodeTreeModel::searchForName(const QString &name, const QModelIndex &parent)
{
    // firstly check parent
    if ( parent.isValid() )
    {
        osg::ref_ptr<osg::Node> node = getPrivateData(parent);
        if ( node.valid() && node->getName() == name.toStdString() )
            return parent;
    }

    for (int i = 0; i < rowCount(parent); i++)
    {
        QModelIndex ind = index(i,0,parent);
        QModelIndex result = searchForName(name,ind);
        if ( result.isValid() )
            return result;
    }
    return QModelIndex();
}


void NodeTreeModel::InsertToModel(osg::Node* childEntity)
{
	osg::ref_ptr<osg::Group> parentObj = childEntity->getParent(0);

	//查找插入点索引
	QModelIndex insertNodeIndex = index(parentObj);
	int childPos = parentObj->getChildIndex(childEntity);

	//插入行开始
	beginInsertRows(insertNodeIndex, childPos, childPos);

	//插入行结束
	endInsertRows();
	bool autoExpand = true;
	if (autoExpand)
	{
		QModelIndex childIndex = index(childEntity);
		if (childIndex.isValid())
			m_treeView->expand(childIndex);
	}
	else
	{
		m_treeView->expand(insertNodeIndex);
	}
}

void  NodeTreeModel::addRecord(osg::Node *node, const QString &name, const QString &parentName, bool hidden)
{
	/*QString              nodeName = resolveName(name);
	osg::BoundingSphere  bs = node->getBound();

	if (!bs.valid())
	{
		node->computeBound();
	}

	node->setName(nodeName.toLocal8Bit().toStdString());

	DataRecord *parent = getParent(parentName);
	DataRecord *newRecord = new DataRecord(nodeName, node, parent);
	newRecord->setCheckState(0, (node->getNodeMask() & SHOW_IN_ALL_WINDOW) == 0 ? Qt::Unchecked : Qt::Checked);
	newRecord->setHidden(hidden);
	_dataRecords.insert(nodeName, newRecord);

	parent->addChild(newRecord);*/

	//! 获取A的classname，从scene一级子类中找到该type，然后添加A
	//DcGp::Container childs = scene->GetChildren();

	//for (int i = 0; i != childs.size(); ++i)
	//{
	//	auto aa = childs[i]->GetType();
	//	auto bb = A->GetClassname();
	//	if (childs[i]->GetType() == A->GetClassname())
	//	{
	//		auto cc = A->metaObject()->superClass();
	//		childs[i]->AddChild(A);
	//		break;
	//	}
	//}
	//m_rootNode->

	//！ 在这里报错，则说明创建的实体对象没有设置类型值 或者A实体没有重载GetClassName函数
	InsertToModel(node);
}

void  NodeTreeModel::removeRecord(const QString &name)
{
	//auto  recordItr = _dataRecords.find(name);

	//if (recordItr == _dataRecords.end())
	//{
	//	return;
	//}

	//DataRecord *record = *recordItr;

	//if (record == _rootTreeItem)
	//{
	//	return;
	//}

	//removeRecord(record);

	//// Remove from parent
	//DataRecord *parent = record->parent();
	//parent->removeChild(record);

	//if (parent->childCount() == 0)
	//{
	//	removeRecord(parent->text(0));
	//}
}

void  NodeTreeModel::addRecord(osgEarth::Layer *layer, const QString &name, const QString &parentName, osgEarth::GeoExtent *extent, bool hidden)
{
	/*QString  nodeName = resolveName(name);

	layer->setName(nodeName.toLocal8Bit().toStdString());

	DataRecord *parent = getParent(parentName);
	DataRecord *newRecord = new DataRecord(nodeName, layer, parent, extent);
	newRecord->setCheckState(0, layer->getEnabled() ? Qt::Checked : Qt::Unchecked);
	newRecord->setHidden(hidden);
	_dataRecords.insert(nodeName, newRecord);

	parent->addChild(newRecord);*/
}
