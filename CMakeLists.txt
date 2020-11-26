########################################################################
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "Prevented in-tree built. This is bad practice.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})


########################################################################
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.5.0)
PROJECT(stereo-client CXX)
ENABLE_TESTING()

if (POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

if (POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()



OPTION(WITH_ZSCAM_CLIENT "Compile zscam_client, need Qt5" ON)


set(CMAKE_MODULE_PATH APPEND "${CMAKE_SOURCE_DIR}/cmake/Modules/")


if (UNIX)
	message(STATUS "current platform: Linux ")
	add_definitions(-fPIC)
	add_definitions(-fpermissive)
	message(STATUS "GCC version: ${CMAKE_CXX_COMPILER_VERSION}")
	if (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.0)
		add_definitions(-D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_{1,2,4})
		message(STATUS "add definitions for gcc std::thread bug")	
	endif() 
	
elseif (WIN32)
	message(STATUS "current platform: Windows")
endif()




set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)


link_directories(${PROJECT_BINARY_DIR}/libs)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules)
IF (WITH_ZSCAM_CLIENT)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app/zscam_client)
ENDIF()


message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
