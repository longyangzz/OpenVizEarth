#include "DlgClassify.h"

 //Qt
#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <osgDB/ReadFile>
#include <osg/Geometry>

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
//DC
#include "DC/LogHandler.h"

DlgClassify::DlgClassify(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	CreateConnects();
}

DlgClassify::~DlgClassify()
{

}

void DlgClassify::CreateConnects()
{
	connect(ui.pbtnOk, SIGNAL(clicked()), this, SLOT(Ok()));
	connect(ui.PbtnCancel, SIGNAL(clicked()), this, SLOT(Cancel()));
	connect(ui.pbtnSelectDirIn, SIGNAL(clicked()), this, SLOT(SelectDirIn()));
	connect(ui.pbtnSelectDirOut, SIGNAL(clicked()), this, SLOT(SelectDirOut()));
	connect(ui.pbtnPrefix, SIGNAL(clicked()), this, SLOT(SelcetPrefixName()));

}

void DlgClassify::SetInput(std::vector<DCVector3D> inPutPoint)
{
	m_inPutPoint = inPutPoint;
}
void DlgClassify::Ok()
{
	

}

void DlgClassify::Cancel()
{
	close();
}

void DlgClassify::SelectDirIn()
{
	QString outDir = QFileDialog::getExistingDirectory(this);
	ui.stxtDirIn->setText(outDir);

	m_fileListIn.clear();
	QDir dir(outDir);
	QStringList nameFilters;
	nameFilters << "*.dat" << "*.las";
	m_fileListIn = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);

}
void DlgClassify::SelectDirOut()
{
	QString outDir = QFileDialog::getExistingDirectory(this);
	ui.stxtDirOut->setText(outDir);
}

void DlgClassify::SelcetPrefixName()
{
	//选择输入文件
	QString fileName = QFileDialog::getOpenFileName(this);
	QFileInfo fInfo(fileName);
	//fInfo.baseName();
	ui.stxtPrefix->setText(fInfo.baseName());
}

void DlgClassify::UpDataFileInList()
{
	//updataFileList
	QStringList tmpList;
	for (auto it = m_fileListIn.begin(); it != m_fileListIn.end(); it++)
	{
		bool iSContain = (*it).contains(ui.stxtPrefix->text(), Qt::CaseSensitive);
		if (iSContain)
		{
			tmpList.push_back((*it));
		}
	}
	m_fileListIn.clear();
	m_fileListIn = tmpList;
}

void DlgClassify::GetOutput(std::vector<DCVector3D> & points)
{
	points = m_LowerPoint;
	m_LowerPoint.clear();
}
