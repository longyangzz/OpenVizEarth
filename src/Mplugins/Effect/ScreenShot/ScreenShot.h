#pragma once
#include <QtPlugin>
#include <DC/MPluginInterface.h>

QT_BEGIN_NAMESPACE
class QToolBar;
class QAction;
class QMenu;
QT_END_NAMESPACE

class ScreenShot : public MPluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "wly.OpenViz.MPluginInterface" FILE "ScreenShot.json")
	Q_INTERFACES(MPluginInterface)

public:
	ScreenShot();
	~ScreenShot();
	virtual void setupUi(QToolBar *toolBar, QMenu *menu) override;

public slots:
	virtual void trigger();

private:
	QAction* _action;
};
