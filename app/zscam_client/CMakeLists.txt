
if (POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()


PROJECT(zscam_client CXX)


set(CMAKE_PREFIX_PATH $ENV{QTDIR} ${CMAKE_PREFIX_PATH})



INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../stereo_camera/include)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} ZSCAM_CLIENT_SRCS)

SET(CMAKE_AUTOMOC ON)


#find_package(OpenCV REQUIRED)
#if(NOT OpenCV_FOUND)
#	message("fail to find OpenCV")
#else()
#	include_directories(${OpenCV_INCLUDE_DIRS})
#	SET(LIBS ${LIBS} ${OpenCV_LIBS})
#	message(STATUS "OpenCV library status:")
#	message(STATUS "    version: ${OpenCV_VERSION}")
#endif()



if (UNIX)
	set(LIBS pthread)
endif ()

 
FIND_PACKAGE(Qt5Widgets)
FIND_PACKAGE(Qt5Gui)

set(QRC_SOURCE_FILES zscam_client.qrc)
qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})


qt5_wrap_ui(UI_HEADERS zscam_client.ui) # ftp_dialog.ui update_dialog.ui
ADD_EXECUTABLE(zscam_client ${ZSCAM_CLIENT_SRCS} ${UI_HEADERS} ${QRC_FILES})
TARGET_LINK_LIBRARIES(zscam_client 
	stereo_camera
	${LIBS}
	${Qt5Core_QTMAIN_LIBRARIES})
qt5_use_modules(zscam_client Widgets Gui)


INSTALL(TARGETS zscam_client DESTINATION bin)

