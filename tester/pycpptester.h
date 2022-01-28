#ifndef PYCPPTESTER_H
#define PYCPPTESTER_H

#include <QObject>

class PyRunnerQt;

class PyCppTester : public QObject
{
    Q_OBJECT
    PyRunnerQt *m_pPyRunner;
    QString m_pyscriptPath;

public:
    explicit PyCppTester(QString pyscriptfullpath, QObject *parent = nullptr);
    ~PyCppTester();
    void execute(QString func, QVariantList params, bool sync=true);
    void executeTestList();
protected slots:
    void onCallFinished(QString);
signals:

};

#endif // PYCPPTESTER_H
