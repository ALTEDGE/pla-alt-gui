#-------------------------------------------------
#
# Project created by QtCreator 2018-05-15T11:48:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pla_GUI
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    wheeltab.cpp \
    thresholdsetter.cpp \
    serial.cpp \
    programtab.cpp \
    profiletab.cpp \
    profile.cpp \
    mainwindow.cpp \
    main.cpp \
    macrotab.cpp \
    macrorecorder.cpp \
    macro.cpp \
    keygrabber.cpp \
    colortab.cpp \
    key.cpp \
    input/controller.cpp \
    input/joystick.cpp \
    input/joysticktracker.cpp \
    input/primaryjoysticktracker.cpp \
    input/steeringtracker.cpp \
    wheelthresholdsetter.cpp

HEADERS += \
    colortab.h \
    config.h \
    editing.h \
    key.h \
    keygrabber.h \
    keysender.h \
    macro.h \
    macrorecorder.h \
    macrotab.h \
    mainwindow.h \
    profile.h \
    profiletab.h \
    programtab.h \
    savabletab.h \
    serial.h \
    thresholdsetter.h \
    traymessage.h \
    wheeltab.h \
    input/controller.h \
    input/joystick.h \
    input/joysticktracker.h \
    input/primaryjoysticktracker.h \
    input/steeringtracker.h \
    wheelthresholdsetter.h

INCLUDEPATH += input

FORMS +=

unix:!macx: LIBS += -lwwwidgets5 -lxdo -lSDL2main -lSDL2
unix:!macx: QMAKE_CXXFLAGS += -Wall -Wextra -Werror -pedantic
win32: LIBS += -lwwwidgets5 -lSDL2 -lSDL2main -luser32 -lSetupAPI
win32: RC_ICONS += icon.ico
win32: QMAKE_CXXFLAGS += /std:c++latest

DISTFILES += \
    ../build-Pla_GUI-Desktop-Debug/stylesheet

