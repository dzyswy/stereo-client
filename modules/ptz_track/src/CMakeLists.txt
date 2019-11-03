
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/fit_calib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/libVISCA2/visca)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/jsoncpp/include)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} FILE_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/fit_calib FILE_SRCS)

set(LIBS ${LIBS} jsoncpp)
set(LIBS ${LIBS} visca) 

add_library(${PROJECT_NAME} SHARED ${FILE_SRCS})
target_link_libraries(${PROJECT_NAME} ${LIBS})
add_library(${PROJECT_NAME}_static STATIC ${FILE_SRCS})
set_target_properties(${PROJECT_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${PROJECT_NAME}_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")


install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(TARGETS ${PROJECT_NAME}_static DESTINATION lib)


