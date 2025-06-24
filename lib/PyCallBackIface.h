#ifndef PYCALLBACKIFACE_H
#define PYCALLBACKIFACE_H
#include <QString>
class PyCallBackIface
{
public:
    virtual void onStdOutput(QString log) = 0;
};
#endif // PYCALLBACKIFACE_H
