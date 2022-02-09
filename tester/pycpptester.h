#ifndef PYCPPTESTER_H
#define PYCPPTESTER_H

#include <QObject>

class PyRunnerQt;

class PyCppTester : public QObject
{
    Q_OBJECT
    PyRunnerQt *m_pPyRunner;
    QString m_pyscriptPath;

    void quitIfThereAreNoMoreCallPending();
public:
    explicit PyCppTester(QString pyscriptfullpath, QObject *parent = nullptr);
    ~PyCppTester();
    void execute(QString func, QVariantList params, bool sync=true);
    void go();
protected slots:
    void executeTestList();
    void onCallFinished(QString);
    void ontestlistfinished();
    void onCloseAppRequest();
signals:
    void testlistfinished();
    void closeAppRequested();
};

#endif // PYCPPTESTER_H
