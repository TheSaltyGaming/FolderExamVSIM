cmake_minimum_required(VERSION 3.29)
project(FolderExamVSIM)

set(CMAKE_CXX_STANDARD 20)

set(LUA_LIB_DIR "${CMAKE_SOURCE_DIR}/Dependency/libs")
set(LUA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Dependency/includes/lua")

include_directories(Dependency/includes)
include_directories(Dependency/libs)
include_directories(ECS)
include_directories(Systems)
include_directories(Mesh)
include_directories(${LUA_INCLUDE_DIR})

add_subdirectory(Dependency/libs/glfw-3.4)

add_executable(FolderExamVSIM main.cpp
        Vertex.cpp
        Other/Collision.cpp
        glad.c
        Other/Camera.h
        Other/Camera.cpp
        Shader.h
        ShaderFileLoader.h
        Math.cpp
        Shader.cpp
        Math.h
        Other/Collision.h
        ShaderFileLoader.cpp
        Vertex.h
        Components/CameraComponent.cpp
        Components/CameraComponent.h
        Components/Component.cpp
        Components/Component.h
        Components/HealthComponent.cpp
        Components/HealthComponent.h
        Components/TransformComponent.cpp
        Components/TransformComponent.h
        ECS/ComponentManager.cpp
        ECS/ComponentManager.h
        ECS/Entity.cpp
        ECS/Entity.h
        ECS/EntityManager.cpp
        ECS/EntityManager.h
        Mesh/Mesh.cpp
        Mesh/Mesh.h
        Systems/RenderSystem.cpp
        Systems/RenderSystem.h
        Systems/System.cpp
        Systems/System.h
        BsplineFunction.cpp
        BsplineFunction.h
        Other/Physics.cpp
        Other/Physics.h
        Mesh/TerrainGrid.cpp
        Mesh/TerrainGrid.h
        Other/BSplineTracer.cpp
        Other/BSplineTracer.h
        Systems/ParticleSystem.cpp
        Systems/ParticleSystem.h
        Systems/ParticleSystem.cpp
        Other/LuaManager.cpp
        Other/LuaManager.h
        Components/MeshComponent.h
)

target_link_directories(FolderExamVSIM PRIVATE ${LUA_LIB_DIR})
target_link_libraries(FolderExamVSIM glfw lua54)

