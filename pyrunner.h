#ifndef PYRUNNER_H
#define PYRUNNER_H
#include <QObject>
#include <QString>
#include <QThread>
#include <QMutex>
#include <QUuid>
#include <QMap>

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTimer>

#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")

struct PyQACCall
{
    QUuid CallID;
    QString functionName;
    bool synch;
    QString returnValue;
    QStringList params;
    bool error;
    QString errorMessage;
};

Q_DECLARE_METATYPE(PyQACCall)

class PyRunner : public QObject
{
    Q_OBJECT

    QStringList m_dependecies;
    QString m_sourceFilePy;
    QString m_scriptFileName;
    QString m_scriptFilePath;
    QThread * m_py_thread;
    QStringList params;
    void processCall(PyQACCall call);

    QStringList m_params;
    QMap<QUuid, PyQACCall> m_calls;
    QMutex m_callsMutex;
    void trackCall(PyQACCall call);
    PyQACCall getCall(QUuid callID);
    void untrackCall(PyQACCall call);
    bool checkCall(QUuid callID);

    void loadCurrentModule();
    void unloadCurrentModule();
    PyObject * getModuleDict();
    PyObject * m_module_dict;


    /*Function context call*/
    PyGILState_STATE openCallContext();
    void closeCallContext(PyGILState_STATE state);


    PyObject * getTupleParams(QStringList params);
    /*debug utils*/
    void printPyTuple(PyObject* tuple);

    QString parseObject(PyObject * object);

public:
    PyRunner(QString scriptPath, QStringList dependecies=QStringList());
    ~PyRunner();

    //changing status
    void start();
    void stop();

    //settings params
    QStringList getParams() const;
    void setParams(const QStringList &params);
    void setParam(QString paramName, QString paramValue);

    //getting results
    QString getResult(QString resultName);
    void getReturnValues();

    //calling custom functions
    void asyncCallFunction(QString functionName, QStringList params);
    QString syncCallFunction(QString functionName, QStringList params);

private:
    void printCalls();

signals:
    void setupSignal();
    void tearDownSignal();
    void startCallSignal(PyQACCall call);
    void callDidFinishedSignal(PyQACCall call);

private slots:
    void setup();
    void tearDown();
    void startCallSlot(PyQACCall call);
    void callDidFinishedSlot(PyQACCall call);
};

#endif // PYRUNNER_H
