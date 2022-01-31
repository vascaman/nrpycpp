#include "pycpptester.h"

#include <QDebug>
#include <QDir>
#include <QThread>
#include <QDateTime>

#include "PyRunnerQt.h"
#include "../lib/sleep_header.h"

PyCppTester::PyCppTester(QString i_pythonscriptfullpath, QObject *parent)
    : QObject{parent}
    , m_pyscriptPath((i_pythonscriptfullpath))
{

    QStringList dependencies;
//    dependencies.append("/home/aru/Projects/PyQAC/Dependency/");
//    dependencies.append("/home/aru/Projects/PyQAC/Dependency2/");
    //dependencies.append("/home/aru/Projects/PyQAC/PyDependencyImporter/");

    qDebug() << "Executing Python script:" << m_pyscriptPath;
    m_pPyRunner = new PyRunnerQt(m_pyscriptPath, dependencies);
    connect(m_pPyRunner, &PyRunnerQt::callCompletedSignal, this, &PyCppTester::onCallFinished);

}


PyCppTester::~PyCppTester()
{
    if (m_pPyRunner)
        delete m_pPyRunner;
}

#include <QTimer>
void PyCppTester::go()
{
    QTimer::singleShot(500, this, [this] {executeTestList();});
}

void PyCppTester::executeTestList()
{
    qDebug() << "Application thread " << QThread::currentThread();

    QVariantList paramlist;
    QString funcname;


    funcname = "upper";
    paramlist << "ciccio";
    execute(funcname, paramlist);

    funcname = "swapp";
    paramlist.clear();
    paramlist << "foo" << "bar";
    execute(funcname, paramlist);

    funcname = "shiftR3";
    paramlist.clear();
    paramlist << "foo" << "bar" << "zoidberg";
    execute(funcname, paramlist);

    funcname = "shiftR3";
    paramlist.clear();
    paramlist << 1 << false << 3.14;
    execute(funcname, paramlist);


    funcname = "lengthy_func";
    paramlist.clear();
    paramlist << 5;
    //execute(funcname, paramlist, false);

    funcname = "sum";
    paramlist.clear();
    paramlist << 1.4 << 41;
    execute(funcname, paramlist);

    funcname = "negate";
    paramlist.clear();
    paramlist << true;
    execute(funcname, paramlist);

    funcname = "lengthy_func";
    paramlist.clear();
    paramlist << 2;
    //execute(funcname, paramlist);

    funcname = "countbytes";
    paramlist.clear();
    QByteArray ba(1000 ,'a');
    paramlist << ba;
    execute(funcname, paramlist);

    funcname = "replbytes";
    paramlist.clear();
    QByteArray ba2(1000 ,'a');
    paramlist << ba2 << "a" << "x";
    execute(funcname, paramlist);

    sleep_for_millis(2000);

    qDebug() << "finished";
    ontestlistfinished();
}


void PyCppTester::execute(QString func, QVariantList params, bool sync)
{
    qDebug() << QDateTime::currentDateTime()<< " - Calling " << func << " with params: " << params;
    if (sync) {
        qDebug() << func << " result:" << m_pPyRunner->syncCallFunction(func, params);
    } else {
        qDebug() << func << " callID:" << m_pPyRunner->asyncCallFunction(func, params);
    }
}



void PyCppTester::onCallFinished(QString c)
{
    qDebug() << Q_FUNC_INFO << c << QDateTime::currentDateTime();
}

#include <QCoreApplication>
void PyCppTester::ontestlistfinished()
{
    qDebug () << "deleting runner...";
    delete m_pPyRunner;
    m_pPyRunner = nullptr;
    qDebug() << "calling app quit...";
    qApp->quit();
}

