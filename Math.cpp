#include "Math.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>

Math::Math()
{
    
}

/// \brief Moves a mesh towards a target mesh
/// \param mesh the mesh that will be moved
/// \param target destination mesh
/// \param speed movement speed
void Math::moveObject(Mesh* mesh, Mesh* target, float speed)
{
    
    // Get the current target point
    glm::vec3 readposition = mesh->globalPosition;


    glm::vec3 targetPoint = target->globalPosition;

    // Check if the NPC is close enough to the target point
    float distanceToTarget = glm::distance(mesh->globalPosition, targetPoint);
    if (distanceToTarget < 0.1f) {
        
        return;
    }


    // Calculate the direction vector from the NPC to the target point
    glm::vec3 direction = glm::normalize(targetPoint - readposition);

    // Move NPC towards target
    glm::vec3 tempposition = mesh->globalPosition;
    tempposition += direction * speed;
    mesh->globalPosition = tempposition;
    
    }

std::vector<Vertex> Math::loadPointCloud(const std::string& filename) {
    std::vector<Vertex> points;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return points;
    }

    // Read the first line to get the total number of points
    std::getline(file, line);
    int totalPoints = std::stoi(line);

    glm::vec3 minPoint(FLT_MAX);
    glm::vec3 maxPoint(-FLT_MAX);

    int currentPoint = 0;
    int lastProgress = -1;
    while (std::getline(file, line)) {
        float x, y, z;
        if (sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z) == 3 || sscanf_s(line.c_str(), "%f\t%f\t%f", &x, &y, &z) == 3) {
            float xf = x;
            float yf = z;
            float zf = y;
            Vertex point(glm::vec3(xf, yf, zf), glm::vec3(0.f), glm::vec3(0.f));

            minPoint = glm::min(minPoint, point.Position);
            maxPoint = glm::max(maxPoint, point.Position);

            points.push_back(point);
            currentPoint++;

            // Update progress bar every 10%
            int progress = (int)((float)currentPoint / totalPoints * 100);
            if (progress / 10 > lastProgress / 10) {
                lastProgress = progress;
                std::cout << "\rProgress: " << std::setw(6) << std::fixed << std::setprecision(2) << progress << "%";
                std::cout.flush();
            }
        }
    }

    file.close();
    std::cout << std::endl; // Move to the next line after progress bar

    // Translate points to move the minimum point to the origin
    for (auto &point : points) {
        point.Position -= minPoint;
        point.Position *= 0.1f;
        point.Position.x *= -1;
    }

    return points;
}

std::vector<TriangleStruct> Math::ExtractTriangles(const Mesh &mesh)
{
    std::vector<TriangleStruct> triangles;
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        glm::vec3 v0 = mesh.vertices[mesh.indices[i]].Position;
        glm::vec3 v1 = mesh.vertices[mesh.indices[i + 1]].Position;
        glm::vec3 v2 = mesh.vertices[mesh.indices[i + 2]].Position;
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        triangles.emplace_back(v0, v1, v2, normal);
    }
    return triangles;
}

glm::vec3 Math::MapCameraToSurface(const glm::vec3& cameraPos, const Mesh& surfaceMesh) {
    std::vector<TriangleStruct> triangles = ExtractTriangles(surfaceMesh);

    float heightSum = 0.0f;
    int count = 0;

    for (const auto& triangle : triangles) {
        float height = barycentricCoordinates(triangle.v0, triangle.v1, triangle.v2, cameraPos);
        if (height != -1) {  // Only count valid heights
            heightSum += height;
            count++;
        }
    }

    if (count > 0) {
        float averageHeight = heightSum / count;
        return glm::vec3(cameraPos.x, averageHeight, cameraPos.z);
    }

    return glm::vec3(-1);  // Return -1 only if no valid triangles found
}

float Math::calculateNormal(glm::vec3 &vector1, glm::vec3 &vector2)
{
    return vector1.x * vector2.z - vector2.x * vector1.z;
}

