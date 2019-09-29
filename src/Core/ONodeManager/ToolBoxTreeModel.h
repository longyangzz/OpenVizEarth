
#ifndef OPENVIZMG_SCENETREEMODEL_H
#define OPENVIZMG_SCENETREEMODEL_H

// Qt
#include <QtCore/QAbstractItemModel>


class QTreeView;
namespace osg
{
	class Node;
}

class ToolBoxTreeView;
class  ToolBoxTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:

	enum
	{
		COL_NAME = 0,
		COL_TYPE,
		COL_DESCRIPTION,
		NB_COL
	};

	ToolBoxTreeModel(QTreeView* treeView);
	virtual ~ToolBoxTreeModel();

	//!--------------------Qt模型视图接口------------------------------------------------
	void                clear() {  }

	Qt::ItemFlags       flags(const QModelIndex &index) const;

	QModelIndex index(int row, int column, const QModelIndex &parent) const;
	QModelIndex index(osg::Node* entity);
	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	bool hasChildren(const QModelIndex &parent) const
	{
		return rowCount(parent) > 0;
	}

	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;


	osg::Node* NodeFromIndex(const QModelIndex &index) const;

public:



private:



private:
	//管理的所有场景
	osg::Node*  m_secnes;

	//视图
	ToolBoxTreeView* m_treeView;

	QHash<QString, QIcon> m_hashIcon;
};

#endif // MPMG_SCENETREEMODEL_H
