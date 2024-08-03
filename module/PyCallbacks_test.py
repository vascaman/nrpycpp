import os
import sys
from threading import Thread
import gc

currentDir = os.path.dirname(os.path.abspath(__file__))
sys.path.append("build")#search path for NrPyCallbacks.so
sys.path.append("deps/Pyside/linux64_Ubuntu_20.04/PySide-qt6.5-python3.10/package_for_wheels") #search path for pyside modules
print(os.environ['LD_LIBRARY_PATH'])
try:
	print("Importing NrPyCallbacks")
	from  NrPyCallbacks import *
	print("Import DONE!")
except Exception as e:
	currentDir = os.path.dirname(os.path.abspath(__file__))
	print("Failed importing NrPyCallbacks")
	print("Try setting proper LD_LIBRARY_PATH:\n")
	print("export LD_LIBRARY_PATH="+os.path.abspath(currentDir+"/../lib/last_build")+":PATH_TO_YOUR_QT_LIBS_FOLDER\n"  )
	sys.exit(str(e))

runnerId = "ciao"

def setParam(param, value):
	global runnerId
	runnerId = value
	print("setting param: "+ param+ ", value: "+ runnerId)

def setDelegate(delegate):
	delegate.sendMessage("DELEGATE SET")

def init():
	print(" -- py init")

def start():
	print(" -- Calling callback")
	_list = []
	for i in range(0,10000):
		_list.append("blablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablabla")
	c = PyCallback()
	c.sendMessage(runnerId,"ciao")
	#print(" -- Called callback")
	#print("gc.is_tracked(\"list\")<<",gc.is_tracked({"_list": []}))
	#print("gc.is_finalized(list)<<",gc.is_finalized(_list))
	del _list
	#print("gc.is_tracked(\"list\")<<",gc.is_tracked({"_list": []}))
	#print("gc.is_finalized(list)<<",gc.is_finalized(_list))
	

def stop():
	print(" -- py stop")

def deinit():
	print(" -- py deinit")
	#print(" gc.isenabled()<<", gc.isenabled())
	#print("gc.is_tracked(\"list\")<<",gc.is_tracked({"_list": []}))
	gc.collect()
	#print("gc.is_tracked(\"list\")<<",gc.is_tracked({"_list": []}))
	#print("gc.is_finalized(list)<<",gc.is_finalized(_list))
	

def getErrorCode():
	print(" -- py getErrorCode")
	return 0

def getErrorMsg():
	print(" -- py getErrorMsg")
	return ""

def main():
	#start()
	print(" -- Done")
	
	


if __name__ == '__main__':
    main()