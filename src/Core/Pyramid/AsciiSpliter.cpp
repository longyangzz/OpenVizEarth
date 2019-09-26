#include <iostream>
#include "QDateTime"

#include "QTextStream"

#include "AsciiSpliter.h"

using namespace DC::Pyra;

AsciiSpliter::AsciiSpliter(const QString& suffix)
	: SPliter(suffix)
{

}

AsciiSpliter::~AsciiSpliter()
{

}

ErrorType AsciiSpliter::GetBoundBoxByFile(const QString& inFile, BoundBox& outBox, unsigned simple/* = 1*/)
{
	BoundBox box;

	Point3D currentPoint;
	//读取文件,每度一个点存储到currentPoint，与最大最小值比较，确定最值
	char currentLine[500];

	QFile file(inFile);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Truncate))
	{
		return eError;
	}

	while(file.readLine(currentLine, 500) > 0)
	{

		//分隔行并给currentPoint赋值
		QStringList list = QString(currentLine).split(QRegExp(",|\\s+"),QString::SkipEmptyParts);

		if (list.size() < 3)
		{
			return eError;
		}

		//currentPoint = Point3D(list[0].toDouble(), list[1].toDouble(), list[2].toDouble());
		currentPoint.x() = list[0].toDouble();
		currentPoint.y() = list[1].toDouble();
		currentPoint.z() = list[2].toDouble();

		//更新box值
		box.m_minCorner[0] = std::min(box.m_minCorner[0], currentPoint[0]);
		box.m_minCorner[1] = std::min(box.m_minCorner[1], currentPoint[1]);
		box.m_minCorner[2] = std::min(box.m_minCorner[2], currentPoint[2]);

		box.m_maxCorner[0] = std::max(box.m_maxCorner[0], currentPoint[0]);
		box.m_maxCorner[1] = std::max(box.m_maxCorner[1], currentPoint[1]);
		box.m_maxCorner[2] = std::max(box.m_maxCorner[2], currentPoint[2]);

		//应用采样率
		for (unsigned i = 0; i < simple - 1; ++i)
		{
			file.readLine(currentLine, 500);
		}
	}

	file.close();

	box.m_length = box.m_maxCorner.x() - box.m_minCorner.x();
	box.m_width = box.m_maxCorner.y() - box.m_minCorner.y();
	outBox = box;
	m_boundBox = box;
	std::cout << "finish Box" << std::endl;
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
	std::cout << current_date.toStdString() << std::endl;
	return eNoError;
}

