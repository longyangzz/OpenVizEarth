#ifndef _SELECTIONMANAGER_H
#define _SELECTIONMANAGER_H

#include <osg/Group>
#include <osg/StateSet>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/Geometry>

class ISelectionDecorator;

class SelectionManager
{

public:

    SelectionManager();             //!< Constructor
    virtual ~SelectionManager();    //!< Destructor

    // Select operations
    void clearSelection();
    bool select(osg::Node *pNode);                       //!< Select a node in the scene.
    bool select(osg::Geometry *pGeom);                   //!< Select a geometry in the scene.
    //bool select(float x, float y);                     //!< Select a point in screen coordinate.
    osg::Node* getSelectedNode();                        //!< Return the current selected node.
    osg::Geometry* getSelectedGeometry();                    //!< Return the current selected geometry.
    const osg::Node* getSelectedNode() const;            //!< Return the current selected node.
    const osg::Geometry* getSelectedGeometry() const;                    //!< Return the current selected geometry.

    // Replace and get the selection decorator
    void setSelectionDecorator(ISelectionDecorator* pDecorator);         //!< Replace the selection decorator for highlighting.
    ISelectionDecorator* getSelectionDecorator();                        //!< Return the selection decorator.
    const ISelectionDecorator* getSelectionDecorator() const;            //!< Return the selection decorator.

protected:

    osg::ref_ptr<osg::Node> m_pSelectedNode;    //!< Selected node
    osg::ref_ptr<osg::Geometry> m_pSelectedGeometry;    //!< Selected geometry

    osg::ref_ptr<ISelectionDecorator> m_pSelectionDecorator;    //!< Decorator node for highlighting
};

#endif // _SELECTIONMANAGER_H
