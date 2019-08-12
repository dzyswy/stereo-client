

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})


SET(LIBS ${LIBS} remote_stereo)
if (UNIX)
	SET(LIBS ${LIBS} pthread)
	MESSAGE(STATUS "current platform: Linux ")
elseif (WIN32)
	MESSAGE(STATUS "current platform: Windows")
endif()

#SET(LIBS ${LIBS} zynq_hardware)

ADD_EXECUTABLE(zscam_client zscam_client.cpp)
TARGET_LINK_LIBRARIES(zscam_client ${LIBS})
	

	
#INSTALL(TARGETS vdma-loop-demo DESTINATION bin)


