#pragma once
#include "Mesh/Mesh.h"
class BsplineFunction
{
public:

    float B2(float T, int i, const std::vector<float>& knots);
    glm::vec3 evaluateBiquadratic(float u, float v, const std::vector<glm::vec3>& controlPoints,
                                  const std::vector<float>& uKnots, const std::vector<float>& vKnots, int n, int m);
    std::vector<glm::vec3> calculateBspline();
    void CreateBspline(Mesh& bsplineMesh);

};
