### Defining an RT worker
```c++

class RTWorker : public RTThreadWorker
{
    Q_OBJECT
public:
    RTWorker();
rtslots:
    void workerSlot(int num)
    {
      qDebug()<<num;
      emit workerSignal();
    }
signals:
    void workerSignal();
};

```

### Using RT worker in QThread
```c++

QThread thread;
RTWorker rtWorker;

// start threads
rtWorker.moveToThread(&thread);
thread.start();

// invoke rtslot
RTThreadWorker::callQueued(&rtWorker, &RTWorker::workerSlot, 100);

// connect to signal
connect(&rtWorker, &RTWorker::workerSignal, this, []{qDebug()<<"workerSignal emitted"});

// join threads
rtWorker.stop();
thread.quit();
thread.wait();

```
