#ifndef MODELLAYERMANAGER_H
#define MODELLAYERMANAGER_H

#include <QObject>

#include <QString>
#include <string>

#include "../../../NameSpace.h"
#include <ONodeManager/DataFormats.h>

#include <DC/MPluginInterface.h>

#include <osg/Object>

#include <osgEarthSymbology/Style>

class SettingsManager;
namespace osgEarth
{
class ModelLayer;

namespace Symbology
{
class Style;
}
}

class ModelLayerManager: public QObject
{
	Q_OBJECT

public:

  ModelLayerManager(const MPluginInterface::StyleConfig &style);

	~ModelLayerManager();

  osg::ref_ptr<osgEarth::ModelLayer>  changeLayerStyle(std::string path, const QString& gemtype, std::string iconPath, float layerHeight);

  osg::ref_ptr<osgEarth::ModelLayer>  createModelLabelLayer(const QString& layerPath, std::string fieldName, float height);

  QVector<attrib>                     getVectorMetaData(const QString& path, QStringList &fieldList);

  void                                getFeatureAttribute(const QString& path, QVector<attrib> &attributeList, QStringList &featureFieldList,
                                                          osgEarth::Symbology::Style *style);

  char                              * getSRS()
  {
    return _origlSRS;
  }

  QString& getGemType()
  {
    return _gemtype;
  }

private:
  char                         *_origlSRS;
  QString                       _gemtype;
  MPluginInterface::StyleConfig  _style;
};

#endif // MODELLAYERMANAGER_H
