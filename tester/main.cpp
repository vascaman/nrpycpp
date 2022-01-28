#include <QCoreApplication>


#include "PyRunnerQt.h"
#include "pycpptester.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    PyCppTester tester;
    tester.execute();

    return a.exec();
}
