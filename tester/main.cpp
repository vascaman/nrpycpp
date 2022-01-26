#include <QCoreApplication>
#include "pyrunner.h"
#include "pyenvironment.h"
#include <QDebug>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


//    PyEnvironment::getInstance().start();


//    QString pythonFilePath = "/home/stefano/Projects/pyqac/samples/PyTest.py";

    QString pythonFilePath = "/home/stefano/Projects/pyqac/samples/pyPingTest/PyQAC.py";
//    pythonFilePath = "/home/stefano/Projects/pyqac-bundles/PyQACPing/PyQACPing.py";
//    pythonFilePath = "/home/stefano/Projects/pyqac-bundles/PyQACPing/test%1/mytest.py";

    QStringList dependencies;
//    dependencies.append("/home/aru/Projects/PyQAC/Dependency/");
//    dependencies.append("/home/aru/Projects/PyQAC/Dependency2/");
    //dependencies.append("/home/aru/Projects/PyQAC/PyDependencyImporter/");

    QStringList params;
    params.append("param_1");
    params.append("param_2");
    params.append("param_3");
    params.append("param_4");

    //qDebug()<<"start iteration "<<i;
PyEnvironment::getInstance().start();
    for (int i=0; i<1;i++)
    {
        qDebug()<<pythonFilePath;
        PyRunner * w = PyEnvironment::getInstance().getInstanceModule(pythonFilePath, dependencies);
//        w->syncCallFunction("printCose", QStringList("cose"));
        qDebug()<<"test result:"<<w->syncCallFunction("printCose", QStringList("cose"));
        w->syncCallFunction("init", QStringList());
        w->setParam("param_0", "8.8.8.8");
        w->start();
        sleep(5);
        w->stop();

        qDebug()<<"test result:"<<w->getResult("result_0").toUtf8().data();

        w->syncCallFunction("deinit", QStringList());
        qDebug()<<"checkError:"<<w->checkError();
        qDebug()<<"errorcode:"<<w->getErrorCode();
        qDebug()<<"errorString:"<<w->getErrorString();
        qDebug()<<"errorMessage:"<<w->getErrorMessage();
        delete (w);

//        PyEnvironment::getInstance().stop();
    }
    sleep(2);

    PyEnvironment::getInstance().stop();
    qDebug()<<"finished";

//    Py_Finalize();

    return a.exec();
}
