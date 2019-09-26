#pragma once
#include <QtPlugin>
#include <EarthDataInterface/EarthDataInterface.h>

QT_BEGIN_NAMESPACE
class QToolBar;
class QAction;
class QMenu;
QT_END_NAMESPACE

class AddArcGISData : public EarthDataInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "wly.OpenViz.MPluginInterface" FILE "AddArcGISData.json")
	Q_INTERFACES(MPluginInterface)

public:
	AddArcGISData();
	~AddArcGISData();
	virtual void setupUi(QToolBar *toolBar, QMenu *menu) override;

public slots:
	void addImage();
	void addTerrain();
	void addFeature();
};
