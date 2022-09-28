QT       += core gui serialport printsupport datavisualization

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets datavisualization

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

SOURCES += \
    app2.cpp \
    app2_constblock.cpp \
    bootloader.cpp \
    main.cpp \
    mainwindow.cpp \
    plot2d.cpp \
    plot3d.cpp \
    plot3dconfigs.cpp \
    qcustomplot.cpp \
    systemcontrol.cpp \

HEADERS += \
    app2.h \
    app2_constblock.h \
    bootloader.h \
    global_config.h \
    mainwindow.h \
    plot2d.h \
    plot3d.h \
    plot3dconfigs.h \
    qcustomplot.h \
    systemcontrol.h

FORMS += \
    app2.ui \
    app2_constblock.ui \
    bootloader.ui \
    mainwindow.ui \
    plot3dconfigs.ui \
    systemcontrol.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
