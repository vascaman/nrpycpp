#include "pyenvironment.h"

PyEnvironment::~PyEnvironment()
{

}

PyRunner * PyEnvironment::getInstanceModule(QString modulePath,QStringList dependecies)
{
    PyRunner* moduleRunner = new PyRunner(modulePath, dependecies);
    return moduleRunner;
}

PyEnvironment::PyEnvironment()
{

}

PyEnvironment &PyEnvironment::getInstance()
{
    static PyEnvironment instance;
    return instance;
}

bool PyEnvironment::start()
{
    Py_Initialize();
    PyEval_InitThreads();
    PyEval_ReleaseLock();
    return true;
}

bool PyEnvironment::stop()
{

    try {

        PyGILState_STATE gstate = PyGILState_Ensure();
        Py_Finalize();
//        PyGILState_Release(gstate);
    } catch (...) {

    }

    return true;
}

