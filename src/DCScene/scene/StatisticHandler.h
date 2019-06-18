
/******************************************************************************
   osGraphX: a 3D file viewer
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
 
#include <osgViewer/ViewerEventHandlers>

class StatisticHandler :
    public osgViewer::StatsHandler
{
public:

    StatisticHandler() :
        osgViewer::StatsHandler(){}

    virtual ~StatisticHandler() {}

    void setCamera(osgViewer::ViewerBase* viewer) {setUpHUDCamera(viewer); }
    void setScene(osgViewer::ViewerBase* viewer){setUpScene(viewer); }
    void setWindowSize(float width, float height) {
// compatibility linux33 (osg 2.6.1)
#ifndef LINUX33
        _statsWidth = 1.5f * width;
        _statsHeight = 1.5f * height;
        _camera->setProjectionMatrix( osg::Matrix::ortho2D(0.0,_statsWidth,0.0,_statsHeight) );
#endif
    }

protected:
};

/*EOF*/
