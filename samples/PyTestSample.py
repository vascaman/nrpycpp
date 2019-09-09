import threading
import time
import subprocess

class TestParamName:
    src_agent_id = "src_agent_id"
    dst_agent_id = "dst_agent_id"
    agent_id = "agent_id"
    ip_to_bind = "ip_to_bind"
    peer_ip = "peer_ip"

    
class AgentInfo:
    def __init__(self):
        self.src_agent_id = -1
        self.dst_agent_id = -1
        self.agent_id = -1
        self.ip_to_bind = '0.0.0.0'
        self.peer_ip = '0.0.0.0'

    def isCaller(self):
        return self.agent_id == self.src_agent_id

    def isCallee(self):
        return self.agent_id == self.dst_agent_id


class TestParams:
    def __init__(self):
        self.param_0 = ''
        self.param_1 = ''
        self.param_2 = ''
        self.param_3 = ''
        self.callerParams = ''
        self.calleeParams = ''


class PyQAC(threading.Thread):
    def __init__(self, thread_id, threadName):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self.threadName = threadName
        self.looping = 0
        self.agentInfo = AgentInfo()
        self.testParams = TestParams()
        print "### Created thread: "+self.threadName+" id: "+str(self.thread_id)

    def setParam(self, paramName, paramValue):
        print "### Setting param "+paramName+" to value "+paramValue;

    def run(self):
        self.running = True
        self.counter = 0
        print '### Thread '+self.threadName+' started'
        role = "caller" if self.agentInfo.isCaller() else "callee"

        print('### self.testParams.callerParams', self.testParams.callerParams)
        print('### self.testParams.calleeParams', self.testParams.calleeParams)
        for key in self.testParams.callerParams:
            print('### key: %(key)s -> %(value)s' % {'key':key, 'value':self.testParams.callerParams[key]} )
        for key in self.testParams.calleeParams:
            print('### key: %(key)s -> %(value)s' % {'key':key, 'value':self.testParams.calleeParams[key]} )

        while(self.running):
            print '### Looping as the %(role)s' % {'role': role}
            time.sleep(1)

    def stop(self):
        print '### Thread '+self.threadName+' stopped'
        self.running = False

    def getResult(self, i_resultName):
        ## here we shoudl get the real result from the test object
        resultsMap = { 'result_0':1.1,
                       'result_1':2.2,
                       'result_2':3.3,
                       'result_3':4.4 }
        if i_resultName in resultsMap:
            return resultsMap[i_resultName]
        else
            return -1


class ModuleContext:
    def __init__(self):
        self.threadName = "PythonTestParamsThread"
        self.threadId = 1
        self.testThread = PyQAC(self.threadId, self.threadName)
        self.agentInfo = AgentInfo()
        self.testParams = TestParams()

    def setAgentInfo(self, i_agentInfo):
        self.agentInfo = i_agentInfo

# global object to store several objects we need
g_moduleContext = ''


def init():
    global g_moduleContext
    g_moduleContext = ModuleContext()
    print 'PY MODULE init()'


def setParam(paramName, paramValue):
    global g_moduleContext
    print("PY MODULE setParam() %(name)s -> %(value)s" % { "name":paramName, "value":paramValue })
    if(paramName==TestParamName.src_agent_id and len(paramValue)>0):
        g_moduleContext.agentInfo.src_agent_id = paramValue
    if (paramName == TestParamName.dst_agent_id and len(paramValue) > 0):
        g_moduleContext.agentInfo.dst_agent_id = paramValue
    if (paramName == TestParamName.agent_id and len(paramValue) > 0):
        g_moduleContext.agentInfo.agent_id = paramValue
    if (paramName == TestParamName.ip_to_bind and len(paramValue) > 0):
        g_moduleContext.agentInfo.ip_to_bind = paramValue
    if (paramName == TestParamName.peer_ip and len(paramValue) > 0):
        g_moduleContext.agentInfo.peer_ip = paramValue

    if(paramName=="param_0" and len(paramValue)>0):
        g_moduleContext.testParams.param_0 = paramValue
    if (paramName == "param_1" and len(paramValue) > 0):
        g_moduleContext.testParams.param_1 = paramValue
    if (paramName == "param_2" and len(paramValue) > 0):
        g_moduleContext.testParams.param_2 = paramValue
    if (paramName == "param_3" and len(paramValue) > 0):
        g_moduleContext.testParams.param_3 = paramValue


def start():
    global g_moduleContext
    print("PY MODULE start()")
    g_moduleContext.testThread.start()


def stop():
    global g_moduleContext
    print("PY MODULE stop()")
    g_moduleContext.testThread.stop()


def getResult(resultName):
    global g_moduleContext
    result = g_moduleContext.testThread.getResult(resultName)
    print("PY MODULE getResult() %(name)s --> %(value)s" %
                                {'name':resultName, 'value':result} )
    return result


def deinit():
    global g_moduleContext
    print("PY MODULE deinit()")
    g_moduleContext.testThread.join()
    g_moduleContext.testThread.terminate()
    

