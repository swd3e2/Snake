cmake_minimum_required(VERSION 3.5)

project(Snake)

add_definitions(-DGLEW_STATIC -D_CRT_SECURE_NO_WARNINGS)

set(SOURCES
    Snake/src/Main.cpp
    Snake/src/Input/InputManager.cpp
    Snake/src/Interface/imgui/imgui.cpp
    Snake/src/Interface/imgui/imgui_draw.cpp
    Snake/src/Interface/imgui/imgui_impl_glfw.cpp
    Snake/src/Interface/imgui/imgui_impl_opengl3.cpp
    Snake/src/Interface/imgui/imgui_widgets.cpp
    Snake/src/Model/Import/tinygltfimpl.cpp
    Snake/src/Model/Import/GltfImporter.cpp
)

set(HEADERS
    Snake/src/Camera.h
    Snake/src/Application.h
    Snake/src/Camera.h
    Snake/src/Common/Helper.h
    Snake/src/Components.h
    Snake/src/FileSystem/File.h
    Snake/src/Graphics/IndexBuffer.h
    Snake/src/Interface/imgui/imconfig.h
    Snake/src/Interface/imgui/imgui.h
    Snake/src/Interface/imgui/imgui_impl_glfw.h
    Snake/src/Interface/imgui/imgui_impl_opengl3.h
    Snake/src/Interface/imgui/imgui_internal.h
    Snake/src/Interface/imgui/imstb_rectpack.h
    Snake/src/Interface/imgui/imstb_textedit.h
    Snake/src/Interface/imgui/imstb_truetype.h
    Snake/src/Interface/MainInterface.h
    Snake/src/Model/Animation.h
    Snake/src/Model/ImportData.h
    Snake/src/Model/Import/GltfImporter.h
    Snake/src/Model/Material.h
    Snake/src/Model/Model.h
    Snake/src/Quad.h
    Snake/src/Registry.h
    Snake/src/RenderSystem.h
    Snake/src/Input/InputManager.h
    Snake/src/Graphics/ShaderBuffer.h
    Snake/src/Graphics/ShaderInputLayout.h
    Snake/src/Graphics/ShaderPipeline.h
    Snake/src/Graphics/Texture.h
    Snake/src/Graphics/vertex.h
    Snake/src/Graphics/VertexBuffer.h
)

set(INCLUDE_DIRS 
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
target_link_libraries(Snake opengl32 Snake/vendor/libs/glew32s Snake/vendor/libs/glfw/glfw3)

set_target_properties(
    Snake PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)