//
// Created by Anders on 22.11.2024.
//

#ifndef PHYSICS_H
#define PHYSICS_H
#include "Mesh.h"
#include "TerrainGrid.h"


class Physics {
public:

    void Update();

    void UpdateBall(std::vector<Mesh*>& balls, Mesh &terrain, float deltaTime, TerrainGrid tGrid);
    void UpdateBallPhysics(Mesh& ball, Mesh &terrain, TerrainGrid &grid, float deltaTime);

    const float GRAVITY = -9.81f; // Assuming gravity is defined
    const float DEFAULT_FRICTION = 0.98f;
    const float LOWER_BOUND_FRICTION = 0.3f;
    const float Y_THRESHOLD = 2.6f; // The y-position threshold

};



#endif //PHYSICS_H
