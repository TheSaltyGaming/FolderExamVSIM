//
// Created by Anders on 04.12.2024.
//

#include "ParticleSystem.h"

#include <algorithm>
#include <glad/glad.h>
#include <glm/gtc/random.hpp>

ParticleSystem::ParticleSystem(int maxParticlesInput)
{
    maxParticles = maxParticlesInput;
}

void ParticleSystem::update(float deltaTime)
{
    for (int i = 0; i < particles.size(); ++i)
    {
        particles[i].velocity.y -= gravity/8 * deltaTime;
        particles[i].position += particles[i].velocity * deltaTime;
        particles[i].lifeSpan -= deltaTime;

        if (particles[i].lifeSpan <= 0.0f)
        {
            particles.erase(particles.begin() + i);
            --i;
        }
    }
}

void ParticleSystem::render(unsigned ShaderProgram)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), particles.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, particles.size());
    glBindVertexArray(0);
}

void ParticleSystem::emit(glm::vec3 &position, glm::vec3 &velocity, float lifeSpan, glm::vec4& color)
{
    int numParticle = 50;
    for (int i = 0; i < numParticle; ++i)
    {
        if (particles.size() < maxParticles)
        {
            Particle p;
            p.position = position;
            p.velocity = glm::sphericalRand(4.f);
            p.lifeSpan = lifeSpan;
            p.color = color;
            particles.push_back(p);
        }
    }
}

void ParticleSystem::setupBuffers()
{
    //simplified the setup in my mesh class, since I'm just going to be drawing me some points here
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(Particle), nullptr, GL_STREAM_DRAW); // Dynamic draw

    glEnableVertexAttribArray(0); // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));

    glEnableVertexAttribArray(1); // Color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));

    glBindVertexArray(0);
}
