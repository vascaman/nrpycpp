import threading
import time


class TestThread(threading.Thread):
    # thread_id     an interger to identify the thread
    # thread_name   a thread name
    def __init__(self, thread_id, thread_name):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self.thread_name = thread_name
        self.status = 'idle'
        self.running = False
        self.counter = 0

    def __print(self):
        print 'status : '+self.status
        print 'running : '+str(self.running)
        print 'counter : '+str(self.counter)

    def run(self):
        self.status = 'running'
        self.running = True
        self.counter = 0
        print 'thread started'
        return getPingTime("8.8.8.8");
        while(self.running or self.counter<4):
            print 'Status : '+self.status
            print 'Counter : '+str(self.counter)
            time.sleep(1)
            self.counter+=1

    def getPingTime(self, hostname):
        #hostname = "google.com" #example
        response = subprocess.check_output("ping -c 1 " + hostname + " | grep 'time='", shell=True)
        tokens = response.split();
        timeMS = tokens[7].split("=")[1]
        return timeMS

    def stop(self):
        print('stopping')
        self.status = 'stopped'
        self.running = False

t = TestThread(1, "Test Thread")

def stop():
    global t
    t.stop()
    print 'count '+str(t.counter)
    print 'status '+t.status
    #t.join()

def start():
    global t
    t.start()

def printStatus():
    global t
    print "*******************************"
    print "thread name : " + t.thread_name
    print "status : " + t.status
    print "*******************************"

def printCose():
    print"printocose";
#start()
#time.sleep(5)
#stop()
#printCose()


