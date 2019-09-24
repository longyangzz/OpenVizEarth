#include "UIFacade.h"
#include "OpenVizSplashScreen.h"

#include <QApplication>
#include <QFile>
#include <QTime>
#include <QSurfaceFormat>

int  main(int argc, char *argv[])
{
	// A trick to get higher fps than 30
	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setSwapInterval(0);
	QSurfaceFormat::setDefaultFormat(format);

	QApplication  app(argc, argv);

	QString rootDir = QCoreApplication::applicationDirPath() + "/";

	// Load an application style
	QFile  styleFile(rootDir + "Resources/styles/OpenViz.qss");

	

	if (styleFile.open(QFile::ReadOnly))
	{
		QString  style(styleFile.readAll());
		app.setStyleSheet(style);
	}

	// Show splash screen
	QPixmap            a(rootDir + "Resources/images/startUpSelf.png");
	OpenVizSplashScreen *splash = new OpenVizSplashScreen(a);

	//! UI界面组装
	UIFacade              w;
	QObject::connect(&w, SIGNAL(sendTotalInitSteps(int)), splash, SLOT(setTotalInitSteps(int)));
	QObject::connect(&w, SIGNAL(sendNowInitName(const QString&)), splash, SLOT(setNowInitName(const QString&)));

	QTime time;
	time.start();
	int start = time.elapsed() / 1000;
	splash->show();
	
	w.initAll();

	while (time.elapsed() / 1000 - start < 3)
	{

	}

	// Begin application
	w.showMaximized();
	splash->finish(&w);
	delete splash;

	return app.exec();
}
