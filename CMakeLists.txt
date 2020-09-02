cmake_minimum_required(VERSION 3.5)

project(Snake)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)# -D_DEBUG)

file(GLOB_RECURSE SOURCES "Snake/src/*.cpp")
file(GLOB_RECURSE HEADERS "Snake/src/*.h")

set(INCLUDE_DIRS 
    Snake/src
    Snake/vendor/lua
    Snake/vendor/stb
    Snake/vendor/tinygltf
    Snake/vendor/glm
    Snake/vendor/entt/src
    Snake/vendor/json/include
    Snake/vendor/lua-bridge/Source
    Snake/vendor/ImGuizmo
    Snake/vendor/bullet/src
    Snake/vendor/glfw/include
    Snake/vendor/glad/include
    Snake/vendor/freetype/include
)

add_executable(Snake ${SOURCES} ${HEADERS})

target_include_directories(Snake PRIVATE ${INCLUDE_DIRS})
target_link_libraries(Snake 
    debug   Snake/vendor/libs/debug/Lua52                     optimized   Snake/vendor/libs/debug/Lua52
    debug   Snake/vendor/libs/debug/Bullet3Collision_Debug    optimized   Snake/vendor/libs/release/Bullet3Collision
    debug   Snake/vendor/libs/debug/Bullet3Common_Debug       optimized   Snake/vendor/libs/release/Bullet3Common
    debug   Snake/vendor/libs/debug/Bullet3Dynamics_Debug     optimized   Snake/vendor/libs/release/Bullet3Dynamics
    debug   Snake/vendor/libs/debug/Bullet3Geometry_Debug     optimized   Snake/vendor/libs/release/Bullet3Geometry
    debug   Snake/vendor/libs/debug/BulletCollision_Debug     optimized   Snake/vendor/libs/release/BulletCollision
    debug   Snake/vendor/libs/debug/BulletDynamics_Debug      optimized   Snake/vendor/libs/release/BulletDynamics
    debug   Snake/vendor/libs/debug/LinearMath_Debug          optimized   Snake/vendor/libs/release/LinearMath
    debug   Snake/vendor/libs/debug/freetyped                 optimized   Snake/vendor/libs/release/freetype
    d3d11.lib
    D3DCompiler.lib
)

message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Debug/Debug)

set_target_properties(
    Snake PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)