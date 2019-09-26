#ifndef SPLITER_H
#define SPLITER_H

#include "QDir"
#include <map>

#include "Common.h"
#include "BoundBox.h"

namespace DC
{
	namespace Pyra
	{
		typedef std::map<QString, QString> IndexFileMap;
		class FileInfoParse
		{
		public: 
			FileInfoParse(QString baseName, const LevelType& level)
				: m_level(level)
				, m_baseName(baseName)
			{
				Parse();
			}

			//解析生成信息
			void Parse()
			{
				m_totalChunk = std::pow(double(4), (m_level - 1));

				m_chunkColumn = std::pow(double(2), (m_level - 1));;
				m_chunkRow = m_chunkColumn;

				//生成文件名
				GenFileName();
			}

			//生成文件名
			void GenFileName()
			{
				//先给每一列赋值
				for (unsigned column = 0; column < m_chunkColumn; ++column)
				{
					for (unsigned row = 0; row < m_chunkColumn; ++row)
					{
						//行列号索引
						QString cIndex = GenIndexNameByIndex(column, row);
						QString fName = m_baseName + QString("_L%1_X%2_Y%3").arg(m_level-1).arg(column).arg(row);
						m_indexToFile[cIndex] = fName;
					}
				}
			}

			IndexFileMap GetFileMap()
			{
				return m_indexToFile;
			}
		public:
			//层级数
			LevelType m_level;

			//basename
			QString m_baseName;

			//分块个数
			unsigned m_totalChunk;

			//行列数
			unsigned m_chunkColumn;
			unsigned m_chunkRow;

			//行列号索引对应的文件名
			IndexFileMap m_indexToFile;
		};

		class SPliter
		{
		public:
			SPliter(const QString& suffix);
			~SPliter();

			//
			QString GetSuffix() {return m_suffix;}

			//读取文件并确定边界盒范围
			virtual	ErrorType GetBoundBoxByFile(const QString& inFile, BoundBox& outBox, unsigned simple = 1) = 0;

			//遍历分区并写出分块文件
			virtual ErrorType WriteSubfile(QString infile, const QDir& outDir, const LevelType& level) = 0;

			//根据最底层数据，合并并抽稀生成其它层
			virtual void WriteFileByCombine(QString infile, unsigned level, const QDir& outDir, unsigned simple = 3) = 0;

			//写出边界盒
			virtual void WriteBoxTofile(QString infile, unsigned level, const QDir& outDir) = 0;
		private:
			QString m_suffix;
		};
	}
}



#endif // SPLITER_H
