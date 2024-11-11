#include "EntityManager.h"
#include "Entity.h"


EntityManager::EntityManager(ComponentManager& compManager) : componentManager(compManager) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        Entity entity(componentManager, i);
        mAvailableEntities.push(entity);
    }
}

Entity EntityManager::CreateEntity()
{
    assert(mLivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

    Entity entity = mAvailableEntities.front();
    mAvailableEntities.pop();
    ++mLivingEntityCount;

    return entity;
}

void EntityManager::DestroyEntity(Entity entity)
{
    assert(entity.GetId() < MAX_ENTITIES && "Entity out of range.");

    mAvailableEntities.push(entity);
    --mLivingEntityCount;
}

