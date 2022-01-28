#ifndef PYCPPTESTER_H
#define PYCPPTESTER_H

#include <QObject>

class PyCppTester : public QObject
{
    Q_OBJECT
public:
    explicit PyCppTester(QObject *parent = nullptr);
    void execute();
protected slots:
    void onCallFinished(QString);
signals:

};

#endif // PYCPPTESTER_H
