//C++标准库
#include <algorithm>
#include <string>
#include <memory>
#include <cmath>
#include <fstream>				// std::ifstream
#include <iostream>				// std::cout
#include <array>

//Qt
#include <QFileInfo>

//liblas
#include <liblas/liblas.hpp>
#include "liblas/point.hpp"
#include "liblas/reader.hpp"
#include "liblas/writer.hpp"
#include "liblas/factory.hpp"

#include "laskernel.hpp"

#include "QTextStream"

#include "LasSpliter.h"
#include "GeneratePldStruct.h"
using namespace DC::Pyra;

LasSpliter::LasSpliter(const QString& suffix)
	: SPliter(suffix)
{
}

LasSpliter::~LasSpliter()
{

}

typedef boost::shared_ptr<liblas::Writer> WriterPtr;
typedef boost::shared_ptr<liblas::CoordinateSummary> SummaryPtr;


WriterPtr start_writer(   std::ostream*& ofs, 
	std::string const& output, 
	liblas::Header const& header)
{
	ofs = liblas::Create(output, std::ios::out | std::ios::binary);
	if (!ofs)
	{
		std::ostringstream oss;
		oss << "Cannot create " << output << "for write.  Exiting...";
		throw std::runtime_error(oss.str());
	}

	WriterPtr writer( new liblas::Writer(*ofs, header));
	return writer;
}

bool process(   std::istream& ifs,
	std::string const& output,
	liblas::Header & header,
	std::vector<liblas::FilterPtr>& filters,
	std::vector<liblas::TransformPtr>& transforms,
	boost::uint32_t split_mb,
	boost::uint32_t split_pts,
	bool verbose,
	bool min_offset)
{
	liblas::ReaderFactory f;
	liblas::Reader reader = f.CreateWithStream(ifs);
	SummaryPtr summary(new::liblas::CoordinateSummary);

	reader.SetFilters(filters);
	reader.SetTransforms(transforms);    

	if (min_offset) 
	{
		liblas::property_tree::ptree tree = SummarizeReader(reader);

		try
		{
			header.SetOffset(tree.get<double>("summary.points.minimum.x()"),
				tree.get<double>("summary.points.minimum.y()"),
				tree.get<double>("summary.points.minimum.z()"));


		}
		catch (liblas::property_tree::ptree_bad_path const& e) 
		{
			std::cerr << "Unable to write minimum header info.  Does the outputted file have any points?";
			std::cerr << e.what() << std::endl;
			return false;
		}
		if (verbose) 
		{
			std::cout << "Using minimum offsets ";
			SetStreamPrecision(std::cout, header.GetScaleX());
			std::cout << header.GetOffsetX() << " ";
			SetStreamPrecision(std::cout, header.GetScaleY());
			std::cout << header.GetOffsetY() << " ";
			SetStreamPrecision(std::cout, header.GetScaleZ());
			std::cout << header.GetOffsetZ() << " ";
			std::cout << std::endl;
		}
		reader.Reset();

		// If we have any reprojection going on, we have to reset the offsets 
		// of the HeaderPtr that is applied to the points as they are reprojected
		// or things will get screwed up when we go to re-assign the header
		// as we write the points with the min-offset
		std::vector<liblas::TransformPtr> transforms = reader.GetTransforms();
		std::vector<liblas::TransformPtr> new_transforms;
		for (std::size_t i = 0; i < transforms.size(); i++)
		{
			liblas::TransformPtr transform = transforms[i];

			if (dynamic_cast<liblas::ReprojectionTransform*>(transform.get()))
			{
				dynamic_cast<liblas::ReprojectionTransform*>(transform.get())->SetHeader(&header);
			}
			new_transforms.push_back(transform);
		}
		reader.SetTransforms(new_transforms);
	}

	std::ostream* ofs = NULL;
	std::string out = output;

	WriterPtr writer;

	if (!split_mb && !split_pts) {
		writer = start_writer(ofs, output, header);

	} else {
		string::size_type dot_pos = output.find_first_of(".");
		out = output.substr(0, dot_pos);
		writer = start_writer(ofs, out+"-1"+".las", header);
	}

	if (verbose)
		std::cout << "Writing output:" 
		<< "\n - : " << output
		<< std::endl;

	//
	// Translation of points cloud to features set
	//
	boost::uint32_t i = 0;
	boost::uint32_t const size = header.GetPointRecordsCount();

	boost::int32_t split_bytes_count = 1024*1024*split_mb;
	boost::uint32_t split_points_count = 0;
	int fileno = 2;


	while (reader.ReadNextPoint())
	{
		if (min_offset)
		{
			liblas::Point p = reader.GetPoint();
			summary->AddPoint(p);
			p.SetHeader(&header);
			writer->WritePoint(p);
		}
		else 
		{
			liblas::Point const& p = reader.GetPoint();
			summary->AddPoint(p);
			writer->WritePoint(p);            
		}
		if (verbose)
			term_progress(std::cout, (i + 1) / static_cast<double>(size));
		i++;
		split_points_count++;

		split_bytes_count = split_bytes_count - header.GetSchema().GetByteSize();        
		if (split_bytes_count < 0 && split_mb > 0 && ! split_pts) {
			// The user specifies a split size in mb, and we keep counting 
			// down until we've written that many points into the file.  
			// After that point, we make a new file and start writing into 
			// that.  

			// dereference the writer so it is deleted before the ofs
			writer = WriterPtr();

			delete ofs;
			ofs = NULL;

			ostringstream oss;
			oss << out << "-"<< fileno <<".las";

			writer = start_writer(ofs, oss.str(), header);

			ostringstream old_filename;
			old_filename << out << "-" << fileno - 1 << ".las";

			liblas::Header hnew = FetchHeader(old_filename.str());
			RepairHeader(*summary, hnew);
			RewriteHeader(hnew, old_filename.str());

			summary =  SummaryPtr(new liblas::CoordinateSummary); 
			fileno++;
			split_bytes_count = 1024*1024*split_mb;
		}

		if (split_pts > 0 && ! split_mb && split_points_count == split_pts) {
			// The user specifies a split size in pts, and we keep counting 
			// down until we've written that many points into the file.  
			// After that point, we make a new file and start writing into 
			// that.  

			// dereference the writer so it is deleted before the ofs
			writer = WriterPtr();

			delete ofs;
			ofs = NULL;

			ostringstream oss;
			oss << out << "-"<< fileno <<".las";

			writer = start_writer(ofs, oss.str(), header);

			ostringstream old_filename;
			old_filename << out << "-" << fileno - 1 << ".las";

			liblas::Header hnew = FetchHeader(old_filename.str());
			RepairHeader(*summary, hnew);
			RewriteHeader(hnew, old_filename.str());

			summary =  SummaryPtr(new liblas::CoordinateSummary); 
			fileno++;
			split_points_count = 0;
		}

	}
	if (verbose)
		std::cout << std::endl;

	// cheap hackery.  We need the Writer to disappear before the stream.  
	// Fix this up to not suck so bad.
	writer = WriterPtr();
	delete ofs;
	ofs = NULL;

	if (!split_mb && !split_pts) {
		reader.Reset();

		liblas::Header hnew = FetchHeader(output);
		RepairHeader(*summary, hnew);
		RewriteHeader(hnew, output);
	}

	return true;
}

