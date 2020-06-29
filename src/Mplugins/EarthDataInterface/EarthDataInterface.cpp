#include "EarthDataInterface.h"

#include <ONodeManager/DataFormats.h>

#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QToolButton>
#include <QMessageBox>

#include <osgText/Text>
#include <osg/LineWidth>
#include <osg/Point>

#include <osgEarth/Map>
#include <osgEarth/TerrainLayer>
#include <osgEarth/ModelLayer>
#include <osgEarth/ImageLayer>
#include <osgEarth/ElevationLayer>

#include "DC/DataType.h"
#include <DC/SettingsManager.h>

#include "ModelLayerManager.h"

ModelLayerManager *EarthDataInterface::_modelLayerManager;
QMenu             *EarthDataInterface::dataMenu;
QToolBar          *EarthDataInterface::dataToolBar;

EarthDataInterface::DataGroup  EarthDataInterface::_dataGroups[ALL_TYPE];

EarthDataInterface::EarthDataInterface()
{
    _pluginCategory = tr("添加数据");
    _pluginName = tr("osgEarth Data Interface");

    _dataGroups[IMAGE_LAYER] = {
        tr("影像"),
        QStringLiteral("addImage"),
        QStringLiteral("Resources/icons/image.png"),
        tr("影像"),
        tr("加载影像")
    };
    _dataGroups[TERRAIN_LAYER] = {
        tr("地形"),
        QStringLiteral("addTerrain"),
        QStringLiteral("Resources/icons/terrain.png"),
        tr("地形"),
        tr("加载地形")
    };
    _dataGroups[FEATURE_LAYER] = {
        tr("矢量"),
        QStringLiteral("addFeature"),
        QStringLiteral("Resources/icons/addshp.png"),
        tr("矢量"),
        tr("加载矢量")
    };
    _dataGroups[MODEL_LAYER] = {
      tr("模型"),
      QStringLiteral("addModel"),
      QStringLiteral("Resources/icons/model.png"),
      tr("模型"),
      tr("加载模型")
    };
}

EarthDataInterface::~EarthDataInterface()
{
}

void EarthDataInterface::CreateLayerContainerNode(QString layerNodeName)
{
	//dataManager->CreateLayerContainerNode(layerNodeName);
}

void  EarthDataInterface::setupUi(QToolBar *toolBar, QMenu *menu)
{
    dataMenu = menu;
    dataToolBar = toolBar;
	
	

    for (unsigned i = 0; i < ALL_TYPE; i++)
    {
		//! 添加注册生成根节点，根据传入的图层名称
		emit   addLayersNode(_dataGroups[i].dataTreeTitle);

        QMenu *menu = getOrAddMenu((LayerType)i);
        getOrAddToolButton((LayerType)i, menu);
    }

    parseEarthNode();
}

void  EarthDataInterface::init()
{
    _modelLayerManager = new ModelLayerManager(getDefaultStyle());

    MPluginInterface::init();
}

void  EarthDataInterface::showDataAttributes(const QString& nodeName)
{
    // TODO: Not used yet

    // GDALDataset *poDataset;
    // poDataset = (GDALDataset*)GDALOpenEx(nodeName.toLocal8Bit().toStdString().c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
    // if (poDataset != NULL)
    // {
    // QMutex* lock = new QMutex;
    // lock->lock();
    // for (int i = 0; i < poDataset->GetLayerCount(); i++)
    // {

    // FeatureTableDlg* dlg = new FeatureTableDlg(poDataset, i, lock, (QWidget*)parent());
    // dlg->setWindowTitle(nodeName + QString(tr(": Layer #")).append(QString::number(i + 1)));
    // dlg->show();
    // }
    // lock->unlock();
    // }
}

QMenu * EarthDataInterface::getOrAddMenu(LayerType dataType)
{
    if (dataType >= ALL_TYPE)
    {
        return nullptr;
    }

    const DataGroup &dataGroup = _dataGroups[dataType];
    QMenu           *menu = dataMenu->findChild<QMenu *>(dataGroup.objectName + "Menu");

    if (menu)
    {
        return menu;
    }

    menu = new QMenu(dataMenu);
    menu->setObjectName(dataGroup.objectName + "Menu");
    QIcon  icon;
    icon.addFile(dataGroup.iconPath, QSize(), QIcon::Normal, QIcon::Off);
    menu->setIcon(icon);
    menu->setTitle(dataGroup.title);
    menu->setToolTip(dataGroup.toolTip);

    dataMenu->addMenu(menu);

    return menu;
}

