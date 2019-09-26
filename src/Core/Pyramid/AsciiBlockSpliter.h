#ifndef ASCIIBLOCKSPLITER_H
#define ASCIIBLOCKSPLITER_H

#include "QMap"
#include "QStringList"
#include "SPliter.h"
#include "AsciiSpliter.h"

namespace DC
{
	namespace Pyra
	{
		class AsciiBlockSpliter : public SPliter
		{
		public:
			AsciiBlockSpliter(const QString& suffix);
			~AsciiBlockSpliter();

			//读取文件并确定边界盒范围
			virtual	ErrorType GetBoundBoxByFile(const QString& inFile, BoundBox& outBox, unsigned simple = 1);

			//遍历分区并写出分块文件
			virtual ErrorType WriteSubfile(QString infile, const QDir& outDir, const LevelType& level);

			//根据最底层数据，合并并抽稀生成其它层
			virtual void WriteFileByCombine(QString infile, unsigned level, const QDir& outDir, unsigned simple = 3);

			//写出边界盒
			virtual void WriteBoxTofile(QString infile, unsigned level, const QDir& outDir);
		private:
			//根据索引值生成四个子块的索引值
			QStringList GenIndexsByIndex(int level, QString index);

			//! 传入的name为全路径名称
			BoundBox GenSubBoxByFileName(const QString& name);

			void UpdataFileMap(QString infile, const QDir& outDir, const LevelType& level);
			
			//由 层 行 列 边界 计算对应的边界
			BoundBox CalcBBox(int level, unsigned col, unsigned row, BoundBox mainBox);

		private:
			BoundBox m_boundBox;

			//Qfile文件解析容器
			std::map<QString, FileTrunkInfo* > m_fileMaps;
			IndexFileMap m_fMap;
		};

	}
}

#endif // ASCIISPLITER_H
