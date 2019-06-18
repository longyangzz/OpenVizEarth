
/******************************************************************************
   GraphX: a 3D file viewer
   Copyright(C) 2011-2012  xbee@xbee.net

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *******************************************************************************/
 
#ifndef _LIGHTPOINTNODEVISITOR_H_
#define _LIGHTPOINTNODEVISITOR_H_

#include <osg/NodeVisitor>
#include <osg/Node>

// light point visitor (in order to switch on/off)
class LightPointNodeVisitor :
    public osg::NodeVisitor
{
public:

    LightPointNodeVisitor(bool val) :
        m_on(val)
    {
        setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);

        // in order to visit all the nodeswith mask 0x0 !!
        setTraversalMask(0xffffffff);
        setNodeMaskOverride(0xffffffff);
    }

    virtual void apply(osg::Node &searchNode)
    {
        osgSim::LightPointNode *lpn = dynamic_cast<osgSim::LightPointNode *>(&searchNode);
        if (NULL != lpn)
        {
            if (m_on)
                searchNode.setNodeMask(0xffffffff);
            else
                searchNode.setNodeMask(0x0);
        }
        traverse(searchNode);
    }

private:

    bool m_on;
};

#endif // _LIGHTPOINTNODEVISITOR_H_
