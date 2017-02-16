#-------------------------------------------------
#
# Project created by QtCreator 2016-10-17T17:22:34
#
#-------------------------------------------------

CONFIG += c++11
QMAKE_CXXFLAGS += -fno-strict-aliasing
INCLUDEPATH += Box2D/
INCLUDEPATH += simulator/
LIBS += -lBox2D -lpthread -pthread

QT       += core gui
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Simulator
TEMPLATE = app
OBJECTS_DIR = Build
MOC_DIR = Build
UI_DIR = Build

SOURCES += main.cpp \
    base/ientity.cpp \
    simulator/environment.cpp \
    simulator/agent.cpp \
    simulator/contactlistener.cpp \
    gui/mainwindow.cpp \
    simulator/configuration.cpp \
    threadclass.cpp \
    gui/environmentwidget.cpp \
    simulator/controllers/c1.cpp \
    simulator/controllers/icontroller.cpp \
    simulator/controllers/c2.cpp \
    simulator/controllers/c3.cpp \
    simulator/controllers/c4.cpp \
    simulator/wall.cpp \
    simulator/controllers/c5.cpp \
    Utils.cpp

HEADERS  += \
    base/ientity.h \
    simulator/environment.h \
    simulator/agent.h \
    simulator/contactlistener.h \
    gui/mainwindow.h \
    simulator/configuration.h \
    threadclass.h \
    gui/environmentwidget.h \
    simulator/controllers/c1.h \
    simulator/controllers/icontroller.h \
    simulator/controllers/c2.h \
    simulator/controllers/c3.h \
    simulator/controllers/c4.h \
    simulator/wall.h \
    simulator/controllers/c5.h \
    Utils.h

FORMS += \
    gui/mainwindow.ui

unix:!macx: LIBS += -L$$PWD/Box2D/Build/Box2D/ -lBox2D

INCLUDEPATH += $$PWD/Box2D/Build/Box2D
DEPENDPATH += $$PWD/Box2D/Build/Box2D

unix:!macx: PRE_TARGETDEPS += $$PWD/Box2D/Build/Box2D/libBox2D.a
