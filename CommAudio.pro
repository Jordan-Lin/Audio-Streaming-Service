#-------------------------------------------------
#
# Project created by QtCreator 2017-03-30T19:31:08
#
#-------------------------------------------------

QT       += core gui\
            multimedia\
            multimedia widgets\
            widgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = CommAudio
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
        calldialogue.cpp \
        updatehandler.cpp \
        fileselectdialogue.cpp \
        utilities.cpp \
        debugwindow.cpp \
        songstreamer.cpp \
        client.cpp \
        server.cpp \
        clienthandler.cpp \
        clientmanager.cpp \
        song.cpp \
        songmanager.cpp \
        songqueue.cpp \
        songstreamer.cpp \
        songstreamreceiver.cpp \
        usermanager.cpp \
        user.cpp \
    audiomanager.cpp

HEADERS  += mainwindow.h \
        calldialogue.h \
        defines.h \
        packets.h \
        updatehandler.h \
        fileselectdialogue.h \
        utilities.h \
        routinestructs.h \
        debugwindow.h \
        songstreamer.h\
        client.h \
        server.h \
        clienthandler.h \
        clientmanager.h \
        lockedmap.h \
        lockedqueue.h \
        packets.h \
        song.h \
        songmanager.h \
        songqueue.h \
        songstreamer.h \
        songstreamreceiver.h \
        usermanager.h \
        user.h \
    audiomanager.h

FORMS    += mainwindow.ui \
        calldialogue.ui \
        fileselectdialogue.ui \
        debugwindow.ui
