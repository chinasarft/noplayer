cmake_minimum_required(VERSION 3.9.0)

project(iceplayer LANGUAGES CXX)

SET(TOP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
if(WIN32)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /source-charset:utf-8")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /source-charset:utf-8")
endif()

if(NOT FFINCDIR)
        message(FATAL_ERROR "shoud specify FFINCDIR")
endif()
include_directories("${FFINCDIR}")

if (QTDIR OR DEFINED ENV{QTDIR} OR DEFINED ENV{QTDIR32} OR DEFINED ENV{QTDIR64})
        # Qt path set by user or env var
        list(APPEND CMAKE_PREFIX_PATH "${QTDIR}")
else()
        message(WARNING "QTDIR variable is missing.  Please set this variable")
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${QTDIR}/lib/cmake/")
message("===============${CMAKE_MODULE_PATH}")

if (DEPS_INC_ROOT)
	include_directories("${DEPS_INC_ROOT}/src/openavio")
else()
	message(WARNING "DEPS_INC_ROOT variable is missing(learnffmpeg)")
endif()
if (DEPS_LIB_ROOT)
	if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		link_directories("${DEPS_LIB_ROOT}/src/openavio/Debug")
	else()
		link_directories("${DEPS_LIB_ROOT}/src/openavio/Release")
	endif()
else()
	message(WARNING "DEPS_LIB_ROOT variable is missing(learnffmpeg build root)")
endif()


find_package(Qt5 COMPONENTS Core Quick Multimedia REQUIRED)

message("1===============${Qt5Multimedia_INCLUDE_DIRS}")
message("2===============${Qt5Multimedia_LIBRARIES}")
#include_directories("${Qt5Multimedia_INCLUDE_DIRS}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(SOURCE_FILES
        main.cpp
        main.qml
        render.cpp
        render.h
	filemediasource.cpp
        filemediasource.h
        iceplayer.cpp
        iceplayer.h
        mediasource.cpp
        mediasource.h
        streamcache.cpp
        streamcache.h
        g711.cpp
        g711.h
	)
if (TUTK_ENABLE)
        list(APPEND SOURCE_FILES
        	tutkclient.cpp
		tutkclient.h
		)
endif()
if (MYDEMO_ENABLE)
        list(APPEND SOURCE_FILES
        	linking.cpp
        	linking.h
		)
endif()

message(${SOURCE_FILES})
add_executable(${PROJECT_NAME}
	${SOURCE_FILES}
	)

target_link_libraries(${PROJECT_NAME} avinput Qt5::Core Qt5::Quick Qt5::Multimedia)
