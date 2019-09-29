#include "ToolBoxTreeView.h"

//qt
#include "QTreeView"
#include "QModelIndex"
#include "QItemSelectionModel"
#include "QKeyEvent"

//osg
#include "osg/Node"

#include "ToolBoxTreeModel.h"


ToolBoxTreeView::ToolBoxTreeView(QWidget *parent)
	: QTreeView(parent)
	, m_pToolBoxTreeModel(nullptr)
{

}

ToolBoxTreeView::~ToolBoxTreeView()
{

}

void ToolBoxTreeView::SetModel(ToolBoxTreeModel* model)
{
	m_pToolBoxTreeModel = model;
}

void ToolBoxTreeView::DeleteSelectItems()
{
	//! 选取模型
	QItemSelectionModel* qitemSelect = selectionModel();

	//! 是否选中了item
	QModelIndexList selectIndexes = qitemSelect->selectedIndexes();
	int selectItemsCount = selectIndexes.size();

	if (selectItemsCount == 0)
	{
		return;
	}


}

void ToolBoxTreeView::keyPressEvent(QKeyEvent *event)
{
	//！ 删除当前选中条目
	if (event->key() == Qt::Key_E)
	{
		DeleteSelectItems();
	}
}
