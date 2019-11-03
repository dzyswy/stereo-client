file(GLOB_RECURSE TEST_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")



set(LIBS ${LIBS} ptz_track) 

if (UNIX)
	set(LIBS ${LIBS} pthread)
endif ()

foreach(SOURCE_FILE ${TEST_SRCS})
  # get file NAME
  get_filename_component(NAME ${SOURCE_FILE} NAME_WE)
    
  add_executable(${NAME} ${SOURCE_FILE})
  target_link_libraries(${NAME} ${LIBS})


  # install
  install(TARGETS ${NAME} DESTINATION bin)


endforeach()

