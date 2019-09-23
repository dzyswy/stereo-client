########################################################################
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "Prevented in-tree built. This is bad practice.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})


########################################################################
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.9.0)
PROJECT(ptz_track CXX)
ENABLE_TESTING()


if (UNIX)
	ADD_DEFINITIONS(-fpermissive)
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath-link=${USER_RPATH}")

endif()



INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/include)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src)
#ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/test)