ErrorType AsciiSpliter::WriteSubfile(QString infile, const QDir& outDir, const LevelType& level)
{
	//根据box，level,在outfile内输出文件
	//1.根据level确定行列号
	QFileInfo info(infile);
	FileInfoParse fParse(info.baseName(),level);
	IndexFileMap fMap = fParse.GetFileMap();


	//读取文件中的每个点，根据点坐标计算点所属的分区号，进而打开对应的文件写入点信息
	/*int colCellNum =  std::pow(double(2),(level-1));
	double xInter = m_bound.m_length / colCellNum;
	double yInter = m_bound.m_width / colCellNum;*/

	char currentLine[500];
	Point3D currentPoint;
	QFile file(infile);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Truncate))
	{
		return eError;
	}

	QTextStream inStream(&file);
	
	try
	{
		while(!inStream.atEnd())
		{
			//分隔行并给currentPoint赋值
			QString line = inStream.readLine();
			QStringList list = line.split(QRegExp(",|\\s+"),QString::SkipEmptyParts);

			if (list.size() < 3)
			{
				return eError;
			}

		//	currentPoint = Point3D(list[0].toDouble(), list[1].toDouble(), list[2].toDouble());
			currentPoint.x() = list[0].toDouble();
			currentPoint.y() = list[1].toDouble();
			currentPoint.z() = list[2].toDouble();
			//根据currentPoint计算分区号并写出

			unsigned columnIndex = 0, rowIndex = 0;
			m_boundBox.GetPointIndexInLevel(currentPoint, level, columnIndex, rowIndex);
			QString indexName = GenIndexNameByIndex(columnIndex, rowIndex);

			//3.创建输出文件流
			//QString currentOutFilename = outFile + "\\" + fMap[indexName] + m_extension;
			QString curFilename = QString("%1.%2").arg(fMap[indexName]).arg(GetSuffix());
			QString currentOutFilename = outDir.filePath(curFilename);

			//boxfile
			QString curboxFilename = QString("%1_box.%2").arg(fMap[indexName]).arg(GetSuffix());
			QString currentBoxOutFilename = outDir.filePath(curboxFilename);

			//不存在当前名字对应的Qfiletrunk对象，则创建
			if (m_fileMaps.find(currentOutFilename) == m_fileMaps.end())
			{
				QFile* odatafile = new QFile(currentOutFilename);
				//QFile* oboxfile = new QFile(currentBoxOutFilename);
				BoundBox* outBox = new BoundBox;
				m_fileMaps[currentOutFilename] = new FileTrunkInfo(odatafile, nullptr, outBox);
			}

			//此时一定存在对应名字所映射的Qfiletrunk对象，但是尚不清楚数据文件是否已经打开
			if (!m_fileMaps[currentOutFilename]->m_qdatafile->isOpen())
			{
				m_fileMaps[currentOutFilename]->m_qdatafile->open(QIODevice::WriteOnly | QIODevice::Text);

				//是否根据文件名字写出文件头
				{
					//WriteHeader(curFilename, currentOutFilename);
				}
			}

			//定义一个当前行变量
			QString writeline;

			//写入当前坐标
			writeline.append(QString("%1").arg(currentPoint.x(), 0, 'f', 3));
			writeline.append(","); //添加分隔符
			writeline.append(QString("%1").arg(currentPoint.y(), 0, 'f', 3));
			writeline.append(","); //添加分隔符
			writeline.append(QString("%1").arg(currentPoint.z(), 0, 'f', 3));

			m_fileMaps[currentOutFilename]->m_qdatafile->write(writeline.toStdString().c_str());
			m_fileMaps[currentOutFilename]->m_qdatafile->write("\n");

			//重新计算最大值 不必在些计算
			m_fileMaps[currentOutFilename]->m_box->m_minCorner.x() = std::min(m_fileMaps[currentOutFilename]->m_box->m_minCorner.x(), currentPoint.x());
			m_fileMaps[currentOutFilename]->m_box->m_minCorner.y() = std::min(m_fileMaps[currentOutFilename]->m_box->m_minCorner.y(), currentPoint.y());
			m_fileMaps[currentOutFilename]->m_box->m_minCorner.z() = std::min(m_fileMaps[currentOutFilename]->m_box->m_minCorner.z(), currentPoint.z());
		    
			m_fileMaps[currentOutFilename]->m_box->m_maxCorner.x() = std::max(m_fileMaps[currentOutFilename]->m_box->m_maxCorner.x(), currentPoint.x());
			m_fileMaps[currentOutFilename]->m_box->m_maxCorner.y() = std::max(m_fileMaps[currentOutFilename]->m_box->m_maxCorner.y(), currentPoint.y());
			m_fileMaps[currentOutFilename]->m_box->m_maxCorner.z() = std::max(m_fileMaps[currentOutFilename]->m_box->m_maxCorner.z(), currentPoint.z());
		}
	}
	catch (...)
	{
		auto aa = 5;
	}

	file.close();
	//关闭m_fileMaps中的所有file，关闭前写出所有box文件
	for (auto it = m_fileMaps.cbegin(); it != m_fileMaps.cend(); ++it)
	{
		//取出文件名
		QString fileName = it->first;
		QFileInfo cInfo(fileName);
		QString abpath = cInfo.absolutePath();
		QString basename = cInfo.baseName();
		QString cOutname = abpath + "\\" + basename + "_box" + ".txt";

		QFile boxFile(cOutname);
		if (!boxFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
		{
			return eError;
		}
		

		//写出box文件
		QString boxInfo;
		boxInfo.append(QString("%1").arg(it->second->m_box->m_minCorner.x(), 0, 'f', 3));
		boxInfo.append(","); //添加分隔符
		boxInfo.append(QString("%1").arg(it->second->m_box->m_minCorner.y(), 0, 'f', 3));
		boxInfo.append(","); //添加分隔符
		boxInfo.append(QString("%1").arg(it->second->m_box->m_minCorner.z(), 0, 'f', 3));

		boxInfo.append(","); //添加分隔符

		boxInfo.append(QString("%1").arg(it->second->m_box->m_maxCorner.x(), 0, 'f', 3));
		boxInfo.append(","); //添加分隔符
		boxInfo.append(QString("%1").arg(it->second->m_box->m_maxCorner.y(), 0, 'f', 3));
		boxInfo.append(","); //添加分隔符
		boxInfo.append(QString("%1").arg(it->second->m_box->m_maxCorner.z(), 0, 'f', 3));

		boxFile.write(boxInfo.toStdString().c_str());
		boxFile.write("\n");

		it->second->m_qdatafile->close();
		delete it->second->m_qdatafile;
		it->second->m_qdatafile = nullptr;
		delete it->second->m_box;
		it->second->m_box = nullptr;

		boxFile.close();
	}
	//foreach(FileTrunkInfo* fi, m_fileMaps)
	//{
	//	//写出box文件
	//	QString boxInfo;
	//	boxInfo.append(QString("%1").arg(fi->m_box->m_minCorner.x(), 0, 'f', 3));
	//	boxInfo.append(","); //添加分隔符
	//	boxInfo.append(QString("%1").arg(fi->m_box->m_minCorner.y(), 0, 'f', 3));
	//	boxInfo.append(","); //添加分隔符
	//	boxInfo.append(QString("%1").arg(fi->m_box->m_minCorner.z(), 0, 'f', 3));

	//	boxInfo.append(","); //添加分隔符

	//	boxInfo.append(QString("%1").arg(fi->m_box->m_maxCorner.x(), 0, 'f', 3));
	//	boxInfo.append(","); //添加分隔符
	//	boxInfo.append(QString("%1").arg(fi->m_box->m_maxCorner.y(), 0, 'f', 3));
	//	boxInfo.append(","); //添加分隔符
	//	boxInfo.append(QString("%1").arg(fi->m_box->m_maxCorner.z(), 0, 'f', 3));

	//	fi->m_qBoxfile->write(boxInfo.toStdString().c_str());
	//	fi->m_qBoxfile->write("\n");

	//	fi->m_qdatafile->close();
	//	fi->m_qBoxfile->close();
	//}
	m_fileMaps.clear();

	std::cout << "finish bootom file" << std::endl;
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
	std::cout << current_date.toStdString() << std::endl;
	return eNoError;
}

