#include "pycpptester.h"

#include <QDebug>
#include <QDir>
#include <QThread>

#include "PyRunnerQt.h"
#include "../lib/sleep_header.h"

PyCppTester::PyCppTester(QObject *parent)
    : QObject{parent}
{

}

void PyCppTester::execute()
{

    QString samplespath = QDir::currentPath() + "/../samples/";
    qDebug() << "Using samples path: " << samplespath;

    QString pythonFilePath = samplespath + "PyTest.py";

    QStringList dependencies;
//    dependencies.append("/home/aru/Projects/PyQAC/Dependency/");
//    dependencies.append("/home/aru/Projects/PyQAC/Dependency2/");
    //dependencies.append("/home/aru/Projects/PyQAC/PyDependencyImporter/");

    QStringList params;
    params.append("param_1");
    params.append("param_2");
    params.append("param_3");
    params.append("param_4");

    qDebug() << "Application thread " << QThread::currentThread();
    for (int i=0; i<1; i++)
    {
        qDebug() << "Executing Python script:" << pythonFilePath;
        PyRunnerQt * w = new PyRunnerQt(pythonFilePath, dependencies);

        connect(w, &PyRunnerQt::callCompletedSignal, this, &PyCppTester::onCallFinished);
        QVariantList paramlist;
        QString funcname;

        QString myparm = "ciccio";
        funcname = "upper";
        paramlist << myparm;
        qDebug() << "Calling " << funcname << " with params: " << paramlist;
        qDebug() << funcname << " result:" << w->syncCallFunction(funcname, paramlist);

        funcname = "swapp";
        paramlist.clear();
        paramlist << "foo" << "bar";
        qDebug() << "Calling " << funcname << " with params: " << paramlist;
        qDebug() << funcname << " result:" << w->syncCallFunction(funcname, paramlist);


        funcname = "shiftR3";
        paramlist.clear();
        paramlist << "foo" << "bar" << "zoidberg";
        qDebug() << "Calling " << funcname << " with params: " << paramlist;
        qDebug() << funcname << " result:" << w->syncCallFunction(funcname, paramlist);

        funcname = "shiftR3";
        paramlist.clear();
        paramlist << 1 << false << 3.14;
        qDebug() << "Calling shiftR3 with params: " << paramlist;
        qDebug() << "shiftR3 result:" << w->syncCallFunction("shiftR3", paramlist);

        funcname = "lengthy_func";
        paramlist.clear();
        paramlist << 5;
        qDebug() << "Calling " << funcname << " with params: " << paramlist;
        qDebug() << funcname << " result:" << w->syncCallFunction(funcname, paramlist);

        funcname = "sum";
        paramlist.clear();
        paramlist << 1.4 << 41;
        qDebug() << "Calling " << funcname << " with params: " << paramlist;
        qDebug() << funcname << " result:" << w->syncCallFunction(funcname, paramlist);

        funcname = "negate";
        paramlist.clear();
        paramlist << true;
        qDebug() << "Calling " << funcname << " with params: " << paramlist;
        qDebug() << funcname << " result:" << w->syncCallFunction(funcname, paramlist);


        paramlist.clear();
        paramlist << 1;
        qDebug() << "Calling ASYNC lengthy_func with params: " << paramlist;
        qDebug() << "lenghty_func result:" << w->asyncCallFunction("lengthy_func", paramlist);


        funcname = "countbytes";
        paramlist.clear();
        QByteArray ba(1000 ,'a');
        paramlist << ba;
        qDebug() << "Calling " << funcname << " with params: " << paramlist;
        qDebug() << funcname << " result:" << w->syncCallFunction(funcname, paramlist);

        funcname = "incbytes";
        paramlist.clear();
        QByteArray ba2(1000 ,'a');
        paramlist << ba2 << 2;
        qDebug() << "Calling " << funcname << " with params: " << paramlist;
        qDebug() << funcname << " result:" << w->syncCallFunction(funcname, paramlist);


        qDebug() << "errorcode:"    << w->getErrorCode();
        qDebug() << "errorString:"  << w->getErrorString();
        qDebug() << "errorMessage:" << w->getErrorMessage();
        //delete (w);
    }
    sleep_for_millis(2000);

    //PyEnvironment::getInstance().stop();
    qDebug() << "finished";
}


void PyCppTester::onCallFinished(QString c)
{
    qDebug() << Q_FUNC_INFO << c;
}
