#ifndef _SCREENSHOT_HANDLER_H_
#define _SCREENSHOT_HANDLER_H_

#include <QtCore/QObject>

#include <osg/Image>
#include <osgViewer/ViewerEventHandlers>

class ScreenshotHandler :
    public QObject, public osgViewer::ScreenCaptureHandler::CaptureOperation
{
    Q_OBJECT

public:

    ScreenshotHandler(const std::string& filename,const std::string& extension);
    ~ScreenshotHandler(){}

    osg::Image *getScreenShot() {return m_copyImage; }

    void operator () (const osg::Image& image, const unsigned int context_id);

signals:

    void newScreenshotAvailable(osg::Image *);

private:

    const std::string m_filename;
    const std::string m_extension;
    osg::ref_ptr<osg::Image> m_copyImage;
};

#endif // _SCREENSHOT_HANDLER_H_