std::vector<liblas::FilterPtr> GetFilters(bool verbose, BoundBox box)
{
	std::vector<liblas::FilterPtr> filters;
	liblas::Bounds<double> extent;
	bool bSetExtent = false;


	boost::char_separator<char> sep(SEPARATORS);

	std::vector<double> vbounds;
	vbounds.push_back(box.m_minCorner.x());
	vbounds.push_back(box.m_minCorner.y());
	vbounds.push_back(box.m_minCorner.z());
	vbounds.push_back(box.m_maxCorner.x());
	vbounds.push_back(box.m_maxCorner.y());
	vbounds.push_back(box.m_maxCorner.z());
	liblas::Bounds<double> bounds;
	if (vbounds.size() == 4) 
	{
		bounds = liblas::Bounds<double>(vbounds[0], 
			vbounds[1], 
			vbounds[2], 
			vbounds[3]);
	} else if (vbounds.size() == 6)
	{
		bounds = liblas::Bounds<double>(vbounds[0], 
			vbounds[1], 
			vbounds[2], 
			vbounds[3], 
			vbounds[4], 
			vbounds[5]);
	} else {
		ostringstream oss;
		oss << "Bounds must be specified as a 4-tuple or "
			"6-tuple, not a "<< vbounds.size()<<"-tuple" << "\n";
		throw std::runtime_error(oss.str());
	}

	if ( bSetExtent ) 
	{
		if (verbose) 
		{
			std::cout << " Growing --extent bounds with those that were set via --[x|y|z][min|max]" << std::endl;
		}
		bounds.grow(extent);
	}

	if (verbose)
	{
		std::cout << "---------------------------------------------------------" << std::endl;
		std::cout << " Clipping file to the extent" << std::endl;
		std::cout << "---------------------------------------------------------" << std::endl;

		std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
		std::cout.precision(6);

		std::cout << " minx: " << bounds.minx() 
			<< " miny: " << bounds.miny() 
			<< " minz: " << bounds.minz() 
			<< std::endl;
		std::cout << " maxx: " << bounds.maxx() 
			<< " maxy: " << bounds.maxy() 
			<< " maxz: " << bounds.maxz() 
			<< std::endl;
		std::cout << "---------------------------------------------------------" << std::endl;
	}

	liblas::FilterPtr bounds_filter = MakeBoundsFilter(bounds, liblas::FilterI::eInclusion);
	// Set to false because we are using this opportunity to set the filter
	// If it were still true after this point, *another* BoundsFilter would be 
	// added to the filters list at the end of this function
	if (bSetExtent)
		bSetExtent = false; 
	filters.push_back(bounds_filter);

	return filters;
}

