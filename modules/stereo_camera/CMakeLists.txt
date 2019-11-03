project(stereo_camera CXX)



set(CMAKE_CXX_STANDARD 11)

if (UNIX)
	add_definitions(-fPIC)
	add_definitions(-fpermissive)
endif()	
 


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

