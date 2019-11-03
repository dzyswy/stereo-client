
include_directories(${CMAKE_CURRENT_SOURCE_DIR})  

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} FILE_SRCS) 

  

add_library(${PROJECT_NAME} STATIC ${FILE_SRCS})  
set_target_properties(${PROJECT_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1) 


install(TARGETS ${PROJECT_NAME} DESTINATION lib) 
