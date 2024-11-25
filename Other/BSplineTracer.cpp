#include "BSplineTracer.h"
#include <iostream>

const int MAX_POINTS = 450;

void BSplineTracer::Update(float deltaTime, Mesh &mesh) {
    accumulatedTime += deltaTime;
    if (accumulatedTime >= recordInterval) {
        Vertex newPoint;
        newPoint.Position = mesh.globalPosition;
        newPoint.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        newPoint.Color = glm::vec3(1.0f, 0.0f, 0.0f);


        points.push_back(newPoint);
        //std::cout << "Added point. Total points: " << points.size() << std::endl;


        if (points.size() > MAX_POINTS) {
            points.erase(points.begin());
            //std::cout << "Removed oldest point. Total points: " << points.size() << std::endl;
        }

        accumulatedTime = 0.0f;

        //Only calculate if we have enough points
        if (points.size() >= 4) {
            CalculateSpline();
        }
    }
}

void BSplineTracer::Clear() {
    points.clear();
    splinePoints.clear();
    accumulatedTime = 0.0f;
    std::cout << "Cleared all points" << std::endl;
}

float BSplineTracer::BasisFunction(int i, int k, float t, const std::vector<float> &knots) {
    try {
        // Bounds checking
        if (i < 0 || i + k > knots.size()) {
            std::cout << "Index out of range in BasisFunction: i=" << i 
                      << " k=" << k << " knots.size()=" << knots.size() << std::endl;
            return 0.0f;
        }

        // Base case
        if (k == 1) {
            return (t >= knots[i] && t < knots[i + 1]) ? 1.0f : 0.0f;

        }

        float d1 = knots[i + k - 1] - knots[i];
        float d2 = knots[i + k] - knots[i + 1];
        float c1 = (d1 > 1e-6f) ? ((t - knots[i]) / d1) : 0.0f;
        float c2 = (d2 > 1e-6f) ? ((knots[i + k] - t) / d2) : 0.0f;
        return c1 * BasisFunction(i, k - 1, t, knots) + c2 * BasisFunction(i + 1, k - 1, t, knots);
    }
    catch (const std::exception& e) {
        std::cout << "Exception in BasisFunction: " << e.what() << std::endl;
        return 0.0f;
    }
}

void BSplineTracer::CalculateSpline() {
    if (points.size() < 4) { // Ensure at least the minimum required points
        std::cout << "Not enough points for spline calculation" << std::endl;
        return;
    }

    splinePoints.clear();

    // Generate uniform knot vector
    std::vector<float> knots;
    const int degree = 3; // Assuming degree is 3 for a cubic B-spline
    const int numKnots = points.size() + degree + 1;
    //std::cout << "Generating knot vector. Points: " << points.size()
              //<< " Degree: " << degree << " Total knots needed: " << numKnots << std::endl;

    for (int i = 0; i < numKnots; i++) {
        float knot = static_cast<float>(i) / (numKnots - 1);
        knots.push_back(knot);
    }

    //std::cout << "Generated " << knots.size() << " knots" << std::endl;

    // Calculate spline points
    const float step = 0.01f; // Define step size for spline calculation
    for (float t = 0.0f; t <= 1.0f; t += step) {
        Vertex splinePoint;
        splinePoint.Position = glm::vec3(0.0f);
        splinePoint.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        splinePoint.Color = glm::vec3(0.0f, 1.0f, 0.0f);

        float sum = 0.0f;
        for (int i = 0; i < points.size(); i++) {
            float basis = BasisFunction(i, degree + 1, t, knots);
            sum += basis;
            splinePoint.Position += points[i].Position * basis;
        }

        // Normalize if necessary
        if (sum > 1e-6f) {
            splinePoint.Position /= sum;
        }

        splinePoints.push_back(splinePoint);
    }

    //std::cout << "Generated " << splinePoints.size() << " spline points" << std::endl;
}