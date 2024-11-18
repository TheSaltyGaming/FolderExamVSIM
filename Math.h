#pragma once
#include <string>

#include "Other/Camera.h"
#include "Mesh/Mesh.h"
#include "Vertex.h"

class Math
{
public:

    Math();
    
    void moveObject(Mesh* mesh, Mesh* target, float speed);

    int pointIndex = 0;
    
    void moveObject(Mesh* mesh, std::vector<glm::vec3> pointList, float speed);

    glm::vec3 barycentricCoordinates(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 P);

    float calculateHeightUsingBarycentric2(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec3& P);

    bool isPointAboveTriangle(const TriangleStruct& triangle, const glm::vec3& point);

    glm::vec3 RandomVec3(float min, float max);

    glm::vec3 deCasteljau(std::vector<glm::vec3> points, float t);

    glm::vec3 lerp(const glm::vec3& start, const glm::vec3& end, float t);

    std::vector<Vertex> loadPointCloud(const std::string& filename);

    std::vector<sTriangle> delauneyTriangle(std::vector<Vertex> &vertices);
};
