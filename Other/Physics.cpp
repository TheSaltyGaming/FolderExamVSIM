//
// Created by Anders on 22.11.2024.
//

#include "Physics.h"

#include "../Math.h"

Math pMath;
void Physics::BallPhysics(float deltaTime, Mesh &ball, Mesh &terrain)
{
    // //give ball gravity
    // glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
    // ball.acceleration = gravity;
    //
    // ball.velocity += ball.acceleration * deltaTime;
    // ball.globalPosition += ball.velocity * deltaTime;
    //
    // glm::vec3 velocity = ball.velocity;
    // glm::vec3 position = ball.globalPosition;
    //
    // glm::vec3 terrainPoint = math.MapCameraToSurface(ball.globalPosition, terrain);
    //
    // if (terrainPoint.y != -1) {  // Valid terrain point found
    //     float penetrationDepth = (terrainPoint.y + radius) - position.y;
    //
    //     if (penetrationDepth > 0) {
    //         // Move ball up to terrain surface (plus radius)
    //         position.y = terrainPoint.y + radius;
    //
    //         // Calculate normal force (opposite to gravity component)
    //         glm::vec3 surfaceNormal = glm::vec3(0, 1, 0);  // Simplified - could calculate actual normal
    //         float normalMagnitude = glm::dot(-gravity, surfaceNormal);
    //         glm::vec3 normalForce = surfaceNormal * normalMagnitude;
    //
    //         // Apply friction
    //         float frictionCoeff = 0.3f;
    //         glm::vec3 frictionDir = -glm::normalize(glm::vec3(velocity.x, 0, velocity.z));
    //         glm::vec3 friction = frictionDir * normalMagnitude * frictionCoeff;
    //
    //         // Update velocity (reflect with some energy loss)
    //         if (glm::length(velocity) > 0.01f) {  // Only if moving significantly
    //             float restitution = 0.5f;  // 1.0 = perfect bounce, 0.0 = no bounce
    //             velocity.y = -velocity.y * restitution;
    //         } else {
    //             velocity = glm::vec3(0);  // Stop if nearly stationary
    //         }
    //     }
    // }
    //
    //     // Update mesh position
    //     ball.globalPosition = position;

}

void Physics::UpdateBall(Mesh &ball, const Mesh &terrain, float deltaTime)
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
                ball.velocity.x *= 0.95f;
                ball.velocity.z *= 0.95f;
            }
        }
    }

    // Update position
    ball.globalPosition = newPosition;
}
