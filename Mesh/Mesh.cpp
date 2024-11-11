#include "Mesh.h"
#include <iostream>
#include <glad/glad.h>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cfloat>


Mesh::Mesh() {
}

Mesh::Mesh(MeshType type, float radius, glm::vec3 color, TransformComponent* tcomp): transform(tcomp) {
    mType = type;
    switch (type) {
        case Cube:
            CreateCube(radius, color);
            break;
        case Triangle:
            CreateTriangle(radius, color);
            break;
        case Square:
            CreateSquare(radius, color);
            break;
        case Pyramid:
            CreatePyramid(radius, color);
            break;
        case Plane:
            CreatePlane(radius, color);
            break;
        case Sphere:
            std::cout << "Error, please specify number of segments" << std::endl;
            break;
    }
}

Mesh::Mesh(MeshType type, float radius, int subdivisions, glm::vec3 color, TransformComponent* tComp): transform(tComp) {
    mType = type;
    switch (type) {
        case Sphere:
            CreateSphere2(radius, subdivisions, color);
            break;
        default:
            std::cout << "Only sphere accepts int segments" << std::endl;
            break;
    }
}


void Mesh::CreateCube(float radius, glm::vec3 color)
{
    glm::vec3 color2 = color+glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 color3 = color-glm::vec3(0.2f, 0.2f, 0.2f);
    // Define cube vertices
    vertices = {
        // Positions           // Normals           // Color
        { glm::vec3(-radius, -radius, -radius), glm::vec3(-1.0f, -1.0f, -1.0f), color },
        { glm::vec3(radius, -radius, -radius), glm::vec3(1.0f, -1.0f, -1.0f), color },
        { glm::vec3(radius, radius, -radius), glm::vec3(1.0f, 1.0f, -1.0f), color },
        { glm::vec3(-radius, radius, -radius), glm::vec3(-1.0f, 1.0f, -1.0f), color2 },
        { glm::vec3(-radius, -radius, radius), glm::vec3(-1.0f, -1.0f, 1.0f), color2 },
        { glm::vec3(radius, -radius, radius), glm::vec3(1.0f, -1.0f, 1.0f), color3 },
        { glm::vec3(radius, radius, radius), glm::vec3(1.0f, 1.0f, 1.0f), color3 },
        { glm::vec3(-radius, radius, radius), glm::vec3(-1.0f, 1.0f, 1.0f), color3 }
    };

    // Define triangles
    indices = {
        0, 1, 2, 2, 3, 0,   // front
        1, 5, 6, 6, 2, 1,   // right
        7, 6, 5, 5, 4, 7,   // back
        4, 0, 3, 3, 7, 4,   // left
        4, 5, 1, 1, 0, 4,   // bottom
        3, 2, 6, 6, 7, 3    // top
    };

    Setup();
    CalculateInitialBoundingBox();
}

void Mesh::CreateTriangle(float radius, glm::vec3 color)
{
    // Define triangle vertices
    vertices = {
        // Positions           // Normals           // Color
        { glm::vec3(-radius, -radius, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), color },
        {  glm::vec3(radius, -radius, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), color },
        {  glm::vec3(0.0f,   radius, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), color }
    };

    // Define triangles
    indices = {
        0, 1, 2    // front
    };

    Setup();
}

void Mesh::CreateSquare(float radius, glm::vec3 color)
{
    // Define square vertices
    vertices = {
        // Positions           // Normals           // Color
        { glm::vec3(-radius, -radius, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), color },
        { glm::vec3(radius, -radius, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), color },
        { glm::vec3(radius, radius, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), color },
        { glm::vec3(-radius, radius, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), color }
    };

    // Define triangles
    indices = {
        0, 1, 2,    // first triangle
        2, 3, 0     // second triangle
    };

    Setup();
}

void Mesh::CreatePyramid(float radius, glm::vec3 color)
{
    // Vertices
    vertices = {
        // Positions           // Normals           // Color
        { glm::vec3(-radius, -radius, -radius), glm::vec3(-1.0f, -1.0f, -1.0f), color }, // 0
        { glm::vec3(radius, -radius, -radius), glm::vec3(1.0f, -1.0f, -1.0f), color }, // 1
        { glm::vec3(radius, -radius,  radius), glm::vec3(1.0f, -1.0f,  1.0f), color }, // 2
        { glm::vec3(-radius, -radius,  radius), glm::vec3(-1.0f, -1.0f,  1.0f), color }, // 3
        { glm::vec3(0.0f,   radius,  0.0f),   glm::vec3(0.0f,  1.0f,  0.0f), color }  // 4
    };

    // Define triangles
    indices = {
        0, 1, 4,    // front
        1, 2, 4,    // right
        2, 3, 4,    // back
        3, 0, 4,    // left
        3, 2, 1, 1, 0, 3     // base
    };

    Setup();
    CalculateInitialBoundingBox();
}

