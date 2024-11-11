#pragma once

class Mesh;

class Collision
{
public:

    Collision();

    bool AABBCollision(Mesh* mesh1, Mesh* mesh2);

    bool SphereCollision(Mesh* mesh1, Mesh* mesh2);

    bool SphereToAABBCollision(Mesh* mesh1, Mesh* mesh2);
    
};
