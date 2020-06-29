#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QMutexLocker>
#include <QtCore/QFileInfo>

#include <stdlib.h> // for getenv

#include "LogHandler.h"
#include "DC/SettingsManager.h"

const QString logHeaderInformation("GraphX Log");

const int sendMessageInterval = 500; // 500 ms

LogHandler* LogHandler::m_instance = 0;

LogHandler *LogHandler::getInstance()
{
    static QMutex mutex;

    if (!m_instance)
    {
        mutex.lock();

        if (!m_instance)
            m_instance = new LogHandler;

        mutex.unlock();
    }

    return m_instance;
}

LogHandler::~LogHandler()
{
    static QMutex mutex;
    mutex.lock();
    m_instance = 0;
    mutex.unlock();
}

LogHandler::LogHandler(  ) :
    m_currLevel(LOG_DEBUG),
    m_bufferized(true),
    m_startEmission(false),
    m_timer(NULL),
    m_saveToLog(false)
{
    //loadSettings();
    m_timer = new QTimer(this);
    m_timer->setInterval(sendMessageInterval);
    connect( m_timer,SIGNAL( timeout() ),this,SLOT( unqueueWaitingMessages() ) );
}

void LogHandler::reportMessage(LogLevel level, const QString &message )
{
    // message Ignored
    if (level < m_currLevel)
        return;

    /* Firstly send message to file log */
    if (m_saveToLog)
        fillAppLogFile(message);

    QString msgText( message );
    msgText.replace( "<", "&lt;" );
    msgText.replace( ">", "&gt;" );

    // add the current time
    msgText = QTime::currentTime().toString("[hh:mm:ss]: ") + msgText;
    QString msg;

    switch (level)
    {
        case LOG_DEBUG:
        {
            msg = QString( "<font color=\"blue\"><b>DEBUG</b></font>: " ) + msgText;
            break;
        }
        case LOG_INFO:
        {
            msg = QString( "<font color=\"green\"><b>INFO</b>: </font>" ) + msgText;
            break;
        }
        case LOG_WARNING:
        {
            msg = QString( "<font color=\"orange\"><b>WARNING</b>: </font>" ) + msgText;
            break;
        }
        case LOG_ERROR:
        {
            msg = QString( " <font color=\"red\"><b>ERROR</b>:</font>" ) + msgText;
            break;
        }
        default:
        {
            msg = msgText;
            break;
        }
    }

    if (m_bufferized || !m_startEmission)
        m_buffer << msg;
    else
        emit newMessage(msg);
}

void LogHandler::reportDebug(const QString &message )
{
    QMutexLocker locker(&m_lock);
    reportMessage(LOG_DEBUG,message);
}

void LogHandler::reportInfo(const QString &message )
{
    QMutexLocker locker(&m_lock);
    reportMessage(LOG_INFO,message);
}

void LogHandler::reportWarning(const QString &message )
{
    QMutexLocker locker(&m_lock);
    reportMessage(LOG_WARNING,message);
}

void LogHandler::reportError(const QString &message )
{
    QMutexLocker locker(&m_lock);
    reportMessage(LOG_ERROR,message);
}

void LogHandler::setBufferization(bool val)
{
    m_bufferized = val;
    if (!m_bufferized) // send all stored messages
    {
        m_timer->stop();
        unqueueWaitingMessages();
    }
    else
    {
        m_timer->start();
    }
}

void LogHandler::startEmission(bool val)
{
    m_startEmission = val;
    if (m_startEmission && m_bufferized)
        m_timer->start();
    else
        m_timer->stop();
}

void LogHandler::unqueueWaitingMessages()
{
    QMutexLocker locker(&m_lock);
    if (m_startEmission)
    {
        if ( !m_buffer.isEmpty() )
        {
            emit newMessages(m_buffer);
            m_buffer.clear();
        }
    }
}

void LogHandler::loadSettings()
{
    SettingsManager settings;

    settings.beginGroup("Application");
    settings.beginGroup("Log");

    m_logDirectory = settings.value("logDirectory","/usr/tmp").toString();

    settings.endGroup();
    settings.endGroup();
}

void LogHandler::setLogDirectory(const QString &path)
{
    if ( m_logDirectory == path || !QFileInfo(path).exists() )
        return;

    m_logDirectory = path;
    if (m_saveToLog && m_logFile)
    {
        m_logFile->close();
        delete m_logStream;
        m_logStream = NULL;
        delete m_logFile;
        m_logFile = NULL;
    }
}

void LogHandler::setLogToFile(bool val)
{
    if (m_saveToLog == val)
        return;

    m_saveToLog = val;
    if (!m_saveToLog && m_logFile)
    {
        m_logFile->close();
        delete m_logStream;
        m_logStream = NULL;
        delete m_logFile;
        m_logFile = NULL;
    }
}

bool LogHandler::fillAppLogFile(const QString &message)
{
    /* Assure one access on the file by a mutex */
    if(!m_logStream)
    {
        /* Product a log error */
        QString cdate = QDate::currentDate().toString("dd-MM-yyyy");
        QString ctime = QTime::currentTime().toString("hh:mm");
        QString username = QString( getenv("USERNAME") );

        // try with USER environment variable
        if ( username.isEmpty() )
            username = QString( getenv("USER") );

        QString logName = QString("%1/%2.log").arg(m_logDirectory).arg(PACKAGE_NAME);

        m_logFile = new QFile(logName);

        // check the size of the file if exists => limit the log file to 10MBytes
        if (m_logFile->size() > 10 * 1024 * 1024)
            m_logFile->remove();

        if ( m_logFile->open(QFile::WriteOnly | QFile::Append) )
        {
            m_logStream = new QTextStream(m_logFile);

            *m_logStream << "-----------------------------------------------------" << "\n";
            *m_logStream << logHeaderInformation << QString(" by %1. Begin session at (%2 - %3) \n").arg(username).arg(cdate).arg(ctime);
            *m_logStream << "-----------------------------------------------------" << "\n\n";
        }
        else
        {
            emit newMessage( QString("<font color=\"red\"><b>Can't open for writting %1</b></font> <br>").arg(logName) );
        }
    }

    if(m_logStream)
    {
        *m_logStream << message << "\n";
        return true;
    }

    return false;
}
