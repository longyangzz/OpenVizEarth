#ifndef GRIDSPLITSTRATEGY_H
#define GRIDSPLITSTRATEGY_H

#include <map>
#include "QMap"
#include "QFile"

#include "pyramidDLL.h"
#include "FileSplitStrategy.h"

#include "BoundBox.h"

namespace DC
{
	namespace Pyra
	{
		class PYRAMID_API GridSplitStrategy : public FileSplitStrategy
		{
		public:
			GridSplitStrategy();
			~GridSplitStrategy();

			//文件分割接口函数
			ErrorType Spit(const QString& inFile, const LevelType& level, const QDir& outDir, bool isAll = true);

			//*****************************Spit相关算法begin*******************************************************
			void SetBoxSample(unsigned sample);
			void SetLevelSample(unsigned sample);
			
			//*****************************Spit相关算法end*******************************************************
		private:
			//文件内数据边界盒
			BoundBox m_bound;

			//分层级数
			LevelType m_level;

			//待分割的文件名
			QString m_inFile;

			//扩展名
			QString m_extension;

			//读取文件获取box时候采样率
			unsigned m_boxSample;

			//合并时候采样率
			unsigned m_levelSample;
		};
	}
}

#endif // GRIDSPLITSTRATEGY_H
