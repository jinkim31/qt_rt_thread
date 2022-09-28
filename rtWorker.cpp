#include "rtWorker.h"
#include <QDebug>


RTWorker::RTWorker()
{

}

void RTWorker::startRTSlot(int num)
{
    ASSERT_RTSLOT_AFFINITY;
    qDebug()<<num;
}
