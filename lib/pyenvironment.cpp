#include "pyenvironment.h"

#include <QMutexLocker>

extern "C" void _nrpycpp_write_callback(const char* s, const char* runner_id) {
    PyEnvironment::getInstance().onStdOutputWriteCallBack(s, runner_id);
}

extern "C" void _nrpycpp_flush_callback(const char* runner_id) {
    PyEnvironment::getInstance().onStdOutputFlushCallback(runner_id);
}
extern "C" void _nrpycpp_exception_callback(const char* s, const char* runner_id) {
    PyEnvironment::getInstance().onExceptionCallback(s, runner_id);
}

extern "C" void _nrpycpp_send_message_callback(const char* message,const char* runner_id) {
    PyEnvironment::getInstance().onSendMessage(message, runner_id);
}

PyEnvironment::~PyEnvironment()
{
    qDebug() << "PyEnvironment dtor";
}

PyRunner* PyEnvironment::getRunner(QString runnerId)
{
    m_runnersLock.lock();
    PyRunner * runner = m_runners.value(runnerId);
    m_runnersLock.unlock();
    return runner;
}

void PyEnvironment::onStdOutputWriteCallBack(const char *s, QString runner_id)
{
    PyRunner * runner = getRunner(runner_id);
    if (runner == nullptr)
        return;
    runner->onStdOutputWriteCallBack(s);

}

void PyEnvironment::onStdOutputFlushCallback(QString runner_id)
{
    PyRunner * runner = getRunner(runner_id);
    if (runner == nullptr)
        return;
    runner->onStdOutputFlushCallback();
}

void PyEnvironment::onExceptionCallback(const char *msg, QString runner_id)
{
    PyRunner * runner = getRunner(runner_id);
    if (runner == nullptr)
        return;
    runner->onExceptionCallback(msg);
}

void PyEnvironment::onSendMessage(const char *msg, QString  runner_id)
{
    PyRunner * runner = getRunner(runner_id);
    if (runner == nullptr)
        return;
    runner->onSendMessage(msg);
}

void PyEnvironment::trackRunner(QString  runnerId, PyRunner *runner)
{
    m_runnersLock.lock();
    m_runners.insert(runnerId, runner);
    m_runnersLock.unlock();
}

void PyEnvironment::untrackRunner(QString runnerId)
{
    m_runnersLock.lock();
    m_runners.remove(runnerId);
    m_runnersLock.unlock();
}


bool PyEnvironment::getSkipFinalize() const
{
    return m_skipFinalize;
}

void PyEnvironment::setSkipFinalize(bool skipFinalize)
{
    m_skipFinalize = skipFinalize;
}

PyEnvironment::PyEnvironment()
{
    m_skipFinalize = false;
    m_initialized = false;
}

PyEnvironment &PyEnvironment::getInstance()
{
    static PyEnvironment instance;
    return instance;
}


bool PyEnvironment::start()
{
    QMutexLocker locker(&m_muxCounter);
    if(!m_initialized)
    {
        qDebug()<<QString::fromWCharArray( Py_GetPythonHome() );

        Py_Initialize();
        //PyEval_InitThreads(); //deprecated in python3.9, embeeded in Py_Initialize
        //PyEval_ReleaseLock(); //Release lock was deprecated, better to use the save state and restore in the stop()
        m_pPyThreadState = PyEval_SaveThread();
        m_initialized = true;
    }
    m_connectedRunners++;
    return m_initialized;
}


bool PyEnvironment::stop()
{
    QMutexLocker locker(&m_muxCounter);
    m_connectedRunners--;
    if (m_connectedRunners == 0) {
        try {
            //We need to restore thread state otherwise we have a crash
            PyEval_RestoreThread(m_pPyThreadState);
            //PyErr_Print();
            if(!m_skipFinalize)
            {
                //PyErr_Print();
                int result = Py_FinalizeEx();
                qDebug()<<"finalized ="<< result;

            }
            m_initialized = false;
        } catch (...) {
            qDebug() << "Got exception in " << Q_FUNC_INFO;
        }
    }

    return true;
}

