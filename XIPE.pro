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
    findusersmainwindow.cpp \
    findusersmngmt.cpp \
    endusersclustermngmtmainwindow.cpp \
    endusersclustermngmttabswindow.cpp \
    uccxmigrationmainwindow.cpp \
    clustermngmtcustomlist.cpp

HEADERS  += mainwindow.h \
    clustertab.h \
    variables.h \
    findusersmainwindow.h \
    findusersmngmt.h \
    endusersclustermngmtmainwindow.h \
    endusersclustermngmttabswindow.h \
    uccxmigrationmainwindow.h \
    clustermngmtcustomlist.h

FORMS    += mainwindow.ui \
    clustertab.ui \
    findusersmainwindow.ui \
    findusersmngmt.ui \
    endusersclustermngmtmainwindow.ui \
    endusersclustermngmttabswindow.ui \
    uccxmigrationmainwindow.ui
