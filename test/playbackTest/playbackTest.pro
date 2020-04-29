QT += testlib
QT += core gui multimedia printsupport concurrent
CONFIG += qt warn_on depend_includepath testcase c++17

QMAKE_CXXFLAGS_RELEASE += -O3

INCLUDEPATH += ../../src/qtWidgets/
INCLUDEPATH += ../../src/playback/
INCLUDEPATH += ../../src/
INCLUDEPATH += ../../src/ffmpegUtil/
INCLUDEPATH += ../../src/playerEngine/
INCLUDEPATH += ../../thirdParty/
INCLUDEPATH += ../../thirdParty/mySpsc/
INCLUDEPATH += ../../thirdParty/qcustomplot/

TEMPLATE = app

SOURCES +=  tst_playbacktest.cpp


HEADERS +=  ../../src/bitrate/bitrateForm.h \
            ../../src/playback/playbackForm.h \
            ../../src/playback/selectStreamDialog.h \
            ../../src/bitrate/selectStreamDialog_copy1.h \
            ../../src/playback/syncedAudioIO.h \
            ../../src/playback/syncedDisplay.h \
            ../../thirdParty/qcustomplot/qcustomplot.h

SOURCES +=  ../../src/bitrate/bitrateForm.cpp \
            ../../src/playback/playbackForm.cpp \
            ../../src/playback/selectStreamDialog.cpp \
            ../../src/bitrate/selectStreamDialog_copy1.cpp \
            ../../src/playback/syncedAudioIO.cpp \
            ../../src/playback/syncedDisplay.cpp \
            ../../thirdParty/qcustomplot/qcustomplot.cpp


FORMS +=    ../../src/bitrate/bitrateForm.ui \
            ../../src/playback/playbackForm.ui \
            ../../src/playback/selectStreamDialog.ui \
            ../../src/bitrate/selectStreamDialog_copy1.ui

win32: LIBS += -L$$PWD/../../windows/ffmpeg-4.2.1-win64-dev/lib/ -lavcodec

INCLUDEPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include


win32: LIBS += -L$$PWD/../../windows/ffmpeg-4.2.1-win64-dev/lib/ -lavdevice

INCLUDEPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include

win32: LIBS += -L$$PWD/../../windows/ffmpeg-4.2.1-win64-dev/lib/ -lavfilter

INCLUDEPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include

win32: LIBS += -L$$PWD/../../windows/ffmpeg-4.2.1-win64-dev/lib/ -lavformat

INCLUDEPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include

win32: LIBS += -L$$PWD/../../windows/ffmpeg-4.2.1-win64-dev/lib/ -lavutil

INCLUDEPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include

win32: LIBS += -L$$PWD/../../windows/ffmpeg-4.2.1-win64-dev/lib/ -lpostproc

INCLUDEPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include

win32: LIBS += -L$$PWD/../../windows/ffmpeg-4.2.1-win64-dev/lib/ -lswresample
INCLUDEPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include

win32: LIBS += -L$$PWD/../../windows/ffmpeg-4.2.1-win64-dev/lib/ -lswscale
INCLUDEPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include
DEPENDPATH += $$PWD/../../windows/ffmpeg-4.2.1-win64-dev/include

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
