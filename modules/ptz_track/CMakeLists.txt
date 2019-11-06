project(ptz_track CXX)



set(CMAKE_CXX_STANDARD 11)

if (UNIX)
	add_definitions(-fPIC)
	add_definitions(-fpermissive)
endif()	



include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stereo_camera/include)

set(LIBS ${LIBS} stereo_camera) 

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

