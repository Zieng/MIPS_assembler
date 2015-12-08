#-------------------------------------------------
#
# Project created by QtCreator 2015-06-15T08:41:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Assembler
TEMPLATE = app


SOURCES += main.cpp\
        startdialog.cpp \
    atobfile_dialog.cpp \
    btoafile_dialog.cpp \
    simulate_mainwindow.cpp

HEADERS  += startdialog.h \
    atobfile_dialog.h \
    btoafile_dialog.h \
    simulate_mainwindow.h

FORMS    += startdialog.ui \
    atobfile_dialog.ui \
    btoafile_dialog.ui \
    simulate_mainwindow.ui

