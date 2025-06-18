cmake_minimum_required(VERSION 3.15)

project(
    p2p-lan-share 
    VERSION 0.0.1 
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS Off)

add_executable(p2p-lan-share src/main.cpp)

target_include_directories(p2p-lan-share PUBLIC "${PROJECT_SOURCE_DIR}/include")
