########################################################################
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "Prevented in-tree built. This is bad practice.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})


########################################################################
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.9.0)
PROJECT(stereo-client CXX)
ENABLE_TESTING()

if (POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

set(CMAKE_MODULE_PATH APPEND "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_PREFIX_PATH $ENV{QTDIR} ${CMAKE_PREFIX_PATH})

if (UNIX)
	ADD_DEFINITIONS(-fPIC)
endif()



set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)

link_directories(${PROJECT_BINARY_DIR}/libs)




ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/rpclib)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/jsoncpp)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/stereo_camera)
#ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/ptz_track)
#ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/app/zscam_client)



MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
