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

};



#endif //PHYSICS_H
