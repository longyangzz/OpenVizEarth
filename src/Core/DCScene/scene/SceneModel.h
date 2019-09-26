#ifndef _SCENEMODEL_H_
#define _SCENEMODEL_H_

#ifdef min
#undef min
#endif

#include "dcscene_global.h"

#include <osg/MatrixTransform>
#include <osgFX/Scribe>

#include <QtCore/QObject>


#include <iostream>

class DCSCENE_EXPORT SceneModel :
    public QObject
{
    Q_OBJECT

public:

    SceneModel(QObject *parent = NULL);
    virtual ~SceneModel();

    void setData(osg::Node *data, bool resetHome = true);
    void resetModel();

    // debug
    bool saveSceneData(const std::string &);

    void setHighlightScene(bool val);

    // get the root node of the scene
    osg::Node *getScene(void);

    // get the terrain node if exist
    osg::Node *getObject(void);

    const osg::Vec3 & getSceneCenter() {return m_sceneCenter; }

signals:

    void loadBegin(bool);
    void loadFinished();

public slots:

private:

    // functions
    void createScene();

    osg::Group *createSceneLight();

    double getNextVal (double curr);

    void reoderUnderlays();

    // Type node (visible, ir3, ...)
    std::map<int, osg::Node *> m_mapTypeNodes;

    // Palette node (default, 0, 1, 2, ...)
    std::map<int, osg::Node *> m_mapPaletteNodes;
    std::map<int, bool> m_mapHidePaletteNodes;

    bool m_hideInstrumentNodes;
    bool m_hideObjectNodes;
    std::map<int, bool> m_mapHideTypeNodes;

    osg::Switch *m_switchRoot;

    osg::Group * m_NodeScene;

    osg::MatrixTransform* m_transformSpinScene;
    osg::MatrixTransform* m_transformSpinObject;

    osg::ref_ptr<osg::Node> m_currentData;

    osg::LightSource *lightSource_;

    bool m_highlightScene;

    osg::Vec3 m_baseLightPosition;

    osgFX::Scribe *m_currentHightlight;

    osg::Vec3 m_sceneCenter;
};

#endif // _SCENEMODEL_H_
