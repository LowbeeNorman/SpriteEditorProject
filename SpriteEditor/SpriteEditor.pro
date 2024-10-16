QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    canvas.cpp \
    checkerboard.cpp \
    eraser.cpp \
    filehandler.cpp \
    frame.cpp \
    graphicslayer.cpp \
    main.cpp \
    mainview.cpp \
    model.cpp \
    paintbucket.cpp \
    pen.cpp \
    sampler.cpp \
    sizedialogbox.cpp \
    sprite.cpp \

HEADERS += \
    canvas.h \
    checkerboard.h \
    eraser.h \
    filehandler.h \
    frame.h \
    graphicslayer.h \
    mainview.h \
    model.h \
    paintbucket.h \
    pen.h \
    sampler.h \
    sizedialogbox.h \
    tool.h \
    sprite.h \

FORMS += \
    mainview.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    resources.qrc
