#ifndef _SCENEVIEWSTATE_H_
#define _SCENEVIEWSTATE_H_

#include <osg/Matrix>
#include <osg/Node>

// forward declaration
namespace DC
{
	class SceneView;
}


class SceneViewState
{
public:

    SceneViewState();
    SceneViewState(DC::SceneView *);
    SceneViewState(const SceneViewState &);
    virtual ~SceneViewState();

    const osg::Matrixd & getViewMatrix() const {return m_viewMatrix; }
    const osg::Vec3d & getCenter() const {return m_center; }
    float getDistance() const {return m_distance; }

    void setRoot(osg::Node*root){m_currRoot = root; }
    osg::Node*getRoot() const {return m_currRoot; }

private:

    osg::Matrixd m_viewMatrix;
    osg::Vec3d m_center;
    double m_distance;
    osg::Node* m_currRoot;
};

#endif // _SCENEVIEWSTATE_H_
