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

if (UNIX)
	ADD_DEFINITIONS(-fPIC)
endif()


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)


ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/rpclib)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/jsoncpp)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/include)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/app/zscam_client)



MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
