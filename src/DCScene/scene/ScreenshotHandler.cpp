#include "ScreenshotHandler.h"
#include <QtCore/QtDebug>

//---------------------------------------------------------------------------------

ScreenshotHandler::ScreenshotHandler(const std::string& filename,const std::string& extension)
    : m_filename(filename),
    m_extension(extension)
{}

void ScreenshotHandler::operator () (const osg::Image& image, const unsigned int /*context_id*/)
{
    m_copyImage = new osg::Image(image,osg::CopyOp::DEEP_COPY_IMAGES);
    emit newScreenshotAvailable(m_copyImage);
    qDebug() << "newScreenshotAvailable";
}
