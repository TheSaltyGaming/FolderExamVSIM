#include "BsplineFunction.h"

float BsplineFunction::B2(float T, int i, const std::vector<float>& knots)
{

    if (knots[i] <= T && T < knots[i + 1]) {
        return (T - knots[i]) / (knots[i + 1] - knots[i]);
    } else if (knots[i + 1] <= T && T < knots[i + 2]) {
        return (knots[i + 2] - T) / (knots[i + 2] - knots[i + 1]);
    } else {
        return 0.0f;
    }
}

glm::vec3 BsplineFunction::evaluateBiquadratic(float u, float v, const std::vector<glm::vec3>& controlPoints,
    const std::vector<float>& uKnots, const std::vector<float>& vKnots, int n, int m)
{
    glm::vec3 point(0.0f);

    for (int i = 0; i < n ; ++i) {
        for (int j = 0; j < m ; ++j) {
            float Bu = B2(u , i, uKnots);
            float Bv = B2(v, j, vKnots);
            point += Bu * Bv * controlPoints[i * m  + j ];
        }
    }

    return point;
}

std::vector<glm::vec3> BsplineFunction::calculateBspline()
{
    std::vector<glm::vec3> CalculatedPoints;
    std::vector<glm::vec3> controlPoints = {
        {1.0f, 0.0f, 0.0f}, {1.0f, 3.0f, 3.0f}, {3.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 2.0f}, {2.0f, 3.0f, 2.0f}, {1.0f, 1.0f, 1.0f},
        {0.0f, 2.0f, 0.0f}, {2.0f, 2.0f, 0.0f}, {3.0f, 2.0f, 1.0f}
    };


    std::vector<float> uKnots = {0, 0, 0, 1, 3, 1, 3};
    std::vector<float> vKnots = {0, 0, 0, 1, 3, 1, 3};


    int resolution = 20;
    for (int i = 0; i <= resolution; ++i) {
        for (int j = 0; j <= resolution; ++j) {
            float u = static_cast<float>(i) / static_cast<float>(resolution);
            float v = static_cast<float>(j) / static_cast<float>(resolution);
            glm::vec3 point = evaluateBiquadratic(u, v, controlPoints, uKnots, vKnots, 3, 3);
            CalculatedPoints.emplace_back(point);
        }
    }
    return CalculatedPoints;

}

void BsplineFunction::CreateBspline(Mesh& bsplineMesh) {
  std::vector<glm::vec3> calculatedPoints = calculateBspline();
    int resolution = 20;

    bsplineMesh.vertices.clear();
    bsplineMesh.indices.clear();

    for (const auto& point : calculatedPoints) {
        bsplineMesh.vertices.emplace_back(
            Vertex{
                point,
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.9f, 0.9f, 0.9f)
            }
        );
    }


    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            int index = i * (resolution + 1) + j;
            // First triangle of the quad
            bsplineMesh.indices.emplace_back(index);
            bsplineMesh.indices.emplace_back(index + 1);
            bsplineMesh.indices.emplace_back(index + resolution + 1);

            // Second triangle of the quad
            bsplineMesh.indices.emplace_back(index + 1);
            bsplineMesh.indices.emplace_back(index + resolution + 2);
            bsplineMesh.indices.emplace_back(index + resolution + 1);
        }
    }


    for (auto& vertex : bsplineMesh.vertices) {
        vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }


    for (size_t i = 0; i < bsplineMesh.indices.size(); i += 3) {
        unsigned int ia = bsplineMesh.indices[i];
        unsigned int ib = bsplineMesh.indices[i + 1];
        unsigned int ic = bsplineMesh.indices[i + 2];

        glm::vec3 normal = glm::normalize(glm::cross(
            bsplineMesh.vertices[ib].Position - bsplineMesh.vertices[ia].Position,
            bsplineMesh.vertices[ic].Position - bsplineMesh.vertices[ia].Position
        ));

        bsplineMesh.vertices[ia].Normal += normal;
        bsplineMesh.vertices[ib].Normal += normal;
        bsplineMesh.vertices[ic].Normal += normal;
    }

    // Normalize all vertex normals
    for (auto& vertex : bsplineMesh.vertices) {
        vertex.Normal = glm::normalize(vertex.Normal);
    }


    bsplineMesh.Setup();
}
