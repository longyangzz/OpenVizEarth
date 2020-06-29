#include <osg/Notify>

#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/fstream>
#include <osgDB/Registry>

#include <iostream>
#include <stdio.h>
#include <string.h>

#include <osgDB/ReadFile>
#ifdef _DEBUG
#pragma comment(lib, "osgd.lib")
#pragma comment(lib, "osgDBd.lib")
#else
#pragma comment(lib, "osg.lib")
#pragma comment(lib, "osgDB.lib")
#endif // DEBUG

class ReaderWriterS3C : public osgDB::ReaderWriter
{
public:

	ReaderWriterS3C()
	{
		supportsExtension("s3c", "s3c format");
	}

	virtual const char* className() const { return "s3c reader"; }

	virtual ReadResult readNode(const std::string& file, const osgDB::ReaderWriter::Options* options) const
	{
		std::string ext = osgDB::getLowerCaseFileExtension(file);
		if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

		std::string fileName = osgDB::findDataFile(file, options);
		if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

		OSG_INFO << "Reading file " << fileName << std::endl;

		osg::Group* g = new osg::Group;
		std::string dir = osgDB::getFilePath(fileName);
		std::string dataPath = dir + "\\Data\\";
		osgDB::DirectoryContents dirNames = osgDB::getDirectoryContents(dataPath);
		for each (std::string dir in dirNames)
		{
			if (dir.find(".") != std::string::npos)
				continue;
			std::string osgbFile = dataPath + dir + "\\" + dir + ".osgb";
			g->addChild(osgDB::readNodeFile(osgbFile));
		}

		return g;

	}

};

// now register with Registry to instantiate the above
// reader/writer.
REGISTER_OSGPLUGIN(s3c, ReaderWriterS3C)