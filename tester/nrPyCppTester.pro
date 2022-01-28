QT -= gui

SOURCES = main.cpp \
    pycpptester.cpp

LIBS = -L$$PWD/../lib/last_build

INCLUDEPATH = $$PWD/../lib/last_build/include

CONFIG(debug, debug|release) {
    win32: LIBSUFFIX = "d"
    macos: LIBSUFFIX = "_debug"
    linux: LIBSUFFIX = "_d"
}
LIBS += -lnrPyCpp$${LIBSUFFIX}

HEADERS += \
    pycpptester.h
