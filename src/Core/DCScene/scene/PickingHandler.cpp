#include "PickingHandler.h"

#include <osg/Notify>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>
#include <osgUtil/LineSegmentIntersector>

#include <osg/ComputeBoundsVisitor>

using namespace osg;
using namespace osgGA;

#include <iostream>

PickingHandler::PickingHandler() :
    TrackballManipulator(),
    m_viewer(NULL),
    m_dummy(0),
    m_recenter(false),
    m_picking(false),
    m_trackballHelper(false),
    m_inverseMouseWheel(false)
{}

PickingHandler::~PickingHandler()
{}

osg::Matrix PickingHandler::matrixListtoSingle(osg::MatrixList tmplist)
{
    osg::Matrix tmp;

    if (tmplist.size() > 0)
    {
        size_t i;
        for (i = 1, tmp = tmplist[0]; i < tmplist.size(); i++)
            tmp *= tmplist[0];
        tmp = tmplist[0];
    }
    return (tmp);
}

void PickingHandler::getUsage(osg::ApplicationUsage& usage) const
{
    usage.addKeyboardMouseBinding("PickingHandler: Space","Reset the viewing position to home");
    usage.addKeyboardMouseBinding("PickingHandler: Shift","Clic to center the view under the cursor (usefull to turn around a object)");
    TrackballManipulator::getUsage(usage);
}

