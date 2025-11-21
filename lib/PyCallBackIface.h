#ifndef PYCALLBACKIFACE_H
#define PYCALLBACKIFACE_H
#include <QString>
#include <QSharedPointer>
class PyCallBackIface
{
public:
    virtual void onStdOutput(const QSharedPointer<QString> & log) = 0;
    virtual void onException(const QSharedPointer<QString> & exceptionMsg) = 0;
    virtual void onSendMessage(const QSharedPointer<QString> & message) = 0;
};
#endif // PYCALLBACKIFACE_H
