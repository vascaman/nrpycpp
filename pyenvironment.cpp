#include "pyenvironment.h"

PyEnvironment::~PyEnvironment()
{
    Py_Finalize();
}

PyRunner * PyEnvironment::getInstanceModule(QString modulePath,QStringList dependecies)
{
    PyRunner* moduleRunner = new PyRunner(modulePath, dependecies);
    return moduleRunner;
}

PyEnvironment::PyEnvironment()
{
    Py_Initialize();
    PyEval_InitThreads();
    PyEval_ReleaseLock();

}

PyEnvironment &PyEnvironment::getInstance()
{
    static PyEnvironment instance;
    return instance;
}

bool PyEnvironment::start()
{
    return true;
}

bool PyEnvironment::stop()
{
    return true;
}

