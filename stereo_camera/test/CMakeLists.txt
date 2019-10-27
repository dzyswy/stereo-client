file(GLOB_RECURSE TEST_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/sample_ini/include)

SET(LIBS ${LIBS} stereo_camera) 

foreach(SOURCE_FILE ${TEST_SRCS})
  # get file NAME
  get_filename_component(NAME ${SOURCE_FILE} NAME_WE)
    
  add_executable(${NAME} ${SOURCE_FILE})
  target_link_libraries(${NAME} ${LIBS})


  # install
  install(TARGETS ${NAME} DESTINATION bin)


endforeach()
