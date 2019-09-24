#include <QSplashScreen>
#include <QProgressBar>
#include <QPixmap>

class OpenVizSplashScreen : public QSplashScreen
{
	Q_OBJECT

public:
	explicit OpenVizSplashScreen(QPixmap& pixmap);
	~OpenVizSplashScreen();

private:
	QFont splashFont;
	QPixmap rePixmap;
	QProgressBar *progressBar;
	int totalSteps, nowStep;

private slots:
	void setTotalInitSteps(int num);
	void setNowInitName(const QString& name);

};

