
#include "BSplineTracer.h"

void BSplineTracer::Update(float deltaTime, Mesh &mesh)
{
    accumulatedTime += deltaTime;

    if (accumulatedTime >= recordInterval)
    {
        Vertex newPoint;
        newPoint.Position = mesh.globalPosition;
        newPoint.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        newPoint.Color = glm::vec3(1.0f, 0.0f, 0.0f);

        points.push_back(newPoint);
        accumulatedTime = 0.0f;
    }
}

void BSplineTracer::Clear()
{
    points.clear();
    accumulatedTime = 0.0f;
}
