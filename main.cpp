
#include <algorithm>
#include <bitset>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>

#include <delaunator.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x3.hpp>
#include <glm/gtc/random.hpp>
#include <lua.hpp>

#include "Shader.h"
#include "ShaderFileLoader.h"
#include "RenderSystem.h"
#include "Mesh/Mesh.h"

#include "EntityManager.h"
#include "Math.h"
#include "Components/TransformComponent.h"
#include "PerlinNoise.hpp"
#include "BsplineFunction.h"
#include "ParticleSystem.h"
#include "TerrainGrid.h"

#include "Other/Camera.h"
#include "Other/Collision.h"
#include "Other/BSplineTracer.h"
#include "Other/LuaManager.h"
#include "Other/Physics.h"


// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void CameraView(std::vector<unsigned> shaderPrograms, glm::mat4 trans, glm::mat4 projection);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void DrawObjects(unsigned VAO, Shader ShaderProgram);
void SetupBsplineSurface();
void UpdateBallTracers();
glm::vec3 RandomColor();
void UpdateBall(Mesh &Ball, Mesh &Terrain, float deltaTIme);
void EntitySetup();
void InstansiateMesh(float minpos, float maxPos, float scale);
void LuaSetup();

// Color
struct colorStruct {
    glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 black = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 cyan = glm::vec3(0.0f, 1.0f, 1.0f);
    glm::vec3 magenta = glm::vec3(1.0f, 0.0f, 1.0f);
    glm::vec3 orange = glm::vec3(1.0f, 0.5f, 0.0f);
    glm::vec3 purple = glm::vec3(0.5f, 0.0f, 0.5f);
    glm::vec3 grey = glm::vec3(0.5f, 0.5f, 0.5f);
};

colorStruct colors;

// Constants
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;
const float PATH_UPDATE_INTERVAL = 0.1f;

// Delta time variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Mouse Input Variables
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// Shader files
std::string vfs = ShaderLoader::LoadShaderFromFile("Triangle.vs");
std::string fs = ShaderLoader::LoadShaderFromFile("Triangle.vs");

// Camera
Camera MainCamera;
int CameraMode = 1;
bool firstCamera = true;
glm::vec3 oldBaryCoords = glm::vec3(0.0f);

// Managers
ComponentManager componentManager;
EntityManager entityManager = EntityManager(componentManager);
RenderSystem renderSystem = RenderSystem(componentManager, entityManager);

ParticleSystem particleSystem = ParticleSystem(50000);
Physics physics;

// Entities
std::unique_ptr<Entity> playerEntity;
std::vector<std::shared_ptr<Entity>> healthPotions;
std::vector<std::shared_ptr<Entity>> enemyEntities;

// Meshes
std::vector<Mesh*> sphereMeshes;
std::vector<Mesh*> rollingBalls;
std::vector<Mesh> pathMeshes;
Mesh PlayerMesh;
Mesh LightCube;
Mesh PointCloud;
Mesh surfaceMesh;
Mesh bsplineSurface;
Mesh CameraMesh;
Mesh rollingBall;
Mesh rollingBall2;
Mesh Ball1PathMesh;
Mesh Ball2PathMesh;

// BSpline Tracers
std::vector<BSplineTracer> ballTracers;
BSplineTracer Ball1Tracer;
BSplineTracer Ball2Tracer;

// Other components
Math math;
Collision collision;
TerrainGrid terrainGrid;
int lives = 6;
glm::vec3 lastPos = glm::vec3(999999.f);
std::vector<unsigned> shaderPrograms;

//LUA HERE
LuaManager lua;

// Timers
float pathUpdateTimer = 0.0f;

