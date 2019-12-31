#-------------------------------------------------
#
# Project created by QtCreator 2019-12-27T22:07:46
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++1z

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kiyosVideoAnalysisTool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += src/qtWidgetsInterface/
INCLUDEPATH += src/qtWidgets/
INCLUDEPATH += src/ffmpegUtil/
INCLUDEPATH += thirdParty/

SOURCES += \
        src/qtWidgets/main.cpp \
        src/qtWidgets/mainWindow.cpp \
    src/qtWidgets/av_dump_format_form.cpp \
    src/qtWidgets/openMediaDialog.cpp \
    src/qtWidgetsInterface/avDumpFormat.cpp

HEADERS += \
        src/qtWidgets/mainWindow.h \
    src/qtWidgets/av_dump_format_form.h \
    src/qtWidgets/openMediaDialog.h \
    src/qtWidgetsInterface/avDumpFormat.h \
    src/ffmpegUtil/customExceptions.h \
    src/ffmpegUtil/ffmpegUPtr.h

FORMS += \
        src/qtWidgets/mainWindow.ui \
    src/qtWidgets/av_dump_format_form.ui \
    src/qtWidgets/openMediaDialog.ui

RESOURCES += \
    src/qtWidgets/icons/icons.qrc

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += libavcodec

unix: PKGCONFIG += libavdevice

unix: PKGCONFIG += libavdevice

unix: PKGCONFIG += libswscale

unix: PKGCONFIG += libavfilter

unix: PKGCONFIG += libavformat

#unix: PKGCONFIG += libavresample

unix: PKGCONFIG += libavutil

#unix: PKGCONFIG += libpostproc
