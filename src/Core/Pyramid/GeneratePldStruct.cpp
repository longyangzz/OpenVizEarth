#include "GeneratePldStruct.h"

#include <OSGDB/WriteFile>
#include <OSG/Geode>
#include <OSG/Geometry>
#include "OSG/PagedLOD"

#include "SPliter.h"

GeneratePldStruct::GeneratePldStruct(DC::Pyra::BoundBox mainBox, QString postFix)
{
	m_mainBox =  mainBox;
	m_postFix = postFix;
}

GeneratePldStruct::~GeneratePldStruct()
{

}



void GeneratePldStruct::WriteOsgBoxToFile(QString infile, unsigned level, const QDir& outDir)
{
	DC::Pyra::BoundBox mainBox = m_mainBox;

	for (int i = 0; i < level; i++)
	{
		QFileInfo info(infile);
		QString abpath = info.absolutePath();
		QString basename = info.baseName();

		//下一层的文件信息
		DC::Pyra::FileInfoParse fParseDown(info.baseName(),i+1);
		DC::Pyra::IndexFileMap fMapdown = fParseDown.GetFileMap();
		for (auto it = fMapdown.begin(); it != fMapdown.end(); it++)
		{
			QStringList childrenFileList = GenIndexsByIndex(i, it->first);


			osg::ref_ptr<osg::Group> root = new osg::Group();
			unsigned colNum = childrenFileList[0].split("_").at(0).toUInt();
			unsigned rowNum = childrenFileList[0].split("_").at(1).toUInt();
			DC::Pyra::BoundBox box = CalcBBox(i, colNum, rowNum, mainBox);
			osg::ref_ptr<osg::PagedLOD> plod = CreatePagedLOD(box,
				QString("%1_L%2_X%3_Y%4" ).arg( basename).arg(i+1)
				.arg( colNum).arg( rowNum));
			root->addChild(plod);

			colNum = childrenFileList[1].split("_").at(0).toUInt();
			rowNum = childrenFileList[1].split("_").at(1).toUInt();
			box = CalcBBox(i, colNum, rowNum, box);
			osg::ref_ptr<osg::PagedLOD> plod1 = CreatePagedLOD(box,
				QString("%1_L%2_X%3_Y%4" ).arg( basename).arg(i+1)
				.arg( colNum).arg( rowNum));
			root->addChild(plod1);

			colNum = childrenFileList[2].split("_").at(0).toUInt();
			rowNum = childrenFileList[2].split("_").at(1).toUInt();
			box = CalcBBox(i, colNum, rowNum, box);
			osg::ref_ptr<osg::PagedLOD> plod2 = CreatePagedLOD(box,
				QString("%1_L%2_X%3_Y%4" ).arg( basename).arg(i+1)
				.arg( colNum).arg( rowNum));
			root->addChild(plod2);

			colNum = childrenFileList[3].split("_").at(0).toUInt();
			rowNum = childrenFileList[3].split("_").at(1).toUInt();
			box = CalcBBox(i, colNum, rowNum, box);
			osg::ref_ptr<osg::PagedLOD> plod3 = CreatePagedLOD(box,
				QString("%1_L%2_X%3_Y%4" ).arg( basename).arg(i+1)
				.arg( colNum).arg( rowNum));
			root->addChild(plod3);

			colNum = (it->first).split("_").at(0).toUInt();
			rowNum = (it->first).split("_").at(1).toUInt();
			QString outboxName = basename+ QString("_L%1_X%2_Y%3").arg(i).arg(colNum).arg(rowNum) + "_box" + ".osg";
			QString outboxFullname = outDir.filePath(outboxName);
			osgDB::writeNodeFile(*root, outboxFullname.toStdString().c_str());
		}
	}
}


QStringList GeneratePldStruct::GenIndexsByIndex(int level, QString index)
{
	QStringList list;

	//行列号
	unsigned column = index.split("_").at(0).toInt() * 2;
	unsigned row = index.split("_").at(1).toInt() * 2;

	list.push_back(QString("%1_%2").arg(column).arg(row));
	list.push_back(QString("%1_%2").arg(column+1).arg(row));
	list.push_back(QString("%1_%2").arg(column).arg(row+1));
	list.push_back(QString("%1_%2").arg(column+1).arg(row+1));

	return list;
}

DC::Pyra::BoundBox& GeneratePldStruct::CalcBBox(int level, unsigned col, unsigned row, DC::Pyra::BoundBox mainBox)
{
	DC::Pyra::BoundBox subBox;
	mainBox.GetSubBoxByIndexInLevel(level, col, row,subBox.m_minCorner, subBox.m_maxCorner );
	return subBox;
}


osg::ref_ptr<osg::PagedLOD> GeneratePldStruct::CreatePagedLOD(DC::Pyra::BoundBox box, QString fileBaseName)
{
	//从boxfile中取出box边界盒坐标值--指定pagelod参数
	osg::Vec3d minCorner(box.m_minCorner.x(), box.m_minCorner.y(), box.m_minCorner.z());
	osg::Vec3d maxCorner(box.m_maxCorner.x(), box.m_maxCorner.y(), box.m_maxCorner.z());

	//计算range
	double rangeValue = sqrt(0.25*((maxCorner-minCorner).length2())) * 7;

	//设置center
	osg::Vec3d center = (maxCorner+minCorner)*0.5;
	osg::ref_ptr<osg::PagedLOD> pageLod = new osg::PagedLOD;
	pageLod->setCenter(center);

	//默认设置mode
	pageLod->setRangeMode(osg::LOD::RangeMode(0));

	//设置rangelist
	osg::LOD::RangeList rangeList;
	{
		osg::LOD::MinMaxPair minmax0(rangeValue, 10000000000);
		rangeList.push_back(minmax0);

		osg::LOD::MinMaxPair minmax1(0, rangeValue);
		rangeList.push_back(minmax1);
	}
	pageLod->setRangeList(rangeList);
	QString dataFile = fileBaseName + m_postFix;
	pageLod->setFileName(0, dataFile.toStdString().c_str());
	QString boxFile = fileBaseName + "_box.osg";
	pageLod->setFileName(1, boxFile.toStdString().c_str());

	return pageLod.get();
}