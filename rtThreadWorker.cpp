#include "rtThreadWorker.h"

RTThreadWorker::RTThreadWorker()
{
    m_isRunning = false;
    m_threadAffinity = nullptr;
    m_timerInterval = 0.0;
}

RTThreadWorker::~RTThreadWorker()
{

}

void RTThreadWorker::stop()
{
    std::unique_lock<std::mutex> lock(m_mutexRunnung);
    m_isRunning = false;
}

void RTThreadWorker::queueNewEvent(const std::function<void ()> &func)
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
    m_threadAffinity = QThread::currentThread();
    m_timePointPrev = std::chrono::high_resolution_clock::now();

    auto isRunningSafe = [&]{std::unique_lock<std::mutex> lock(m_mutexRunnung); return m_isRunning;};
    while(isRunningSafe())
    {
        handleQueuedEvents();
        if(std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - m_timePointPrev).count() > m_timerInterval)
        {
            m_timePointPrev = std::chrono::high_resolution_clock::now();
            timerCallback();
        }
    }
}

void RTThreadWorker::setTimerInterval(const double timeInSec)
{
    m_timerInterval = timeInSec;
}

void RTThreadWorker::assertRTSlotAffinity()
{
    if(m_threadAffinity != QThread::currentThread()) throw RTSlotDirectCallException();
}

void RTThreadWorker::assertRTSlotAffinity(const std::string &name)
{
    if(m_threadAffinity != QThread::currentThread()) throw RTSlotDirectCallException(name);
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
