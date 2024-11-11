#pragma once
#include "System.h"
#include "../Mesh/Mesh.h"
#include "../Components/TransformComponent.h"
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EntityManager.h"

class RenderSystem : public System
{
public:


    RenderSystem(ComponentManager& componentManager, EntityManager& EntityManager) : mComponentManager(componentManager), mEntityManager(EntityManager){};

    void Update(float deltaTime) override;

    void RenderEntity(std::vector<Entity>& entities);

    unsigned int mShaderProgram;

private:
    ComponentManager& mComponentManager;
    EntityManager& mEntityManager;
};
