cmake_minimum_required(VERSION 3.15)

project(
    p2p-lan-share 
    VERSION 0.0.1 
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS Off)

include(FetchContent)

FetchContent_Declare(
    asio
    GIT_REPOSITORY  https://github.com/chriskohlhoff/asio.git
    GIT_TAG         asio-1-30-2
)

FetchContent_MakeAvailable(asio)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
find_package(Threads)
target_link_libraries(asio INTERFACE Threads::Threads)

add_executable(p2p-lan-share src/main.cpp)

target_link_libraries(p2p-lan-share PRIVATE asio)

target_include_directories(p2p-lan-share PUBLIC "${PROJECT_SOURCE_DIR}/include")
