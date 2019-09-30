#ifndef OPENVIZMG_OBJECTTREEVIEW_H
#define OPENVIZMG_OBJECTTREEVIEW_H

#include <QTreeView>

class ToolBoxTreeModel;
class ToolBoxTreeView : public QTreeView
{
	Q_OBJECT

public:
	ToolBoxTreeView(QWidget *parent = 0);
	~ToolBoxTreeView();

	void DeleteSelectItems();
	void SetModel(ToolBoxTreeModel* model);

private:
	void keyPressEvent(QKeyEvent *event);
private:
	//model
	ToolBoxTreeModel* m_pToolBoxTreeModel;
};

#endif // MPMG_OBJECTTREEVIEW_H
