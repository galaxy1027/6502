set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(nesEmu)
include_directories(inc)
FILE(GLOB SOURCE "src/*.cpp")
ADD_EXECUTABLE(nesEmu ${SOURCE})
