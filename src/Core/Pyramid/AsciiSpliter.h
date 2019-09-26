#ifndef ASCIISPLITER_H
#define ASCIISPLITER_H

#include "QMap"
#include "QStringList"
#include "SPliter.h"

namespace DC
{
	namespace Pyra
	{
		struct FileTrunkInfo
		{
			FileTrunkInfo()
				: m_box(nullptr)
				, m_qdatafile(nullptr)
			{

			}

			FileTrunkInfo(QFile* qdatafile, QFile* qBoxfile, BoundBox* box)
				: m_box(nullptr)
				, m_qdatafile(nullptr)
			{
				m_box = box;
				m_qdatafile = qdatafile;
			}

			BoundBox* m_box;
			QFile* m_qdatafile;
		};

		class AsciiSpliter : public SPliter
		{
		public:
			AsciiSpliter(const QString& suffix);
			~AsciiSpliter();

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

			//! 写文件头
			void WriteHeader(QString& basename, QString& fullName);
		
		private:
			BoundBox m_boundBox;

			//Qfile文件解析容器
			std::map<QString, FileTrunkInfo* > m_fileMaps;
		};

	}
}

#endif // ASCIISPLITER_H
