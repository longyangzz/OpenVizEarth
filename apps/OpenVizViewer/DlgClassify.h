#ifndef DLGCLASSIFY_H
#define DLGCLASSIFY_H

#include <QDialog>
#include "QStringList"
#include "ui_DlgClassify.h"
#include <osg/Vec3f>

typedef osg::Vec3f DCVector3D;

class DlgClassify;
class DlgClassify :public QDialog 
{
	Q_OBJECT
public:
	DlgClassify(QWidget *parent = 0);
	~DlgClassify();

	enum PlaneType
	{
		eDefault             = 0,
		eXYPlane			 = 1,
		eYZPlane		     = 2,
		eXZPlane             = 3
	};
	
	void SetInput(std::vector<DCVector3D> inPutPoint);

	typedef struct
	{
		int quhao;
		std::vector<DCVector3D>  m_px32;
	}fq32;
	std::vector<fq32> m_fq32;

	void GetOutput(std::vector<DCVector3D> & points);
	std::vector<DCVector3D> GetFilter();
private:
	Ui::DlgClassify ui;

	void CreateConnects();
	
	void UpDataFileInList();
	std::vector<DCVector3D> m_inPutPoint;
	std::vector<DCVector3D> m_LowerPoint;

	std::vector<DCVector3D> m_filterPoint;
	double XFBL;
	double YFBL;
	double ZFBL;
	double m_altitude;  //允许高差
	double m_heightScale;  //多少倍的高程值
	QStringList m_fileListIn;
private slots:
	void Ok();
	void Cancel();
	void SelectDirIn();
	void SelectDirOut();
	void SelcetPrefixName();


};

#endif // DLGCLASSIFY_H
