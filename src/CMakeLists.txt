if (POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()



include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/regctl)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} REMOTE_STEREO_SRCS)
#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/regctl REMOTE_STEREO_SRCS)


ADD_LIBRARY(remote_stereo SHARED ${REMOTE_STEREO_SRCS})
TARGET_LINK_LIBRARIES(remote_stereo 
	"-Wl,--whole-archive" 
	rpc
	"-Wl,--no-whole-archive")
ADD_LIBRARY(remote_stereo_static STATIC ${REMOTE_STEREO_SRCS})
set_target_properties(remote_stereo_static PROPERTIES OUTPUT_NAME "remote_stereo")
set_target_properties(remote_stereo PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(remote_stereo_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(remote_stereo PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set_target_properties(remote_stereo_static PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

#IF(UNIX)
#    SET_PROPERTY(TARGET remote_stereo PROPERTY VERSION ${remote_stereo_VERSION})
#    SET_PROPERTY(TARGET remote_stereo PROPERTY SOVERSION ${remote_stereo_VERSION_MAJOR})
#ENDIF(UNIX)
INSTALL(TARGETS remote_stereo DESTINATION lib)
INSTALL(TARGETS remote_stereo_static DESTINATION lib)
#INSTALL(DIRECTORY . DESTINATION include FILES_MATCHING PATTERN "*.h")
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/remote_stereo.h DESTINATION include)
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/stream_func.h DESTINATION include)
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/record_media.h DESTINATION include)
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/discovery/broadcast_client.h DESTINATION include)