#pragma execution_character_set("utf-8")
#include "AddObliqueModel.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QDirIterator>
#include <QDomDocument>

#include <osg/PositionAttitudeTransform>
#include <osgEarth/SpatialReference>
#include <osgEarth/GeoData>
#include <osgEarthDrivers/model_simple/SimpleModelOptions>
#include <osgEarth/ModelLayer>
#include <osgEarthAnnotation/ModelNode>

#include <osg/BlendFunc>
#include <osg/Point>
#include <osg/PointSprite>
#include <osg/Texture2D>

#include "DC/LogHandler.h"

#include "LoadThread.h"

static const double zOffset = 0.1;

static inline void  getFolderFile(QString &path, QFileInfoList &file_to_use)
{
  QDirIterator  firstLevelDir(path, QDir::Dirs);

	while (firstLevelDir.hasNext())
	{
		firstLevelDir.next();

		if (firstLevelDir.filePath().endsWith('.'))
    {
      continue;
    }

    QDirIterator  secondLevelDir(firstLevelDir.filePath(), QDir::Dirs);

		while (secondLevelDir.hasNext())
		{
			secondLevelDir.next();

			if (secondLevelDir.filePath().endsWith('.'))
      {
        continue;
      }

			file_to_use.push_back(QFileInfo(secondLevelDir.filePath() + "/" + secondLevelDir.fileName() + ".osgb"));
		}
	}
}

AddObliqueModel::AddObliqueModel()
{
  _pluginCategory = tr("添加数据");
  _pluginName     = tr("Oblique Imagery Model");
}

AddObliqueModel::~AddObliqueModel()
{
}

void  AddObliqueModel::setupUi(QToolBar *toolBar, QMenu *menu)
{
	// Oblique Photography
  QAction *openOpAction = new QAction(_mainWindow);

	openOpAction->setObjectName(QStringLiteral("openOpAction"));
  QIcon  icon2;
	icon2.addFile(QStringLiteral("resources/icons/oblique_model.png"), QSize(), QIcon::Normal, QIcon::Off);
	openOpAction->setIcon(icon2);
	openOpAction->setText(tr("倾斜摄影"));
	openOpAction->setToolTip(tr("添加倾斜摄影模型"));

	menu->addAction(openOpAction);
	toolBar->addAction(openOpAction);

	connect(openOpAction, SIGNAL(triggered()), this, SLOT(addObliqueModel()));
}

void AddObliqueModel::onLoadingDone(const QString& nodeName, osg::Node *model, const osgEarth::GeoPoint &geoOrigin)
{
  //osgEarth::Drivers::SimpleModelOptions opt;
  //opt.node() = model;
  //opt.paged() = true;
  //opt.location() = geoOrigin.vec3d() + osg::Vec3d(0, 0, zOffset);

  //osg::ref_ptr<osgEarth::ModelLayer> layer = new osgEarth::ModelLayer(opt);
  //addLayerToMap(nodeName, layer, MODEL_LAYER, _pluginName);

  osgEarth::Viewpoint vp;
  //vp.setNode(layer->getNode());
  vp.setNode(model);
  emit setViewPoint(vp);
  emit loadingDone();

  auto anchorPoint = getNearestAnchorPoint(geoOrigin.vec3d());
  anchorPoint->addChild(model);

  recordNode(model, nodeName);
}

