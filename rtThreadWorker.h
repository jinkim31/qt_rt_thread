#ifndef RTTHREADWORKER_H
#define RTTHREADWORKER_H

#include <QObject>
#include <QTimer>
#include <queue>
#include <map>
#include <functional>
#include <mutex>
#include <QDebug>
#include <QThread>
#include <stdexcept>

#define rtslots public

class RTThreadWorker : public QObject
{
public:
    class RTSlotDirectCallException : public std::runtime_error
    {
    public:
        RTSlotDirectCallException() : runtime_error("An RTslot was called directly from an outside thread. Call the slot with RTThreadWorker::callQueued() instead to ensure thread safety.\n") {};
        explicit RTSlotDirectCallException(std::string slotName) : runtime_error("RTslot \"" + slotName + "\" was called directly from an outside thread. Call the slot with RTThreadWorker::callQueued() to ensure thread safety.\n") {};
    };

    template<typename ObjPtr, typename FuncPtr, class... Args>
    static void callQueued(ObjPtr objPtr, FuncPtr funcPtr, Args... args);
    RTThreadWorker();
    ~RTThreadWorker();
    void stop();
    void queueNewEvent(const std::function<void(void)>& func);
    void moveToThread(QThread* thread);
private slots:
    void start();
protected:
    QThread* m_threadAffinity;
    virtual void timerCallback() = 0;
    void setTimerInterval(const double timeInSec);
    void assertRTSlotAffinity();
    void assertRTSlotAffinity(const std::string& name);
private:
    // mutexes
    std::mutex m_mutexRunnung;
    std::mutex m_mutexEvent;

    // shared resources
    std::queue<std::function<void(void)>> m_eventQueue;
    bool m_isRunning;

    // safe resources
    double m_timerInterval;
    std::chrono::high_resolution_clock::time_point m_timePointPrev;
    void handleQueuedEvents();
};

template<typename ObjPtr, typename FuncPtr, class... Args>
void RTThreadWorker::callQueued(ObjPtr objPtr, FuncPtr funcPtr, Args... args)
{
    ((RTThreadWorker*)objPtr)->queueNewEvent(std::bind(funcPtr, objPtr, args...));
}

#endif