QToolButton * EarthDataInterface::getOrAddToolButton(LayerType dataType, QMenu *menu)
{
    if (dataType >= ALL_TYPE)
    {
        return nullptr;
    }

    const DataGroup &dataGroup = _dataGroups[dataType];
    QToolButton     *button = dataMenu->findChild<QToolButton *>(dataGroup.objectName + "Button");

    if (button)
    {
        return button;
    }

    button = new QToolButton(dataToolBar);
    button->setObjectName(dataGroup.objectName + "Button");
    QIcon  icon;
    icon.addFile(dataGroup.iconPath, QSize(), QIcon::Normal, QIcon::Off);
    button->setIcon(icon);
    button->setText(dataGroup.title);
    button->setToolTip(dataGroup.toolTip);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setCheckable(true);
    button->setMenu(menu);
	button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    dataToolBar->addWidget(button);

    return button;
}

void  EarthDataInterface::getFeatureAttribute(const QString& path, QVector<attrib> &attributeList, QStringList &featureFieldList,
    osgEarth::Symbology::Style *style)
{
    _modelLayerManager->getFeatureAttribute(path, attributeList, featureFieldList, style);
    _dataManager->updateAttributeList(path, attributeList);
    _dataManager->updateFeatureFieldList(path, featureFieldList);
}

void  EarthDataInterface::addLayerToMap(const QString& name, osg::ref_ptr<osgEarth::ModelLayer> layer, LayerType dataType, const QString& parent)
{
    if (!layer.valid())
    {
        QMessageBox::warning((QWidget *)(this->parent()), tr("Error"), tr("Create node failed!"));
        return;
    }

    for (int i = 0; i < MAX_SUBVIEW; i++)
    {
        _mainMap[i]->addLayer(layer);
    }

    // Check if the layer is added successfully
    auto added = _mainMap[0]->getLayerByName(layer->getName());
    if (added && added->getEnabled())
    {
      switch (dataType)
      {
      case(FEATURE_LAYER):
        emit recordData(layer, name, tr("矢量"));
        layer->setUserValue("gemtype", _modelLayerManager->getGemType().toStdString());
        layer->setUserValue("layerheight", 0);
        break;
      case(MODEL_LAYER):
        if (!parent.isEmpty())
          emit recordData(layer, name, parent);
        else
          osgEarth::notify(osg::WARN) << "Model map adding failed, invalid tree parent";
        break;
      default:
        osgEarth::notify(osg::WARN) << "Model map adding failed, invalid data type";
        break;
      }
    }
    else
    {
      QMessageBox::warning((QWidget *)this->parent(), tr("Error"), tr("Data loading failed!"));
    }
}

void EarthDataInterface::addLayerToMap(const QString& name, osg::ref_ptr<osgEarth::Layer> layer, LayerType dataType, QVector<attrib>& attribute, osgEarth::GeoExtent * extent)
{
  for (int i = 0; i < MAX_SUBVIEW; i++)
  {
    _mainMap[i]->addLayer(layer);
  }

  // Check if the layer is added successfully
  auto added = _mainMap[0]->getLayerByName(layer->getName());
  if (added && added->getEnabled())
  {
	emit  recordData(layer, name, _dataGroups[dataType].dataTreeTitle, extent);
	_dataManager->updateAttributeList(name, attribute);
  }
  else
  {
    QMessageBox::warning((QWidget *)parent(), tr("Error"), tr("Data loading failed!"));
  }
}

void  EarthDataInterface::parseEarthNode()
{
    osgEarth::LayerVector  layers;
    _mainMap[0]->getLayers(layers);

    for (auto layer : layers)
    {
        QString parent;
        osgEarth::Layer* terrainLayer = NULL;

        if (!terrainLayer)
        {
            terrainLayer = dynamic_cast<osgEarth::ImageLayer *>(layer.get());

            if (terrainLayer)
            {
                parent = _dataGroups[IMAGE_LAYER].dataTreeTitle;
            }
        }

        if (!terrainLayer)
        {
            terrainLayer = dynamic_cast<osgEarth::ElevationLayer *>(layer.get());

            if (terrainLayer)
            {
                parent = _dataGroups[TERRAIN_LAYER].dataTreeTitle;
            }
        }

        if (terrainLayer)
        {
            std::string  originalName = layer->getName();
            QString      name = QString::fromStdString(originalName);
            emit         recordData(terrainLayer, name, parent);

            // The layer's name may have changed, set its copy's as well
            for (unsigned i = 1; i < MAX_SUBVIEW; i++)
            {
                if (_mainMap[i] != NULL)
                {
                    _mainMap[i]->getLayerByName(originalName)->setName(layer->getName());
                }
            }
        }

        return;
    }
}
