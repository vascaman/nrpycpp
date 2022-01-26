#include "pyrunner.h"
#include <QDebug>
#include <QSharedPointer>



PyRunner::~PyRunner()
{
    /*
     * Thread will self destroy when the finished() signal arrives
    */

    tearDown();
    Py_DecRef(m_module_dict);
}

PyRunner::PyRunner(QString scriptPath, QStringList dependecies)
{
    m_errorCode = PyRunnerError_OK;
    m_syntaxError = false;
    m_sourceFilePy = scriptPath;
    m_dependencies = dependecies;
    m_module_dict = NULL;
    qRegisterMetaType<PyFunctionCall>("PyFunctionCall");

    m_py_thread = new QThread();
    m_py_thread->setObjectName("NrPyCpp-" + QFileInfo(scriptPath).completeBaseName() + QUuid::createUuid().toString());
    m_py_thread->start();

    connect(m_py_thread, &QThread::finished, m_py_thread, &QThread::deleteLater);

    connect(this, &PyRunner::startCallSignal, this, &PyRunner::startCallSlot);
    connect(this, &PyRunner::setupSignal, this, &PyRunner::setup);
    connect(this, &PyRunner::tearDownSignal, this, &PyRunner::tearDown);

    this->moveToThread(m_py_thread);
    emit(setupSignal());
}

void PyRunner::setup()
{
    try {
        QFileInfo scriptFileInfo(m_sourceFilePy);
        if(!scriptFileInfo.exists())
        {
            qDebug() << QString("[ERROR] Cannot find script: %1")
                            .arg(m_sourceFilePy);
            return;
        }

        m_scriptFileName = scriptFileInfo.completeBaseName();
        m_scriptFilePath = scriptFileInfo.dir().path();
    }
    catch(...)
    {
        /*
         *  who i sgenerating the exception?
         */
        qDebug() << "[ERROR] cannot setup pyrunner";
        PyErr_Print();
    }
}

void PyRunner::tearDown()
{
    unloadCurrentModule();
    m_py_thread->exit();
}

//TODO - this function probably belongs to atena agent and should be in a derived class (2022-01-25 FL)
void PyRunner::start()
{
    qDebug() << "pyrunner::start()";
    asyncCallFunction("start");
}


//TODO - this function probably belongs to atena agent and should be in a derived class (2022-01-25 FL)
void PyRunner::stop()
{
    qDebug() << "pyrunner::stop()";
    syncCallFunction("stop");
}

QString PyRunner::getResult(QString resultName)
{
    qDebug() << "pyrunner::getResult()";
    QString functionName = "getResult";

    QStringList params;
    params.append(resultName);
    QString result = syncCallFunction(functionName, params);
    qDebug() << QString("result for '%1' is %2").arg(resultName).arg(result);
    return  result;
}

PyGILState_STATE PyRunner::openCallContext()
{
    PyGILState_STATE gstate = PyGILState_Ensure();
    return gstate;
}

void PyRunner::closeCallContext(PyGILState_STATE state)
{
    PyGILState_Release(state);

    //not needed https://stackoverflow.com/questions/8451334/why-is-pygilstate-release-throwing-fatal-python-errors
    //PyEval_ReleaseLock();
}

