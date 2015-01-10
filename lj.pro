#-------------------------------------------------
#
# Project created by QtCreator 2014-10-16T22:51:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lj
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    opencv2qt.cpp \
    nclseg.cpp \
    analyse.cpp \
    feature.cpp \
    classification.cpp \
    svm.cpp \
    gram_schmidt_seg.cpp

HEADERS  += mainwindow.h \
    opencv2qt.h \
    nclseg.h \
    analyse.h \
    feature.h \
    classification.h \
    svm.h \
    gram_schmidt_seg.h

FORMS    += mainwindow.ui

RC_ICONS = microscope.ico

INCLUDEPATH += /usr/local/include \
                /usr/local/include/opencv \
                /usr/local/include/opencv2

LIBS += /usr/local/lib/libopencv_highgui.so \
        /usr/local/lib/libopencv_core.so    \
        /usr/local/lib/libopencv_imgproc.so
RESOURCES += \
    pic.qrc