std::vector<liblas::TransformPtr> GetTransforms(bool verbose, liblas::Header& header)
{
	std::vector<liblas::TransformPtr> transforms;

	return transforms;
}

ErrorType LasSpliter::GetBoundBoxByFile(const QString& inFile, BoundBox& outBox, unsigned simple/* = 1*/)
{
	BoundBox box;

	//打开文件
	std::ifstream ifs;
	ifs.open(inFile.toStdString(), std::ios::in | std::ios::binary);

	if (ifs.fail())
	{
		//DC::Logger::Error(QObject::tr("Fail to open file [%1].").arg(inFile));
		return eError;
	}

	liblas::Reader* reader = 0;
	unsigned nbOfPoints = 0;
	std::vector<std::string> dimensions;

	try
	{
		reader = new liblas::Reader(liblas::ReaderFactory().CreateWithStream(ifs));
		//处理压缩与非压缩文件
		liblas::Header const& header = reader->GetHeader();

		//获取字段
		dimensions = header.GetSchema().GetDimensionNames();
		//获取点个数
		nbOfPoints = header.GetPointRecordsCount();

		//获取最大最小值
		Point3D minCorner;
		minCorner.x() = header.GetMinX();
		minCorner.y() = header.GetMinY();
		minCorner.z() = header.GetMinZ();

			//(header.GetMinX(), header.GetMinY(), header.GetMinZ());
		//Point3D maxCorner(header.GetMaxX(), header.GetMaxY(), header.GetMaxZ());

		Point3D maxCorner;
		maxCorner.x() = header.GetMaxX();
		maxCorner.y() = header.GetMaxY();
		maxCorner.z() = header.GetMaxZ();
		box = BoundBox(minCorner, maxCorner);
	}
	catch (...)
	{
		delete reader;
		ifs.close();

		//DC::Logger::Error(QObject::tr("Fail to read file [%1].").arg(fileName));
		return eError;
	}



	box.m_length = box.m_maxCorner.x() - box.m_minCorner.x();
	box.m_width = box.m_maxCorner.y() - box.m_minCorner.y();
	outBox = box;
	m_boundBox = box;
	//std::cout << "finish Box" << std::endl;
	//QDateTime current_date_time = QDateTime::currentDateTime();
	//QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
	//std::cout << current_date.toStdString() << std::endl;
	return eNoError;
}

ErrorType LasSpliter::WriteFileByBoundBox(const QString& infile, const  QString& outFilename, BoundBox box)
{
	std::string output = outFilename.toStdString();
	
	boost::uint32_t split_mb = 0;
	boost::uint32_t split_pts = 0;

	bool verbose = false;
	bool bMinOffset = false;
	
	std::vector<liblas::FilterPtr> filters;
	std::vector<liblas::TransformPtr> transforms;

	bool bCompressed = false;
	liblas::Header header;

	std::istream* ifs = liblas::Open(infile.toStdString(), std::ios::in | std::ios::binary);
	if (!ifs)
	{
		return eError;
	}
	// set_ifstream_buffer(ifs, default_buffer_size);

	{// scope the reader, so it goes away before the stream does
		liblas::ReaderFactory f;
		liblas::Reader reader = f.CreateWithStream(*ifs);
		header = reader.GetHeader();
	}

	filters = GetFilters(verbose, box);
	transforms = GetTransforms(verbose, header);

	if (bCompressed)
	{
		header.SetCompressed(true);
	}
	else 
	{
		SetHeaderCompression(header, output);
	}

	bool op = process(  *ifs, 
		output,
		header, 
		filters,
		transforms,
		split_mb,
		split_pts,
		verbose,
		bMinOffset
		);
	if (!op) {
		return eError;
	}

	if (ifs != 0)
	{
		liblas::Cleanup(ifs);
	}
}

