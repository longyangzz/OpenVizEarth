#ifndef GENERATE_PLD_STRUCT_H
#define GENERATE_PLD_STRUCT_H
#include <QString>
#include <QDir>
#include "BoundBox.h"

class GeneratePldStruct
{
public: 
	GeneratePldStruct(DC::Pyra::BoundBox mainBox, QString postFix);
	~GeneratePldStruct();

	void WriteOsgBoxToFile(QString infile, unsigned level, const QDir& outDir);
private:
	//根据索引值生成四个子块的索引值
	QStringList GenIndexsByIndex(int level, QString index);
	
	//由 层 行 列 边界 计算对应的边界
	DC::Pyra::BoundBox& CalcBBox(int level, unsigned col, unsigned row, DC::Pyra::BoundBox mainBox);

	osg::ref_ptr<osg::PagedLOD> CreatePagedLOD(DC::Pyra::BoundBox box, QString fileBaseName);
private:
	DC::Pyra::BoundBox m_mainBox;
	QString m_postFix;
};
#endif