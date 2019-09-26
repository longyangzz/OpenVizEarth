#ifndef PYRAMID_H
#define PYRAMID_H

//Qt
#include "QString"
#include "QDir"

#include "Common.h"
#include "pyramidDLL.h"

namespace DC
{
	namespace Pyra
	{
		//存储一个策略对象
		class FileSplitStrategy;
		class PYRAMID_API GeneratorContext
		{
		public:
			GeneratorContext(const QString& inFile, const LevelType& level, const QDir& outDir, bool isAll = true);
			~GeneratorContext();
			
			//动态更改当前维护的策略
			void SetSplitStrategy(FileSplitStrategy* stratery);

			//生成金字塔结构
			ErrorType Generate();
		//成员变量
		private:
			//分层级数
			LevelType m_level;

			//待分割的文件名
			QString m_inFile;

			//输出路径
			QDir m_outDir;

			//分割类型
			bool m_allspit;

			//策略对象-接口类
			FileSplitStrategy* m_splitStrategy;
		};

	}
}

#endif // PYRAMID_H