void Math::moveObject(Mesh* mesh, std::vector<glm::vec3> pointList, float speed)
{
    // Get the current target point
    glm::vec3 readposition = mesh->globalPosition;


    glm::vec3 targetPoint = pointList[pointIndex+1];


    // Calculate the direction vector from the NPC to the target point
    glm::vec3 direction = glm::normalize(targetPoint - readposition);

    // Move NPC towards target
    glm::vec3 tempposition = mesh->globalPosition;
    tempposition += direction * speed;
    mesh->globalPosition = tempposition;

    // Check if the NPC is close enough to the target point
    float distanceToTarget = glm::distance(mesh->globalPosition, targetPoint);
    if (distanceToTarget < 0.1f) { // Threshold value
        // Move to the next point
        pointIndex++;

        // Loop back to start
        if (pointIndex >= pointList.size()-1) {
            pointIndex = 0;
        }
    }
}

float Math::barycentricCoordinates(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 objectPos)
{
    // Project to XZ plane
    glm::vec2 pointXZ(objectPos.x, objectPos.z);
    glm::vec2 aXZ(A.x, A.z);
    glm::vec2 bXZ(B.x, B.z);
    glm::vec2 cXZ(C.x, C.z);

    // Check if point is within triangle bounds (rough AABB check first)
    float minX = std::min({aXZ.x, bXZ.x, cXZ.x});
    float maxX = std::max({aXZ.x, bXZ.x, cXZ.x});
    float minZ = std::min({aXZ.y, bXZ.y, cXZ.y});
    float maxZ = std::max({aXZ.y, bXZ.y, cXZ.y});

    // Add some margin for floating point precision
    const float MARGIN = 0.1f;
    if (pointXZ.x < minX - MARGIN || pointXZ.x > maxX + MARGIN ||
        pointXZ.y < minZ - MARGIN || pointXZ.y > maxZ + MARGIN) {
        return -1;
        }

    // Calculate barycentric coordinates in 2D
    glm::vec2 v0 = bXZ - aXZ;
    glm::vec2 v1 = cXZ - aXZ;
    glm::vec2 v2 = pointXZ - aXZ;

    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);

    float denom = d00 * d11 - d01 * d01;
    if (abs(denom) < 0.0001f) return -1;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    //Used for floating point errors, but i probably don't have to. Looks elegant though right? :)
    const float EPSILON = 0.01f;
    if (u >= -EPSILON && v >= -EPSILON && w >= -EPSILON &&
        u <= 1 + EPSILON && v <= 1 + EPSILON && w <= 1 + EPSILON)
    {
        float height = A.y * u + B.y * v + C.y * w;
        return height;
    }
    return -1;
}

float Math::calculateHeightUsingBarycentric2(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C,
    const glm::vec3& P)
{
    return -1;
}

bool Math::isPointAboveTriangle(const TriangleStruct& triangle, const glm::vec3& point)
{

    // glm::vec3 baryCoords = barycentricCoordinates(triangle.v0, triangle.v1, triangle.v2, point, TODO);
    //
    //
    // if (baryCoords.x >= 0 && baryCoords.x <= 1 &&
    //     baryCoords.y >= 0 && baryCoords.y <= 1 &&
    //     baryCoords.z >= 0 && baryCoords.z <= 1)
    // {
    //
    //     if (point.y > triangle.v0.y && point.y > triangle.v1.y && point.y > triangle.v2.y)
    //     {
    //         return true;
    //     }
    // }

    return false;
}

glm::vec3 Math::RandomVec3(float min, float max)
{
    float x = min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    float y = min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    float z = min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    return glm::vec3(x, y, z);
}

glm::vec3 Math::deCasteljau(std::vector<glm::vec3> points, float t)
{
    for (int i = 0; i < points.size() - 1; i++)
    {
        for (int j = 0; j < points.size() - i - 1; j++)
        {
            points[j] = points[j] * (1 - t)  + points[j + 1] * t;
        }
    }
    return points[0];
}

glm::vec3 Math::lerp(const glm::vec3 &start, const glm::vec3 &end, float t) {
    return start + t * (end - start);
}