void EntitySetup()
{
    playerEntity = std::make_unique<Entity>(entityManager.CreateEntity());

    auto transformComponent = std::make_shared<TransformComponent>();
    transformComponent->position = glm::vec3(0.0f, 0.0f, 0.0f);
    transformComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    transformComponent->rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    transformComponent->scale = glm::vec3(0.1f, 0.1f, 0.1f);

    auto meshComponent = std::make_shared<Mesh>(Cube, 1.0f, glm::vec3(colors.magenta), transformComponent.get());

    componentManager.AddComponent<TransformComponent>(playerEntity->GetId(), transformComponent);
    componentManager.AddComponent<Mesh>(playerEntity->GetId(), meshComponent);

    // componentManager.AddComponent<TransformComponent>(newEntity.GetId(), transformComponent);
    // componentManager.AddComponent<Mesh>(newEntity.GetId(), meshComponent);

    // Set up health potions
    glm::vec3 scale = glm::vec3(0.05f);
    glm::vec3 initialPosition = glm::vec3(0.0f);


    for (int i = 0; i < 3; ++i) {
        // Create new entity for the health potion
        auto healthPotion = std::make_shared<Entity>(entityManager.CreateEntity());

        // Add Transform Component
        auto potionTransformComponent = std::make_shared<TransformComponent>();
        potionTransformComponent->position = initialPosition + glm::vec3(i * 1.0f, 0.5f, 0.0f);
        potionTransformComponent->scale = scale;
        potionTransformComponent->rotation = glm::vec3(0.0f, 0.0f, 0.0f);

        // Add Mesh Component
        auto potionMeshComponent = std::make_shared<Mesh>(Cube, 1.0f, colors.blue, potionTransformComponent.get());

        componentManager.AddComponent<TransformComponent>(healthPotion->GetId(), potionTransformComponent);
        componentManager.AddComponent<Mesh>(healthPotion->GetId(), potionMeshComponent);

        // Add health potion entity to the global vector
        healthPotions.push_back(healthPotion);
    }


    int SphereCount = 1;


    ballTracers.clear();
    pathMeshes.clear();

    for (int i = 0; i < SphereCount; ++i) {
        // Create new entity for the sphere
        auto sphereEntity = std::make_shared<Entity>(entityManager.CreateEntity());

        // Add Transform Component
        auto sphereTransformComponent = std::make_shared<TransformComponent>();
        sphereTransformComponent->position = glm::vec3(
            math.RandomVec3(-6.7, 6.7).x,
            0.5, // y
            math.RandomVec3(-6.7, 6.7).z
        );
        sphereTransformComponent->scale = glm::vec3(0.1f, 0.1f, 0.1f);
        sphereTransformComponent->rotation = glm::vec3(0.0f, 0.0f, 0.0f);

        // Add Mesh Component
        auto sphereMeshComponent = std::make_shared<Mesh>(Sphere, 1.f, 4, RandomColor(), sphereTransformComponent.get());

        componentManager.AddComponent<TransformComponent>(sphereEntity->GetId(), sphereTransformComponent);
        componentManager.AddComponent<Mesh>(sphereEntity->GetId(), sphereMeshComponent);

        // Create and set up tracer for this sphere
        BSplineTracer tracer;  // Create new tracer
        ballTracers.push_back(tracer);  // Store the tracer

        // Create and set up path mesh for the tracer
        Mesh pathMesh;
        pathMesh.Setup();
        pathMeshes.push_back(pathMesh);

        // Add sphere entity to the enemyEntities vector
        enemyEntities.push_back(sphereEntity);
    }
}

