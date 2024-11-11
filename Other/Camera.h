#pragma once
#include <vector>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>

class Camera
{
public:

    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    glm::vec3 direction;

    float yaw = -90.0f;
    float pitch = 0.f;
    float roll = 0.f;
    void tick();
    


    
};
