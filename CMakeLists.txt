project(fontconfig-examples)
cmake_minimum_required(VERSION 3.0)

# setting defaults
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(fontconfig REQUIRED IMPORTED_TARGET fontconfig)

add_executable(fce-list fce-list.cpp)
target_link_libraries(fce-list PRIVATE PkgConfig::fontconfig)