void PyRunner::processCall(PyFunctionCall call)
{
     //TODO - I don't get this check, syntax_error is set only in the syncCallFunction
    // but after the process call is happening, so how can this enter the check? (2022-01-25 FL)
    if(m_syntaxError)
    {
        emit(callDidFinishedSlot(call));
        return;
    }

    try {

        PyGILState_STATE gstate = openCallContext();

        PyObject * py_lib_mod_dict = getModuleDict(); //borrowed reference of global variable
        if ( !py_lib_mod_dict ) {
            //FIXME - log error and return (2022-01-26 FL)
            qCritical() << "CRITICAL - cannot get Python module dictionary...";
        }
        Py_IncRef(py_lib_mod_dict);

        if(!py_lib_mod_dict)
        {
            call.error = true;
            call.errorMessage.append("PyQAC ERROR: Cannot find module at \"" + m_sourceFilePy+"\"!");
            //FIXME - should not we return an error? (2022-01-26 FL)
            qCritical() << "CRITICAL - cannot use Python module dictionary...";
        }

        char * p = new char[call.functionName.length() + 1];
        QSharedPointer<char> function = QSharedPointer<char>(p);
        strcpy(function.data(), call.functionName.toUtf8().constData());

        //get function name
        PyObject * py_function_name = NULL;

        //PyString_FromString
        if(!call.error)
            py_function_name = PyUnicode_FromString(function.data()); // this->PyStringFromString(call.functionName);// PyString_FromString(function);//new reference

        if(!py_function_name)
            call.error=true;

        //get function object
        PyObject * py_func = NULL;
        if(!call.error)
        {
            py_func = PyDict_GetItem(py_lib_mod_dict, py_function_name);//borrowed reference
            Py_IncRef(py_func);
        }

        if(!py_func){
            call.error = true;
            call.errorMessage.append("ERROR: cannot find python function named \"" + call.functionName+"\"!");
            qDebug() << call.errorMessage;
            //FIXME - shouldn't we return here? (2022-01-26 FL)
        }


        PyObject * py_args = getTupleParams(call.params);//borrowed reference

        PyObject * py_ret = NULL;

        if(!call.error)
            py_ret = PyObject_CallObject(py_func, py_args);//new reference

        if(!py_ret)
            call.error=true;

        if(!call.error)
        {
            call.returnValue = parseObject(py_ret);
            Py_DecRef(py_ret);
        } else {
            call.errorMessage.append("PyQAC ERROR : retrieving result from function named \""+call.functionName+"\"!");

        }

        Py_DecRef(py_lib_mod_dict);
        Py_DecRef(py_function_name);
        Py_DecRef(py_func);

        if(params.size())
            Py_DecRef(py_args);

        closeCallContext(gstate);
        emit(callDidFinishedSlot(call));
    } catch (...)
    {
        //qDebug() << e.what();
        PyErr_Print();
    }
}

QStringList PyRunner::getParams() const
{
    return m_params;
}

void PyRunner::setParams(const QStringList &params)
{
    m_params = params;

    QString stringParams;

    stringParams = "['"+params[0]+"'";

    for(int i=1; i<params.length(); i++)
    {
        stringParams += ",'"+params[i]+"'";
    }

    stringParams += "]";

    //qDebug()<<stringParams;

    syncCallFunction("setParams", params);
}

void PyRunner::setParam(QString paramName, QString paramValue)
{
    QString functionName = "setParam";

    QStringList params;
    params.append(paramName);
    params.append(paramValue);
    syncCallFunction(functionName, params);
}

void PyRunner::trackCall(PyFunctionCall call)
{
    m_callsMutex.lock();
    m_calls.insert(call.CallID, call);
    //printCalls();
    m_callsMutex.unlock();
}

void PyRunner::printCalls()
{
    //FIXME - we're accessing m_calss without mutex (2022-01-26 FL)
    foreach(PyFunctionCall call, m_calls.values())
    {
        qDebug()<< "Call Id: "<<call.CallID<<"; name: "<<call.functionName;
    }
}

PyFunctionCall PyRunner::getCall(QUuid callID)
{
    m_callsMutex.lock();
    PyFunctionCall returnValue = m_calls.value(callID);
    m_callsMutex.unlock();
    return returnValue;
}

void PyRunner::untrackCall(PyFunctionCall call)
{
    m_callsMutex.lock();
    m_calls.remove(call.CallID);
    //printCalls();
    m_callsMutex.unlock();
}

bool PyRunner::checkCall(QUuid callID)
{
    m_callsMutex.lock();
    bool returnValue = m_calls.contains(callID);
    m_callsMutex.unlock();
    return returnValue;
}

PyObject * PyRunner::getModuleDict()
{
    if(!m_module_dict)
    {
        loadCurrentModule();

        PyObject * scriptName = PyUnicode_FromString(m_scriptFileName.toUtf8().data()); //new reference

        //script meta data
        PyObject * m_py_lib_mod = PyImport_Import(scriptName); //new reference

        if(!m_py_lib_mod)
        {
            //qDebug()<<"problem";
            PyErr_Print();
            m_module_dict = NULL;
            return NULL;
        }

        m_module_dict = PyModule_GetDict(m_py_lib_mod);//borrowed reference

        Py_IncRef(m_module_dict);

        if(!m_module_dict)
        {
            //qDebug()<<"problem";
            PyErr_Print();
            m_module_dict = NULL;
            return NULL;
        }
//        printPyDict(m_module_dict);
        Py_DecRef(m_py_lib_mod);
        Py_DecRef(scriptName);
    }

    return  m_module_dict;
}

