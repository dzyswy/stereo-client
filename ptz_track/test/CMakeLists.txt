file(GLOB_RECURSE test_srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/rpclib/include)



SET(LIBS ${LIBS} stereo_camera)
SET(LIBS ${LIBS} ptz_track) 

if (UNIX)
	set(LIBS ${LIBS} pthread)
endif ()


foreach(source_file ${test_srcs})
  # get file name
  get_filename_component(name ${source_file} NAME_WE)
    
  add_executable(${name} ${source_file})
  target_link_libraries(${name} ${LIBS})


  # install
  install(TARGETS ${name} DESTINATION bin)


endforeach()
