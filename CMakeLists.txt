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
INCLUDE_DIRECTORIES("${MAINFOLDER}/src/ffmpegUtil")
INCLUDE_DIRECTORIES("${MAINFOLDER}/src/qtWidgets")
INCLUDE_DIRECTORIES("${MAINFOLDER}/src/qtWidgetsInterface")
INCLUDE_DIRECTORIES("${MAINFOLDER}/src/playerEngine")
INCLUDE_DIRECTORIES("${MAINFOLDER}/thirdParty")

INCLUDE_DIRECTORIES("${MAINFOLDER}/thirdParty/catch") #catch.hpp

## example project Sources
FILE(GLOB_RECURSE EXAMPLE_SRCS01 ${MAINFOLDER}/src/*.cpp ${MAINFOLDER}/src/qtWidgets/*.ui ${MAINFOLDER}/src/qtWidgets/*.qrc)

find_package(Qt5 COMPONENTS Widgets PrintSupport Network REQUIRED)

if(Qt5Widgets_FOUND)
    message("-- Found Qt5")
else()
  MESSAGE(WARNING "-> Qt5 not found")
endif()


## Project Dependencies

#SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ~/boost/include)
#SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ~/boost/lib)
## Project Dependencies
#find_package(Boost REQUIRED COMPONENTS system program_options regex filesystem)
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
#LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
#INCLUDE_DIRECTORIES(~/boost/include)
#LINK_DIRECTORIES(~/boost/lib)

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

pkg_check_modules(AVCODEC "libavcodec")
#if (AVCODEC_FOUND)
include_directories(${AVCODEC_INCLUDE_DIRS})
link_directories(${AVCODEC_LIBRARY_DIRS})

pkg_check_modules(AVUTIL "libavutil")
include_directories(${AVUTIL_INCLUDE_DIRS})
link_directories(${AVUTIL_LIBRARY_DIRS})

pkg_check_modules(AVFORMAT REQUIRED "libavformat")
include_directories(${AVFORMAT_INCLUDE_DIRS})
link_directories(${AVFORMAT_LIBRARY_DIRS})

pkg_check_modules(AVCODEC REQUIRED "libavcodec")
include_directories(${AVCODEC_INCLUDE_DIRS})
link_directories(${AVCODEC_LIBRARY_DIRS})

pkg_check_modules(AVUTIL REQUIRED "libavutil")
include_directories(${AVUTIL_INCLUDE_DIRS})
link_directories(${AVUTIL_LIBRARY_DIRS})

SET(FFMPEG_LIBRARIES_TMP ${AVDEVICE_LIBRARIES} ${AVFILTER_LIBRARIES} ${AVFORMAT_LIBRARIES} ${AVCODEC_LIBRARIES} ${AVUTIL_LIBRARIES} ${AVFORMAT_LIBRARIES} ${AVCODEC_LIBRARIES} ${AVUTIL_LIBRARIES})

## Configure Files
FILE(GLOB_RECURSE CONFIGINPUTS1 include/*.h.in.cmake)
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


## Creating Binaries for examples
SET(PROJECT_LIBS ${CMAKE_DL_LIBS} Qt5::Widgets Qt5::Network ${FFMPEG_LIBRARIES_TMP}) 

SET(EXAMPLE1_NAME ${APPLICATION_NAME})
SET(EXAMPLE1_BIN ${EXAMPLE1_NAME})
ADD_EXECUTABLE(${EXAMPLE1_BIN} ${MAINFOLDER}/src/qtWidgets/main.cpp ${EXAMPLE_SRCS01})

TARGET_LINK_LIBRARIES(${EXAMPLE1_BIN} ${PROJECT_LIBS})

INSTALL(TARGETS ${EXAMPLE1_BIN} DESTINATION bin)

#set(BUILD_TESTING true CACHE BOOL "Enable building of tests.")

macro(create_test TARGET_NAME SOURCE SOURCE2 DEPENDENCIES)
    add_executable(${TARGET_NAME} ${SOURCE} ${SOURCE2})
    target_link_libraries(
        ${TARGET_NAME}
        ${DEPENDENCIES}
        catchMain
        )
endmacro()

if (BUILD_TESTING)
    ADD_LIBRARY(catchMain ./tests/00CatchMain.cpp)
    enable_testing()
    create_test(test_avDumpFormat ./tests/test_avDumpFormat.cpp ${EXAMPLE_SRCS01} ${PROJECT_LIBS})
    create_test(test_avDumpFormat2 ./tests/test_avDumpFormat.cpp ${EXAMPLE_SRCS01} ${PROJECT_LIBS})
endif (BUILD_TESTING)