PyObject *PyRunner::getTupleParams(QStringList params)//borrowed reference (WHICH REF? FL)
{
    if(params.size()==0) {
        return NULL;
    }

    QString formatString = "(";
    for(int i=0; i<params.size(); i++)
    {
        formatString += "s";
    }
    formatString += ")";

    char * p = new char[formatString.length() + 1];
    QSharedPointer<char> formatChars = QSharedPointer<char>(p);
    strcpy(formatChars.data(), formatString.toUtf8().constData());

    PyErr_Print();//if you remove this, python >3 stops working

    PyObject * args;

    if(params.count()==1)
    {
        args = Py_BuildValue(formatChars.data(), qPrintable(params[0]));//new reference
    }
    else if(params.count()==2)
    {
        args = Py_BuildValue(formatChars.data(), qPrintable(params[0]), qPrintable(params[1]));//new reference
    }
    else
    {
        args = Py_BuildValue(formatChars.data());
        qDebug()<<"Problem parsing PYQAC args"; //FIXME we should throw or add an error return code from this call
    }

    return args;
}

void PyRunner::printPyDict(PyObject *dict)
{
    PyObject * keys = PyDict_Keys(dict);
    int size = static_cast<int>(PyList_Size(keys));
    qDebug()<< "Dict size "<<size;
    for (int i=0; i<size; i++)
    {
        PyObject * key = PyList_GetItem(keys, i);
        PyObject * value = PyDict_GetItem(dict, key); //Borrowed reference
        qDebug()<<"value"<<i<<parseObject(value);
    }
}

void PyRunner::printPyList(PyObject *list)
{
    int size = static_cast<int>(PyList_Size(list));
    qDebug()<< "list size "<<size;
    for (int i=0; i<size; i++)
    {
        PyObject * tupleValue = PyList_GetItem(list,i); //Borrowed reference
//        PyObject* objectsRepresentation = PyObject_Repr(tupleValue);//New reference
        qDebug()<<"value"<<i<<parseObject(tupleValue);
//        Py_DecRef(objectsRepresentation);
    }
}

void PyRunner::printPyTuple(PyObject *tuple)
{
    int tupleSize = static_cast<int>(PyTuple_Size(tuple));
    qDebug()<< "tuple size "<<tupleSize;
    for (int i=0; i<tupleSize; i++)
    {
        PyObject * tupleValue = PyTuple_GetItem(tuple,i); //Borrowed reference
        PyObject* objectsRepresentation = PyObject_Repr(tupleValue);//New reference
        const char* s = PyByteArray_AsString(objectsRepresentation);
        Py_DecRef(objectsRepresentation);
    }
}

QString PyRunner::parseObject(PyObject *object)
{
    PyTypeObject* type = object->ob_type;
    const char* p = type->tp_name;

    QString returnValue = "";

    if(QString("str").compare(p)==0)
    {
        PyObject* objectsRepresentation = PyObject_Repr(object);//new reference

        /* Python 3.9 ONLY code - we dismissed Python 2.7 */
        PyObject* str = PyUnicode_AsEncodedString(objectsRepresentation, "utf-8", "~E~");//new reference
        const char* s = PyBytes_AsString(str);
#if(0)
        Py_DecRef(str);
        PyErr_Print();
#endif
        returnValue = QString(s);
        returnValue = returnValue.mid(1,returnValue.length()-2);
        Py_DecRef(str);
        PyErr_Print();
        Py_DecRef(objectsRepresentation);
    }
    else if(QString("int").compare(p)==0)
    {
        PyObject * strObject = PyObject_Str(object);//new reference
        returnValue = parseObject(strObject);
        Py_DecRef(strObject);
    }
    else if(QString("float").compare(p)==0)
    {
        PyObject * strObject = PyObject_Str(object);//new reference
        returnValue = parseObject(strObject);
        Py_DecRef(strObject);
    }
    else if(QString("double").compare(p)==0)
    {
        PyObject * strObject = PyObject_Str(object);//new reference
        returnValue = parseObject(strObject);
        Py_DecRef(strObject);
    }
    else if(QString("NoneType").compare(p)==0)
    {
        //do nothing
    }
    else
    {
//        qDebug()<<"PyQAC Warning: attemp to parse unknown type"<<p;
        PyObject * strObject = PyObject_Str(object);//new reference
        returnValue = parseObject(strObject);
        Py_DecRef(strObject);
    }

    return returnValue;
}

