QT       += core gui
QT       += charts
QT       += network
QT       += xlsx
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
RC_ICONS = logo.ico

RESOURCES += \
    res.qrc \
    res.qrc

SOURCES += \
    calibclient.cpp \
    clientsettingwindow.cpp \
    clientsimupage.cpp \
    cpcstatuspage.cpp \
    datacomparemodel.cpp \
    datacomparewindow.cpp \
    main.cpp \
    mainstartup.cpp \
    mainwindow.cpp \
    md19client.cpp \
    measureserver.cpp \
    midesettingpage.cpp \
    settingpage.cpp \
    smpsstatuspage.cpp \
    testdevpage.cpp

HEADERS += \
    calibclient.h \
    clientsettingwindow.h \
    clientsimupage.h \
    cpcstatuspage.h \
    datacomparemodel.h \
    datacomparewindow.h \
    mainstartup.h \
    mainwindow.h \
    md19client.h \
    measureserver.h \
    midesettingpage.h \
    settingpage.h \
    smpsstatuspage.h \
    testdevpage.h

FORMS += \
    clientsettingwindow.ui \
    clientsimupage.ui \
    cpcstatuspage.ui \
    datacomparewindow.ui \
    mainstartup.ui \
    mainwindow.ui \
    midesettingpage.ui \
    settingpage.ui \
    smpsstatuspage.ui \
    testdevpage.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
