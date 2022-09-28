#include "rtWorker.h"
#include <QDebug>

RTWorker::RTWorker()
{
    setTimerInterval(0.1);
}

void RTWorker::timerCallback()
{
    qDebug()<<"timer";
}

void RTWorker::startRTSlot(int num)
{
    assertRTSlotAffinity();
    qDebug()<<num;
}