ErrorType LasSpliter::WriteSubfile(QString infile, const QDir& outDir, const LevelType& level)
{
	//根据box，level,在outfile内输出文件
	//1.根据level确定行列号
	QFileInfo info(infile);
	FileInfoParse fParse(info.baseName(),level);
	IndexFileMap fMap = fParse.GetFileMap();
	float xinter = m_boundBox.m_length / fParse.m_chunkColumn;
	float yinter = m_boundBox.m_width / fParse.m_chunkRow;
	//遍历每个边界盒及文件名写出数据
	for (auto it = fMap.begin(); it != fMap.end(); ++it)
	{
		QString curFilename = QString("%1.%2").arg(it->second).arg(GetSuffix());
		QString currentOutFilename = outDir.filePath(curFilename);

		//根据索引号计算边界盒值
		QStringList indexList = it->first.split("_");
		Point3D minCorner(m_boundBox.m_minCorner);
		minCorner.x() += indexList[0].toInt()*xinter;
		minCorner.y() += indexList[1].toInt()*yinter;
		minCorner.z()+= 0;
		//Point3D(indexList[0].toInt()*xinter,indexList[1].toInt()*yinter,0);
		Point3D maxCorner(minCorner);
		maxCorner.x() += xinter;
		maxCorner.y() += yinter;
		maxCorner.z()+= 0;
		//= minCorner + Point3D(xinter,yinter,0);
		maxCorner.z() = m_boundBox.m_maxCorner.z();
		BoundBox box(minCorner, maxCorner);
		ErrorType isWrite = WriteFileByBoundBox(infile, currentOutFilename, box);
	}
	
	GeneratePldStruct gPldStruct(m_boundBox, ".las");
	gPldStruct.WriteOsgBoxToFile(infile, level, outDir);

	//读取文件中的每个点，根据点坐标计算点所属的分区号，进而打开对应的文件写入点信息
	/*int colCellNum =  std::pow(double(2),(level-1));
	double xInter = m_bound.m_length / colCellNum;
	double yInter = m_bound.m_width / colCellNum;*/

	//char currentLine[500];
	//Point3D currentPoint;
	//QFile file(infile);
	//if (!file.open(QIODevice::ReadOnly | QIODevice::Truncate))
	//{
	//	return eError;
	//}

	//QTextStream inStream(&file);

	//while(!inStream.atEnd())
	//{
	//	//分隔行并给currentPoint赋值
	//	QString line = inStream.readLine();
	//	QStringList list = line.split(QRegExp("\\s+"),QString::SkipEmptyParts);

	//	if (list.size() < 3)
	//	{
	//		return eError;
	//	}

	//	currentPoint = Point3D(list[0].toDouble(), list[1].toDouble(), list[2].toDouble());
	//	//根据currentPoint计算分区号并写出
	//	
	//	unsigned columnIndex = 0, rowIndex = 0;
	//	m_boundBox.GetPointIndexInLevel(currentPoint, level, columnIndex, rowIndex);
	//	QString indexName = GenIndexNameByIndex(columnIndex, rowIndex);

	//	//3.创建输出文件流
	//	//QString currentOutFilename = outFile + "\\" + fMap[indexName] + m_extension;
	//	QString curFilename = QString("%1.%2").arg(fMap[indexName]).arg(GetSuffix());
	//	QString currentOutFilename = outDir.filePath(curFilename);

	//	//不存在当前名字对应的Qfile对象，则创建
	//	if (!m_fileMaps.contains(currentOutFilename))
	//	{
	//		QFile* ofile = new QFile(currentOutFilename);
	//		m_fileMaps[currentOutFilename] = ofile;
	//	}
	//	
	//	//此时一定存在对应名字所映射的Qfile对象，但是尚不清楚是否已经打开
	//	if (!m_fileMaps[currentOutFilename]->isOpen())
	//	{
	//		m_fileMaps[currentOutFilename]->open(QIODevice::WriteOnly | QIODevice::Text);
	//	}
	//	//if (!ofile.open(QIODevice::Append | QIODevice::Text))
	//	//{
	//	//	return eError;
	//	//}

	//	//定义一个当前行变量
	//	QString writeline;

	//	//写入当前坐标
	//	writeline.append(QString("%1").arg(currentPoint.x(), 0, 'f', 3));
	//	writeline.append(","); //添加分隔符
	//	writeline.append(QString("%1").arg(currentPoint.y(), 0, 'f', 3));
	//	writeline.append(","); //添加分隔符
	//	writeline.append(QString("%1").arg(currentPoint.z(), 0, 'f', 3));

	//	m_fileMaps[currentOutFilename]->write(writeline.toStdString().c_str());
	//	m_fileMaps[currentOutFilename]->write("\n");
	//	
	//}

	//file.close();
	////关闭m_fileMaps中的所有file
	//foreach(QFile* fi, m_fileMaps)
	//{
	//	fi->close();
	//}
	//m_fileMaps.clear();

	//std::cout << "finish bootom file" << std::endl;
	//QDateTime current_date_time = QDateTime::currentDateTime();
	//QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
	//std::cout << current_date.toStdString() << std::endl;
	return eNoError;
}

