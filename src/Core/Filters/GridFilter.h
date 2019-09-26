#ifndef GRID_FILTER
#define GRID_FILTER

#include <vector>
#include <QStringList>

#include <osg/Vec3f>
#include "FiltersDLL.h"

typedef osg::Vec3f DCVector3D;
typedef struct
{
	int quhao;
	std::vector<DCVector3D>  m_px32;
}fq32;
std::vector<fq32> m_fq32;

class FILTERS_API GridFilter
{
public:
	GridFilter(std::vector<DCVector3D> inPutPoint, QString fileOut);
	GridFilter(QString fileIn, QString fileOut);
	~GridFilter();

	void SetFBL(double xFBL, double yFBL);

	void GetOutput(std::vector<DCVector3D> & points);

	void DoFilter();
private:
	void Fastfast(std::vector<DCVector3D> & points, unsigned num);
	bool ReadTXTAscii(QString fileIn);
	bool ReadLasFile(QString fileIn);
	bool WriteDatAsccii(QString fileOut);

	std::vector<DCVector3D> m_inPutPoint;
	std::vector<DCVector3D> m_LowerPoint;
	double XFBL;
	double YFBL;
	double ZFBL;
	double m_altitude;  //允许高差
	double m_heightScale;  //多少倍的高程值
	QStringList m_scaleList;
	QString m_fileIn; //读取的文件
	QString m_fileOut; //写出的文件
	//PlaneType m_planeType;
	
};
#endif