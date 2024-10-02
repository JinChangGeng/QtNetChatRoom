#-------------------------------------------------
#
# Project created by QtCreator 2024-09-20T16:57:58
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = tcpClient
TEMPLATE = app
RC_FILE += icon_config.rc

SOURCES += main.cpp\
        widget.cpp \
    chatroom.cpp \
    reg.cpp \
    history.cpp

HEADERS  += widget.h \
    chatroom.h \
    reg.h \
    history.h

FORMS    += widget.ui \
    chatroom.ui \
    reg.ui \
    history.ui

RESOURCES += \
    res.qrc

DISTFILES += \
    icon_config.rc
