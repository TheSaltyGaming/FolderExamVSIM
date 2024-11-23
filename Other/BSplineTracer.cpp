//
// Created by Ander on 23.11.2024.
//

#include "BSplineTracer.h"

BSplineTracer::BSplineTracer(int degree) : degree(degree) {}

void BSplineTracer::AddPoint(const glm::vec3& point) {
    controlPoints.push_back(point);
}

float BSplineTracer::BSplineBasis(int i, int k, float t) const {
    if (k == 0) {
        std::vector<float> knots = GenerateKnotVector();
        return (t >= knots[i] && t < knots[i + 1]) ? 1.0f : 0.0f;
    }

    std::vector<float> knots = GenerateKnotVector();
    float denom1 = knots[i + k] - knots[i];
    float denom2 = knots[i + k + 1] - knots[i + 1];

    float c1 = (denom1 != 0) ? ((t - knots[i]) / denom1 * BSplineBasis(i, k - 1, t)) : 0;
    float c2 = (denom2 != 0) ? ((knots[i + k + 1] - t) / denom2 * BSplineBasis(i + 1, k - 1, t)) : 0;

    return c1 + c2;
}
glm::vec3 BSplineTracer::GetPoint(float t) const {
    if (controlPoints.size() < degree + 1) return glm::vec3(0.0f);

    std::vector<float> knots = GenerateKnotVector();
    glm::vec3 point(0.0f);

    // Scale t to fit the knot vector range
    t = t * (knots.back() - knots[degree]) + knots[degree];

    for (int i = 0; i < controlPoints.size(); i++) {
        float basis = BSplineBasis(i, degree, t);
        point += controlPoints[i] * basis;
    }

    return point;
}

std::vector<float> BSplineTracer::GenerateKnotVector() const {
    int n = controlPoints.size() - 1;
    int m = n + degree + 1;
    std::vector<float> knots(m + 1);

    // Uniformly spaced knots
    for (int i = 0; i <= m; i++) {
        if (i < degree + 1) knots[i] = 0.0f;
        else if (i > n) knots[i] = 1.0f;
        else knots[i] = (float)(i - degree) / (n - degree + 1);
    }

    return knots;
}

Mesh BSplineTracer::CreatePathMesh(float thickness, const glm::vec3& color) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Generate points along the curve
    const int numSegments = 50;
    std::vector<glm::vec3> pathPoints;

    for (int i = 0; i <= numSegments; i++) {
        float t = (float)i / numSegments;
        pathPoints.push_back(GetPoint(t));
    }

    // Create circular cross-sections along the path
    const int numSides = 8;
    for (int i = 0; i < pathPoints.size(); i++) {
        glm::vec3 forward = (i < pathPoints.size() - 1) ?
            glm::normalize(pathPoints[i + 1] - pathPoints[i]) :
            glm::normalize(pathPoints[i] - pathPoints[i - 1]);

        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        glm::vec3 up = glm::normalize(glm::cross(right, forward));

        for (int j = 0; j < numSides; j++) {
            float angle = (float)j / numSides * 2.0f * 3.14159f;
            glm::vec3 offset = right * cos(angle) + up * sin(angle);
            glm::vec3 position = pathPoints[i] + offset * thickness;
            glm::vec3 normal = glm::normalize(offset);

            vertices.push_back({position, normal, color});
        }
    }

    // Generate indices for triangles
    for (int i = 0; i < pathPoints.size() - 1; i++) {
        for (int j = 0; j < numSides; j++) {
            int j1 = (j + 1) % numSides;
            int base = i * numSides;

            indices.push_back(base + j);
            indices.push_back(base + numSides + j1);
            indices.push_back(base + numSides + j);

            indices.push_back(base + j);
            indices.push_back(base + j1);
            indices.push_back(base + numSides + j1);
        }
    }

    Mesh pathMesh;
    pathMesh.vertices = vertices;
    pathMesh.indices = indices;
    pathMesh.Setup();

    return pathMesh;
}

void BSplineTracer::Clear() {
    controlPoints.clear();
}