#pragma once
#include "Component.h"
#include "glm/vec3.hpp"

struct TransformComponent : public Component {
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    //TODO: Consider making separate component or smth
    glm::vec3 velocity{0.0f, 0.0f, 0.0f};
};
