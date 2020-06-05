cmake_minimum_required(VERSION 3.5)

project(Snake)

add_definitions(-DGLEW_STATIC -D_CRT_SECURE_NO_WARNINGS -D_DEBUG)

file(GLOB_RECURSE SOURCES "Snake/src/*.cpp")
file(GLOB_RECURSE HEADERS "Snake/src/*.h")

set(INCLUDE_DIRS 
    Snake/src
    Snake/vendor/glew
    Snake/vendor/lua
    Snake/vendor/stb
    Snake/vendor/tinygltf
    Snake/vendor/glm
    Snake/vendor/entt/src
    Snake/vendor/json/include
    Snake/vendor/glfw/include
    Snake/vendor/lua-bridge/Source
)

add_executable(Snake ${SOURCES} ${HEADERS})

target_include_directories(Snake PRIVATE ${INCLUDE_DIRS})
target_link_libraries(Snake opengl32 ../Snake/vendor/libs/glew32s ../Snake/vendor/libs/glfw/glfw3 ../Snake/vendor/libs/Lua52)

message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Debug/Debug)

set_target_properties(
    Snake PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)