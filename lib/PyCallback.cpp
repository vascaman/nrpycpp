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

/*!
 * \brief PyRunner::sendMessage is a message sent by the python side, the message is then bubled to the correct PyRunner
 * \param pyRunnerId is the id of the PyRunner the message is for
 * \param msg a string message, can be a json or whatever, this is just a channell actual syntax must be discussed between the involved parts
 * \note this method is called by the Python side
 */

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
