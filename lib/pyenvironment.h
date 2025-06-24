#ifndef PYENVIRONMENT_H
#define PYENVIRONMENT_H

#include "pyrunner.h"
#include <QMutex>
#include <QMap>

/*struct PyEnvModule
{
    QString moduleName;
    QString modulePath;
    PyRunner * module;
};

Q_DECLARE_METATYPE(PyEnvModule)*/

class PyEnvironment
{
    bool m_initialized;
    bool m_skipFinalize;
    QMutex m_muxCounter;
    int m_connectedRunners = 0;
    PyThreadState *m_pPyThreadState = nullptr;

    PyEnvironment();
    PyEnvironment(PyEnvironment const& copy);
    PyEnvironment & operator = (PyEnvironment const&copy);
    QMutex m_runnersLock;
    QMap<QString, PyRunner*> m_runners;
public:
    static PyEnvironment &getInstance();
    bool start();
    bool stop();
    ~PyEnvironment();
    void trackRunner(QString runnerId, PyRunner* runner);
    void untrackRunner(QString runnerId);
    PyRunner *getRunner(QString runnerId);

    //PyRunner * getInstanceModule(QString modulePath, QStringList dependecies = QStringList());
    //void unloadModule(PyRunner* runner);
    bool getSkipFinalize() const;
    void setSkipFinalize(bool skipFinalize);

    void onStdOutputWriteCallBack(const char* s, QString runner_id);
    void onStdOutputFlushCallback(QString runner_id);
};

#endif // PYENVIRONMENT_H
