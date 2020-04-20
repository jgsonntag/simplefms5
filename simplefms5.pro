#-------------------------------------------------
#
# Project created by QtCreator 2018-03-30T16:03:26
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simplefms5
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    genericclient.cpp \
    setupgui.cpp \
    utility.cpp

HEADERS  += mainwindow.h \
    genericclient.h \
    setupgui.h \
    constants.h \
    utility.h

FORMS    += mainwindow.ui
