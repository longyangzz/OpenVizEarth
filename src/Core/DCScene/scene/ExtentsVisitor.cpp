#include "ExtentsVisitor.h"

// handle geode drawable extents to expand the box
//------------------------------------------------------------------------
// apply.
//------------------------------------------------------------------------

void ExtentsVisitor::apply(osg::Geode &node)
{
    osg::BoundingBox bb;

    // update bounding box
    for (size_t i = 0; i < node.getNumDrawables(); ++i)
        // expand overall bounding box
        bb.expandBy( node.getDrawable(i)->getBound() );

    osg::BoundingBox xbb;

    // transform corners by current matrix
    for (size_t i = 0; i < 8; ++i)
    {
        osg::Vec3 xv = bb.corner(i) * m_TransformMatrix;
        xbb.expandBy(xv);
    }

    // update overall bounding box size
    m_BoundingBox.expandBy(xbb);

    // continue traversing the graph
    traverse(node);
}

//------------------------------------------------------------------------

void ExtentsVisitor::apply(osg::Transform &node)      // handle geode drawable extents to expand the box
{
    osg::Matrix matrixRestore(m_TransformMatrix);
    node.computeLocalToWorldMatrix(m_TransformMatrix, this);

    // Continue traversing the graph.
    traverse(node);

    // Restore the previous accumulated transformation.
    m_TransformMatrix = matrixRestore;
}

const osg::BoundingBox &ExtentsVisitor::GetBound() const
{
    return m_BoundingBox;
}
