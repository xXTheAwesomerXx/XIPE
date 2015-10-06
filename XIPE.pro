#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T09:42:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XIPE
TEMPLATE = app
LIBS += -LC:/OpenSSL-Win32/lib -lubsec
INCLUDEPATH += C:/OpenSSL-Win32/include
QT += network
QT += xml

SOURCES += main.cpp\
        mainwindow.cpp \
    clustertab.cpp \
    variables.cpp \
    findusersmngmt.cpp \
    endusersclustermngmtmainwindow.cpp \
    endusersclustermngmttabswindow.cpp \
    uccxmigrationmainwindow.cpp \
    clustermngmtcustomlist.cpp \
    uccxtabbedwindow.cpp \
    uccxmigrationtab.cpp \
    uccxdefaultsdialog.cpp \
    uccxccgcustomlist.cpp \
    cmuserdevicemanagermainwindow.cpp \
    loginterfacedialog.cpp

HEADERS  += mainwindow.h \
    clustertab.h \
    variables.h \
    findusersmngmt.h \
    endusersclustermngmtmainwindow.h \
    endusersclustermngmttabswindow.h \
    uccxmigrationmainwindow.h \
    clustermngmtcustomlist.h \
    uccxtabbedwindow.h \
    uccxmigrationtab.h \
    uccxdefaultsdialog.h \
    uccxccgcustomlist.h \
    cmuserdevicemanagermainwindow.h \
    loginterfacedialog.h

FORMS    += mainwindow.ui \
    clustertab.ui \
    findusersmngmt.ui \
    endusersclustermngmtmainwindow.ui \
    endusersclustermngmttabswindow.ui \
    uccxmigrationmainwindow.ui \
    uccxtabbedwindow.ui \
    uccxmigrationtab.ui \
    uccxdefaultsdialog.ui \
    cmuserdevicemanagermainwindow.ui \
    loginterfacedialog.ui
