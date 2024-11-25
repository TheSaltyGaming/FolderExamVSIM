//
// Created by Ander on 23.11.2024.
//

#ifndef BSPLINETRACER_H
#define BSPLINETRACER_H
#include <vector>
#include <glm/vec3.hpp>

#include "BSplineTracer.h"
#include "BSplineTracer.h"
#include "Mesh.h"


class BSplineTracer {
public:

    BSplineTracer() : accumulatedTime(0.0f) {};

    void Update(float deltaTime, Mesh &mesh);

    void Clear();


    std::vector<Vertex> &GetSplinePoints()
    {
        //Clean up splinePoints
        if (splinePoints.size() > 0) {
            if (splinePoints.back().Position.x < 0.1f && splinePoints.back().Position.y < 0.1f && splinePoints.back().Position.z < 0.1f) {
                splinePoints.pop_back();
            }
            //remove the first point in the splinePoints vector if it is between 0.0f and 0.1f on the xyz
            if (splinePoints.front().Position.x < 0.1f && splinePoints.front().Position.y < 0.1f && splinePoints.front().Position.z < 0.1f) {
                splinePoints.erase(splinePoints.begin());
            }
        }
        return splinePoints;
    }


    const std::vector<Vertex>& GetControlPoints() const {
        return points;
    }




private:
    std::vector<Vertex> points;
    std::vector<Vertex> splinePoints;
    float accumulatedTime = 0.0f;
    float recordInterval = 0.4f;

    // B-spline parameters
    const int degree = 3;  // Cubic B-spline
    const float step = 0.01f;  // Step Size

    float BasisFunction(int i, int k, float t, const std::vector<float>& knots);

    void CalculateSpline();






};



#endif //BSPLINETRACER_H
