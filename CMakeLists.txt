cmake_minimum_required(VERSION 3.20)

project(kv-store)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(kvstore main.cpp src/Service.cpp src/RESPParser.cpp)

target_include_directories(kvstore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/includes")

enable_testing()

find_package(GTest REQUIRED)

add_executable(kvtests
    tests/test_storage.cpp
    tests/test_service.cpp
    tests/test_RESPParser.cpp
    src/Service.cpp
    src/RESPParser.cpp)

target_link_libraries(kvtests PRIVATE GTest::gtest_main)

include(GoogleTest)

gtest_discover_tests(kvtests)

target_include_directories(kvtests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/includes")