void  AddObliqueModel::loadObliqueModelfromXML(const QString& pathXML)
{
  // Check data validity
  QFile  file(pathXML);

	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::information(NULL, tr("Error"), tr("Fail to open!"));

		return;
	}

  QDomDocument  doc;

	if (!doc.setContent(&file))
	{
		QMessageBox::information(NULL, tr("Error"), tr("Fail to open!"));
		file.close();

		return;
	}

	if (doc.isNull())
	{
		QMessageBox::information(NULL, tr("Error"), tr("Invalid XML file!"));
		file.close();

		return;
	}

  QFileInfo  fi      = QFileInfo(pathXML);
  QString    XMLPath = fi.absolutePath();

  // Get SRS info
  QString       srsInfo;
  QStringList   _srsOriginInfo;
  QDomElement   xmlroot = doc.documentElement();
  QDomNodeList  list    = xmlroot.childNodes();
  QDomElement   element;

	for (int i = 0; i < list.count(); i++)
	{
		QCoreApplication::processEvents();

		element = list.item(i).toElement();

		if (element.tagName() == "SRS")
    {
      srsInfo = element.text();
    }

    if (element.tagName() == "SRSOrigin")
    {
      _srsOriginInfo = element.text().split(',');
    }
  }

	if (srsInfo.isEmpty() || _srsOriginInfo.isEmpty())
	{
		QMessageBox::information(NULL, tr("Error"), tr("Invalid XML file!"));
		file.close();

		return;
	}

  // Get model origin
  auto  srs = osgEarth::SpatialReference::get(srsInfo.toStdString());

  osg::ref_ptr<osg::PositionAttitudeTransform>  model = new osg::PositionAttitudeTransform;
	model->setUserData(srs);

  QString  nodeName = XMLPath.split("/").back();
  model->setUserValue("filepath", pathXML.toLocal8Bit().toStdString());

  osg::Vec3d origin(_srsOriginInfo[0].toDouble(), _srsOriginInfo[1].toDouble(), _srsOriginInfo[2].toDouble() + zOffset);
  osgEarth::GeoPoint  geoOrigin = osgEarth::GeoPoint(srs, origin);
	geoOrigin = geoOrigin.transform(_globalSRS);

  auto anchorPoint = getNearestAnchorPoint(geoOrigin.vec3d());
  model->setPosition(geoOrigin.vec3d() - anchorPoint->getPosition());
  model->setUserValue("zOffset", zOffset);

  // Begin loading
  QFileInfoList  allFileList;
	getFolderFile(XMLPath, allFileList);

  emit        loadingProgress(10);
  LoadThread *loader = new LoadThread(_loadingLock, model, allFileList);
	connect(loader, &LoadThread::finished, loader, &QObject::deleteLater);
	connect(loader, &LoadThread::progress, this, &AddObliqueModel::loadingProgress);
  connect(loader, &LoadThread::done, [this, model, nodeName, geoOrigin]() {
    onLoadingDone(nodeName, model, geoOrigin);
  });

	loader->start();
}

void  AddObliqueModel::addObliqueModel()
{
  QString  fileName = QFileDialog::getOpenFileName(nullptr, tr("Open File"), " ", tr("Model file(*.s3c *.osgb *.xml);;Allfile(*.*)"));

  //for (auto path : XMLFileNames)
  {
	  
  }
  if ((fileName.section(".", 1, 1) == "s3c") || (fileName.section(".", 1, 1) == "osgb"))
  {
	  loadObliqueModel(fileName);
  }
  else if ((fileName.section(".", 1, 1) == "xml"))
  {
	  loadObliqueModelfromXML(fileName);
  }

  LogHandler::getInstance()->reportInfo(tr("Loading of %1 ...").arg(fileName));
}

void AddObliqueModel::loadObliqueModel(const QString& fileName)
{
	//加载s3c模型
	
	{
		osg::ref_ptr<osg::PositionAttitudeTransform>  pcModel = new osg::PositionAttitudeTransform;
		_currentAnchor->addChild(pcModel);

		osg::ref_ptr<osg::Node>  pointCloud = osgDB::readNodeFile(fileName.toLocal8Bit().toStdString());

		if (pointCloud.valid())
		{
			pcModel->addChild(pointCloud);

			/*osg::StateSet *set = new osg::StateSet();

			set->setMode(GL_BLEND, osg::StateAttribute::ON);
			osg::BlendFunc *fn = new osg::BlendFunc();
			fn->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::DST_ALPHA);
			set->setAttributeAndModes(fn, osg::StateAttribute::ON);

			set->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);

			set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
			set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

			pcModel->setStateSet(set);

			recordNode(pcModel, fileName.split("/").back().section(".", 0, 0));

			pcModel->setUserValue("filepath", fileName.toLocal8Bit().toStdString());*/

		}
	}

}
