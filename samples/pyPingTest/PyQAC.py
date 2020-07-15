import threading
import time
import subprocess

class PyQAC(threading.Thread):
    def __init__(self, thread_id, thread_name):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self.thread_name = thread_name
        self.looping = 0
        self.daemon = True
        self.pingCounter = 0
        self.totalPingTime = 0
        self.hostTarget = "8.8.8.8"
        print "created thread: "+self.thread_name+" id: "+str(self.thread_id)

    def run(self):
        self.looping=True
        print 'thread started'
        while(self.looping):
            print 'Looping'
            time.sleep(1)

    def setParam(self, paramName, paramValue):
        print "setting param "+paramName+" to value "+paramValue;

    def run(self):
        self.running = True
        self.counter = 0
        print 'thread '+self.thread_name+' started'
        while(self.running):
            time.sleep(1)
            self.counter+=1
            self.totalPingTime += self.getPingTime(self.hostTarget)
        print 'thread '+self.thread_name+' stopped'

    def getPingTime(self, hostname):
            response = subprocess.check_output("ping -c 1 " + hostname + " | grep 'time='", shell=True)
            tokens = response.split();
            timeMS = float(tokens[6].split("=")[1])
            print "ping to "+hostname+" = "+str(timeMS)+"MS"
            return timeMS

    def stop(self):
        print 'thread '+self.thread_name+' stopping soon...'
        self.running = False

    def getAvgPingTime(self):
        return self.totalPingTime/self.counter


myThread = type('PyQAC', (object,), {})()
# PyQAC(1, "PythonPingThread")
threadId = 0
def init():
    global myThread
    global threadId
    threadId+=1
    myThread = PyQAC(threadId, "PythonPingThread"+str(threadId))

def printCose(cose):
	result = "ciao"+cose
	print result
	return result

def start():
    global myThread
    myThread.start()

def stop():
    global myThread
    myThread.stop()

def checkError():
    return "";

def deinit():
    global myThread
    myThread.join()
    #myThread.terminate()

def setParam(paramName, paramValue):
    if(paramName=="param_0" and len(paramValue)>0):
        global myThread
        myThread.hostTarget=paramValue

def getResult(resultName):
    if(resultName=="result_0"):
        global myThread
        return myThread.getAvgPingTime()
    return ""
