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

void Physics::UpdateBallPhysics(Mesh& ball, Mesh& terrain, TerrainGrid& grid, float deltaTime) {
    glm::vec3 oldPosition = ball.globalPosition;
    float ballRadius = ball.Radius * ball.globalScale.x;

    glm::vec3 netForce = glm::vec3(0.0f);

    // Gravityyyy
    netForce += glm::vec3(0.0f, MASS * GRAVITY, 0.0f);

    // Get terrain height and normal
    float terrainHeight = grid.GetTerrainHeight(oldPosition, grid, terrain.vertices, terrain.indices);
    glm::vec3 terrainNormal = grid.GetTerrainNormal(oldPosition, grid, terrain.vertices, terrain.indices);

    if (terrainHeight != -1) {
        float targetHeight = terrainHeight + ballRadius;

        // Normalkraft
        if (oldPosition.y <= targetHeight) {
            float penetrationDepth = targetHeight - oldPosition.y;
            glm::vec3 normalForce = terrainNormal * penetrationDepth * MASS * 200.0f;

            netForce += normalForce;

            // Friction
            float currentFriction = (oldPosition.y < Y_THRESHOLD) ? LOWER_BOUND_FRICTION : DEFAULT_FRICTION;

            if (glm::length(ball.velocity) > 0.01f) {
                glm::vec3 frictionDirection = -glm::normalize(ball.velocity);
                glm::vec3 frictionForce = frictionDirection * (1.0f - currentFriction) * glm::length(normalForce);
                netForce += frictionForce;
            }
        }
    }

    // Physic :)
    glm::vec3 acceleration = netForce / MASS;
    ball.velocity += acceleration * deltaTime;

    // Give ball friction when close to ground
    if (terrainHeight != -1) {
        float targetHeight = terrainHeight + ballRadius;
        if (oldPosition.y <= targetHeight + 0.1f) {
            float currentFriction = (oldPosition.y < Y_THRESHOLD) ? LOWER_BOUND_FRICTION : DEFAULT_FRICTION;
            ball.velocity.x *= currentFriction;
            ball.velocity.z *= currentFriction;
        }
    }

    // Update position
    glm::vec3 newPosition = oldPosition + ball.velocity * deltaTime + 0.5f * acceleration * deltaTime * deltaTime;

    // Final collision check
    terrainHeight = grid.GetTerrainHeight(newPosition, grid, terrain.vertices, terrain.indices);
    if (terrainHeight != -1) {
        float targetHeight = terrainHeight + ballRadius;
        if (newPosition.y < targetHeight) {
            newPosition.y = targetHeight;

            // Bounce with energy loss
            if (ball.velocity.y < 0) {
                ball.velocity.y = -ball.velocity.y * 0.4f;
            }
        }
    }

    ball.globalPosition = newPosition;
}