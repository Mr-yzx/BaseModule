cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME threadpool)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../lib)

include_directories(/root/BaseModule/utils)
aux_source_directory(. SRC_LIST)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} pthread)
install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/../lib
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/../lib
)