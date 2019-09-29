#ifndef OPENVIZMG_COMPLETELINEEDIT_H
#define OPENVIZMG_COMPLETELINEEDIT_H

#include <QLineEdit>
#include <QStringList>

class QListView;
class QStringListModel;
class QModelIndex;
class QToolButton;
class QMenu;
class QAction;

class CompleteLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	enum MatchMode
	{
		eStartWith = 0,
		eContains = 1
	};

	explicit CompleteLineEdit(QWidget *parent = 0);
	CompleteLineEdit(bool enableCaseSensitive, bool matchMode, QWidget* parent = 0);

	~CompleteLineEdit();

	void SetInactiveText(const QString& text);
	void InitModelView();

private:
	void Init(bool enableCaseSensitive, bool enableWholeWords);
	QMenu* CreateOptionMenu(bool enableCaseSensitive, bool matchMode);

protected:
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void focusInEvent(QFocusEvent* event);
	virtual void focusOutEvent(QFocusEvent *e);
	virtual void paintEvent(QPaintEvent* event);
	virtual void resizeEvent(QResizeEvent* event);

signals:

	//! 辅助搜索相关
private slots:
	//! 显示选项控制菜单
	void ShowOptionMenu();

	//! 更改大小写敏感
	void ChangeCaseSensitive(bool checked);

	//! 更改匹配选项
	void ChangeMatchMode(bool checked);
	//! 搜索相关
public slots:
	void SetCompleter(const QString &text);
	void CompleteText(const QModelIndex &index);

private:
	//! 匹配的数据model
	QStringListModel* model;

	//! 装饰相关
	QToolButton* m_searchButton;
	QMenu* m_optionMenu;
	QAction* m_actionCaseSensitive;
	QAction* m_actionMatchMode;
	QString m_inactiveText;

	//! 选项控制
	//! 大小写控制
	Qt::CaseSensitivity m_caseSensitive;

	//! 匹配模式
	MatchMode m_matchMode;
public:
	//! 整个搜索范围
	QStringList word_list;

	QListView* listView;
};

#endif 
