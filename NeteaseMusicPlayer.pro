#-------------------------------------------------
#
# Project created by QtCreator 2017-12-07T11:07:24
#
#-------------------------------------------------

QT       += core gui
QT	+=network
QT += multimedia
CONFIG += openssl-linked
LIBS+=-lWininet

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NeteaseMusicPlayer
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
    main.cpp \
    mainwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    searchwindow.cpp \
    globalsetting.cpp \
    utilitytools.cpp \
    globalsetting.cpp \
    main.cpp \
    mainwindow.cpp \
    searchwindow.cpp \
    utilitytools.cpp \
    libQREncode/bitstream.c \
    libQREncode/mask.c \
    libQREncode/mmask.c \
    libQREncode/mqrspec.c \
    libQREncode/qrencode.c \
    libQREncode/qrinput.c \
    libQREncode/qrspec.c \
    libQREncode/rscode.c \
    libQREncode/split.c \
    lrcparser.cpp

HEADERS += \
        mainwindow.h \
    mainwindow.h \
    mainwindow.h \
    mainwindow.h \
    searchwindow.h \
    mystruct.h \
    globalsetting.h \
    utilitytools.h \
    libQREncode/bitstream.h \
    libQREncode/config.h \
    libQREncode/mask.h \
    libQREncode/mmask.h \
    libQREncode/mqrspec.h \
    libQREncode/qrencode.h \
    libQREncode/qrencode_inner.h \
    libQREncode/qrinput.h \
    libQREncode/qrspec.h \
    libQREncode/rscode.h \
    libQREncode/split.h \
    globalsetting.h \
    mainwindow.h \
    mystruct.h \
    searchwindow.h \
    utilitytools.h \
    lrcparser.h

FORMS += \
        mainwindow.ui \
    searchwindow.ui

RESOURCES += \
    Resources/imagereousrce.qrc

DEFINES +=HAVE_CONFIG_H 1
