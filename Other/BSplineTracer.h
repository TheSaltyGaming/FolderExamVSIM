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

    const std::vector<Vertex>& GetPoints() const { return points; }

    void SetRecordInterval(float interval) {
        recordInterval = interval;
    }



private:
    std::vector<Vertex> points;
    float accumulatedTime;
    float recordInterval = 0.1f;





};



#endif //BSPLINETRACER_H
