cmake_minimum_required(VERSION 3.20)

project(bf-tree)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

target_link_libraries(TestBFTree PUBLIC BFTree gtest gtest_main)