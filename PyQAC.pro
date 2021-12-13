!include (depspath.pri) {
    error( "PyQAC Cannot find depspath.pri" )
}

VERSION = 1.0.4

QT -= gui
CONFIG += c++11 console
CONFIG -= app_bundle
CONFIG += debug_and_release

TEMPLATE = lib
win32{
    CONFIG -= dll
    CONFIG += shared static
}
OUTPUT_DIR = $${QMAKE_CXX}_qt-$${QT_VERSION}
CONFIG(release, debug|release){
    TARGET = release/$$OUTPUT_DIR/$$TARGET
    message("PyQAC release $$QT_VERSION")
}

CONFIG(debug, debug|release){
    TARGET = debug/$$OUTPUT_DIR/$$TARGET
    message("PyQAC debug $$QT_VERSION")
}

message("Output dir: $$TARGET")

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
#        main.cpp \
        pyenvironment.cpp \
        pyrunner.cpp


INCLUDEPATH += $${PYTHON_INCLUDE_PATH}
LIBS += -L$${PYTHON_LIB_PATH} -lpython$${PYTHON_VERSION}
message("Linking python lib version: $${PYTHON_VERSION}")
message("LIBS = $${LIBS}")

contains( PYTHON_VERSION, 3.9 ) {
    DEFINES *= PY_MAJOR_VERSION=3
}
contains( PYTHON_VERSION, 2.7 ) {
    DEFINES *= PY_MAJOR_VERSION=2
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    pyenvironment.h \
    pyrunner.h

DISTFILES += \
    PyQAC.py \
    PyQAC2.py \
    PyTest.py

