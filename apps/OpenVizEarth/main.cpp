
//Qt
#include <QtWidgets/QApplication>
#include "QFile"
#include "QTextCodec"
#include "QTranslator"

//osg
#include <osgDB/Registry>

#include "Mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	osgDB::Registry::instance()->getDataFilePathList().push_back( QString(qApp->applicationDirPath() + "/Resources/data/").toStdString () );

	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

	//创建语言翻译器
	QTranslator pTranslator;
	//加载语言文件
	pTranslator.load(":/translate/Resources/languages/zh_cn/qt_zh_CN.lng");
	//应用程序安装语言翻译器
	a.installTranslator(&pTranslator);

	//加载样式表
	QFile qssFile(":/Qss/Resources/qss/stylesheet.qss");  

	qssFile.open(QFile::ReadOnly);  

	if(qssFile.isOpen())  

	{  

		QString qss = QLatin1String(qssFile.readAll());  

		qApp->setStyleSheet(qss);  

		qssFile.close();  
	}  

	//! 命令行参数解析
	int argc_ = argc;
    QVector<QByteArray> data;
    QVector<QString > argv_;

    // get the command line arguments as unicode string
    {
        QStringList args = a.arguments();
        for (QStringList::iterator it = args.begin(); it != args.end(); ++it) {
            argv_.push_back(*it);
        }
        //argv_.push_back(0); // 0-terminated string
    }

	MainWindow w;
	
	w.showMaximized();
	if(argv_.size() > 1)
	{
		w.PraseArgs(argv_);
	}
	
	return a.exec();
}
