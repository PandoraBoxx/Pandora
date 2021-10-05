#-------------------------------------------------
#
# Project created by QtCreator 2021-09-16T13:55:33
#
#-------------------------------------------------

QT       += core gui serialport sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pandora
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
    serialSettings.cpp \
    mainWindow.cpp \
    encryptTool.cpp \
    contactManager.cpp \
    serialInterface.cpp \
    messageManager.cpp \
    progressBar.cpp \
    soundManager.cpp \
    contactKeyboard.cpp \
    aboutSettings.cpp

HEADERS += \
    serialSettings.h \
    mainWindow.h \
    encryptTool.h \
    contactManager.h \
    serialInterface.h \
    messageManager.h \
    progressBar.h \
    soundManager.h \
    contactKeyboard.h \
    aboutSettings.h

FORMS += \
    mainWindow.ui

LIBS += -L/usr/local/openssl/lib -lssl -lcrypto

RESOURCES += \
    applogo.qrc
