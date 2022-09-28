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
#define ASSERT_RTSLOT_AFFINITY if(threadAffinity != QThread::currentThread()) throw RTSlotDirectCallException();
#define ASSERT_RTSLOT_AFFINITY_NAME(name) if(threadAffinity != QThread::currentThread()) throw RTSlotDirectCallException(name);

class RTThreadWorker : public QObject
{
public:
    class RTSlotDirectCallException : public std::runtime_error
    {
    public:
        RTSlotDirectCallException() : runtime_error("RT slot was called directly. Call the slot with RTThreadWorker::callQueued() instead to ensure thread safety.\n") {};
        explicit RTSlotDirectCallException(std::string slotName) : runtime_error("RT slot \"" + slotName + "\" was called directly. Call the function with RTThreadWorker::callQueued() to ensure thread safety.\n") {};
    };

    template<typename ObjPtr, typename FuncPtr, class... Args>
    static void callQueued(ObjPtr objPtr, FuncPtr funcPtr, Args... args);
    RTThreadWorker();
    ~RTThreadWorker();
    void stop();
    void queueNewEvent(const std::function<void(void)>& func);
public:
    void moveToThread(QThread* thread);
private slots:
    void start();
protected:
    QThread* threadAffinity;
private:
    std::queue<std::function<void(void)>> m_eventQueue;
    std::mutex m_mutexRunnung;
    std::mutex m_mutexEvent;
    bool m_isRunning;
    void handleQueuedEvents();
};

template<typename ObjPtr, typename FuncPtr, class... Args>
void RTThreadWorker::callQueued(ObjPtr objPtr, FuncPtr funcPtr, Args... args)
{
    RTThreadWorker* workerPtr = objPtr;
    workerPtr->queueNewEvent(std::bind(funcPtr, objPtr, args...));
}

#endif