void InstansiateMesh(float minpos, float maxPos, float scale)
{
    std::cout << "Instansiating mesh with pos " << minpos << " " << maxPos << " and scale " << scale << std::endl;



    auto sphereEntity = std::make_shared<Entity>(entityManager.CreateEntity());

    // Add Transform Component
    auto sphereTransformComponent = std::make_shared<TransformComponent>();
    sphereTransformComponent->position = glm::vec3(
        math.RandomVec3(minpos, maxPos).x,
        0.5, // y
        math.RandomVec3(minpos, maxPos).z
    );
    sphereTransformComponent->scale = glm::vec3(scale, scale, scale);
    sphereTransformComponent->rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    // Add Mesh Component
    auto sphereMeshComponent = std::make_shared<Mesh>(Sphere, 1.f, 4, RandomColor(), sphereTransformComponent.get());

    componentManager.AddComponent<TransformComponent>(sphereEntity->GetId(), sphereTransformComponent);
    componentManager.AddComponent<Mesh>(sphereEntity->GetId(), sphereMeshComponent);

    enemyEntities.push_back(sphereEntity);
}

void LuaSetup()
{
    return;
}


void DrawObjects(unsigned VAO, Shader ShaderProgram)
{
    //Drawmeshes here, draw meshes (this comment is for CTRL + F search)
    ShaderProgram.use();
    glBindVertexArray(VAO);

    //draw all meshes
    for (Mesh* sphere : sphereMeshes)
    {
        sphere->Draw(ShaderProgram.ID);
    }


    //PointCloud.Draw(ShaderProgram.ID);

    surfaceMesh.Draw(ShaderProgram.ID);


    bsplineSurface.Draw(ShaderProgram.ID);

    rollingBall.Draw(ShaderProgram.ID);
    rollingBall2.Draw(ShaderProgram.ID);

    for (auto& pathMesh : pathMeshes) {
        pathMesh.Draw(ShaderProgram.ID);
    }

    Ball1PathMesh.Draw(ShaderProgram.ID);
    Ball2PathMesh.Draw(ShaderProgram.ID);

    particleSystem.render(ShaderProgram.ID);

}

void EntityPhysics()
{
#pragma region OldHoming code
    for (auto& entity : enemyEntities)
    {
        auto sphereMeshComponent = componentManager.GetComponent<Mesh>(entity->GetId());
        auto sphereTransformComponent = componentManager.GetComponent<TransformComponent>(entity->GetId());

        if (sphereMeshComponent && sphereTransformComponent)
        {
            sphereMeshComponent->Physics(deltaTime);

            // Hoaming towards player
            if (glm::distance(PlayerMesh.globalPosition, sphereTransformComponent->position) < 3)
            {
                lastPos = PlayerMesh.globalPosition;

                glm::vec3 direction = glm::normalize(PlayerMesh.globalPosition - sphereTransformComponent->position);
                sphereMeshComponent->velocity += direction * 0.01f;
            }
            // Hoaming towards last position
            else if (glm::distance(lastPos, sphereTransformComponent->position) < 3)
            {
                glm::vec3 direction = glm::normalize(lastPos - sphereTransformComponent->position);
                sphereMeshComponent->velocity += direction * 0.01f;
            }
            else
            {
                sphereMeshComponent->velocity = glm::vec3(0.f);
            }

            // Speed cap
            if (glm::length(sphereMeshComponent->velocity) > 0.5f)
            {
                sphereMeshComponent->velocity = glm::normalize(sphereMeshComponent->velocity) * 0.5f;
            }
        }
    }
#pragma endregion

    for (auto& entity : enemyEntities)
    {
        auto sphereMeshComponent = componentManager.GetComponent<Mesh>(entity->GetId());
        auto sphereTransformComponent = componentManager.GetComponent<TransformComponent>(entity->GetId());

        if (sphereMeshComponent && sphereTransformComponent)
        {
            sphereMeshComponent->Physics(deltaTime);

            // Assign random velocity
            sphereMeshComponent->velocity = glm::vec3(
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f,
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f,
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f
            );

            // Speed cap
            if (glm::length(sphereMeshComponent->velocity) > 0.5f)
            {
                sphereMeshComponent->velocity = glm::normalize(sphereMeshComponent->velocity) * 1.5f;
            }
        }
    }
}

