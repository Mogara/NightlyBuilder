#-------------------------------------------------
#
# Project created by QtCreator 2015-04-06T23:01:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NightlyBuilder
TEMPLATE = app


SOURCES += main.cpp\
        maindialog.cpp \
    rundialog.cpp \
    statemanager.cpp \
    states/statepulling.cpp \
    states/stategeneratingbotversioncpp.cpp \
    states/state.cpp \
    states/stateqmaking.cpp \
    states/statemaking.cpp \
    states/statedeploying.cpp \
    states/statepackaging.cpp \
    states/stateuploading.cpp \
    globalconfig.cpp

HEADERS  += maindialog.h \
    rundialog.h \
    statemanager.h \
    states/statepulling.h \
    states/stategeneratingbotversioncpp.h \
    states/state.h \
    states/stateqmaking.h \
    states/statemaking.h \
    states/statedeploying.h \
    states/statepackaging.h \
    states/stateuploading.h \
    globalconfig.h
