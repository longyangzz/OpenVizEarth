#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/BoundingBox>
#include <osg/Matrix>
#include <osg/MatrixTransform>

//========================================================================
//  ExtentsVisitor.
/// Visit all nodes and compute bounding box extents.
//========================================================================

class ExtentsVisitor :
    public osg::NodeVisitor
{
public:

    ExtentsVisitor() :
        NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
    virtual ~ExtentsVisitor() {}

    virtual void apply(osg::Geode &node);
    virtual void apply(osg::Transform &node);  // override apply from NodeVisitor

    ///
    /// Return the bounding box of the nodes.
    ///
    const osg::BoundingBox &GetBound() const;

protected:

    osg::BoundingBox m_BoundingBox; // bound box
    osg::Matrix m_TransformMatrix;  // current transform matrix
};
