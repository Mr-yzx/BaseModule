cmake_minimum_required(VERSION 3.10)

project(test_threadpool)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

include_directories(/root/BaseModule/threadpool)
include_directories(/root/BaseModule/utils)

aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})



target_link_libraries(${PROJECT_NAME} PRIVATE
            pthread
            /root/BaseModule/lib/libthreadpool.so) 
