#include "pyrunner.h"

#include <QDebug>
#include <QSharedPointer>
#include <QVariant>

#include "sleep_header.h"

#define NRPYQT_DEBUG1

#ifdef NRPYQT_DEBUG
#define PRINT_THREAD_INFO qDebug() << Q_FUNC_INFO << QThread::currentThread();
#else
#define PRINT_THREAD_INFO
#endif

PyRunner::~PyRunner()
{
    /*
     * Thread will self destroy when the finished() signal arrives
    */

    PRINT_THREAD_INFO
    tearDown();
    Py_DecRef(m_module_dict);
}

PyRunner::PyRunner(QString scriptPath, QStringList dependecies)
{
    PRINT_THREAD_INFO
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
    //connect(this, &PyRunner::setupSignal, this, &PyRunner::setup);
    connect(this, &PyRunner::tearDownSignal, this, &PyRunner::tearDown);

    this->moveToThread(m_py_thread);
    qDebug() << "after move to thread";
    PRINT_THREAD_INFO

    //The call to emit in the ctor makes no sense, especially with parenthesis that is not an emit but a function call
    //emit(setupSignal());
    //let's call stup directly
    setup();
}

void PyRunner::setup()
{
    PRINT_THREAD_INFO
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
         *  who is generating the exception?
         */
        qCritical() << "[ERROR] cannot setup pyrunner";
        PyErr_Print();
    }
}

void PyRunner::tearDown()
{
    PRINT_THREAD_INFO
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

//TODO - this function probably belongs to atena agent and should be in a derived class (2022-01-25 FL)
QString PyRunner::getResult(QString resultName)
{
    qDebug() << "pyrunner::getResult()";
    QString functionName = "getResult";

    QVariantList params;
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
    PRINT_THREAD_INFO
     //TODO - I don't get this check, syntax_error is set only in the syncCallFunction
    // but after the process call is happening, so how can this enter the check? (2022-01-25 FL)
    if(m_syntaxError)
    {
        emit(callDidFinishedSlot(call)); //FIXME - this is NOT a signal (2022-01-25 FL)
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

        QVariantList vl;
        //foreach (QString s, call.params)
        //    vl << s;
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
            call.errorMessage.append("NrPyCpp ERROR : retrieving result from function named \"" + call.functionName + "\"!");

        }

        Py_DecRef(py_lib_mod_dict);
        Py_DecRef(py_function_name);
        Py_DecRef(py_func);

        if(params.size()) //FIXME - params is a member variable that is never used (WTF?) (2022-01-27 FL)
            Py_DecRef(py_args);

        closeCallContext(gstate);
        emit(callDidFinishedSlot(call)); //FIXME - this is NOT a signal (2022-01-25 FL)
    } catch (...)
    {
        //qDebug() << e.what();
        PyErr_Print();
    }
}


//this is specific to atena i believe
QStringList PyRunner::getParams() const
{
    return m_params;
}

//this is specific to atena i believe
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
    QVariantList paramsvl;
    foreach (QString s, params)
        paramsvl << s;

    syncCallFunction("setParams", paramsvl);
}


//this is specific to atena i believe
void PyRunner::setParam(QString paramName, QString paramValue)
{
    QString functionName = "setParam";

    QStringList params;
    params.append(paramName);
    params.append(paramValue);

    QVariantList paramsvl;
    foreach (QString s, params)
        paramsvl << s;
    syncCallFunction(functionName, paramsvl);
}

void PyRunner::trackCall(PyFunctionCall call)
{
    PRINT_THREAD_INFO
    m_callsMutex.lock();
    m_calls.insert(call.CallID, call);
    //printCalls();
    m_callsMutex.unlock();
}

void PyRunner::printCalls()
{
    //FIXME - we're accessing m_calls without mutex (2022-01-26 FL)
    foreach(PyFunctionCall call, m_calls.values())
    {
        qDebug()<< "Call Id: "<<call.CallID<<"; name: "<<call.functionName;
    }
}

