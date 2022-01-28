#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QDir>
#include <QVariantList>
#include <QThread>

#include "PyRunnerQt.h"

#include "../lib/sleep_header.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

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

        QString myparm = "ciccio";
        QVariantList paramlist;
        paramlist << myparm;
        qDebug() << "Calling upper with param: " << myparm;
        qDebug() << "test result:" << w->syncCallFunction("upper", paramlist);

        paramlist.clear();
        paramlist << "foo" << "bar";
        qDebug() << "Calling swapp with params: " << paramlist;
        qDebug() << "swapp result:" << w->syncCallFunction("swapp", paramlist);


        paramlist.clear();
        paramlist << "foo" << "bar" << "zoidberg";
        qDebug() << "Calling shiftR3 with params: " << paramlist;
        qDebug() << "shiftR3 result:" << w->syncCallFunction("shiftR3", paramlist);

        paramlist.clear();
        paramlist << 1 << false << 3.14;
        qDebug() << "Calling shiftR3 with params: " << paramlist;
        qDebug() << "shiftR3 result:" << w->syncCallFunction("shiftR3", paramlist);

        paramlist.clear();
        paramlist << 5;
        qDebug() << "Calling lengthy_func with params: " << paramlist;
        qDebug() << "lenghty_func result:" << w->syncCallFunction("lengthy_func", paramlist);

        paramlist.clear();
        paramlist << 1.4 << 41;
        qDebug() << "Calling sum with params: " << paramlist;
        qDebug() << "sum result:" << w->syncCallFunction("sum", paramlist);

        paramlist.clear();
        paramlist << true;
        qDebug() << "Calling negate with params: " << paramlist;
        qDebug() << "negate result:" << w->syncCallFunction("negate", paramlist);


        /* maybe needed for iQAC test(?)
        w->setParam("param_0", "8.8.8.8");
        w->start();
        sleep(5);
        w->stop();
        qDebug()<<"test result:"<<w->getResult("result_0").toUtf8().data();
        */

        //w->syncCallFunction("deinit", QStringList());
        //qDebug() << "checkError:"   << w->checkError();
        qDebug() << "errorcode:"    << w->getErrorCode();
        qDebug() << "errorString:"  << w->getErrorString();
        qDebug() << "errorMessage:" << w->getErrorMessage();
        delete (w);
    }
    sleep_for_millis(2000);

    //PyEnvironment::getInstance().stop();
    qDebug() << "finished";

    return a.exec();
}
