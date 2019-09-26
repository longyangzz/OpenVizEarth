//c++
#include <math.h>
#include <map>
#include <iostream>

//Qt
#include "QFile"
#include "QTextStream"
#include "QStringList"
#include "QFileInfo"
#include "QDateTime"
#include "QDir"

#include "GridSplitStrategy.h"
//#include "Quadtree.h"
#include "SplitFactory.h"

using namespace DC::Pyra;
 

GridSplitStrategy::GridSplitStrategy()
	: FileSplitStrategy()
	, m_extension(SUFFIX)
	, m_boxSample(1)
	, m_levelSample(2)
{

}

GridSplitStrategy::~GridSplitStrategy()
{

}

ErrorType GridSplitStrategy::Spit(const QString& inFile, const LevelType& level, const QDir& outDir, bool isAll/* = true*/)
{
	m_inFile = inFile;
	m_level = level;

	if (m_inFile.isEmpty() || m_level < 1)
	{
		return eError;
	}

	//输出路径不存在则报错
	if (!outDir.exists())
	{
		return eError;
	}

	//获取扩展名，根据扩展名，调用不同的分块器
	QFileInfo info(inFile);
	QString exten = info.suffix();

	//根据
	SplitFactory factory(exten);
	SPliter* spliter = factory.CreateSpliter();

	ErrorType hasBound = spliter->GetBoundBoxByFile(m_inFile, m_bound, m_boxSample);

	if (hasBound == eNoError)
	{
		//分块写出
		ErrorType writeSuccess = spliter->WriteSubfile(m_inFile, outDir, level);

		//合并并抽稀生成其它层级数据
		if (writeSuccess == eNoError)
		{
			if (isAll)
			{
				spliter->WriteFileByCombine(m_inFile, level, outDir, m_levelSample);
			}
		}

		//写出边界盒文件
		spliter->WriteBoxTofile(m_inFile, m_level, outDir);
		
		return writeSuccess;
	}
	return hasBound;
}


void GridSplitStrategy::SetBoxSample(unsigned sample)
{
	m_boxSample = sample;
}

void GridSplitStrategy::SetLevelSample(unsigned sample)
{
	m_levelSample = sample;
}