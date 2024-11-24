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

    const float GRAVITY = -9.81f;
    const float DEFAULT_FRICTION = 0.98f;
    const float LOWER_BOUND_FRICTION = 0.7f;
    const float Y_THRESHOLD = 2.6f;
    const float MASS = 1.0f;
    const float NORMAL_FORCE_COEFFICIENT = 1.0f;

};



#endif //PHYSICS_H
