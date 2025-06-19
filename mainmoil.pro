#-------------------------------------------------
#
# Qt_Mainmoil : Sample Qt Moil Application
#
#-------------------------------------------------

QT       += core gui sql network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mainmoil
TEMPLATE = app
# CONFIG += console

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
# DEFINES += QT_DEPRECATED_WARNINGS
# DEFINES += CUDA_ENABLED
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        mlabel.cpp \
        renderarea.cpp

# INCLUDEPATH += ../moildev/

win32 {
INCLUDEPATH += $$(OPENCV_SDK_DIR)/include

LIBS += -L$$PWD -lmoildev
LIBS += -L$$(OPENCV_SDK_DIR)/x86/mingw/lib \
        -lopencv_core320        \
        -lopencv_highgui320     \
        -lopencv_imgcodecs320   \
        -lopencv_imgproc320     \
        -lopencv_videoio320
}

unix {
# message ($$CONFIG)

contains(CONFIG, cross_compile):{
message ("cross compile")
INCLUDEPATH += $$(PKG_CONFIG_SYSROOT_DIR)/usr/include/opencv4
LIBS += -lmoildev
LIBS += -L$$(PKG_CONFIG_SYSROOT_DIR)/usr/lib64/
LIBS += -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lopencv_imgcodecs \
        -lopencv_videoio
}
else {
message ("no cross compile")
INCLUDEPATH += /usr/include/opencv4
# INCLUDEPATH += /usr/local/include/opencv4
LIBS += -lmoildev
# Set Qt Library location according to the actual situation
# LIBS += -L/usr/lib/x86_64-linux-gnu
# Qt 5.6.3
LIBS += -L~/Qt5.6.3/5.6.3/gcc_64/lib
LIBS += -L/usr/local/lib \
        -L/usr/lib \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lopencv_imgcodecs \
        -lopencv_videoio
}
}

HEADERS += \
        mainwindow.h \
        configdata.h \
        moildev.h \
        mlabel.h \
        renderarea.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    mainmoil.qrc

DISTFILES +=
