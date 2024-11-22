//
// Created by Anders on 22.11.2024.
//

#ifndef PHYSICS_H
#define PHYSICS_H
#include "Mesh.h"


class Physics {
public:

    void Update();

    void UpdateBall(std::vector<Mesh*>& balls, const Mesh& terrain, float deltaTime);
    void UpdateBallPhysics(Mesh& ball, const Mesh& terrain, float deltaTime);

    const float GRAVITY = -9.81f;

};



#endif //PHYSICS_H
