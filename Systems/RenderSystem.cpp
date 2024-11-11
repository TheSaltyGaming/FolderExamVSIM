#include "RenderSystem.h"

#include <iostream>

#include "../ECS/ComponentManager.h"
#include "../Mesh/Mesh.h"
#include "glad/glad.h"
#include "glm/fwd.hpp"


void RenderSystem::Update(float deltaTime)
{
	if (!mShaderProgram) {
		std::cout << "Shader program not set!" << std::endl;
		return;
	}
	std::vector<Entity> entitiesWithMesh = mEntityManager.GetAllEntitiesWithComponent<Mesh>();
	RenderEntity(entitiesWithMesh);

}

void RenderSystem::RenderEntity(std::vector<Entity>& entities)
{
	for (const Entity& entity : entities) {
		// Retrieve the MeshComponent
		std::shared_ptr<Mesh> meshComponent = mComponentManager.GetComponent<Mesh>(entity.GetId());
		// If there's a valid MeshComponent, we draw it
		if (meshComponent) {
			meshComponent->Draw(mShaderProgram);
		}
	}
}
