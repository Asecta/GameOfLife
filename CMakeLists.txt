cmake_minimum_required(VERSION 3.14)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(GameOfLife)

add_executable(GameOfLife src/main.cpp src/gameimpl/GameOfLifeInstance.h src/gameimpl/GameOfLifeInstance.h)

find_package(TBB CONFIG REQUIRED)
target_link_libraries(GameOfLife PRIVATE TBB::tbb)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(GameOfLife PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_include_directories(GameOfLife PRIVATE ${SDL2_INCLUDE_DIRS})

