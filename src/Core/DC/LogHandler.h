#ifndef _LOGHANDLER_H_
#define _LOGHANDLER_H_

#include "dc_global.h"

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QStringList>

class QTimer;
class QTextStream;
class QFile;

/*!
 *	日志
 */

class DC_EXPORT LogHandler : public QObject
{
    Q_OBJECT

public:

    enum LogLevel
    {
        LOG_DEBUG = 0,                      /*!< 只调试*/
        LOG_INFO,                           /*!< 信息 */
        LOG_WARNING,                        /*!< 警告 */
        LOG_ERROR                           /*!< 错误*/
    };

    static LogHandler *getInstance();
    ~LogHandler();

    void setMessageLevel(LogLevel level)
    {
        m_currLevel = level;
    }

    void setLogDirectory(const QString &);
    void setLogToFile(bool);

    void setBufferization(bool);
    void startEmission(bool);

public slots:

    void reportDebug(const QString &message );
    void reportInfo(const QString &message );
    void reportWarning(const QString &message );
    void reportError(const QString &message );

signals:

    void newMessage(const QString &);
    void newMessages(const QStringList &);

private slots:

    void unqueueWaitingMessages();

private:

    LogHandler(  );

    LogHandler(const LogHandler &); // copy 构造
    LogHandler& operator=(const LogHandler &); // 赋值构造


    void reportMessage(LogLevel level, const QString &message );
    void unqueueMessages();
    void loadSettings();
    bool fillAppLogFile(const QString &message);

    QMutex m_lock;
    static LogHandler* m_instance;
    LogLevel m_currLevel;
    QStringList m_buffer;
    bool m_bufferized;
    bool m_startEmission;
    QTimer *m_timer;
    QString m_logDirectory;
    QTextStream * m_logStream;
    QFile *m_logFile;
    bool m_saveToLog;
};

#endif  // _LOGHANDLER_H_
