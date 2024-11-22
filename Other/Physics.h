//
// Created by Anders on 22.11.2024.
//

#ifndef PHYSICS_H
#define PHYSICS_H
#include "Mesh.h"


class Physics {
public:

    void BallPhysics(float deltaTime, Mesh& ball, Mesh& terrain);

    void UpdateBall(Mesh& ball, const Mesh& terrain, float deltaTime);

    const float GRAVITY = -9.81f;

};



#endif //PHYSICS_H
