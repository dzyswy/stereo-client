project(stream_client CXX)



set(CMAKE_CXX_STANDARD 11)

if (UNIX)
	add_definitions(-fPIC)
endif()	



include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
 
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src) 

