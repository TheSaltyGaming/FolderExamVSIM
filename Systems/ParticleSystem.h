// ParticleSystem.h
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class ParticleSystem {
public:
    ParticleSystem(int maxParticles);
    void update(float deltaTime);
    void render(unsigned ShaderProgram);
    void emit(const glm::vec3& position, float lifeSpan);
    void setupBuffers();

private:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
    std::vector<float> lifeSpans;
    int maxParticles;
    float gravity = 9.81f;
    unsigned int VAO, VBO;
};

#endif // PARTICLESYSTEM_H