## Project Properties
CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)
PROJECT(kiyosVideoAnalysisTool)

SET(APPLICATION_NAME "kiyosVideoAnalysisTool")
SET(APPLICATION_VERSION_STRING 0.1)

if (WIN32)
    message("!! This doesn't work on windows, please use qtcreator")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

## Project Output Paths
SET(MAINFOLDER ${PROJECT_SOURCE_DIR})
SET(EXECUTABLE_OUTPUT_PATH "${MAINFOLDER}/../build/bin")

## Project Search Paths
LIST(APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}")
SET(CMAKE_MODULE_PATH "${MAINFOLDER}/cmake")
INCLUDE_DIRECTORIES("${MAINFOLDER}/src")
INCLUDE_DIRECTORIES("${MAINFOLDER}/src/ffmpegUtil")
INCLUDE_DIRECTORIES("${MAINFOLDER}/src/qtWidgets")
INCLUDE_DIRECTORIES("${MAINFOLDER}/src/qtWidgetsInterface")
INCLUDE_DIRECTORIES("${MAINFOLDER}/src/bitrate")
INCLUDE_DIRECTORIES("${MAINFOLDER}/src/playback")
INCLUDE_DIRECTORIES("${MAINFOLDER}/src/playerEngine")
INCLUDE_DIRECTORIES("${MAINFOLDER}/thirdParty")
INCLUDE_DIRECTORIES("${MAINFOLDER}/thirdParty/mySpsc")
INCLUDE_DIRECTORIES("${MAINFOLDER}/thirdParty/qcustomplot")
INCLUDE_DIRECTORIES("${MAINFOLDER}/src/metadata")
INCLUDE_DIRECTORIES("${MAINFOLDER}/thirdParty/catch") #catch.hpp
## example project Sources
FILE(GLOB_RECURSE EXAMPLE_SRCS01 ${MAINFOLDER}/src/*.cpp ${MAINFOLDER}/src/qtWidgets/*.ui ${MAINFOLDER}/src/qtWidgets/*.qrc ${MAINFOLDER}/thirdParty/qcustomplot/qcustomplot.cpp)

find_package(Qt5 COMPONENTS Widgets PrintSupport Network Multimedia Concurrent REQUIRED)

if(Qt5Widgets_FOUND)
    message("-- Found Qt5")
else()
  MESSAGE(WARNING "-> Qt5 not found")
endif()

find_package(PkgConfig REQUIRED)

pkg_check_modules(AVDEVICE "libavdevice")
include_directories(${AVDEVICE_INCLUDE_DIRS})
link_directories(${AVDEVICE_LIBRARY_DIRS})

pkg_check_modules(AVFILTER "libavfilter")
include_directories(${AVFILTER_INCLUDE_DIRS})
link_directories(${AVFILTER_LIBRARY_DIRS})

pkg_check_modules(AVFORMAT "libavformat")
include_directories(${AVFORMAT_INCLUDE_DIRS})
link_directories(${AVFORMAT_LIBRARY_DIRS})

pkg_check_modules(AVCODEC REQUIRED "libavcodec")
include_directories(${AVCODEC_INCLUDE_DIRS})
link_directories(${AVCODEC_LIBRARY_DIRS})

pkg_check_modules(AVUTIL REQUIRED "libavutil")
include_directories(${AVUTIL_INCLUDE_DIRS})
link_directories(${AVUTIL_LIBRARY_DIRS})

pkg_check_modules(AVRESAMPLE REQUIRED "libavresample")
include_directories(${AVRESAMPLE_INCLUDE_DIRS})
link_directories(${AVRESAMPLE_LIBRARY_DIRS})

pkg_check_modules(SWRESAMPLE REQUIRED "libswresample")
include_directories(${SWRESAMPLE_INCLUDE_DIRS})
link_directories(${SWRESAMPLE_LIBRARY_DIRS})

pkg_check_modules(SWSCALE REQUIRED "libswscale")
include_directories(${SWSCALE_INCLUDE_DIRS})
link_directories(${SWSCALE_LIBRARY_DIRS})

SET(FFMPEG_LIBRARIES_TMP ${AVUTIL_LIBRARIES} ${AVDEVICE_LIBRARIES} ${AVFILTER_LIBRARIES} ${AVFORMAT_LIBRARIES} ${AVCODEC_LIBRARIES}
     ${AVRESAMPLE_LIBRARIES} ${SWRESAMPLE_LIBRARIES} ${SWSCALE_LIBRARIES})

## Configure Files
FILE(GLOB_RECURSE CONFIGINPUTS1 src/*.h.in.cmake)
LIST(APPEND CONFIGINPUTS "${CONFIGINPUTS1}")
FOREACH(CONFIGINPUT ${CONFIGINPUTS})
    STRING(REPLACE ".h.in.cmake" ".h" CONFIGOUTPUT1 "${CONFIGINPUT}")
    CONFIGURE_FILE("${CONFIGINPUT}" "${CONFIGOUTPUT1}")
ENDFOREACH(CONFIGINPUT ${CONFIGINPUTS})

## Use compiler debugging flags per default
INCLUDE(CxxFlags)
set(CMAKE_CXX_STANDARD 17)
IF(CMAKE_BUILD_TYPE MATCHES debug)
   message("debug build")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -std=c++1z -g  -pthread")
ELSEIF(CMAKE_BUILD_TYPE MATCHES threadSanitizer)
   message("threadSanitizer build")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -fsanitize=thread -g -O1")
ELSEIF(CMAKE_BUILD_TYPE MATCHES release)
   message("release build")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -std=c++1z -g  -pthread")
ELSE()
   message("release build with march=native")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -std=c++1z -g -march=native -pthread")
ENDIF()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
## Creating Binaries for examples
SET(PROJECT_LIBS ${CMAKE_DL_LIBS} Qt5::Widgets Qt5::Network Qt5::Multimedia Qt5::PrintSupport Qt5::Concurrent ${FFMPEG_LIBRARIES_TMP}) 

SET(EXAMPLE1_NAME ${APPLICATION_NAME})
SET(EXAMPLE1_BIN ${EXAMPLE1_NAME})
ADD_EXECUTABLE(${EXAMPLE1_BIN} ${MAINFOLDER}/src/qtWidgets/main.cpp ${EXAMPLE_SRCS01})

TARGET_LINK_LIBRARIES(${EXAMPLE1_BIN} ${PROJECT_LIBS})

INSTALL(TARGETS ${EXAMPLE1_BIN} DESTINATION bin)


