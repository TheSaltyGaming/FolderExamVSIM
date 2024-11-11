// EntityManager.h
#pragma once
#include <queue>
#include <array>
#include "Entity.h"
#include "ComponentManager.h"

class EntityManager
{
public:
    EntityManager(ComponentManager& compManager);

    Entity CreateEntity();
    void DestroyEntity(Entity entity);

    template <typename T>
    std::vector<Entity> GetAllEntitiesWithComponent() {
        std::vector<Entity> entitiesWithComponent;

        for (unsigned int i = 0; i < MAX_ENTITIES; ++i) {
            if (componentManager.HasComponent<T>(i)) {
                // Create an entity object and add it to the list if it has the component
                entitiesWithComponent.emplace_back(componentManager, i);
            }
        }

        return entitiesWithComponent;
    }

private:
    std::queue<Entity> mAvailableEntities{};
    uint32_t mLivingEntityCount{};
    ComponentManager& componentManager;
};