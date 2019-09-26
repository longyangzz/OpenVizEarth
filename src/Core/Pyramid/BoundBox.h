#ifndef DC_PYRA_BOUNDBOX_H
#define DC_PYRA_BOUNDBOX_H

#include "pyramidDLL.h"
//common
//#include "Common/BasicTypes.h"
//#include "Common/Const.h"

#include "Common.h"

namespace DC
{
	namespace Pyra
	{
		typedef osg::Vec3 Point3D;
		class PYRAMID_API BoundBox
		{
		public:
			
			Point3D m_minCorner;
			Point3D m_maxCorner;
			double m_length;  //长
			double m_width;   //宽
			double m_zBuffer; //z值

			double m_sphereRadius;

			BoundBox()
			{
				m_minCorner.x() = 1e10;
				m_minCorner.y() = 1e10;
				m_minCorner.z() = 1e10;

				m_minCorner.x() = 1e-10;
				m_minCorner.y() = 1e-10;
				m_minCorner.z() = 1e-10;
				m_length = m_width = m_zBuffer = 0.0;
			}

			BoundBox(const Point3D& minCorner, const Point3D& maxCorner)
				: m_minCorner(minCorner)
				, m_maxCorner(maxCorner)
			{
				m_length = m_maxCorner.x()- m_minCorner.x();
				m_width = m_maxCorner.y() - m_minCorner.y();
				m_zBuffer = m_maxCorner.z() - m_minCorner.z();
	
				m_sphereRadius = sqrt(0.25 *  (maxCorner- minCorner).length2() );
			}

			Point3D GetCenter() const
			{
				//忽略z值的处理
				Point3D pp(m_maxCorner);
				pp.x() += m_minCorner.x();
				pp.y() += m_minCorner.y();
				pp.z() += m_minCorner.z();
				pp.x() *= 0.5;
				pp.y() *= 0.5;
				return pp;
			}

			void GetPointIndexInLevel(const Point3D& point, const int& level, unsigned& column, unsigned& row)
			{
				int colCellNum =  std::pow(double(2),(level-1));
				double xInter = m_length / colCellNum;
				double yInter = m_width / colCellNum;
		
				Point3D tp(point);
				tp.x() -= m_minCorner.x();
				tp.y() -= m_minCorner.y();
				tp.z() -= m_minCorner.z();
				column =  std::floor(tp.x() / xInter) == colCellNum ? std::floor(tp.x() / xInter) - 1 : std::floor(tp.x() / xInter);

				row = std::floor(tp.y() / yInter)  == colCellNum ? std::floor(tp.y()/ yInter) - 1 : std::floor(tp.y() / yInter);

			}

			void GetSubBoxByIndexInLevel(const int& level, unsigned& column, unsigned& row,
				Point3D& subMinCorner, Point3D& subMaxCorner )
			{
				int colCellNum =  std::pow(double(2),(level-1));
				double xInter = m_length / colCellNum;
				double yInter = m_width / colCellNum;

				
				subMinCorner.x() = xInter * column + m_minCorner.x();
				subMinCorner.y() = yInter * row + m_minCorner.y();
				subMinCorner.z() = m_minCorner.z();

				subMaxCorner.x() = xInter + subMinCorner.x();
				subMaxCorner.y() = yInter + subMinCorner.y();
				subMaxCorner.z() = m_maxCorner.z();
			}
		};
	}
}

#endif