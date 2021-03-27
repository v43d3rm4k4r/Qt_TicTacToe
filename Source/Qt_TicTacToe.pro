QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    ClientServer.cpp \
    ModeWindow.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    ClientServer.h \
    MainWindow.h \
    ModeWindow.h

FORMS += \
    MainWindow.ui

win32:RC_ICONS += ../images/TicTacToeLabel.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
