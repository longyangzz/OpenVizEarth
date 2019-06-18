#include "SelectionManager.h"
#include "SelectionDecorator.h"

#include <osgDB/Registry>
#include <osgDB/FileUtils>

#include "OsgData.h"

SelectionManager::SelectionManager()
{
    // create a selection decorator for highlighting.
    m_pSelectionDecorator = new DefaultSelectionDecorator;
}

/*!
    Destructor
 */
SelectionManager::~SelectionManager()
{}

void SelectionManager::clearSelection()
{
    // deselect the previous selected node
    while (m_pSelectionDecorator->getNumChildren() > 0)
        m_pSelectionDecorator->removeChild(0,1);

    if (m_pSelectionDecorator->getNumParents() > 0)
        m_pSelectionDecorator->getParent(0)->removeChild( m_pSelectionDecorator.get() );
}

/*!
    Select a node in the scene.
    @param[in] pNode Node to be selected (or NULL to deselect).
    @return Return 'true' if the node is selected. Return 'false' when deselected.
 */
bool SelectionManager::select(osg::Node* pNode)
{
    // select the new node
    if (pNode)
    {
        m_pSelectionDecorator->addChild(pNode);

        osg::Group* pParent = pNode->getParent(0);

        if (pParent)
        {
            pParent->addChild( m_pSelectionDecorator.get() );
        }
    }

    // keep the pointer of the selected node
    m_pSelectedNode = pNode;

    return (pNode!=NULL);
}

/*!
    Select a geometry element in the scene.
    @param[in] pGeom geometry to be selected (or NULL to deselect).
    @return Return 'true' if the geometry is selected. Return 'false' when deselected.
 */
bool SelectionManager::select(osg::Geometry* pGeom)
{
    // select the new node
    if (pGeom)
    {
        // creation of a dummy geode
        //osg::Geode *newparent = dynamic_cast<osg::Geode*>(pGeom->getParent(0)->clone(osg::CopyOp::DEEP_COPY_STATESETS));
        osg::Geode *newparent = new osg::Geode;

        // duplicate current geometry
        osg::Geometry *newgeom = dynamic_cast<osg::Geometry*>( pGeom->clone(osg::CopyOp::DEEP_COPY_PRIMITIVES) );

        newparent->addDrawable(newgeom);

        m_pSelectionDecorator->addChild(newparent);

        osg::Node* pParent = pGeom->getParent(0);

        if (pParent)
            pParent->getParent(0)->addChild( m_pSelectionDecorator.get() );
    }

    // keep the pointer of the selected node
    m_pSelectedGeometry = pGeom;

    return (pGeom!=NULL);
}

/*!
    Return the current selected node.
    @return Current selected node.
 */
osg::Node* SelectionManager::getSelectedNode()
{
    return m_pSelectedNode.get();
}

/*!
    Return the current selected geometry.
    @return Current selected geometry.
 */
osg::Geometry* SelectionManager::getSelectedGeometry()
{
    return m_pSelectedGeometry.get();
}

/*!
    Return the current selected node.
    @return Current selected node.
 */
const osg::Node* SelectionManager::getSelectedNode() const
{
    return m_pSelectedNode.get();
}

/*!
    Return the current selected geometry.
    @return Current selected geometry.
 */
const osg::Geometry* SelectionManager::getSelectedGeometry() const
{
    return m_pSelectedGeometry.get();
}

/*!
    Replace the selection decorator for highlighting.
    @param[in] pDecorator New selection decorator.
 */
void SelectionManager::setSelectionDecorator(ISelectionDecorator* pDecorator)
{
    if (!pDecorator)
        return;

    m_pSelectionDecorator = pDecorator;
    select( m_pSelectedNode.get() );
}

/*!
    Return the selection decorator.
    @return Current selection decorator.
 */
ISelectionDecorator* SelectionManager::getSelectionDecorator()
{
    return m_pSelectionDecorator.get();
}

/*!
    Return the selection decorator.
    @return Current selection decorator.
 */
const ISelectionDecorator* SelectionManager::getSelectionDecorator() const
{
    return m_pSelectionDecorator.get();
}
