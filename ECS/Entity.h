#pragma once
#include <memory>

class ComponentManager;

class Entity {
public:

    Entity();

    Entity(ComponentManager& manager, int id) : id(id), componentManager(manager) {}

    unsigned int GetId() const { return id; }


private:
    unsigned int id;
    ComponentManager& componentManager;
};

const unsigned int MAX_ENTITIES = 5000;