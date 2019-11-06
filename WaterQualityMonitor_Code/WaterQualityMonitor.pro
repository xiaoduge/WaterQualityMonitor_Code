#-------------------------------------------------
#
# Project created by QtCreator 2019-11-05T08:34:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += serialport

TARGET = WaterQualityMonitor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

#Disable debug when relase
Release:DEFINES += QT_NO_DEBUG_OUTPUT

RC_ICONS = app.ico

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TRANSLATIONS = cn.ts
TRANSLATIONS += en.ts

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dcommandgenerator.cpp \
    dhexcmd.cpp \
    dhintdialog.cpp \
    dwarningdialog.cpp \
    ddialogtitle.cpp

HEADERS += \
        mainwindow.h \
    dcommandgenerator.h \
    dhexcmd.h \
    dhintdialog.h \
    dwarningdialog.h \
    ddialogtitle.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    res.qrc