void LasSpliter::WriteBoxTofile(QString infile, unsigned level, const QDir& outDir)
{
	QFileInfo info(infile);

	QString outboxName = QString("%1_%2.%3").arg(info.baseName()).arg(BOXEXT).arg("box");
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

void LasSpliter::WriteFileByCombine(QString infile, unsigned level, const QDir& outDir, unsigned simple/* = 3*/)
{
	//读取infile的header
	liblas::Header header;
	std::istream* ifs1 = liblas::Open(infile.toStdString(), std::ios::in | std::ios::binary);
	if (!ifs1)
	{
		return;
	}
	{// scope the reader, so it goes away before the stream does
		liblas::ReaderFactory f;
		liblas::Reader reader = f.CreateWithStream(*ifs1);
		header = reader.GetHeader();
	}
	delete ifs1;
	ifs1 = NULL;


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
		float xinter = m_boundBox.m_length / fParseUp.m_chunkColumn;
		float yinter = m_boundBox.m_width / fParseUp.m_chunkRow;


		//遍历IndexFileMap，并在对应的file中写入内容并输出
		for (auto it = fMapUp.begin(); it != fMapUp.end(); ++it)
		{
			//根据key值，创建file对象
			QString cName = QString("%1.%2").arg(it->second).arg(GetSuffix());
			QString currentOutFilename = outDir.filePath(cName);

			//更新header对应的边界值
			//根据索引号计算边界盒值
			QStringList indexList = it->first.split("_");
	/*		Point3D minCorner = m_boundBox.m_minCorner + Point3D(indexList[0].toInt()*xinter,indexList[1].toInt()*yinter,0);
			Point3D maxCorner = minCorner + Point3D(xinter,yinter,0);
*/
			Point3D minCorner(m_boundBox.m_minCorner);
			minCorner.x() += indexList[0].toInt()*xinter;
			minCorner.y() += indexList[1].toInt()*yinter;
			minCorner.z()+= 0;
			//Point3D(indexList[0].toInt()*xinter,indexList[1].toInt()*yinter,0);
			Point3D maxCorner(minCorner);
			maxCorner.x() += xinter;
			maxCorner.y() += yinter;
			maxCorner.z()+= 0;
			
			maxCorner.z() = m_boundBox.m_maxCorner.z();

			header.SetMin(minCorner.x(), minCorner.y(), minCorner.z());
			header.SetMax(maxCorner.x(), maxCorner.y(), maxCorner.z());

			//创建一个输出对象
			std::ostream* ofs = NULL;
			WriterPtr writer;
			writer = start_writer(ofs, currentOutFilename.toStdString(), header);

			//根据key值，生成对应的四个key
			QStringList fileList = GenIndexsByIndex(i, it->first);

			//遍历四个子文件，将其内容写入到outfile中
			for (int fnum = 0; fnum != 4; ++fnum)
			{
				char currentLine[500];
				QString curName = QString("%1.%2").arg(fMapdown[fileList[fnum]]).arg(GetSuffix());
				QString currentinFilename = outDir.filePath(curName);
				//创建一个reader
				std::istream* ifs = liblas::Open(currentinFilename.toStdString(), std::ios::in | std::ios::binary);
				if (!ifs)
				{
					continue;
				}

				liblas::ReaderFactory f;
				liblas::Reader reader = f.CreateWithStream(*ifs);

				while (reader.ReadNextPoint())
				{
					liblas::Point const& p = reader.GetPoint();
					
					writer->WritePoint(p);

					//应用采样率
					for (unsigned i = 0; i < simple-1; ++i)
					{
						reader.ReadNextPoint();
					}
				}

				delete ifs;

			}
			
			//清空并调整header
			writer = WriterPtr();
			delete ofs;
			ofs = NULL;
		}
	}

	//std::cout << "finish combine file" << std::endl;
	//QDateTime current_date_time = QDateTime::currentDateTime();
	//QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
	//std::cout << current_date.toStdString() << std::endl;
}

QStringList LasSpliter::GenIndexsByIndex(int level, QString index)
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