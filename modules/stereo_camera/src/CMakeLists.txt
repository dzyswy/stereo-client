 
include_directories(${CMAKE_CURRENT_SOURCE_DIR}) 
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/jsoncpp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/broadcast_client/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/rpc_client/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/stream_client/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/json_struct)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/media_record)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ftp_client)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} FILE_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/json_struct FILE_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/media_record FILE_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ftp_client FILE_SRCS)
 
set(LIBS ${LIBS} jsoncpp) 
set(LIBS ${LIBS} broadcast_client)
set(LIBS ${LIBS} rpc_client)
set(LIBS ${LIBS} stream_client)

add_library(${PROJECT_NAME} SHARED ${FILE_SRCS})
target_link_libraries(${PROJECT_NAME} ${LIBS})
add_library(${PROJECT_NAME}_static STATIC ${FILE_SRCS})
set_target_properties(${PROJECT_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${PROJECT_NAME}_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")


install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(TARGETS ${PROJECT_NAME}_static DESTINATION lib)
