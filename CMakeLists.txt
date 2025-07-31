cmake_minimum_required(VERSION 3.10.2)


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR};${CMAKE_MODULE_PATH};")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 20)

project(sw-renderer-2 VERSION 1.0.0)
add_definitions(-D_USE_MATH_DEFINES)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${CMAKE_SOURCE_DIR})

find_package(glm CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

set(${PROJECT_NAME}_SOURCE
    ./app.cpp
    ./camera.cpp
    ./gamecontroller.cpp
    ./main.cpp
    ./mesh.cpp
    ./sdlrenderer.cpp
    ./sdltexture.cpp
    ./sdlwindow.cpp
    ./triangle.cpp
    )

set(${PROJECT_NAME}_INCLUDE
    ./app.hpp
    ./camera.hpp
    ./gamecontroller.hpp
    ./log.hpp
    ./framebuffer.hpp
    ./mesh.hpp
    ./sdlrenderer.hpp
    ./sdltexture.hpp
    ./sdlwindow.hpp
    ./triangle.hpp
    ./teapot.hpp
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

#message("$<TARGET_FILE_DIR:sw-renderer-2>")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/gamecontrollerdb.txt" $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

