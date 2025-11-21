#ifndef PYCALLBACK_H
#define PYCALLBACK_H
#include <QString>>

class PyCallback
{
public:
    explicit PyCallback();
    virtual ~PyCallback();
    virtual void sendMessage(QString pyRunnerId, QString msg);

};

#endif // PYCALLBACK_H
