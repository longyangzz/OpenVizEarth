#include "FindNameListVisitor.h"

FindNameListVisitor::FindNameListVisitor() :
    osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
    // in order to visit all the nodeswith mask 0x0 !!
    setTraversalMask(0xffffffff);
    setNodeMaskOverride(0xffffffff);
}

void FindNameListVisitor::apply(osg::Node &searchNode)
{
    if ( !searchNode.getName().empty() )
        m_nameList << searchNode.getName().c_str();

    traverse(searchNode);
}
