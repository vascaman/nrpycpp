#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QDir>
#include <QVariantList>

#include "PyRunnerQt.h"

#ifndef WIN32
#include <unistd.h> //for sleep()
#endif

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

    //qDebug()<<"start iteration "<<i;
    //PyEnvironment::getInstance().start();
    for (int i=0; i<1; i++)
    {
        qDebug() << pythonFilePath;
        PyRunnerQt * w = new PyRunnerQt(pythonFilePath, dependencies);

        QString myparm = "ciccio";
        QVariantList vl;
        vl << myparm;
        qDebug() << "Calling upper with param: " << myparm;
        qDebug() << "test result:" << w->syncCallFunction("upper", vl);

        QVariantList twoparms;
        twoparms << "foo" << "bar";
        qDebug() << "Calling swapp with params: " << twoparms;
        qDebug() << "swapp result:" << w->syncCallFunction("swapp", twoparms);


        QVariantList threeparms;
        threeparms << "foo" << "bar" << "zoidberg";
        qDebug() << "Calling shiftR3 with params: " << threeparms;
        qDebug() << "swapp result:" << w->syncCallFunction("shiftR3", threeparms);

        QVariantList threeparmsVariant;
        threeparmsVariant << 1 << 2 << 3.14;
        qDebug() << "Calling shiftR3 with params: " << threeparmsVariant;
        qDebug() << "shiftR3 result:" << w->syncCallFunction("shiftR3", threeparmsVariant);



        twoparms.clear();
        twoparms << 1.4 << 41;
        qDebug() << "Calling sum with params: " << twoparms;
        qDebug() << "sum result:" << w->syncCallFunction("sum", twoparms);

        twoparms.clear();
        twoparms << true;
        qDebug() << "Calling negate with params: " << twoparms;
        qDebug() << "negate result:" << w->syncCallFunction("negate", twoparms);


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
    sleep(2);

    //PyEnvironment::getInstance().stop();
    qDebug() << "finished";

    return a.exec();
}
