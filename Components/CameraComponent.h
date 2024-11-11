#pragma once
#include "glm/vec3.hpp"

struct CameraComponent {
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float yaw;
    float pitch;
};