void Mesh::subDivide(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int n) 
{
    if (n > 0) {
        glm::vec3 v12 = glm::normalize(v1 + v2);
        glm::vec3 v23 = glm::normalize(v2 + v3);
        glm::vec3 v31 = glm::normalize(v3 + v1);

        subDivide(v1, v12, v31, n - 1);
        subDivide(v2, v23, v12, n - 1);
        subDivide(v3, v31, v23, n - 1);
        subDivide(v12, v23, v31, n - 1);
    }
    else {
        // Add vertices and generate indices
        unsigned int index1 = vertices.size();
        vertices.push_back({ v1, glm::normalize(v1), ObjectColor });

        unsigned int index2 = vertices.size();
        vertices.push_back({ v2, glm::normalize(v2), ObjectColor-glm::vec3(0.3,0.3,0.3) });

        unsigned int index3 = vertices.size();
        vertices.push_back({ v3, glm::normalize(v3), ObjectColor-glm::vec3(0.2,0.2,0.2) });

        indices.push_back(index1);
        indices.push_back(index2);
        indices.push_back(index3);
    }
}

void Mesh::CreateSphere2(float radius, int subdivisions, glm::vec3 color)
{
    Radius = radius;
    ObjectColor = color;
    
    // Define the octahedron vertices and scale them by the radius
    glm::vec3 v0{ 0, 0, radius };
    glm::vec3 v1{ radius, 0, 0 };
    glm::vec3 v2{ 0, radius, 0 };
    glm::vec3 v3{ -radius, 0, 0 };
    glm::vec3 v4{ 0, -radius, 0 };
    glm::vec3 v5{ 0, 0, -radius };

    // Subdivide the 8 faces of the octahedron
    subDivide(v0, v1, v2, subdivisions);
    subDivide(v0, v2, v3, subdivisions);
    subDivide(v0, v3, v4, subdivisions);
    subDivide(v0, v4, v1, subdivisions);
    subDivide(v5, v2, v1, subdivisions);
    subDivide(v5, v3, v2, subdivisions);
    subDivide(v5, v4, v3, subdivisions);
    subDivide(v5, v1, v4, subdivisions);

    std::cout << vertices[2].Position.x << std::endl;
    std::cout << vertices[2].Position.y << std::endl;
    std::cout << vertices[2].Position.z << std::endl;

    std::cout << vertices[3].Position.x << std::endl;
    std::cout << vertices[3].Position.y << std::endl;
    std::cout << vertices[3].Position.z << std::endl;

    Setup();
    CalculateInitialBoundingBox();
}

void Mesh::CreatePlane(float radius, glm::vec3 color)
{
    glm::vec3 color2 = color+glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 color3 = color-glm::vec3(0.2f, 0.2f, 0.2f);
    // Define plane vertices
    vertices = {
        // Positions           // Normals           // Color
        { glm::vec3(-radius, 0.0f, -radius), glm::vec3(0.0f, 1.0f, 0.0f), color },
        {  glm::vec3(radius, 0.0f, -radius), glm::vec3(0.0f, 1.0f, 0.0f), color2 },
        {  glm::vec3(radius, 0.0f,  radius), glm::vec3(0.0f, 1.0f, 0.0f), color3 },
        { glm::vec3(-radius, 0.0f,  radius), glm::vec3(0.0f, 1.0f, 0.0f), color }
    };

    // Define triangles
    indices = {
        2, 1, 0,    // first triangle
        0, 3, 2     // second triangle
    };

    Setup();
    CalculateInitialBoundingBox();
}

void Mesh::Setup()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);
    // Color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
    glEnableVertexAttribArray(2);

    // Unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::CalculateBoundingBox()
{
    glm::mat4 model = GetTransform();

    minVert = glm::vec3(FLT_MAX);
    maxVert = glm::vec3(-FLT_MAX);

    for (const auto& corner : boundingBoxCorners)
    {
        glm::vec3 worldVert = glm::vec3(model * glm::vec4(corner, 1.0f));
        minVert = glm::min(minVert, worldVert);
        maxVert = glm::max(maxVert, worldVert);
    }
}

