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


OPTION(WITH_ZSCAM_CLIENT "Compile zscam_client, need Qt5" ON)



set(CMAKE_MODULE_PATH APPEND "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_CXX_STANDARD 11)



if (UNIX)
	MESSAGE(STATUS "current platform: Linux ")
	ADD_DEFINITIONS(-fPIC)
	ADD_DEFINITIONS(-fpermissive)
#	ADD_DEFINITIONS(-mfpu=neon)
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath-link=${USER_RPATH}")
elseif (WIN32)
	MESSAGE(STATUS "current platform: Windows")
endif()


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/stereo_camera/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/ptz_track/include)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)


link_directories(${PROJECT_BINARY_DIR}/libs)

ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/sample_ini) 
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/jsoncpp)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libVISCA2)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/broadcast_client)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/rpc_client)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/stream_client)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/ptz_track)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/stereo_camera)
IF (WITH_ZSCAM_CLIENT)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/app/zscam_client)
ENDIF()


MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
