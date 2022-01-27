QT -= gui

SOURCES = main.cpp

LIBS = -L$$PWD/../lib/last_build

INCLUDEPATH = $$PWD/../lib/last_build/include

#INCLUDEPATH += /usr/include/python3.10

CONFIG(debug, debug|release) {
    win32: LIBSUFFIX = "d"
    macos: LIBSUFFIX = "_debug"
    linux: LIBSUFFIX = "_d"
}
LIBS += -lnrPyCpp$${LIBSUFFIX}
