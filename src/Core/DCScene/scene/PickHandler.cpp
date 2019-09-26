#include "PickHandler.h"

//事件处理函数
bool CPickHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	switch(ea.getEventType())
	{
		//每一帧
	case(osgGA::GUIEventAdapter::FRAME):
		{
			osg::ref_ptr<osgViewer::View> viewer = dynamic_cast<osgViewer::View*>(&aa);
			//得到视图矩阵
			viewer ->getCamera() ->getViewMatrixAsLookAt(position, center, up) ;
			if (viewer)
			{	
				//执行PICK动作
				pick(viewer.get(),ea);
			}
			return false;
		}    
	default:
		return false;
	}

}
//PICK动作
void CPickHandler::pick(osg::ref_ptr<osgViewer::View> viewer, const osgGA::GUIEventAdapter& ea)
{
	//创建一个线段交集检测对象
	osgUtil::LineSegmentIntersector::Intersections intersections;

	std::string gdlist="";
	//申请一个流
	std::ostringstream os;
	//得到鼠标的位置
	float x = ea.getX();
	float y = ea.getY();
	//如果没有发生交集运算，及鼠标没有点中物体
	if (viewer->computeIntersections(x,y,intersections))
	{
		//得到相交交集的交点
		for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
			hitr != intersections.end();
			++hitr)
		{
			//输入流
			/*os<<"Mouse in World  X:"<< hitr->getWorldIntersectPoint().x()<<"     Y: "<<
				 hitr->getWorldIntersectPoint().y()<<"     Z: "<< hitr->getWorldIntersectPoint().z()<<std::endl ;*/
		}
	}
	//输入流
	//os<<"Viewer Position X: "<<position[0]<<"     Y: "<<position[1]<<"     Z: "<<position[2]<<std::endl ;

	gdlist += os.str();
	
	//设置显示内容
	setLabel(gdlist);
}
