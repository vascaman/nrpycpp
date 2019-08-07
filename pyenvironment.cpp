#include "pyenvironment.h"

PyEnvironment::~PyEnvironment()
{
    Py_Finalize();
}

PyRunner * PyEnvironment::getInstanceModule(QString modulePath)
{
    PyRunner* moduleRunner = new PyRunner(modulePath);
    return moduleRunner;
}

PyEnvironment::PyEnvironment()
{
    Py_Initialize();
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

