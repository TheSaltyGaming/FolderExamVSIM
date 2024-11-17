#pragma once
#include <glm/geometric.hpp>

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

struct Edge {
    size_t v1, v2;
    bool operator==(const Edge& other) const {
        return (v1 == other.v1 && v2 == other.v2) || (v1 == other.v2 && v2 == other.v1);
    }
};

struct sTriangle {
    size_t v0, v1, v2;
    glm::vec3 circumcenter;
    float circumradiusSquared;

    sTriangle(size_t v0, size_t v1, size_t v2)
        : v0(v0), v1(v1), v2(v2) {}

    // Function to check if a point is inside the circumcircle
    bool isPointInsideCircumcircle(const glm::vec3& p) const {
        glm::vec3 d = p - circumcenter;
        float distSq = glm::dot(d, d);
        return distSq < circumradiusSquared;
    }
};