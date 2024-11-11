#pragma once
#include <cassert>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "Entity.h"
//TODO: Do i forward declare?
#include "../Components/Component.h"


class ComponentManager
{
public:

    template <typename T>
    void AddComponent(unsigned int entityID, std::shared_ptr<T> component) {
        const std::type_index type = typeid(T);
        mComponents[type][entityID] = component;
    }

    template <typename T>
    void RemoveComponent(unsigned int entityID) {
        const std::type_index type = typeid(T);
        auto typeIt = mComponents.find(type);
        if (typeIt != mComponents.end()) {
            typeIt->second.erase(entityID);
        }
    }

    template <typename T>
    std::shared_ptr<T> GetComponent(unsigned int entityID) {
        const std::type_index type = typeid(T);
        auto typeIt = mComponents.find(type);
        if (typeIt != mComponents.end()) {
            auto entityIt = typeIt->second.find(entityID);
            if (entityIt != typeIt->second.end()) {
                return std::static_pointer_cast<T>(entityIt->second);
            }
        }
        return nullptr;
    }

    template <typename T>
    bool HasComponent(unsigned int entityID) {
        const std::type_index type = typeid(T);
        auto typeIt = mComponents.find(type);
        if (typeIt != mComponents.end()) {
            return typeIt->second.find(entityID) != typeIt->second.end();
        }
        return false;
    }

private:

    // Component map. Outer map holds the type of the component, inner map holds the entity ID and the component instance.
    std::unordered_map<std::type_index, std::unordered_map<unsigned int, std::shared_ptr<Component>>> mComponents;
};
