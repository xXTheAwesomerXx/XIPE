#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T09:42:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XIPE
TEMPLATE = app
QT += network
QT += xml
RC_FILE = xipe.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    clustertab.cpp \
    variables.cpp \
    endusersclustermngmtmainwindow.cpp \
    endusersclustermngmttabswindow.cpp \
    uccxmigrationmainwindow.cpp \
    clustermngmtcustomlist.cpp \
    uccxtabbedwindow.cpp \
    uccxmigrationtab.cpp \
    uccxdefaultsdialog.cpp \
    uccxccgcustomlist.cpp \
    cmuserdevicemanagermainwindow.cpp \
    loginterfacedialog.cpp \
    cmuserdevicemanager.cpp

HEADERS  += mainwindow.h \
    clustertab.h \
    variables.h \
    endusersclustermngmtmainwindow.h \
    endusersclustermngmttabswindow.h \
    uccxmigrationmainwindow.h \
    clustermngmtcustomlist.h \
    uccxtabbedwindow.h \
    uccxmigrationtab.h \
    uccxdefaultsdialog.h \
    uccxccgcustomlist.h \
    cmuserdevicemanagermainwindow.h \
    loginterfacedialog.h \
    cmuserdevicemanager.h

FORMS    += mainwindow.ui \
    clustertab.ui \
    endusersclustermngmtmainwindow.ui \
    endusersclustermngmttabswindow.ui \
    uccxmigrationmainwindow.ui \
    uccxtabbedwindow.ui \
    uccxmigrationtab.ui \
    uccxdefaultsdialog.ui \
    cmuserdevicemanagermainwindow.ui \
    loginterfacedialog.ui \
    cmuserdevicemanager.ui \
    rtreportinginterface.ui

RESOURCES +=
