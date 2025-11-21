import json
import os
import sys

script_dir = os.path.dirname(os.path.abspath(__file__))
json_error = "Unable to find value from json configuration file"

options = []

# option, function, error, description
options.append(("--lib-name",
                lambda: get_lib_name(),
                json_error,
                "Print module lib name"))
                
options.append(("--lib-include-dir",
                lambda: get_lib_include_dir(),
                json_error,
                "Print source lib include dir"))

options.append(("--lib-dir",
                lambda: get_lib_dir(),
                json_error,
                "Print source lib dir"))
                
options.append(("--lib-bin",
                lambda: get_lib_bin(),
                json_error,
                "Print source lib bin"))
#python

options.append(("--python-interpeter",
                lambda: get_python_interpeter(),
                json_error,
                "Print python intepeter"))

options.append(("--python-include-dir",
                lambda: get_python_include_dir(),
                json_error,
                "Print lib python include dir"))
#qt

options.append(("--qt-include-dir",
                lambda: get_qt_include_dir(),
                json_error,
                "Print Qt include dir"))

options.append(("--qt-lib-dir",
                lambda: get_qt_lib_dir(),
                json_error,
                "Print lib Qt lib dir"))

#pyside

options.append(("--pyside-header",
                lambda: get_pyside_header(),
                json_error,
                "Print pyside binding hearder file"))

options.append(("--pyside-typesystem-file",
                lambda: get_pyside_typesystem_file(),
                json_error,
                "Print pyside binding typesystem file"))

options.append(("--pyside-typesystems-path",
                lambda: get_pyside_typesystem_dir(),
                json_error,
                "Print pyside native typesystem dir"))

options.append(("--pyside-include-dir",
                lambda: get_pyside_include_dir(),
                json_error,
                "Print pyside shared lib include dir"))

options.append(("--pyside-shared-lib",
                lambda: get_pyside_shared_lib(),
                json_error,
                "Print pyside shared lib path"))                

#shiboken

options.append(("--shiboken-module-path",
                lambda: get_shiboken_module_path(),
                json_error,
                "Print shiboken python module path"))

options.append(("--shiboken-include-dir",
                lambda: get_shiboken_include_dir(),
                json_error,
                "Print shiboken shared lib include dir"))

options.append(("--shiboken-shared-lib",
                lambda: get_shiboken_shared_dir(),
                json_error,
                "Print shiboken shared lib path"))

options.append(("--shiboken-generator-path",
                lambda: get_shiboken_generator_path(),
                json_error,
                "Print shiboken generator path"))
                
options.append(("--shiboken-bin",
                lambda: get_shiboken_bin(),
                json_error,
                "Print shiboken generator bin"))

def get_value(key):
	with open(script_dir+'/config.json') as f:
		data = json.load(f)
	f.close()
	return data[key]

#source lib

def get_absolute_path(path):
	if os.path.isabs(path):
		return path
	
	current_path = os.path.dirname(os.path.realpath(__file__))
	absolute_path = os.path.join(current_path, path)
	absolute_path = os.path.abspath(absolute_path)
	return absolute_path

def get_lib_name():
	return get_value("source_lib")["lib_name"]

def get_lib_include_dir():
	lib_include_dir = get_value("source_lib")["lib_include_dir"]
	return get_absolute_path(lib_include_dir)

def get_lib_dir():
	lib_dir = get_value("source_lib")["lib_dir"]
	return get_absolute_path(lib_dir)

def get_lib_bin():
	return get_value("source_lib")["lib_bin"]

#python

def get_python_interpeter():
	return get_value("python")["python_interpeter"]

def get_python_include_dir():
	return get_value("python")["python_include_dir"]

#qt

def get_qt_include_dir():
	qt_include_dir = get_value("qt")["qt_include_dir"]
	return get_absolute_path(qt_include_dir)
    

def get_qt_lib_dir():
	qt_lib_dir = get_value("qt")["qt_lib_dir"]
	return get_absolute_path(qt_lib_dir)

#pyside

def get_pyside_header():
	return get_value("pyside_config")["pyside_header"]

def get_pyside_typesystem_file():
	return get_value("pyside_config")["pyside_typesystem_file"]

def get_pyside_typesystem_dir():
	pyside_typesystems_path = get_value("pyside_config")["pyside_typesystems_path"]
	return get_absolute_path(pyside_typesystems_path)

def get_pyside_include_dir():
	pyside_include_dir = get_value("pyside_config")["pyside_include_dir"]
	return get_absolute_path(pyside_include_dir)

def get_pyside_shared_lib():
	pyside_shared_lib = get_value("pyside_config")["pyside_shared_lib"]
	return get_absolute_path(pyside_shared_lib)

#shiboken

def get_shiboken_module_path():
	shiboken_module_path = get_value("shiboken")["shiboken_module_path"]
	return get_absolute_path(shiboken_module_path)

def get_shiboken_include_dir():
	shiboken_include_dir = get_value("shiboken")["shiboken_include_dir"]
	return get_absolute_path(shiboken_include_dir)

def get_shiboken_shared_dir():
	shiboken_shared_lib = get_value("shiboken")["shiboken_shared_lib"]
	return get_absolute_path(shiboken_shared_lib)

def get_shiboken_generator_path():
	shiboken_generator_path = get_value("shiboken")["shiboken_generator_path"]
	return get_absolute_path(shiboken_generator_path)

def get_shiboken_bin():
	return get_value("shiboken")["shiboken_bin"]	


path = "/home/netresults.wintranet/aru/Qt/6.5.0/gcc_64/include"
rel_path = "./deps/vdk/linux64_Ubuntu_20.04/vdk_qt6"

#print(get_absolute_path(path))
#print(get_absolute_path(rel_path))
#sys.exit(0)
options_usage = ''
for i, (flag, _, _, description) in enumerate(options):
    options_usage += '    '+flag.ljust(45)+" "+description
    if i < len(options) - 1:
        options_usage += '\n'

usage = """
Utility to determine include/link options of shiboken/PySide and Python for qmake/CMake projects
that would like to embed or build custom shiboken/PySide bindings.

Usage: pyside_config.py [option]
Options:
%s
    -a                                            Print all options and their values
    --help/-h                                     Print this help
"""

option = sys.argv[1] if len(sys.argv) == 2 else '-a'
if option == '-h' or option == '--help':
	print(usage % options_usage)
	sys.exit(0)

print_all = option == "-a"
for argument, handler, error, _ in options:
	if option == argument or print_all:
		handler_result = handler()
		if handler_result is None:
		    sys.exit(error)

		line = handler_result
		if print_all:
		    line = argument.ljust(40)+line
		print(line)
