#include "PyCallback.h"
#include <QDebug>
#include <QThread>
#include <pyenvironment.h>
PyCallback::PyCallback()
{

}

PyCallback::~PyCallback()
{

}

void PyCallback::sendMessage(QString pyRunnerId,QString msg)
{
    qDebug()<<"PyCallback:"<<msg<<"from thread"<<pyRunnerId;
    PyRunner * runner = PyEnvironment::getInstance().getRunner(pyRunnerId);
    if(!runner)
    {
        //notify error
        return;
    }

    runner->sendMessage(msg);
}
