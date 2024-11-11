#pragma once
#include "glm/vec3.hpp"

struct Vertex
{
    
    Vertex() = default;
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec3 color) : Position(position), Normal(normal), Color(color) { };

    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Color;
};


struct TriangleStruct {
    glm::vec3 v0, v1, v2, normal;

    TriangleStruct(const glm::vec3& vertex0, const glm::vec3& vertex1, const glm::vec3& vertex2, const glm::vec3& normal1)
    : v0(vertex0), v1(vertex1), v2(vertex2), normal(normal1) {}

    TriangleStruct() : v0(0.0f), v1(0.0f), v2(0.0f) {}
};