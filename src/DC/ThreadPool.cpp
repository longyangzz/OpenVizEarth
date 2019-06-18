#include "ThreadPool.h"

bool ThreadPool::instanceFlag = false;
ThreadPool* ThreadPool::single = NULL;

ThreadPool* ThreadPool::getInstance()
{
    if(!instanceFlag)
    {
        single = new ThreadPool();
        instanceFlag = true;
        return single;
    }
    else
    {
        return single;
    }
}

ThreadPool::ThreadPool()
{
// create the treads
    for (int i = 0; i <  QThread::idealThreadCount(); i++)
    {
        m_threadPool << new QThread;
        qDebug("ThreadPool: start thread %d ",i);
        m_threadPool.at(i)->start();
    }
}

void ThreadPool::stop()
{
    // stop each running thread properly
    foreach(QThread * th, m_threadPool)
    {
        if ( th->isRunning() )
        {
            th->quit();
            th->wait(1000);
        }
    }

    foreach(QThread * th, m_reservedThreadPool)
    {
        if ( th->isRunning() )
        {
            th->quit();
            th->wait(1000);
        }
    }
}

QThread *ThreadPool::getThread()
{
    int index = qrand() % m_threadPool.size();
    qDebug("send thread %d to caller",index);
    return m_threadPool[qrand() % m_threadPool.size()];
}

QThread *ThreadPool::getReservedThread()
{
    qDebug("send a reserved thread to caller");
    QThread *th = new QThread();
    m_reservedThreadPool << th;
    th->start();
    return th;
}
