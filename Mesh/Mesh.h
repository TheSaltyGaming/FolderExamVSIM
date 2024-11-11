#pragma once
#include <vector>
#include "../Vertex.h"
#include "../Components/TransformComponent.h"
#include "glm/fwd.hpp"
#include "glm/vec3.hpp"

enum MeshType {Cube, Triangle, Square, Pyramid, Sphere, Plane};

class Mesh : public Component
{
public:

    Mesh();
    Mesh(MeshType type, float radius, glm::vec3 color, TransformComponent* tComp);
    Mesh(MeshType type, float radius, int segments, glm::vec3 color, TransformComponent* tComp);
    

    void CreateCube(float radius, glm::vec3 color);
    void CreateTriangle(float radius, glm::vec3 color);
    void CreateSquare(float radius, glm::vec3 color);
    void CreatePyramid(float radius, glm::vec3 color);

    void CreateSphere2(float radius, int subdivisions, glm::vec3 color);

    void subDivide(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int n);
    
    void CreatePlane(float radius, glm::vec3 color);


    void Setup();
    void CalculateBoundingBox();
    void SetColor(glm::vec3 color);

    void Draw(unsigned int shaderProgram);

    glm::mat4 GetTransform();
    
    MeshType mType;

    bool markedForDeletion = false;
    int health = 2;


    
    bool pickupable = false;

    // glm::mat4 model = glm::mat4(1.0f);
    unsigned int VBO, VAO, EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 globalPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 globalRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 globalScale = glm::vec3(1.0f, 1.0f, 1.0f);
    
    glm::vec3 minVert = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 maxVert = glm::vec3(0.0f, 0.0f, 0.0f);

    void DrawBoundingBox(unsigned int ShaderProgram);

    void Physics(float deltaTime);

    glm::vec3 ClosestPointOnAABB(glm::vec3& point) const;

    float mass = 1;
    float Radius = 1;
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 boundingBoxCorners [8];

    void CalculateInitialBoundingBox();

    glm::vec3 ObjectColor = glm::vec3(1.0f, 1.0f, 1.0f);

    TransformComponent* transform;
    
};
