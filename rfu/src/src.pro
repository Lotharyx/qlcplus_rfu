include(../../variables.pri)

TEMPLATE = lib
LANGUAGE = C++
TARGET   = qlcplusrfu

CONFIG += qt
QT     += core script network
INCLUDEPATH     += ../../engine/src
DEPENDPATH      += ../../engine/src
QMAKE_LIBDIR    += ../../engine/src

LIBS += -lqlcplusengine

# qhttpserver files
HEADERS = \
    rfu.h \
    rfu_actor.h \
    rfu_messages.h \
    rfu_socket.h
SOURCES = \
    rfu.cpp \
    rfu_actor.cpp \
    rfu_messages.cpp \
    rfu_socket.cpp

target.path = $$INSTALLROOT/$$LIBSDIR
INSTALLS   += target