bool PickingHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us)
{
    bool handled = false;
    m_viewer = dynamic_cast<osgViewer::View*>(&us);
    if (!m_viewer)
        return false;

    switch( ea.getEventType() )
    {
        case osgGA::GUIEventAdapter::PUSH:
        {
            if (ea.getButtonMask()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
                pick(ea);
            break;
        }

        case osgGA::GUIEventAdapter::DRAG:
        {
            if (ea.getButtonMask()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
            {
                pick(ea);
                if (m_trackballHelper)
                    emit rotateView();
            }
            else if (ea.getButtonMask()==osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON && m_trackballHelper)
                emit dragView();
            else if (ea.getButtonMask()==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON && m_trackballHelper)
                emit zoomViewIn();
            break;
        }

        case GUIEventAdapter::SCROLL:
        {
            switch ( ea.getScrollingMotion() )
            {
                case GUIEventAdapter::SCROLL_UP:

                    //handle scroll down;
                    zoom(ZOOMIN,m_inverseMouseWheel);
                    handled = true;
                    break;
                case GUIEventAdapter::SCROLL_DOWN:

                    //handle scroll up
                    zoom(ZOOMOUT,m_inverseMouseWheel);
                    handled = true;
                    break;
                default:

                    break;
            }
            break;
        }

        case GUIEventAdapter::KEYDOWN:
        {
            if (ea.getKey() == GUIEventAdapter::KEY_Shift_L || ea.getKey() == GUIEventAdapter::KEY_Shift_R)
            {
                m_recenter = true;
                handled = true;
            }
            else if (ea.getKey() == GUIEventAdapter::KEY_Control_L || ea.getKey() == GUIEventAdapter::KEY_Control_R)
            {
                m_picking = true;
                handled = true;
            }
            else if (ea.getKey()== GUIEventAdapter::KEY_Space)
            {
                home(ea,us);
                us.requestRedraw();
                return true;
            }

            break;
        }

        case (GUIEventAdapter::KEYUP):
        {
            m_recenter = false;
            m_picking = false;
			
            if (ea.getKey() == GUIEventAdapter::KEY_Shift_L || ea.getKey() == GUIEventAdapter::KEY_Shift_R)
            {
                m_recenter = false;
                handled = true;
            }
            else if (ea.getKey() == GUIEventAdapter::KEY_Control_L || ea.getKey() == GUIEventAdapter::KEY_Control_R)
            {
                m_picking = false;
                handled = true;
            }
            break;
        }
        default:
        {
        }
    }

    if (handled)
        return true;
    else
        return TrackballManipulator::handle(ea, us);
}

void PickingHandler::zoom(int sens, int inverse, bool disableInfinateZoom/* = false*/)
{
    if (inverse)
    {
        if (sens == ZOOMIN)
            sens = ZOOMOUT;
        else
            sens = ZOOMIN;
    }

	//根据disableInfinateZoom值不同设置缩放类型，该参数用来表明是否可以无穷放大与缩小
	if (disableInfinateZoom)
	{
		if (sens == ZOOMIN)
		{
			_distance *= 1.2;
			if (_distance > 1e+7)
				_distance = 1e+7;

			if (m_trackballHelper)
				emit zoomViewIn();
		}
		else
		{
			_distance *= 0.8;
			if (_distance < 0.05)
				_distance = 0.05;

			if (m_trackballHelper)
				emit zoomViewOut();
		}
	}
	else //模仿OrbitManipulator::zoomModel函数
	{
		if (sens == ZOOMIN)
    {
		float dy = 0.1;
		float scale = 1 + dy;

		// minimum distance
		float minDist = _minimumDistance;
		if( getRelativeFlag( _minimumDistanceFlagIndex ) )
			minDist *= _modelSize;

		if( _distance*scale > minDist )
		{
			// regular zoom
			_distance *= scale;
		}
		else
		{
			if( true )
			{
				// push the camera forward
				float scale = -_distance;
				Matrixd rotation_matrix( _rotation );
				Vec3d dv = (Vec3d( 0.0f, 0.0f, -1.0f ) * rotation_matrix) * (dy * scale);
				_center += dv;
			}
			else
			{
				// set distance on its minimum value
				_distance = minDist;
			}
		}

        if (m_trackballHelper)
            emit zoomViewIn();
    }
    else
    {
		float dy = -0.1;
		float scale = 1 + dy;

		// minimum distance
		float minDist = _minimumDistance;
		if( getRelativeFlag( _minimumDistanceFlagIndex ) )
			minDist *= _modelSize;

		if( _distance*scale > minDist )
		{
			// regular zoom
			_distance *= scale;
		}
		else
		{
			if( true )
			{
				// push the camera forward
				float scale = -_distance;
				Matrixd rotation_matrix( _rotation );
				Vec3d dv = (Vec3d( 0.0f, 0.0f, -1.0f ) * rotation_matrix) * (dy * scale);
				_center += dv;
			}
			else
			{
				// set distance on its minimum value
				_distance = minDist;
			}
		}

        if (m_trackballHelper)
            emit zoomViewOut();
    }
	}
}

void PickingHandler::pick(const osgGA::GUIEventAdapter& ea)
{
    if (!m_recenter && !m_picking)
        return;

    osg::Node* scene = m_viewer->getSceneData();
    if (!scene)
        return;

    osg::Node* node = 0;
    osg::Group* parent = 0;

    bool usePolytopePicking = false;
    if (usePolytopePicking)
    {
        double mx = ea.getXnormalized();
        double my = ea.getYnormalized();
        double w = 0.05;
        double h = 0.05;
        osgUtil::PolytopeIntersector* picker = new osgUtil::PolytopeIntersector( osgUtil::Intersector::PROJECTION, mx - w, my - h, mx + w, my + h );

        osgUtil::IntersectionVisitor iv(picker);

        m_viewer->getCamera()->accept(iv);

        if ( picker->containsIntersections() )
        {
            osgUtil::PolytopeIntersector::Intersection intersection = picker->getFirstIntersection();

            osg::NodePath& nodePath = intersection.nodePath;
            node = (nodePath.size()>=1) ? nodePath[nodePath.size() - 1] : 0;
            parent = (nodePath.size()>=2) ? dynamic_cast<osg::Group*>(nodePath[nodePath.size() - 2]) : 0;
        }
    }
    else
    {
        // use window coordinates
        // remap the mouse x,y into viewport coordinates.
        osg::Viewport* viewport = m_viewer->getCamera()->getViewport();
        float mx = viewport->x() + (int)( (float)viewport->width() * (ea.getXnormalized() * 0.5f + 0.5f) );
        float my = viewport->y() + (int)( (float)viewport->height() * (ea.getYnormalized() * 0.5f + 0.5f) );
        osgUtil::LineSegmentIntersector* picker = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::WINDOW, mx, my );

        osgUtil::IntersectionVisitor iv(picker);

        m_viewer->getCamera()->accept(iv);

        if ( picker->containsIntersections() )
        {
            osgUtil::LineSegmentIntersector::Intersection intersection = picker->getFirstIntersection();

            if (m_recenter)
            {
                osg::NodePath& nodePath = intersection.nodePath;
                node = (nodePath.size()>=1) ? nodePath[nodePath.size() - 1] : 0;

                // world matrix transform
                osg::Matrix mat = matrixListtoSingle( node->getWorldMatrices() );

                _center = intersection.localIntersectionPoint * mat;

                return;
            }

            osg::NodePath& nodePath = intersection.nodePath;
            node = (nodePath.size()>=1) ? nodePath[nodePath.size() - 1] : 0;

            emit picked( intersection.drawable.get() );
            parent = (nodePath.size()>=2) ? dynamic_cast<osg::Group*>(nodePath[nodePath.size() - 2]) : 0;
        }
    }
}

/** Compute the home position.
 *
 *  The computation considers camera's fov (field of view) and model size and
 *  positions camera far enough to fit the model to the screen.
 *
 *  camera parameter enables computations of camera's fov. If camera is NULL,
 *  scene to camera distance can not be computed and default value is used,
 *  based on model size only.
 *
 *  useBoundingBox parameter enables to use bounding box instead of bounding sphere
 *  for scene bounds. Bounding box provide more precise scene center that may be
 *  important for many applications.*/
void PickingHandler::computeHomePosition(const osg::Camera *camera, bool useBoundingBox)
{
    if (getNode())
    {
        osg::BoundingSphere boundingSphere;

        OSG_INFO<<" CameraManipulator::computeHomePosition("<<camera<<", "<<useBoundingBox<<")"<<std::endl;

        if (useBoundingBox)
        {
            // compute bounding box
            // (bounding box computes model center more precisely than bounding sphere)
            osg::ComputeBoundsVisitor cbVisitor;
            getNode()->accept(cbVisitor);
            osg::BoundingBox &bb = cbVisitor.getBoundingBox();

            if (bb.valid()) boundingSphere.expandBy(bb);
            else boundingSphere = getNode()->getBound();
        }
        else
        {
            // compute bounding sphere
            boundingSphere = getNode()->getBound();
        }

        // set dist to default
        double dist = 3.5f * boundingSphere.radius();

        if (camera)
        {

            // try to compute dist from frustrum
            double left,right,bottom,top,zNear,zFar;
            if (camera->getProjectionMatrixAsFrustum(left,right,bottom,top,zNear,zFar))
            {
                double vertical2 = fabs(right - left) / zNear / 2.;
                double horizontal2 = fabs(top - bottom) / zNear / 2.;
                double dim = horizontal2 < vertical2 ? horizontal2 : vertical2;
                double viewAngle = atan2(dim,1.);
                dist = boundingSphere.radius() / sin(viewAngle);
            }
            else
            {
                // try to compute dist from ortho
                if (camera->getProjectionMatrixAsOrtho(left,right,bottom,top,zNear,zFar))
                {
                    dist = fabs(zFar - zNear) / 2.;
                }
            }
        }

        // set home position
        setHomePosition(boundingSphere.center() + osg::Vec3d(0.0,0.0f,dist),
                        boundingSphere.center(),
                        osg::Vec3d(0.0f,1.0f,1.0f),
                        _autoComputeHomePosition);
    }
}
