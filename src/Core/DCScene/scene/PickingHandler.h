#ifndef _PICKINGHANDLER_H_
#define _PICKINGHANDLER_H_

#include <osgGA/TrackballManipulator>
#include <osgGA/GUIActionAdapter>
#include <osgViewer/Viewer>

#include <QtCore/QObject>

class PickingHandler :
    public QObject, public osgGA::TrackballManipulator
{
    Q_OBJECT

public:

    enum
    {
        ZOOMIN = 0,
        ZOOMOUT
    };

    PickingHandler();

    virtual const char* className() const { return "PickingHandler"; }

    /** handle events, return true if handled, false otherwise.*/
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

    /** Get the keyboard and mouse usage of this manipulator.*/
    virtual void getUsage(osg::ApplicationUsage& usage) const;

	//! 计算并建立home坐标系统
	virtual void computeHomePosition(const osg::Camera *camera = NULL, bool useBoundingBox = false);

    void pick(const osgGA::GUIEventAdapter& ea);

    void setEnabledTrackballHelper(bool val) {m_trackballHelper = val; }
    void setEnabledInverseMouseWheel(bool val) {m_inverseMouseWheel = val; }

signals:

    void picked(osg::Drawable *);

    void picked(double,double,double);

    void zoomViewIn();
    void zoomViewOut();
    void dragView();
    void rotateView();
    void recenterViewTo(double,double,double);

protected:

    virtual ~PickingHandler();
    osg::Matrix matrixListtoSingle(osg::MatrixList tmplist);
    void zoom(int sens, int inverse, bool disableInfinateZoom = false);

    //! 视景器
    osgViewer::View* m_viewer;

    int m_dummy;
    bool m_recenter;
    bool m_picking;
    bool m_trackballHelper;
    bool m_inverseMouseWheel;
};

#endif // _PICKINGHANDLER_H_