void PyRunner::loadCurrentModule()
{
    PyObject* sys = PyImport_ImportModule( "sys" ); //new reference
    PyObject* sys_path = PyObject_GetAttrString( sys, "path" ); //new reference
    PyObject* folder_path = PyUnicode_FromString( m_scriptFilePath.toUtf8().data() ); //new reference

    //add script path to python search path
    PyList_Append( sys_path, folder_path );

    //add dependecies path to python search path
    foreach (QString dependecy, m_dependencies)
    {
        PyObject* dependency_path = PyUnicode_FromString(dependecy.toUtf8().data());//new reference
        PyList_Append( sys_path, dependency_path);
        Py_DecRef(dependency_path);
    }

    //printPyList(sys_path);

    Py_DecRef(sys);
    Py_DecRef(sys_path);
    Py_DecRef(folder_path);
}

void PyRunner::unloadCurrentModule()
{
    PyGILState_STATE gstate = PyGILState_Ensure();
    //PyEval_InitThreads();

    //unload imported paths
    QString unloadPathCommand = "sys.path.remove(\""+m_scriptFilePath+"\")";
    QString unloadModuleCommand = "del sys.modules[\""+m_scriptFileName+"\"]";
    PyRun_SimpleString("import sys");
    PyRun_SimpleString(unloadPathCommand.toUtf8().data());
    PyRun_SimpleString(unloadModuleCommand.toUtf8().data());

    foreach (QString dependency, m_dependencies)
    {
        unloadPathCommand = "sys.path.remove(\""+dependency+"\")";
        PyRun_SimpleString(unloadPathCommand.toUtf8().data());
    }

    PyGILState_Release(gstate);
//    PyEval_ReleaseLock();
}

void PyRunner::getReturnValues()
{

}

QString PyRunner::syncCallFunction(QString functionName, QStringList params)
{
    PyFunctionCall call;
    call.CallID = QUuid::createUuid();
    call.synch = true;
    call.functionName = functionName;
    call.params = params;
    call.error = false;

    this->moveToThread(m_py_thread);

    emit(startCallSignal(call));

    while (!checkCall(call.CallID))
    {
#ifdef WIN32
         _sleep(1000);
#else
        usleep(1);
#endif
    }

    call = getCall(call.CallID);

    untrackCall(call);

    if(call.error)
    {
        m_syntaxError = true;
        m_errorCode = PyRunnerError_SYNTAX_ERROR;
        m_errorString = "Syntax error";
        m_errorMessage = call.errorMessage;
        return call.errorMessage;
    }

    return call.returnValue;
}

int PyRunner::getErrorCode()
{
    return m_errorCode;
}

QString PyRunner::getErrorString()
{
    return m_errorString;
}

QString PyRunner::getErrorMessage()
{
    return m_errorMessage;
}


void PyRunner::checkError()
{
    if(!m_syntaxError)
        m_errorCode = syncCallFunction("getErrorCode").toInt();

    if(!m_syntaxError)
        m_errorMessage = syncCallFunction("getErrorMsg");

    if(m_errorCode!=0 && !m_syntaxError)
        m_errorString = m_scriptFileName+" reported error";
}

void PyRunner::asyncCallFunction(QString functionName, QStringList params)
{
    PyFunctionCall call;
    call.CallID = QUuid::createUuid();
    call.synch = false;
    call.functionName = functionName;
    call.params = params;
    call.error = false;

    this->moveToThread(m_py_thread);
    emit(startCallSignal(call));
}

void PyRunner::startCallSlot(PyFunctionCall call)
{
    try {
        processCall(call);
    } catch (...)
    {
        // log errors
    }

}

void PyRunner::callDidFinishedSlot(PyFunctionCall call)
{
    if(call.synch)
    {
        trackCall(call);
    }
}
