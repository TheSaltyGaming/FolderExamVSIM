//
// Created by Ander on 23.11.2024.
//

#ifndef BSPLINETRACER_H
#define BSPLINETRACER_H
#include <vector>
#include <glm/vec3.hpp>

#include "Mesh.h"


class BSplineTracer {
public:

    BSplineTracer(int degree = 3);

    // Add a position to the control points
    void AddPoint(const glm::vec3& point);

    // Get a point on the curve at parameter t (0 <= t <= 1)
    glm::vec3 GetPoint(float t) const;

    // Generate a mesh representing the path
    Mesh CreatePathMesh(float thickness = 0.1f, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));

    // Clear all control points
    void Clear();

    std::vector<glm::vec3> controlPoints;
private:

    int degree;

    // Calculate basis functions
    float BSplineBasis(int i, int k, float t) const;

    // Generate knot vector based on control points
    std::vector<float> GenerateKnotVector() const;

    // Helper function to find knot span
    int FindKnotSpan(float t, const std::vector<float>& knots) const;

};



#endif //BSPLINETRACER_H
