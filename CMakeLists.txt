cmake_minimum_required(VERSION 3.14)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif()

project(GameOfLife)

add_executable(GameOfLife WIN32 src/main.cpp)

find_package(TBB CONFIG REQUIRED)
target_link_libraries(GameOfLife PRIVATE TBB::tbb)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(GameOfLife PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_include_directories(GameOfLife PRIVATE ${SDL2_INCLUDE_DIRS})

