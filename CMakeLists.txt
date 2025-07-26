cmake_minimum_required(VERSION 3.10)
project(nesEmu)

# Build options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SDL2
find_package(SDL2 REQUIRED)

# Add headers
include_directories(${SDL2_INCLUDE_DIRS} inc)

# Add source files
file(GLOB SOURCE "src/*.cpp")

# Generate executable
add_executable(nesEmu ${SOURCE})

# Link SDL2
target_link_libraries(nesEmu ${SDL2_LIBRARIES})
