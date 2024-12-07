// ParticleSystem.cpp
#include "ParticleSystem.h"
#include <algorithm>
#include <glad/glad.h>
#include <glm/gtc/random.hpp>

ParticleSystem::ParticleSystem(int maxParticlesInput): maxParticles(maxParticlesInput)
{
    positions.reserve(maxParticles);
    velocities.reserve(maxParticles);
    lifeSpans.reserve(maxParticles);
}

void ParticleSystem::update(float deltaTime) {
    for (size_t i = 0; i < lifeSpans.size(); ++i) {
        velocities[i].y -= gravity / 8.0f * deltaTime;
        positions[i] += velocities[i] * deltaTime;
        lifeSpans[i] -= deltaTime;

        if (lifeSpans[i] <= 0.0f) {

            size_t last = lifeSpans.size() - 1;
            positions[i] = positions[last];
            velocities[i] = velocities[last];
            lifeSpans[i] = lifeSpans[last];
            positions.pop_back();
            velocities.pop_back();
            lifeSpans.pop_back();
            --i;
        }
    }
}

void ParticleSystem::render(unsigned ShaderProgram) {
    glUseProgram(ShaderProgram);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec3), positions.data());

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, positions.size());
    glBindVertexArray(0);
}

void ParticleSystem::emit(const glm::vec3& position, float lifeSpan) {
    int numParticles = 50;
    for (int i = 0; i < numParticles; ++i) {
        if (positions.size() < static_cast<size_t>(maxParticles)) {
            positions.emplace_back(position);
            velocities.emplace_back(glm::sphericalRand(4.0f));
            lifeSpans.emplace_back(lifeSpan);
        }
    }
}

void ParticleSystem::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    //VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec3), nullptr, GL_STREAM_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
}