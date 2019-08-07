#include <QCoreApplication>
#include "pyrunner.h"
#include "pyenvironment.h"
#include <QDebug>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    PyEnvironment::getInstance().start();

    QStringList params;
    params.append("param_1");
    params.append("param_2");
    params.append("param_3");
    params.append("param_4");

    //qDebug()<<"start iteration "<<i;

    for (int i=0; i<3;i++)
    {
        PyRunner * w = PyEnvironment::getInstance().getInstanceModule("/home/aru/Projects/PyQAC/PyQAC.py");
        w->syncCallFunction("init", QStringList());
        w->setParam("param_0", "192.168.23.242");
        w->start();
        sleep(5);
        w->stop();
        w->syncCallFunction("deinit", QStringList());
        qDebug()<<"test result:"<<w->getResult("result_0");
        w->deleteLater();
    }
    qDebug()<<"finished";

    return a.exec();
}
