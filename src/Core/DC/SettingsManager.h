#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H
#include "dc_global.h"

#include "../NameSpace.h"

#include <osg/Vec4>

#include <QObject>
#include <QString>
#include <QSettings>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

namespace osgEarth {
	class SpatialReference;
}

#define PACKAGE_ORGANIZATION "DC"
#define PACKAGE_NAME "OPenViz"
#define PACKAGE_VERSION "5.0.0"

class DC_EXPORT SettingsManager : public QSettings
{
	Q_OBJECT

public:
	SettingsManager(QObject* parent = nullptr);
	~SettingsManager();

    void setOrAddSetting(const QString& key, const QVariant& value);
    QVariant getOrAddSetting(const QString& key, const QVariant& defaultValue);

    void setupUi(QMenu* menu);
	
	void setGlobalSRS(const osgEarth::SpatialReference * globalSRS);
	const osgEarth::SpatialReference* getGlobalSRS();

public slots:
    void reset();

private:
	const osgEarth::SpatialReference* _globalSRS;
};


#endif
