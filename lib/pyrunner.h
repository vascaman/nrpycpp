/**************************************************************************
 *  Copyright (C) 2022 by NetResults S.r.l. ( http://www.netresults.it )  *
 *  Author( s ) :                                                         *
 *         Stefano Aru                  <s.aru@netresults.it>             *
 *         Francesco Lamonica      <f.lamonica@netresults.it>             *
 **************************************************************************/

#ifndef PYRUNNER_H
#define PYRUNNER_H
#include <QObject>
#include <QString>
#include <QThread>
#include <QMutex>
#include <QUuid>
#include <QMap>
#include <QVariantList>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTimer>

#pragma push_macro("slots")
#undef slots
#include <Python.h>
#pragma pop_macro("slots")

struct PyFunctionCall
{
    QUuid CallID;
    QString functionName;
    bool synch;
    QVariant returnValue;
    QVariantList params;
    bool error;
    QString errorMessage;
};

Q_DECLARE_METATYPE(PyFunctionCall)

enum  PyRunnerError
{
    PyRunnerError_OK = 0,//no error
    PyRunnerError_SYNTAX_ERROR = 3001,
    PyRunnerError_SEMANTIC_ERROR = 3002
};

class PyRunner : public QObject
{
    Q_OBJECT

    QStringList m_dependencies;
    QString m_sourceFilePy;
    QString m_scriptFileName;
    QString m_scriptFilePath;
    QThread * m_py_thread;
    void processCall(PyFunctionCall call);

    QStringList m_params;
    QMap<QUuid, PyFunctionCall> m_calls;
    QMutex m_callsMutex;
    void trackCall(PyFunctionCall call);
    PyFunctionCall getCall(QUuid callID);
    void untrackCall(PyFunctionCall call);
    bool checkCall(QUuid callID);

    void loadCurrentModule();
    void unloadCurrentModule();
    PyObject * getModuleDict();
    PyObject * m_module_dict;

    void setup();

    /*Function context call*/
    PyGILState_STATE openCallContext();
    void closeCallContext(PyGILState_STATE state);

    QVariant parseObject(PyObject * object);
    PyObject * getTupleParams(QVariantList paramList);

    /*debug utils*/
    void printPyTuple(PyObject* tuple);
    void printCalls();
    void printPyList(PyObject *list);
    void printPyDict(PyObject *list);

    //errors
    int m_errorCode;
    bool m_syntaxError;
    QString m_errorString;
    QString m_errorMessage;

public:
    PyRunner(QString scriptPath, QStringList dependencies=QStringList());
    ~PyRunner();

    //START_WRAPPER_METHODS
    //changing status
    //void start();
    //void stop();

    //settings params
    QStringList getParams() const;
    //void setParams(const QStringList &params);
    //void setParam(QString paramName, QString paramValue);

    //getting results
    //QString getResult(QString resultName);
    //void getReturnValues();

    //calling custom functions
    QString asyncCallFunction(QString functionName, QVariantList params = QVariantList());
    QVariant syncCallFunction(QString functionName, QVariantList params = QVariantList());

    //checking for errors
    int getErrorCode();
    QString getErrorString();
    QString getErrorMessage();

    //void checkError();
    //END_WRAPPER_METHODS
private:
    void handleCompletedCall(PyFunctionCall call);

signals:
    //void setupSignal();
    void tearDownSignal();
    void startCallRequestedSignal(PyFunctionCall call);
    //START_SIGNAL_METHODS
    void callCompletedSignal(QString);//PyFunctionCall call);
    //END_SIGNAL_METHODS

private slots:
    void tearDown();
    void onStartCallRequest(PyFunctionCall call);
};

#endif // PYRUNNER_H
