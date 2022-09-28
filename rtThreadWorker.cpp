#include "rtThreadWorker.h"

RTThreadWorker::RTThreadWorker()
{
    m_isRunning = false;
    threadAffinity = nullptr;
}

RTThreadWorker::~RTThreadWorker()
{

}

void RTThreadWorker::stop()
{
    std::unique_lock<std::mutex> lock(m_mutexRunnung);
    m_isRunning = false;
}

void RTThreadWorker::addQueuedEvent(const std::function<void ()> &func)
{
    std::unique_lock<std::mutex> lock(m_mutexEvent);
    m_eventQueue.push(func);
}

void RTThreadWorker::moveToThread(QThread *thread)
{
    this->QObject::moveToThread(thread);
    QMetaObject::invokeMethod(this, &RTThreadWorker::start);
}

void RTThreadWorker::start()
{
    m_isRunning = true;
    threadAffinity = QThread::currentThread();
    auto isRunningSafe = [&]{std::unique_lock<std::mutex> lock(m_mutexRunnung); return m_isRunning;};
    while(isRunningSafe())
    {
        handleQueuedEvents();
    }
}

void RTThreadWorker::handleQueuedEvents()
{
    while(!m_eventQueue.empty())
    {
        // lock mutex and copy front function
        std::unique_lock<std::mutex> eventLock(m_mutexEvent);
        auto func = m_eventQueue.front();
        m_eventQueue.pop();
        eventLock.unlock();

       // execute function
       func();
    }
}
