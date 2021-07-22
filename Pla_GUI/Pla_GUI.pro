#-------------------------------------------------
#
# Project created by QtCreator 2018-05-15T11:48:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PLA_ALT
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
win32: DEFINES += PLA_WINDOWS

SOURCES += \
    keysender.cpp \
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
    wheelthresholdsetter.h \
    runguard.h

INCLUDEPATH += input

unix:!macx: LIBS += -lwwwidgets5 -lxdo -lSDL2main -lSDL2
unix:!macx: QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
win32: LIBS += -L. -lwwwidgets5 -lSDL2 -lSDL2main -luser32 -lSetupAPI
win32: RC_ICONS += ..\assets\icon.ico
win32: QMAKE_CXXFLAGS += /std:c++latest