void Mesh::SetColor(glm::vec3 color) {
    glm::vec3 color2 = color+glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 color3 = color-glm::vec3(0.2f, 0.2f, 0.2f);
    if (mType == Cube) {
        ObjectColor = color;
        vertices[0].Color = color;
        vertices[1].Color = color;
        vertices[2].Color = color;
        vertices[3].Color = color2;
        vertices[4].Color = color2;
        vertices[5].Color = color3;
        vertices[6].Color = color3;
        vertices[7].Color = color3;

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Mesh::Draw(unsigned shaderProgram)
{
    if (markedForDeletion) return;

    glm::mat4 model = GetTransform();
    
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

    CalculateBoundingBox();
    // DrawBoundingBox(shaderProgram);
}

glm::mat4 Mesh::GetTransform()
{
    //TODO: uncomment when components are implemented
    // glm::vec3 pos = transform->position;
    // glm::vec3 rot = ss->rotation;
    // glm::vec3 scale = transform->scale;

    glm::vec3 pos = globalPosition;
    glm::vec3 rot = globalRotation;
    glm::vec3 scale = globalScale;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    
    model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, scale);
    
    Radius = 1 * scale.x;
    
    return model;
}

void Mesh::DrawBoundingBox(unsigned int shaderProgram)
{
    glm::vec3 vertices[8] = {
        minVert,
        glm::vec3(minVert.x, minVert.y, maxVert.z),
        glm::vec3(minVert.x, maxVert.y, minVert.z),
        glm::vec3(minVert.x, maxVert.y, maxVert.z),
        glm::vec3(maxVert.x, minVert.y, minVert.z),
        glm::vec3(maxVert.x, minVert.y, maxVert.z),
        glm::vec3(maxVert.x, maxVert.y, minVert.z),
        maxVert
    };
    
    
    unsigned int indices[24] = {
        0, 1, 1, 5, 5, 4, 4, 0, // bottom face
        2, 3, 3, 7, 7, 6, 6, 2, // top face
        0, 2, 1, 3, 5, 7, 4, 6  // vertical lines
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(shaderProgram);
    
    glm::mat4 model = glm::mat4(1.0f);
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));

    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::Physics(float deltaTime)
{
    //TODO: uncomment when components are implemented
    // transform->position += velocity * deltaTime;
    if (transform)
    {
        transform->position += velocity * deltaTime;

        globalPosition = transform->position;
        globalRotation = transform->rotation;
        globalScale = transform->scale;
    }
    else
    {
        globalPosition += velocity * deltaTime;
    }

}

/// 
/// @param point object to check against
/// @return closest point on AABB to object
glm::vec3 Mesh::ClosestPointOnAABB(glm::vec3& point) const
{
    glm::vec3 closestPoint = point;

    if (point.x < minVert.x)
    {
        closestPoint.x = minVert.x;
    }
    else if (point.x > maxVert.x)
    {
        closestPoint.x = maxVert.x;
    }

    if (point.y < minVert.y)
    {
        closestPoint.y = minVert.y;
    }
    else if (point.y > maxVert.y)
    {
        closestPoint.y = maxVert.y;
    }
    
    if (point.z < minVert.z)
    {
        closestPoint.z = minVert.z;
    }
    else if (point.z > maxVert.z)
    {
        closestPoint.z = maxVert.z;
    }

    return closestPoint;
}

void Mesh::CalculateInitialBoundingBox()
{
    minVert = glm::vec3(FLT_MAX);
    maxVert = glm::vec3(-FLT_MAX);

    for (const auto& vertex : vertices)
    {
        minVert = glm::min(minVert, vertex.Position);
        maxVert = glm::max(maxVert, vertex.Position);
    }

    boundingBoxCorners[0] = minVert;
    boundingBoxCorners[1] = glm::vec3(minVert.x, minVert.y, maxVert.z);
    boundingBoxCorners[2] = glm::vec3(minVert.x, maxVert.y, minVert.z);
    boundingBoxCorners[3] = glm::vec3(minVert.x, maxVert.y, maxVert.z);
    boundingBoxCorners[4] = glm::vec3(maxVert.x, minVert.y, minVert.z);
    boundingBoxCorners[5] = glm::vec3(maxVert.x, minVert.y, maxVert.z);
    boundingBoxCorners[6] = glm::vec3(maxVert.x, maxVert.y, minVert.z);
    boundingBoxCorners[7] = maxVert;
}