PyFunctionCall PyRunner::getCall(QUuid callID)
{
    PRINT_THREAD_INFO
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

/*!
 * \internal
 * \brief PyRunner::checkCall verifies if the returning call has the callID requested
 * \param callID
 * \return true if the result arrived, false otherwise
 * \note this is executed on the application thread (it is called from the syncCallFunction())
 */
bool PyRunner::checkCall(QUuid callID)
{
    PRINT_THREAD_INFO
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


QString getPythonParamTypeString(QVariant v)
{
    QString rettype; //empty is invalid string
    if (v.type() == QVariant::String) {
        return "s";
    } else if (v.type() == QVariant::Int || v.type() == QVariant::UInt
                || v.type() == QVariant::LongLong || v.type() == QVariant::ULongLong) {
        return "l";
    }

    return rettype;
}


PyObject* generatePyObjectPtrFromArg(QVariant v)
{
    if (v.type() == QVariant::String) {
        return Py_BuildValue("s", v.toString().constData());
    } else if (v.type() == QVariant::Int || v.type() == QVariant::UInt
                || v.type() == QVariant::LongLong || v.type() == QVariant::ULongLong) {
        return Py_BuildValue("l", v.toLongLong());
    }
    return nullptr;
}


PyObject *PyRunner::getTupleParams(QVariantList params)//borrowed reference (WHICH REF? FL)
{
    PRINT_THREAD_INFO
    if(params.size()==0) {
        return NULL;
    }

    QString formatString = "(";
    foreach (QVariant v, params)
    {
        QString pyvartype = getPythonParamTypeString(v);
        formatString += pyvartype;
    }
    formatString += ")";

    char * p = new char[formatString.length() + 1];
    QSharedPointer<char> formatChars = QSharedPointer<char>(p); //FIXME - why the shared pointer? it is not used anywhere
    strcpy(formatChars.data(), formatString.toUtf8().constData());

    PyErr_Print();//if you remove this, python >3 stops working

    PyObject *tup = PyTuple_New(params.size());
    for (int i = 0; i < params.size(); i++) {
        if (params[i].type() == QVariant::String) {
            PyTuple_SET_ITEM(tup, i, PyUnicode_FromString(params[i].toString().toStdString().c_str()));
        } else if (params[i].type() == QVariant::Int || params[i].type() == QVariant::LongLong) {
           PyTuple_SET_ITEM(tup, i, PyLong_FromLong(params[i].toLongLong()));
        } else if (params[i].type() == QVariant::UInt || params[i].type() == QVariant::ULongLong) {
            PyTuple_SET_ITEM(tup, i, PyLong_FromLong(params[i].toULongLong()));
        } else if (params[i].type() == QVariant::Double) {
            PyTuple_SET_ITEM(tup, i, PyFloat_FromDouble(params[i].toDouble()));
        } else if (params[i].type() == QVariant::Bool) {
            PyTuple_SET_ITEM(tup, i, PyBool_FromLong(params[i].toInt())); //FIXME - this conversion is flaky
        }
        // Note that PyTuple_SET_ITEM steals the reference we get from PyLong_FromLong.
    }
    return tup;
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
    PRINT_THREAD_INFO
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


//this is specific to atena i believe
void PyRunner::getReturnValues()
{

}

QString PyRunner::syncCallFunction(QString functionName, QVariantList params)
{
    PRINT_THREAD_INFO
    PyFunctionCall call;
    call.CallID = QUuid::createUuid();
    call.synch = true;
    call.functionName = functionName;
    call.params = params;
    call.error = false;

    //The following move to thread makes no sense, we already did that in ctor
    this->moveToThread(m_py_thread);

    emit startCallSignal(call);

    while (!checkCall(call.CallID))
    {
        sleep_for_millis(1000);
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
        m_errorString = m_scriptFileName + " reported error";
}

void PyRunner::asyncCallFunction(QString functionName, QStringList params)
{
    PRINT_THREAD_INFO
    PyFunctionCall call;
    call.CallID = QUuid::createUuid();
    call.synch = false;
    call.functionName = functionName;
    QVariantList paramsvl;
    foreach (QString s, params)
        paramsvl << s;
    call.params = paramsvl;
    call.error = false;

    this->moveToThread(m_py_thread);
    qDebug() << "after move to thread";
    PRINT_THREAD_INFO
    emit(startCallSignal(call));
}

void PyRunner::startCallSlot(PyFunctionCall call)
{
    PRINT_THREAD_INFO
    try {
        processCall(call);
    } catch (...)
    {
        // log errors
    }

}

void PyRunner::callDidFinishedSlot(PyFunctionCall call)
{
    PRINT_THREAD_INFO
    if(call.synch)
    {
        trackCall(call);
    }
}
