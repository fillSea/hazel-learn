cmake_minimum_required(VERSION 3.20)
project(Hazel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 生成 compile_commands.json 供 clangd 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ============ 第三方依赖 ============

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
# add_subdirectory(Hazel/vendor/GLFW)

# Glad
# add_library(Glad STATIC
#     Hazel/vendor/Glad/src/glad.c
# )
# target_include_directories(Glad PUBLIC Hazel/vendor/Glad/include)

# imgui
# file(GLOB IMGUI_SOURCES
#     Hazel/vendor/imgui/*.cpp
#     Hazel/vendor/imgui/*.h
# )
# add_library(imgui STATIC ${IMGUI_SOURCES})
# target_include_directories(imgui PUBLIC Hazel/vendor/imgui)

# glm (header-only)
# add_library(glm INTERFACE)
# target_include_directories(glm INTERFACE Hazel/vendor/glm)

# spdlog (header-only)
# add_library(spdlog INTERFACE)
# target_include_directories(spdlog INTERFACE Hazel/vendor/spdlog/include)

# stb_image (后期需要，纹理加载时添加)
# add_library(stb_image STATIC Hazel/vendor/stb_image/stb_image.cpp)
# target_include_directories(stb_image PUBLIC Hazel/vendor/stb_image)

# yaml-cpp (后期需要，场景序列化时添加)
# add_subdirectory(Hazel/vendor/yaml-cpp)

# entt (后期需要，ECS 时添加，header-only)
# add_library(entt INTERFACE)
# target_include_directories(entt INTERFACE Hazel/vendor/entt/include)

# ImGuizmo (后期需要，编辑器 Gizmo 时添加)
# add_library(ImGuizmo STATIC Hazel/vendor/ImGuizmo/ImGuizmo.cpp)
# target_include_directories(ImGuizmo PUBLIC Hazel/vendor/ImGuizmo)

# Box2D (后期需要，物理系统时添加)
# add_subdirectory(Hazel/vendor/Box2D)

# ============ Hazel 引擎核心库 ============

file(GLOB_RECURSE HAZEL_SOURCES CONFIGURE_DEPENDS
    Hazel/src/*.cpp
    Hazel/src/*.h
)

add_library(Hazel SHARED ${HAZEL_SOURCES})

target_include_directories(Hazel PUBLIC
    Hazel/src
    # Hazel/vendor/spdlog/include
    # Hazel/vendor/GLFW/include
    # Hazel/vendor/Glad/include
    # Hazel/vendor/imgui
    # Hazel/vendor/glm
    # Hazel/vendor/entt/include        # 后期添加
    # Hazel/vendor/yaml-cpp/include    # 后期添加
    # Hazel/vendor/ImGuizmo            # 后期添加
)

# target_link_libraries(Hazel PUBLIC
    # glfw
    # Glad
    # imgui
    # glm
    # spdlog
    # opengl32
    # stb_image     # 后期添加
    # yaml-cpp      # 后期添加
    # entt          # 后期添加
    # ImGuizmo      # 后期添加
    # Box2D         # 后期添加
# )

target_compile_definitions(Hazel
    PUBLIC
        HZ_PLATFORM_WINDOWS
    PRIVATE
        HZ_BUILD_DLL
)

# target_compile_definitions(Hazel PUBLIC
#     GLFW_INCLUDE_NONE
#     $<$<CONFIG:Debug>:HZ_DEBUG>
#     $<$<CONFIG:Release>:HZ_RELEASE>
# )

# 预编译头（视频后期会引入，初期可以不加）
# target_precompile_headers(Hazel PRIVATE Hazel/src/hzpch.h)

# ============ Sandbox 应用 ============

file(GLOB_RECURSE SANDBOX_SOURCES CONFIGURE_DEPENDS
    Sandbox/src/*.cpp
    Sandbox/src/*.h
)

add_executable(Sandbox ${SANDBOX_SOURCES})

target_include_directories(Sandbox PRIVATE
    Hazel/src
#     # Hazel/vendor/spdlog/include
#     # Hazel/vendor/glm
)

target_link_libraries(Sandbox PRIVATE Hazel)

# ============ Hazelnut 编辑器（后期添加） ============

# file(GLOB_RECURSE HAZELNUT_SOURCES
#     Hazelnut/src/*.cpp
#     Hazelnut/src/*.h
# )
# add_executable(Hazelnut ${HAZELNUT_SOURCES})
# target_include_directories(Hazelnut PRIVATE
#     Hazel/src
#     Hazel/vendor/spdlog/include
#     Hazel/vendor/glm
#     Hazel/vendor/entt/include
#     Hazel/vendor/ImGuizmo
# )
# target_link_libraries(Hazelnut PRIVATE Hazel)
