QT += testlib
QT += core gui multimedia printsupport concurrent
CONFIG += qt warn_on depend_includepath testcase c++17

QMAKE_CXXFLAGS_RELEASE += -O3

INCLUDEPATH += ../../src/qtWidgets/

TEMPLATE = app

SOURCES +=  tst_openmediatest.cpp


HEADERS +=  ../../src/qtWidgets/openMediaDialog.h \


SOURCES +=  ../../src/qtWidgets/openMediaDialog.cpp \


FORMS +=    ../../src/qtWidgets/openMediaDialog.ui \

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
