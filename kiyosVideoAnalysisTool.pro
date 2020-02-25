#-------------------------------------------------
#
# Project created by QtCreator 2019-12-27T22:07:46
#
#-------------------------------------------------

QT       += core gui multimedia printsupport
CONFIG   += c++17

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

INCLUDEPATH += src/
INCLUDEPATH += src/qtWidgetsInterface/
INCLUDEPATH += src/qtWidgets/
INCLUDEPATH += src/ffmpegUtil/
INCLUDEPATH += src/playerEngine/
INCLUDEPATH += thirdParty/
INCLUDEPATH += thirdParty/mySpsc/
INCLUDEPATH += thirdParty/qcustomplot/

SOURCES += \
    src/qtWidgets/main.cpp \
    src/qtWidgets/mainWindow.cpp \
    src/qtWidgets/av_dump_format_form.cpp \
    src/qtWidgets/openMediaDialog.cpp \
    src/qtWidgetsInterface/avDumpFormat.cpp \
    src/qtWidgetsInterface/bitrateForm.cpp \
    src/qtWidgetsInterface/displayForm.cpp \
    src/qtWidgetsInterface/selectStreamDialog.cpp \
    src/qtWidgetsInterface/simplePlaybackForm.cpp \
    src/qtWidgetsInterface/simpleDisplay.cpp \
    src/qtWidgetsInterface/simpleAudio.cpp \
    src/qtWidgetsInterface/simpleAudioIO.cpp \
    src/qtWidgetsInterface/syncedAudioIO.cpp \
    src/qtWidgetsInterface/syncedDisplay.cpp \
    thirdParty/qcustomplot/qcustomplot.cpp

HEADERS += \
    src/appinfo.h \
    src/ffmpegUtil/openglVariables.h \
    src/qtWidgets/mainWindow.h \
    src/qtWidgets/av_dump_format_form.h \
    src/qtWidgets/mainWindowDialogs.h \
    src/qtWidgets/openMediaDialog.h \
    src/qtWidgetsInterface/avDumpFormat.h \
    src/ffmpegUtil/customExceptions.h \
    src/ffmpegUtil/ffmpegUPtr.h \
    src/playerEngine/Mk01/engine.h \
    src/playerEngine/Mk02/engineUtil.h \
    src/playerEngine/Mk02/engineContainer.h \
    src/playerEngine/Mk02/playerDemuxer.h \
    src/playerEngine/Mk02/playerVideoDec.h \
    src/playerEngine/Mk02/playerAudioDec.h \
    src/qtWidgetsInterface/bitrateForm.h \
    src/qtWidgetsInterface/displayForm.h \
    src/qtWidgetsInterface/selectStreamDialog.h \
    src/qtWidgetsInterface/simplePlaybackForm.h \
    src/qtWidgetsInterface/simpleDisplay.h \
    src/qtWidgetsInterface/simpleAudio.h \
    src/qtWidgetsInterface/simpleAudioIO.h \
    src/qtWidgetsInterface/syncedAudioIO.h \
    src/qtWidgetsInterface/syncedDisplay.h \
    thirdParty/qcustomplot/qcustomplot.h

FORMS += \
        src/qtWidgets/mainWindow.ui \
    src/qtWidgets/av_dump_format_form.ui \
    src/qtWidgets/openMediaDialog.ui \
    src/qtWidgetsInterface/bitrateForm.ui \
    src/qtWidgetsInterface/displayForm.ui \
    src/qtWidgetsInterface/selectStreamDialog.ui \
    src/qtWidgetsInterface/simplePlaybackForm.ui

RESOURCES += \
    src/qtWidgets/icons/icons.qrc

win32: LIBS += -L$$PWD/windows/ffmpeg-4.2.1-win64-dev/lib/ -lavcodec

INCLUDEPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include


win32: LIBS += -L$$PWD/windows/ffmpeg-4.2.1-win64-dev/lib/ -lavdevice

INCLUDEPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include

win32: LIBS += -L$$PWD/windows/ffmpeg-4.2.1-win64-dev/lib/ -lavfilter

INCLUDEPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include

win32: LIBS += -L$$PWD/windows/ffmpeg-4.2.1-win64-dev/lib/ -lavformat

INCLUDEPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include

win32: LIBS += -L$$PWD/windows/ffmpeg-4.2.1-win64-dev/lib/ -lavutil

INCLUDEPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include

win32: LIBS += -L$$PWD/windows/ffmpeg-4.2.1-win64-dev/lib/ -lpostproc

INCLUDEPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include

win32: LIBS += -L$$PWD/windows/ffmpeg-4.2.1-win64-dev/lib/ -lswresample
INCLUDEPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include

win32: LIBS += -L$$PWD/windows/ffmpeg-4.2.1-win64-dev/lib/ -lswscale
INCLUDEPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/windows/ffmpeg-4.2.1-win64-dev/include

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

DISTFILES +=
