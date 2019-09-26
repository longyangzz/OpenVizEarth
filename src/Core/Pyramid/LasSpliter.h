#ifndef LASSPLITER_H
#define LASSPLITER_H

#include "QMap"
#include "QStringList"
#include "SPliter.h"

namespace DC
{
	namespace Pyra
	{
		class LasSpliter : public SPliter
		{
		public:
			LasSpliter(const QString& suffix);
			~LasSpliter();

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

			ErrorType WriteFileByBoundBox(const QString& infile, const  QString& outFilename, BoundBox box);
		private:
			BoundBox m_boundBox;

			//Qfile文件解析容器
			QMap<QString, QFile* > m_fileMaps;
		};

	}
}

#endif // LASSPLITER_H
