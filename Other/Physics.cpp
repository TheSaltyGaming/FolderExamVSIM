//
// Created by Anders on 22.11.2024.
//

#include "Physics.h"

#include "Collision.h"
#include "../Math.h"

Math pMath;
Collision pCollision;

void Physics::UpdateBall(std::vector<Mesh *> &balls, const Mesh &terrain, float deltaTime)
{

    for (auto ball : balls) {
        UpdateBallPhysics(*ball, terrain, deltaTime);
    }


    for (size_t i = 0; i < balls.size(); i++) {
        for (size_t j = i + 1; j < balls.size(); j++) {
            pCollision.SphereCollision(balls[i], balls[j]);
        }
    }
}

void Physics::UpdateBallPhysics(Mesh &ball, const Mesh &terrain, float deltaTime)
{
    // Store previous position for interpolation
    glm::vec3 oldPosition = ball.globalPosition;

    // Update velocity with gravity
    ball.velocity.y += GRAVITY * deltaTime;

    // Calculate new position based on velocity
    glm::vec3 newPosition = oldPosition + ball.velocity * deltaTime;

    // Find terrain height at new xz position
    glm::vec3 surfacePoint = pMath.MapCameraToSurface(newPosition, terrain);

    if (surfacePoint.y != -1) {
        float ballRadius = ball.Radius * ball.globalScale.x;
        float targetHeight = surfacePoint.y + ballRadius;

        // If ball is below target height (colliding with terrain)
        if (newPosition.y < targetHeight) {
            // Apply bounce
            float penetrationDepth = targetHeight - newPosition.y;
            newPosition.y = targetHeight;

            if (ball.velocity.y < 0) {
                // Bounce with energy loss
                ball.velocity.y = -ball.velocity.y * 0.5f;

                // Apply friction to horizontal velocity when bouncing
                ball.velocity.x *= 0.98f;
                ball.velocity.z *= 0.98f;
            }
        }
    }

    // Update position
    ball.globalPosition = newPosition;
}
