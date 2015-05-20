#-------------------------------------------------
#
# Project created by QtCreator 2011-11-27T11:52:26
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FightDown
TEMPLATE = app

# in linux
#INCLUDEPATH += /home/pswz-zhangy/devel/programs/kinect/openni/Include/ \
#        /home/pswz-zhangy/devel/programs/kinect/nite/Include/
#
#LIBS += -lXnCore -lOpenNI

# in windows
LIBS += -L"D:/Program Files/OpenNI/Lib64/" -lopenNI64
INCLUDEPATH += "D:/Program Files/OpenNI/Include"
LIBS += -L"D:/Program Files/PrimeSense/NITE/Lib64/" -lXnVNITE64_1_5_2
INCLUDEPATH += "D:/Program Files/PrimeSense/NITE/Include"




SOURCES += main.cpp\
        mainwindow.cpp \
    screenmanager.cpp \
    boardmanager.cpp \
    badguymanager.cpp \
    playermanager.cpp \
    dartmanager.cpp \
    statemachine.cpp \
    board.cpp \
    character.cpp \
    player.cpp \
    badguy.cpp \
    dart.cpp \
    nuimanager.cpp \
    manager.cpp

HEADERS  += mainwindow.h \
    screenmanager.h \
    boardmanager.h \
    badguymanager.h \
    playermanager.h \
    dartmanager.h \
    statemachine.h \
    board.h \
    character.h \
    player.h \
    badguy.h \
    dart.h \
    nuimanager.h \
    manager.h

FORMS    += mainwindow.ui

RESOURCES += \
    pics.qrc














