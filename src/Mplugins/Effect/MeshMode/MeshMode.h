#pragma once
#include <QtPlugin>
#include <DC/MPluginInterface.h>

QT_BEGIN_NAMESPACE
class QToolBar;
class QAction;
class QMenu;
QT_END_NAMESPACE

class MeshMode : public MPluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "wly.OpenViz.MPluginInterface" FILE "MeshMode.json")
	Q_INTERFACES(MPluginInterface)

public:
	MeshMode();
	~MeshMode();
	virtual void setupUi(QToolBar *toolBar, QMenu *menu) override;

public slots:
	virtual void trigger();

protected:
	QAction* _action;
	int _mode;
};
