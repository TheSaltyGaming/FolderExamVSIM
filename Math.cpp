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

std::vector<sTriangle> Math::delauneyTriangle(std::vector<Vertex> &vertices) {

}

std::vector<glm::vec3> Math::generateBSplineControlPoints(const Mesh& mesh, int subdivisions)
{
    std::vector<glm::vec3> controlPoints;

    // Iterate over each triangle in the mesh
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        // Access the three vertices of the triangle
        Vertex v0 = mesh.vertices[mesh.indices[i]];
        Vertex v1 = mesh.vertices[mesh.indices[i + 1]];
        Vertex v2 = mesh.vertices[mesh.indices[i + 2]];

        // Subdivide the triangle
        for (int u = 0; u <= subdivisions; ++u) {
            for (int v = 0; v <= subdivisions - u; ++v) {
                float a = static_cast<float>(u) / subdivisions;
                float b = static_cast<float>(v) / subdivisions;
                float c = 1.0f - a - b;

                // Barycentric combination to get the point
                glm::vec3 point = a * v0.Position + b * v1.Position + c * v2.Position;
                controlPoints.push_back(point);
            }
        }
    }
    //optimize shit or smth if needed
    return controlPoints;
}

void Math::evaluateBSplineSurface(const BSplineSurface &spline, std::vector<Vertex> &surfaceVertices,
    std::vector<unsigned int> &surfaceIndices, int resolutionU, int resolutionV)
{
    auto bernstein = [&](int i, float t) -> float {
        // Bernstein basisfunksjon
        switch (i) {
            case 0: return (1 - t) * (1 - t);
            case 1: return 2 * t * (1 - t);
            case 2: return t * t;
            default: return 0.0f;
        }
    };

    // Vertices are tasty om nom nom
    for (int u = 0; u <= resolutionU; ++u) {
        float t = static_cast<float>(u) / resolutionU;
        for (int v = 0; v <= resolutionV; ++v) {
            float s = static_cast<float>(v) / resolutionV;

            glm::vec3 position(0.0f);

            // surface positions
            for (int i = 0; i <= spline.degreeU; ++i) {
                float Bu = bernstein(i, t);
                for (int j = 0; j <= spline.degreeV; ++j) {
                    float Bv = bernstein(j, s);
                    int index = i * (spline.numControlPointsV) + j;
                    if (index < spline.controlPoints.size()) {
                        position += Bu * Bv * spline.controlPoints[index];
                    }
                }
            }

            Vertex vertex;
            vertex.Position = position;
            vertex.Color = glm::vec3(0.0f, 1.0f, 0.0f); //green bspline
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            surfaceVertices.push_back(vertex);
        }
    }

    // Indices are also tasty
    for (int u = 0; u < resolutionU; ++u) {
        for (int v = 0; v < resolutionV; ++v) {
            unsigned int i0 = u * (resolutionV + 1) + v;
            unsigned int i1 = (u + 1) * (resolutionV + 1) + v;
            unsigned int i2 = (u + 1) * (resolutionV + 1) + (v + 1);
            unsigned int i3 = u * (resolutionV + 1) + (v + 1);

            // Tri 1
            surfaceIndices.push_back(i0);
            surfaceIndices.push_back(i1);
            surfaceIndices.push_back(i2);

            // tri 2
            surfaceIndices.push_back(i0);
            surfaceIndices.push_back(i2);
            surfaceIndices.push_back(i3);
        }
    }

    // Are normals just the seasoning on top?
    for (size_t i = 0; i < surfaceIndices.size(); i += 3) {
        unsigned int idx0 = surfaceIndices[i];
        unsigned int idx1 = surfaceIndices[i + 1];
        unsigned int idx2 = surfaceIndices[i + 2];

        glm::vec3 v0 = surfaceVertices[idx1].Position - surfaceVertices[idx0].Position;
        glm::vec3 v1 = surfaceVertices[idx2].Position - surfaceVertices[idx0].Position;
        glm::vec3 normal = glm::normalize(glm::cross(v0, v1));

        surfaceVertices[idx0].Normal += normal;
        surfaceVertices[idx1].Normal += normal;
        surfaceVertices[idx2].Normal += normal;
    }

    // Normalize the accumulated normals
    for (auto& vertex : surfaceVertices) {
        vertex.Normal = glm::normalize(vertex.Normal);
    }
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

glm::vec3 Math::barycentricCoordinates(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 P)
{
    glm::vec3 v0 = B - A, v1 = C - A, v2 = P - A;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return glm::vec3(u, v, w);
}

float Math::calculateHeightUsingBarycentric2(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C,
    const glm::vec3& P)
{
    // Calculate barycentric coordinates for P within the triangle ABC.
    glm::vec3 baryCoords = barycentricCoordinates(A, B, C, P);

    // Interpolate Y cood at P.
    float height = A.y * baryCoords.x + B.y * baryCoords.y + C.y * baryCoords.z;
    return height;
}

bool Math::isPointAboveTriangle(const TriangleStruct& triangle, const glm::vec3& point)
{
    // Calculate barycentric coordinates for the point with respect to the triangle
    glm::vec3 baryCoords = barycentricCoordinates(triangle.v0, triangle.v1, triangle.v2, point);

    // Check if the point lies within the triangle
    if (baryCoords.x >= 0 && baryCoords.x <= 1 &&
        baryCoords.y >= 0 && baryCoords.y <= 1 &&
        baryCoords.z >= 0 && baryCoords.z <= 1)
    {
        // Check if the point is above the triangle
        if (point.y > triangle.v0.y && point.y > triangle.v1.y && point.y > triangle.v2.y)
        {
            return true;
        }
    }

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