void AsciiSpliter::WriteBoxTofile(QString infile, unsigned level, const QDir& outDir)
{
	QFileInfo info(infile);

	QString outboxName = QString("%1_%2.%3").arg(info.baseName()).arg(BOXEXT).arg(GetSuffix());
	QString outboxFullname = outDir.filePath(outboxName);
	QFile boxFile(outboxFullname);
	QTextStream boxStream(&boxFile);
	if (!boxFile.open(QIODevice::WriteOnly))
	{
		return;
	}
	boxStream << level << endl;
	boxStream << m_boundBox.m_minCorner.x() << "," << m_boundBox.m_minCorner.y() << "," << m_boundBox.m_minCorner.z() << endl;
	boxStream << m_boundBox.m_maxCorner.x() << "," << m_boundBox.m_maxCorner.y() << "," << m_boundBox.m_maxCorner.z() << endl;
	boxFile.close();
}

void AsciiSpliter::WriteFileByCombine(QString infile, unsigned level, const QDir& outDir, unsigned simple/* = 3*/)
{
	//根据底层文件合并生成上层文件
	//1.根据level确定行列号
	for (int i = level-1; i != 0; i--)
	{
		QFileInfo info(infile);

		//下一层的文件信息
		FileInfoParse fParseDown(info.baseName(),i+1);
		IndexFileMap fMapdown = fParseDown.GetFileMap();

		//上一层信息
		FileInfoParse fParseUp(info.baseName(),i);
		IndexFileMap fMapUp = fParseUp.GetFileMap();

		//遍历IndexFileMap，并在对应的file中写入内容并输出
		for (auto it = fMapUp.begin(); it != fMapUp.end(); ++it)
		{
			//根据key值，生成对应的四个key
			QStringList fileList = GenIndexsByIndex(i, it->first);

			//判断四个子文件，至少存在一个，即为有效，才创建新的outfile
			bool validSubfile = true;
			for (auto kk = 0; kk != fileList.size(); ++kk)
			{
				QString curName = QString("%1.%2").arg(fMapdown[fileList[kk]]).arg(GetSuffix());
				QString currentinFilename = outDir.filePath(curName);
				QFileInfo info(currentinFilename);
				if (info.exists())
				{
					validSubfile = true;
					break;
				}
				validSubfile = false;
			}
			
			
			if (!validSubfile)
			{
				continue;
			}

			//根据key值，创建file对象
			QString cName = QString("%1.%2").arg(it->second).arg(GetSuffix());
			QString currentOutFilename = outDir.filePath(cName);
			QFile outFile(currentOutFilename);
			if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
			{
				continue;
			}

			//同上创建box对象
			QString cBoxName = QString("%1_box.%2").arg(it->second).arg(GetSuffix());
			QString cBoxurrentOutFilename = outDir.filePath(cBoxName);
			QFile outBoxFile(cBoxurrentOutFilename);
			if (!outBoxFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
			{
				continue;
			}

			//写文件头
			{
				//WriteHeader(cName, currentOutFilename);
			}

			//遍历四个子文件，将其数据内容写入到outfile中，并记录box值，最后写出
			BoundBox comBineBox;
			for (int fnum = 0; fnum != 4; ++fnum)
			{
				char currentLine[500];
				QString curName = QString("%1.%2").arg(fMapdown[fileList[fnum]]).arg(GetSuffix());
				QString currentinFilename = outDir.filePath(curName);
				QFile inFile(currentinFilename);
				if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
				{
					continue;
				}

				//文件头共10行，需要先跳过，然后才是数据
				/*for (unsigned i = 0; i < 10; ++i)
				{
					inFile.readLine(currentLine, 500);
				}*/

				while(inFile.readLine(currentLine, 500) > 0)
				{
					outFile.write(currentLine);
					//更新box
					//分隔行并给currentPoint赋值
					QStringList boxlist = QString(currentLine).split(QRegExp(",|\\s+"),QString::SkipEmptyParts);

					Point3D currentPoint;
					currentPoint.x() = boxlist[0].toDouble(); 
					currentPoint.y() = boxlist[1].toDouble();
					currentPoint.z() = boxlist[0].toDouble();

					comBineBox.m_minCorner[0] = std::min(comBineBox.m_minCorner[0], currentPoint[0]);
					comBineBox.m_minCorner[1] = std::min(comBineBox.m_minCorner[1], currentPoint[1]);
					comBineBox.m_minCorner[2] = std::min(comBineBox.m_minCorner[2], currentPoint[2]);

					comBineBox.m_maxCorner[0] = std::max(comBineBox.m_maxCorner[0], currentPoint[0]);
					comBineBox.m_maxCorner[1] = std::max(comBineBox.m_maxCorner[1], currentPoint[1]);
					comBineBox.m_maxCorner[2] = std::max(comBineBox.m_maxCorner[2], currentPoint[2]);

					//应用采样率
					for (unsigned i = 0; i < simple-1; ++i)
					{
						inFile.readLine(currentLine, 500);
					}
				}

				inFile.close();

			}
			//写出box
			QString boxInfo;
			boxInfo.append(QString("%1").arg(comBineBox.m_minCorner.x(), 0, 'f', 3));
			boxInfo.append(","); //添加分隔符
			boxInfo.append(QString("%1").arg(comBineBox.m_minCorner.y(), 0, 'f', 3));
			boxInfo.append(","); //添加分隔符
			boxInfo.append(QString("%1").arg(comBineBox.m_minCorner.z(), 0, 'f', 3));

			boxInfo.append(","); //添加分隔符

			boxInfo.append(QString("%1").arg(comBineBox.m_maxCorner.x(), 0, 'f', 3));
			boxInfo.append(","); //添加分隔符
			boxInfo.append(QString("%1").arg(comBineBox.m_maxCorner.y(), 0, 'f', 3));
			boxInfo.append(","); //添加分隔符
			boxInfo.append(QString("%1").arg(comBineBox.m_maxCorner.z(), 0, 'f', 3));

			outBoxFile.write(boxInfo.toStdString().c_str());
			outBoxFile.write("\n");

			//关闭句柄
			outBoxFile.close();
			outFile.close();
		}
	}

	//std::cout << "finish combine file" << std::endl;
	//QDateTime current_date_time = QDateTime::currentDateTime();
	//QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
	//std::cout << current_date.toStdString() << std::endl;
}

QStringList AsciiSpliter::GenIndexsByIndex(int level, QString index)
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

BoundBox AsciiSpliter::GenSubBoxByFileName(const QString& name)
{
	BoundBox box;

	//! 根据已知信息找到边界盒最大最小值，然后构造box即可
	//判断文件名的basename中是否包含_字符
	QFileInfo fInfo(name);
	QString baseName = fInfo.baseName();

	if (baseName == "changshu_L0_X0_Y0" || baseName == "changshu_L0_X0_Y0.txt")
	{
		auto aa = 5;
	}
	

	bool hasUnderline = baseName.contains("_");
	int underLineNum = baseName.count("_");

	Q_ASSERT(underLineNum == 3);

	//1.取出来Lindex, xindex,yindex值
	QStringList bnList = baseName.split("_");
	int lIndex = bnList[1].remove(0,1).toInt();
	int xIndex = bnList[2].remove(0,1).toInt();;
	int yIndex = bnList[3].remove(0,1).toInt();;

	int realLevel = lIndex + 1;

	//2.行和列相等，确定行和列
	int cr = std::pow(double(2), (realLevel - 1));

	Point_3D minCorner(m_boundBox.m_minCorner)  ;
	minCorner.x() += m_boundBox.m_length / float(cr) * xIndex;
	minCorner.y() += m_boundBox.m_width / float(cr) * yIndex;
	minCorner.z() += 0;
	Point_3D maxCorner = m_boundBox.m_minCorner;
	maxCorner.x() += m_boundBox.m_length / float(cr) * (xIndex + 1);
	maxCorner.y() += m_boundBox.m_width / float(cr) * (yIndex+1);
	maxCorner.z() +=  (m_boundBox.m_maxCorner.z() - m_boundBox.m_minCorner.z());

	box = BoundBox(minCorner, maxCorner);

	return box;
}

void AsciiSpliter::WriteHeader(QString& curFilename, QString& currentOutFilename)
{
	//根据box以及当前文件索引创建并写出文件头
	BoundBox subbox = GenSubBoxByFileName(currentOutFilename);
	//写文件头
	m_fileMaps[currentOutFilename]->m_qdatafile->write("DCLW PFA");
	m_fileMaps[currentOutFilename]->m_qdatafile->write("\n");

	//! center
	QString centerInfo = QString("Center %1 %2 %3").arg(subbox.GetCenter().x()).arg(subbox.GetCenter().y()).arg(subbox.GetCenter().z());
	m_fileMaps[currentOutFilename]->m_qdatafile->write(centerInfo.toStdString().c_str());
	m_fileMaps[currentOutFilename]->m_qdatafile->write("\n");

	//! Rabgelist
	QString rangeInfo1 = QString("RangeList 2 {");
	m_fileMaps[currentOutFilename]->m_qdatafile->write(rangeInfo1.toStdString().c_str());
	m_fileMaps[currentOutFilename]->m_qdatafile->write("\n");

	QString rangeInfo2 = QString("%1 %2").arg(subbox.m_sphereRadius * 7.0).arg(10000000000);
	m_fileMaps[currentOutFilename]->m_qdatafile->write(rangeInfo2.toStdString().c_str());
	m_fileMaps[currentOutFilename]->m_qdatafile->write("\n");

	QString rangeInfo3 = QString("%1 %2").arg(0).arg(subbox.m_sphereRadius * 7.0);
	m_fileMaps[currentOutFilename]->m_qdatafile->write(rangeInfo3.toStdString().c_str());
	m_fileMaps[currentOutFilename]->m_qdatafile->write("\n");

	QString rangeInfo4 = QString("}");
	m_fileMaps[currentOutFilename]->m_qdatafile->write(rangeInfo4.toStdString().c_str());
	m_fileMaps[currentOutFilename]->m_qdatafile->write("\n");

	//! filenamelist
	QString filenameInfo1 = QString("FileNameList 2 {");
	m_fileMaps[currentOutFilename]->m_qdatafile->write(filenameInfo1.toStdString().c_str());
	m_fileMaps[currentOutFilename]->m_qdatafile->write("\n");

	QString filenameInfo2 = QString("\"\"");
	m_fileMaps[currentOutFilename]->m_qdatafile->write(filenameInfo2.toStdString().c_str());
	m_fileMaps[currentOutFilename]->m_qdatafile->write("\n");

	QString filenameInfo3 = curFilename;
	m_fileMaps[currentOutFilename]->m_qdatafile->write(filenameInfo3.toStdString().c_str());
	m_fileMaps[currentOutFilename]->m_qdatafile->write("\n");

	QString filenameInfo4 = QString("}");
	m_fileMaps[currentOutFilename]->m_qdatafile->write(filenameInfo4.toStdString().c_str());
	m_fileMaps[currentOutFilename]->m_qdatafile->write("\n");
}