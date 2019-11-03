project(zscam_client CXX)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_PREFIX_PATH $ENV{QTDIR} ${CMAKE_PREFIX_PATH})



include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/sample_ini/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../modules/stereo_camera/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ptz_track/include)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} FILE_SRCS)

set(CMAKE_AUTOMOC ON)
set(QMAKE_CXXFLAGS ${QMAKE_CXXFLAGS} -std=c++11)

#find_package(OpenCV REQUIRED)
#if(NOT OpenCV_FOUND)
#	message("fail to find OpenCV")
#else()
#	include_directories(${OpenCV_INCLUDE_DIRS})
#	SET(LIBS ${LIBS} ${OpenCV_LIBS})
#	message(STATUS "OpenCV library status:")
#	message(STATUS "    version: ${OpenCV_VERSION}")
#endif()


find_package(Qt5Widgets)
find_package(Qt5Gui)

set(QRC_SOURCE_FILES zscam_client.qrc)
qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})

set(LIBS ${LIBS} sample_ini)
set(LIBS ${LIBS} stereo_camera)
set(LIBS ${LIBS} ptz_track) 


if (UNIX)
	set(LIBS ${LIBS} pthread)
endif ()

 

qt5_wrap_ui(UI_HEADERS zscam_client.ui ftp_dialog.ui) # update_dialog.ui
add_executable(${PROJECT_NAME} ${FILE_SRCS} ${UI_HEADERS} ${QRC_FILES})
target_link_libraries(${PROJECT_NAME} 
	${LIBS}
	${Qt5Core_QTMAIN_LIBRARIES})
qt5_use_modules(${PROJECT_NAME} Widgets Gui)


install(TARGETS ${PROJECT_NAME} DESTINATION bin)

