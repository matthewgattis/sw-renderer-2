cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR};${CMAKE_MODULE_PATH};")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Install vcpkg, if necessary.  This must be done before calling the project command so that the 
# toolchain file is found.
SET(VCPKG_ROOT "${CMAKE_BINARY_DIR}/vcpkg")
SET(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
include(cmake/vcpkg_dependencies.cmake)
install_vcpkg(master)

project(sw-renderer VERSION 1.0.0)
add_definitions(-D_USE_MATH_DEFINES)

message("Platform toolset: ${VCPKG_PLATFORM_TOOLSET}")
message("Target triplet: ${VCPKG_TARGET_TRIPLET}")

set_vcpkg_dependencies(
    glm
    sdl2
    )

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${CMAKE_SOURCE_DIR})

find_package(glm CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

set(${PROJECT_NAME}_SOURCE
    ./app.cpp
    ./main.cpp
    ./sdlrenderer.cpp
    ./sdltexture.cpp
    ./sdlwindow.cpp
    )

set(${PROJECT_NAME}_INCLUDE
    ./app.hpp
    ./log.hpp
    ./framebuffer.hpp
    ./g_line.c
    ./sdlrenderer.hpp
    ./sdltexture.hpp
    ./sdlwindow.hpp
    )

include_directories(${PROJECT_NAME}
    )

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_INCLUDE}
    ${${PROJECT_NAME}_SOURCE}
    )

target_link_libraries(${PROJECT_NAME} PRIVATE
    PRIVATE glm::glm
    PRIVATE SDL2::SDL2 SDL2::SDL2main
    )

