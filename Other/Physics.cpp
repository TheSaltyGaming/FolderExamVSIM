//
// Created by Anders on 22.11.2024.
//

#include "Physics.h"

#include "Collision.h"
#include "../Math.h"

Math pMath;
Collision pCollision;

void Physics::UpdateBall(std::vector<Mesh *> &balls, Mesh &terrain, float deltaTime, TerrainGrid tGrid)
{
    for (auto ball : balls) {
        UpdateBallPhysics(*ball, terrain, tGrid, deltaTime);
    }

    // Ball-to-ball collisions
    for (size_t i = 0; i < balls.size(); i++) {
        for (size_t j = i + 1; j < balls.size(); j++) {
            pCollision.SphereCollision(balls[i], balls[j]);
        }
    }
}

void Physics::UpdateBallPhysics(Mesh& ball, Mesh &terrain, TerrainGrid &grid, float deltaTime)
{
    glm::vec3 oldPosition = ball.globalPosition;
    ball.velocity.y += GRAVITY * deltaTime;
    glm::vec3 newPosition = oldPosition + ball.velocity * deltaTime;

    float terrainHeight = grid.GetTerrainHeight(newPosition, grid, terrain.vertices, terrain.indices);

    if (terrainHeight != -1) {
        float ballRadius = ball.Radius * ball.globalScale.x;
        float targetHeight = terrainHeight + ballRadius;

        if (newPosition.y < targetHeight) {
            float penetrationDepth = targetHeight - newPosition.y;
            newPosition.y = targetHeight;

            if (ball.velocity.y < 0) {
                ball.velocity.y = -ball.velocity.y * 0.5f;
                ball.velocity.x *= 0.98f;
                ball.velocity.z *= 0.98f;
            }
        }
    }

    ball.globalPosition = newPosition;
}
