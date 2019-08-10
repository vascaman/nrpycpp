#
#   PyTestSample.py
#   (c) 2019 NetResults Srl
#
#   Sample py code on how to use the python module API
#   to write a simple external target test
#


import threading
import time

import logging
from logging.handlers import RotatingFileHandler

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
    def __init__(self, thread_id, threadName, logger):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self.threadName = threadName
        self.logger = logger
        self.looping = 0
        self.agentInfo = AgentInfo()
        self.testParams = TestParams()
        self.logger.info("Created thread %(name)s with id %(id)d" % {'name':self.threadName, 'id':self.thread_id})

    def run(self):
        self.running = True
        self.counter = 0
        self.logger.info("Thread %(name)s started" % {'name':self.threadName})
        while(self.running):
            self.logger.info("looping")
            time.sleep(1)

    def stop(self):
        self.logger.info("Thread %(name) stopped" % {'name':self.threadName})
        self.running = False

    def getResult(self, i_resultName):
        ## here we shoudl get the real result from the test object
        resultsMap = { 'result_0':1.1,
                       'result_1':2.2,
                       'result_2':3.3,
                       'result_3':4.4 }
        if i_resultName in resultsMap:
            return resultsMap[i_resultName]
        else:
            return str(-1)


class ModuleContext:
    def __init__(self):
        self.threadName = "PythonTestParamsThread"
        self.threadId = 1
        logging.basicConfig(filename=self.threadName + ".log",
                            level=logging.INFO,
                            format='[%(asctime)s][%(name)s] [%(levelname)s] %(message)s')
        self.logger = logging.getLogger(self.threadName)
        #logRotationHandler = RotatingFileHandler(maxBytes=1024*1024, backupCount=3)
        #self.logger.addHandler(logRotationHandler)
        self.testThread = PyQAC(self.threadId, self.threadName, self.logger)
        self.agentInfo = AgentInfo()
        self.testParams = TestParams()
        # start logging
        self.logger.info("Created PY MODULE Context")

# global object to store several objects we need
g_moduleContext = ModuleContext()


def init():
    global g_moduleContext
    g_moduleContext.logger.info("PY MODULE init()")


def setParam(paramName, paramValue):
    global g_moduleContext
    g_moduleContext.logger.info("PY MODULE setParam() %(name)s -> %(value)s" % { "name":paramName, "value":paramValue })
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
    g_moduleContext.logger.info("PY MODULE start()")
    g_moduleContext.testThread.start()


def stop():
    global g_moduleContext
    g_moduleContext.logger.info("PY MODULE stop()")
    g_moduleContext.testThread.stop()


def getResult(resultName):
    global g_moduleContext
    result = g_moduleContext.testThread.getResult(resultName)
    g_moduleContext.logger.info("PY MODULE getResult() %(name)s --> %(value)s" %
                                {'name':resultName, 'value':result} )
    return result


def deinit():
    global g_moduleContext
    g_moduleContext.logger.info("PY MODULE deinit()")
    g_moduleContext.testThread.join()
    g_moduleContext.testThread.terminate()

