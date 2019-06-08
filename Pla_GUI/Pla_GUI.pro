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
        main.cpp \
        mainwindow.cpp \
    thresholdsetter.cpp \
    colortab.cpp \
    wheeltab.cpp \
    macrotab.cpp \
    keygrabber.cpp \
    controllerroutine.cpp \
    joysticktracker.cpp \
    key.cpp \
    serial.cpp \
    macro.cpp \
    programtab.cpp \
    macrorecorder.cpp \
    profiletab.cpp \
    profile.cpp

HEADERS += \
        mainwindow.h \
    thresholdsetter.h \
    colortab.h \
    wheeltab.h \
    macrotab.h \
    keygrabber.h \
    controllerroutine.h \
    joysticktracker.h \
    key.h \
    serial.h \
    macro.h \
    programtab.h \
    macrorecorder.h \
    savabletab.h \
    editing.h \
    profiletab.h \
    profile.h

FORMS +=

unix:!macx: LIBS += -lwwwidgets5 -lxdo -lSDL2main -lSDL2
win32: LIBS += -lwwwidgets5 -lSDL2 -lSDL2main -luser32 -lSetupAPI
win32: RC_ICONS += icon.ico

DISTFILES += \
    ../build-Pla_GUI-Desktop-Debug/stylesheet