void render(GLFWwindow* window, Shader ourShader, unsigned VAO)
{
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));


    //TEMP CODE:
    auto transformComponent = componentManager.GetComponent<TransformComponent>(playerEntity->GetId());
    transformComponent->position = PlayerMesh.globalPosition;
    transformComponent->scale = glm::vec3(0.25f, 0.25f, 0.25f);

    // Before the main loop, set the ambient color
    glm::vec3 ambientColor = glm::vec3(0.3f, 0.3f, 0.3f); // A soft grey ambient light
    ourShader.use(); // Use the shader program
    ourShader.setVec3("ambientColor", ambientColor);  // Ensure setVec3 is implemented for Shader

    ourShader.use();
    glm::vec3 lightPos(1.2f, 30.0f, 2.0f);
    ourShader.setVec3("lightPos", lightPos);  // Example: Set light position
    ourShader.setVec3("viewPos", MainCamera.cameraPos);
    ourShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // Example: white light

    LightCube.globalPosition = lightPos;

    //Enabele wireframe mode

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //turn up pointsize
    glLineWidth(5.f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {



        glLineWidth(12);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        CameraView(shaderPrograms, model, projection);


        CameraMesh.globalPosition = MainCamera.cameraPos;
        CameraMesh.CalculateBoundingBox();




        // auto meshComponent = componentManager.GetComponent<Mesh>(playerEntity->GetId());
        // meshComponent->Physics(deltaTime);

        // Call physics for all mesh components
        std::vector<Entity> entitiesWithMesh = entityManager.GetAllEntitiesWithComponent<Mesh>();
        for (const Entity& entity : entitiesWithMesh) {
            if (auto meshComponent = componentManager.GetComponent<Mesh>(entity.GetId())) {
                meshComponent->Physics(deltaTime);
            }
        }

        auto transformComponent = componentManager.GetComponent<TransformComponent>(playerEntity->GetId());
        transformComponent->position = PlayerMesh.globalPosition;

        //for every sphere do physics
        //EntityPhysics();

        physics.UpdateBall(rollingBalls, surfaceMesh, deltaTime, terrainGrid);

        // ballTracers[0].Update(deltaTime, rollingBall);
        // pathMeshes[0].vertices = ballTracers[0].GetSplinePoints();
        //
        // pathMeshes[0].Setup();

        for (size_t i = 0; i < enemyEntities.size(); ++i) {
            // Get the mesh component for this entity
            auto meshComponent = componentManager.GetComponent<Mesh>(enemyEntities[i]->GetId());
        }

        if (glm::length(rollingBall.velocity) > 0.1f) {
            Ball1Tracer.Update(deltaTime, rollingBall);
            Ball1PathMesh.vertices = Ball1Tracer.GetSplinePoints();
            Ball1PathMesh.Setup();
        }

        if (glm::length(rollingBall2.velocity) > 0.1f) {
            Ball2Tracer.Update(deltaTime, rollingBall2);
            Ball2PathMesh.vertices = Ball2Tracer.GetSplinePoints();
            Ball2PathMesh.Setup();
        }
        //physics.UpdateBallPhysics(rollingBall, surfaceMesh, deltaTime);

        //cout camera position
        //std::cout << "Camera Position: " << MainCamera.cameraPos.x << " " << MainCamera.cameraPos.y << " " << MainCamera.cameraPos.z << std::endl;
        if (CameraMode == 2)
        {
            if (firstCamera)
            {
                MainCamera.cameraPos = PlayerMesh.globalPosition;
                MainCamera.cameraPos.y += 5.0f;

                firstCamera = false;
            }
            PlayerMesh.globalPosition.x = MainCamera.cameraPos.x;
            PlayerMesh.globalPosition.z = MainCamera.cameraPos.z;

            MainCamera.pitch = -89.0f;
        }
        else
        {
            firstCamera = true;

        }

        //PARTICLE SYSTEM STUFF HERE

        glm::vec3 emitPosition(-20.0f, 100.0f, 20.0f);
        float lifeSpan = 11.f;

        particleSystem.emit(emitPosition, lifeSpan);

        particleSystem.update(deltaTime);

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawObjects(VAO, ourShader);

        renderSystem.Update(deltaTime);

        //CollisionChecking();


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Triangulate_Terrain(std::vector<Vertex> &points)
{
    std::vector<Vertex> downsampledPoints;
    //Terrain Resolution
    int step = 10;
    for (size_t i = 0; i < points.size(); i += step) {
        downsampledPoints.push_back(points[i]);
    }

    points = downsampledPoints;

    std::vector<double> coords;
    for (const auto& point : points) {
        coords.push_back(point.Position.x);
        coords.push_back(point.Position.z);
    }


    delaunator::Delaunator d(coords);


    std::vector<Vertex> surfaceVertices;
    std::vector<unsigned int> surfaceIndices;

#pragma region colorGradient
    // Define base and highlight colors
    glm::vec3 baseColor = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 redColor = glm::vec3(1.0f, 0.0f, 0.0f);

    // Initialize the Perlin noise generator with a seed
    siv::PerlinNoise perlin{ 12345 };

    // Parameters for Perlin noise
    float noiseScale = 0.6f;
    int noiseOctaves = 6;
    float noiseIntensity = 0.2f;

    for (const auto& point : points) {
        Vertex vertex = point;

        // Compute Perlin noise value for this vertex
        float noiseValue = perlin.octave2D_01(
            point.Position.x * noiseScale,
            point.Position.z * noiseScale,
            noiseOctaves
        );

        // Calculate color intensity based on noise
        float colorIntensity = glm::clamp(0.5f + noiseIntensity * (noiseValue - 0.5f), 0.0f, 1.0f);

        if (vertex.Position.y < 2.0f) {
            vertex.Color = redColor; // Set color to red
        } else {
            vertex.Color = baseColor * 2.f * colorIntensity; // Existing color logic
        }

        // Initialize normals
        vertex.Normal = glm::vec3(0.0f);

        // Add the processed vertex to the surface vertices
        surfaceVertices.push_back(vertex);
    }
#pragma endregion



    for (std::size_t i = 0; i < d.triangles.size(); i += 3) {
        surfaceIndices.push_back(d.triangles[i]);
        surfaceIndices.push_back(d.triangles[i + 1]);
        surfaceIndices.push_back(d.triangles[i + 2]);
    }

    // Calculate normals for the terrain
    for (size_t i = 0; i < surfaceIndices.size(); i += 3) {
        unsigned int i0 = surfaceIndices[i];
        unsigned int i1 = surfaceIndices[i + 1];
        unsigned int i2 = surfaceIndices[i + 2];

        glm::vec3 v1 = surfaceVertices[i1].Position - surfaceVertices[i0].Position;
        glm::vec3 v2 = surfaceVertices[i2].Position - surfaceVertices[i0].Position;
        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

        surfaceVertices[i0].Normal += normal;
        surfaceVertices[i1].Normal += normal;
        surfaceVertices[i2].Normal += normal;
    }

    for (auto& vertex : surfaceVertices) {
        vertex.Normal = glm::normalize(vertex.Normal);
    }

    surfaceMesh = Mesh();
    surfaceMesh.vertices = surfaceVertices;
    surfaceMesh.indices = surfaceIndices;
    surfaceMesh.Setup();

    float cellSize = 10.f;
    terrainGrid = TerrainGrid(surfaceVertices, surfaceIndices, cellSize);


    BsplineFunction bspline;
    bspline.CreateBspline(bsplineSurface);

    bsplineSurface.globalPosition = glm::vec3(0.0f, 4.0f, 0.0f);

    // ballTracers.push_back(Ball1Tracer);
    //
    // Mesh pathMesh = Mesh();
    // pathMesh.Setup();
    // pathMeshes.push_back(pathMesh);

    Ball1PathMesh.Setup();
    Ball2PathMesh.Setup();
}

void SetupMeshes()
{
    //Create meshes here, Make meshes here, Setup meshes here, define meshes here, setupObjects setup objects create objects
    //(this comment is for CTRL + F search)

    LightCube = Mesh(Cube, 1.f, colors.red, nullptr);
    LightCube.globalScale = glm::vec3(4.1f, 4.1f, 4.1f);
    LightCube.globalPosition = glm::vec3(0.0f, 0.5f, 0.0f);
    LightCube.Setup();

    PlayerMesh = Mesh(Cube, 1.f, colors.magenta, nullptr);

    std::vector<Vertex> points = math.loadPointCloud("pointCloud.txt");

    PointCloud = Mesh();
    PointCloud.vertices = points;
    PointCloud.Setup();

    PlayerMesh.globalPosition = glm::vec3(0.0f, 0.5f, 0.0f);
    PlayerMesh.globalScale = glm::vec3(0.2f, 0.2f, 0.2f);



    Triangulate_Terrain(points);


    rollingBall = Mesh(Sphere, 1.f, 4, colors.red, nullptr);
    rollingBall.globalPosition = glm::vec3(0.0f, 0.5f, 0.0f);
    rollingBall.velocity = glm::vec3(0.1f);
    rollingBall.Setup();

    rollingBall2 = Mesh(Sphere, 1.f, 4, colors.red, nullptr);
    rollingBall2.globalPosition = glm::vec3(1.0f, 0.5f, 0.0f);
    rollingBall2.velocity = glm::vec3(0.1f);
    rollingBall2.Setup();

    rollingBalls.push_back(&rollingBall);
    rollingBalls.push_back(&rollingBall2);

#pragma region OtherMeshes


    CameraMesh = Mesh(Cube, 0.5f, colors.white, nullptr);
    CameraMesh.globalPosition = MainCamera.cameraPos;

#pragma endregion



    particleSystem.setupBuffers();

    glPointSize(3.f);
}

int main()
{
    srand(time(0));



    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Architect Engine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback); // Set the mouse callback function

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------

    //Shader ourShader("Triangle.vert", "Triangle.frag"); // you can name your shader files however you like
    //Shader ourShader("VertShaderOld.vert", "FragShaderOld.frag"); // you can name your shader files however you like
    Shader ourShader("Shaders/Shader.vert", "Shaders/Shader.frag");

    shaderPrograms.push_back(ourShader.ID);



    /// SETUP MESHES HER
    EntitySetup();
    SetupMeshes();
    LuaSetup();


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);



    //RENDER FUNCTION HERE!!!!!!!
    renderSystem.mShaderProgram = ourShader.ID;
    render(window, ourShader, VAO);

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    glm::vec3 cameraFrontXZ = glm::normalize(glm::vec3(MainCamera.cameraFront.x, 0.0f, MainCamera.cameraFront.z));

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        //make random sphere move
        int randomSphere = rand() % sphereMeshes.size();
        if (sphereMeshes[randomSphere]->velocity == glm::vec3(0.f,0.f,0.f))
        {
            sphereMeshes[randomSphere]->velocity = glm::vec3(math.RandomVec3(-4, 4).x, 0.0f, math.RandomVec3(-4, 4).z);
        }
    }    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        if (lua.DoFile("lua/testFile.lua"))
        {
            lua.CallLuaFunction("SpawnActor");

        }
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        EntityPhysics();
        for (auto& tracer: ballTracers)
        {
            tracer.EnableSpline();
        }

    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        for (auto& tracer : ballTracers)
        {
            tracer.Clear();
            tracer.DisableSpline();
        }

        for (auto& entity : enemyEntities)
        {
            auto sphereMeshComponent = componentManager.GetComponent<Mesh>(entity->GetId());
            auto sphereTransformComponent = componentManager.GetComponent<TransformComponent>(entity->GetId());

            if (sphereMeshComponent && sphereTransformComponent)
            {
                sphereMeshComponent->velocity = glm::vec3(0.f);
            }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
    }


    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        cameraSpeed *= 5.0f; // Double the camera speed
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        CameraMode = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        CameraMode = 2;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        MainCamera.cameraPos += cameraSpeed * cameraFrontXZ;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        MainCamera.cameraPos -= cameraSpeed * cameraFrontXZ;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        MainCamera.cameraPos -= glm::normalize(glm::cross(MainCamera.cameraFront, MainCamera.cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        MainCamera.cameraPos += glm::normalize(glm::cross(MainCamera.cameraFront, MainCamera.cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        MainCamera.cameraPos += cameraSpeed * MainCamera.cameraUp; // Move camera up
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        MainCamera.cameraPos -= cameraSpeed * MainCamera.cameraUp; // Move camera down
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        MainCamera.cameraPos.y += 0.01;
    }

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        rollingBall.globalPosition = MainCamera.cameraPos;
        rollingBall.velocity = glm::normalize(glm::vec3(MainCamera.cameraFront.x, 0.0f, MainCamera.cameraFront.z)) * 5.0f;

        Ball1Tracer.Clear();
        Ball1Tracer.EnableSpline();

        pathUpdateTimer = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        rollingBall2.globalPosition = MainCamera.cameraPos;
        rollingBall2.velocity = glm::normalize(glm::vec3(MainCamera.cameraFront.x, 0.0f, MainCamera.cameraFront.z)) * 7.5f;

        Ball2Tracer.Clear();
        Ball2Tracer.EnableSpline();


        pathUpdateTimer = 0.0f;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    // Prevents sudden jump in camera movement
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    MainCamera.yaw   += xoffset;
    MainCamera.pitch += yoffset;

    if(MainCamera.pitch > 89.0f)
        MainCamera.pitch = 89.0f;
    if(MainCamera.pitch < -89.0f)
        MainCamera.pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(MainCamera.yaw)) * cos(glm::radians(MainCamera.pitch));
    direction.y = sin(glm::radians(MainCamera.pitch));
    direction.z = sin(glm::radians(MainCamera.yaw)) * cos(glm::radians(MainCamera.pitch));
    MainCamera.cameraFront = glm::normalize(direction);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

/// \brief Handles Camera view functions
/// \param shaderPrograms vector of all shaders
/// \param trans transformation matrix
/// \param projection projection matrix
void CameraView(std::vector<unsigned> shaderPrograms, glm::mat4 trans, glm::mat4 projection)
{
    for (unsigned shaderProgram : shaderPrograms)
    {
        glUseProgram(shaderProgram);

        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        MainCamera.tick();

        glm::mat4 view;
        view = glm::lookAt(MainCamera.cameraPos, MainCamera.cameraPos + MainCamera.cameraFront, MainCamera.cameraUp);

        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Pass the transformation matrix to the vertex shader
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    }
}

glm::vec3 RandomColor()
{
    return glm::vec3(
    (rand() % 256) / 255.0f,
    (rand() % 256) / 255.0f,
    (rand() % 256) / 255.0f
);
}


void UpdateBall(Mesh &ball, Mesh &Terrain, float deltaTIme)
{
    // Gravity
    glm::vec3 gravity(0.0f, -9.81f, 0.0f);
    glm::vec3 force = gravity * ball.mass;
    glm::vec3 acceleration = force / ball.mass;

    // Velocity and position you know how it is
    ball.velocity += acceleration * deltaTime;
    ball.globalPosition += ball.velocity * deltaTime;

    glm::vec3 mappedPos = math.MapCameraToSurface(ball.globalPosition, Terrain);

    // Stop ball from goin below terrain
    if (ball.globalPosition.y < mappedPos.y) {
        ball.globalPosition.y = mappedPos.y;
        ball.velocity.y = 0.0f; //TODO: adjust later maybe idk
    }
}

