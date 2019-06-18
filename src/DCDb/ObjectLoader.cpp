#include <QtCore/QFileInfo>

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>

//DC
#include "DC/LogHandler.h"

#include "ObjectLoader.h"


#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

//---------------------------------------------------------------------------------

ObjectLoader::ObjectLoader(QObject *parent) :
    QObject(parent),
    m_optimize(false)
{}

void ObjectLoader::newObjectToLoad(const QString &file, QString type)
{
    m_file = file;

    // go to file path (for relative included resources)
    chdir( QFileInfo(m_file).absolutePath().toStdString().c_str() );

    // load the scene.
    LogHandler::getInstance()->reportInfo( tr("Loading of %1 ...").arg(file) );
    m_loadedModel = osgDB::readNodeFile( file.toStdString() );

    if (!m_loadedModel)
    {
        QString mess = tr("Error loading file %1").arg(file);
        LogHandler::getInstance()->reportError( mess );
    }

    // optimize the scene graph, remove redundant nodes and state etc.
    if (m_optimize)
    {
        LogHandler::getInstance()->reportInfo( tr("Optimization of the graph ...") );
        osgUtil::Optimizer optimizer;
        optimizer.optimize( m_loadedModel.get() );
    }

    emit newObjectToView(m_loadedModel, type);
}
