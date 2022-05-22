cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR};${CMAKE_MODULE_PATH};")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Install vcpkg, if necessary. This must be done before calling the project command so that the 
# toolchain file is found.
#SET(VCPKG_ROOT "${CMAKE_BINARY_DIR}/vcpkg")
#SET(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
#include(cmake/vcpkg_dependencies.cmake)
#install_vcpkg(master)

project(sw-renderer-2 VERSION 1.0.0)
add_definitions(-D_USE_MATH_DEFINES)

#message("Platform toolset: ${VCPKG_PLATFORM_TOOLSET}")
#message("Target triplet: ${VCPKG_TARGET_TRIPLET}")

#set_vcpkg_dependencies(
#    glm
#    sdl2
#    )
#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${CMAKE_SOURCE_DIR})

find_package(glm REQUIRED)
find_package(SDL2 REQUIRED)

set(${PROJECT_NAME}_SOURCE
    ./app.cpp
    ./main.cpp
    ./sdlrenderer.cpp
    ./sdltexture.cpp
    ./sdlwindow.cpp
    ./triangle.cpp
    )

set(${PROJECT_NAME}_INCLUDE
    ./app.hpp
    ./log.hpp
    ./framebuffer.hpp
    ./sdlrenderer.hpp
    ./sdltexture.hpp
    ./sdlwindow.hpp
    ./triangle.hpp
    )

include_directories(${PROJECT_NAME}
    ${GLM_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIR}
    )

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_INCLUDE}
    ${${PROJECT_NAME}_SOURCE}
    )

target_link_libraries(${PROJECT_NAME} PRIVATE
    PRIVATE ${SDL2_LIBRARY}
    )

