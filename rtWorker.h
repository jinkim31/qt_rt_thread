#ifndef RT_WORKER_H
#define RT_WORKER_H

#include "rtThreadWorker.h"

class RTWorker : public RTThreadWorker
{
    Q_OBJECT
public:
    RTWorker();
rtslots:
    void startRTSlot(int num);
signals:
    void testReady();
};

#endif
