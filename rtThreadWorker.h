#ifndef RTTHREADWORKER_H
#define RTTHREADWORKER_H

#include <QObject>
#include <QTimer>
#include <queue>
#include <map>
#include <functional>
#include <mutex>
#include <QDebug>

#define rtslots public
class RTThreadWorker : public QObject
{
public:
    template<typename ObjPtr, typename FuncPtr, class... Args>
    static void callQueued(ObjPtr objPtr, FuncPtr funcPtr, Args... args);
    RTThreadWorker();
    ~RTThreadWorker();
    void stop();
    void addQueuedEvent(const std::function<void(void)>& func);
public slots:
    void start();   // must be call through queued signal-slot. blocking loop will run in calling thread otherwise
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
    workerPtr->addQueuedEvent(std::bind(funcPtr, objPtr, args...));
}

#endif
