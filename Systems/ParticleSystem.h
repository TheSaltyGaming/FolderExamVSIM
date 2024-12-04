//
// Created by Anders on 04.12.2024.
//

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Shader;

struct Particle
{
    glm::vec3 position;
    glm::vec3 velocity;
    float lifeSpan;
    glm::vec4 color;
    glm::vec3 normal;
};



class ParticleSystem {

public:
    ParticleSystem(int maxParticles);

    void update(float deltaTime);
    void render(unsigned ShaderProgram);
    void emit(glm::vec3& position, glm::vec3& velocity, float lifeSpan, glm::vec4 &color);
    void setupBuffers();

private:
    std::vector<Particle> particles;
    int maxParticles;
    float gravity = 9.81f;

    unsigned int VAO, VBO;

};



#endif //PARTICLESYSTEM_H
