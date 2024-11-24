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

void Physics::UpdateBallPhysics(Mesh& ball, Mesh &terrain, TerrainGrid &grid, float deltaTime) {
    glm::vec3 oldPosition = ball.globalPosition;

    // Apply gravity to the y-velocity
    ball.velocity.y += GRAVITY * deltaTime;

    // Update position based on velocity
    glm::vec3 newPosition = oldPosition + ball.velocity * deltaTime;

    // Get the terrain height at the new position
    float terrainHeight = grid.GetTerrainHeight(newPosition, grid, terrain.vertices, terrain.indices);

    if (terrainHeight != -1) { // Assuming -1 signifies no terrain detected
        float ballRadius = ball.Radius * ball.globalScale.x;
        float targetHeight = terrainHeight + ballRadius;

        if (newPosition.y < targetHeight) { // Collision detected
            float penetrationDepth = targetHeight - newPosition.y;
            newPosition.y = targetHeight; // Correct the position to be on the terrain

            if (ball.velocity.y < 0) { // If moving downward
                // Reverse and dampen the y-velocity to simulate a bounce
                ball.velocity.y = -ball.velocity.y * 0.5f;

                // Determine the appropriate friction based on y-position
                float friction;
                if (newPosition.y < Y_THRESHOLD) {
                    friction = LOWER_BOUND_FRICTION; // High friction when y < 10
                } else {
                    friction = DEFAULT_FRICTION; // Low friction otherwise
                }

                // Apply friction to the horizontal velocities
                ball.velocity.x *= friction;
                ball.velocity.z *= friction;
            }
        }
    }

    // Update the ball's global position
    ball.globalPosition = newPosition;
}