#include "ToolBoxTreeModel.h"

// Qt
#include <QtGui/QIcon>
#include "QTreeView"

#include "osg/Node"

#include "ToolBoxTreeView.h"


//==============================================================================

ToolBoxTreeModel::ToolBoxTreeModel(QTreeView* treeView)
    : QAbstractItemModel()
	, m_secnes(new osg::Node)
{
    m_hashIcon.insert( "DcGpEntity", QIcon(":/treeview/Resources/treeview/entity.png") );
    m_hashIcon.insert( "DcGpPointCloud", QIcon(":/treeview/Resources/treeview/pointcloud.png") );
	m_hashIcon.insert( "PlanePolygon", QIcon(":/treeview/Resources/treeview/polygon.png") );
	m_hashIcon.insert( "IndividualPlane", QIcon(":/treeview/Resources/treeview/polygon.png"));
	m_hashIcon.insert( "Outlines", QIcon(":/treeview/Resources/treeview/outlines.png"));

	m_treeView = static_cast<ToolBoxTreeView* >(treeView);
	m_treeView->SetModel(this);
}

//==============================================================================

ToolBoxTreeModel::~ToolBoxTreeModel()
{

}

//==============================================================================

QModelIndex ToolBoxTreeModel::index(int row, int column,
                                const QModelIndex &parent) const
{
    QModelIndex index;

	

    return index;
}


//==============================================================================

osg::Node* ToolBoxTreeModel::NodeFromIndex(const QModelIndex &index) const
{
	if (index.isValid())
	{
		return static_cast<osg::Node *>(index.internalPointer());
	}
	else
	{
		return m_secnes;
	}
}

QModelIndex ToolBoxTreeModel::parent(const QModelIndex &index) const
{
	osg::Node* node = NodeFromIndex(index);
	if (!node)
		return QModelIndex();


    return QModelIndex();
}

//==============================================================================

int ToolBoxTreeModel::rowCount(const QModelIndex &parent) const
{


    return 0;
}

//==============================================================================

int ToolBoxTreeModel::columnCount(const QModelIndex & /*parent */) const
{
    return 1;
}

//==============================================================================

QVariant ToolBoxTreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	osg::Node* entity = NodeFromIndex(index);
	QHash<QString, QIcon>::const_iterator it;
	
	if (!m_secnes)
	{
		return QVariant();
	}
	
	
	
	return QVariant();
}


bool ToolBoxTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	

	return false;
}



//==============================================================================

QVariant ToolBoxTreeModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if ( section == COL_NAME )
            return QString( "Name" );
        else if ( section == COL_TYPE )
            return QString( "Type" );
        else if ( section == COL_DESCRIPTION )
            return QString( "Desc" );
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

//==============================================================================

Qt::ItemFlags ToolBoxTreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

	//Õ®”√flags
	defaultFlags |= (Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);

	return defaultFlags;
}