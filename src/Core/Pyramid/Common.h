#ifndef DC_PRYAMID_COMMOM_H
#define DC_PRYAMID_COMMOM_H
#include <osg/ref_ptr>
#include <osg/Vec3>
#include <OSG/Geode>
#include <OSG/Geometry>
//#include "Common/BasicTypes.h"
#include "QString"

namespace DC
{
	namespace Pyra
	{
		//一些公用的const变量定义
		const QString SUFFIX = "pdb";   //生成文件扩展名
		const QString BOXEXT = "box";   //box文件名组合名

		//库内数据类型
		typedef int LevelType;
		typedef osg::Vec3f Point3D;
		typedef Point3D Point_3D;
		//库内使用的报错信息
		enum ErrorType
		{
			eNoError			=	0,
			eError				=	1
		};

		//文件命名格式
		enum SubNameFormat
		{
			eOSGFormat				=	0,
			eDCFormat				=	1
		};

		//分割type
		enum SplitType
		{
			eAll					=	0,  //分割并写出所有层
			eSingle					=	1   //分割并写出指定层
		};

		//全局函数
		inline QString GenIndexNameByIndex(unsigned column, unsigned row)
		{
			return QString("%1_%2").arg(column).arg(row);//QString::number(column).append("_").append(QString::number(row));
		}
	}
}

#endif