#include <algorithm>
#include <bitset>

#include "Shader.h"
#include "ShaderFileLoader.h"
#include <iostream>
#include <map>
#include <set>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Other/Camera.h"
#include "Other/Collision.h"
#include "EntityManager.h"
#include "Math.h"
#include "RenderSystem.h"
#include "Mesh/Mesh.h"
#include "glm/mat4x3.hpp"
#include "Components/TransformComponent.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void CameraView(std::vector<unsigned> shaderPrograms, glm::mat4 trans, glm::mat4 projection);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void DrawObjects(unsigned VAO, Shader ShaderProgram);

void CollisionChecking();

void Attack();
glm::vec3 RandomColor();

ComponentManager componentManager;
EntityManager entityManager = EntityManager(componentManager);

RenderSystem renderSystem = RenderSystem(componentManager, entityManager);


void EntitySetup();

std::unique_ptr<Entity> playerEntity;

std::vector<std::shared_ptr<Entity>> healthPotions;

std::vector<std::shared_ptr<Entity>> enemyEntities;

std::vector<Mesh*> sphereMeshes;

Mesh PlayerMesh;

Math math;
Collision collision;

Mesh PointCloud;



Mesh CameraMesh;

int lives = 6;

glm::vec3 lastPos = glm::vec3(999999.f);


// settings

 unsigned int SCR_WIDTH = 1280;
 unsigned int SCR_HEIGHT = 720;

struct colorStruct
{
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

///Delta time variables
///--------------------
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

///Mouse Input Variables
///---------------------
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;

std::string vfs = ShaderLoader::LoadShaderFromFile("Triangle.vs");
std::string fs = ShaderLoader::LoadShaderFromFile("Triangle.vs");

Camera MainCamera;

int CameraMode = 1;
bool firstCamera = true;


std::vector<unsigned> shaderPrograms;

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


    int SphereCount = 100;

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

        // Add sphere entity to the enemyEntities vector
        enemyEntities.push_back(sphereEntity);
    }
}


void DrawObjects(unsigned VAO, Shader ShaderProgram)
{
    //Drawmeshes here, draw meshes (this comment is for CTRL + F search)
    ShaderProgram.use();
    glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    // sphere_mesh.Draw(ShaderProgram.ID);
    // sphere2Mesh.Draw(ShaderProgram.ID);

    //draw all meshes
    for (Mesh* sphere : sphereMeshes)
    {
        sphere->Draw(ShaderProgram.ID);
    }

    PlayerMesh.Draw(ShaderProgram.ID);

    PointCloud.Draw(ShaderProgram.ID);

    //CameraMesh.Draw(ShaderProgram.ID);




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


        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawObjects(VAO, ourShader);

        renderSystem.Update(deltaTime);

        CollisionChecking();


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void SetupMeshes()
{
    //Create meshes here, Make meshes here, Setup meshes here, define meshes here, setupObjects setup objects create objects
    //(this comment is for CTRL + F search)

    PlayerMesh = Mesh(Cube, 1.f, colors.magenta, nullptr);

    PointCloud = Mesh();
    PointCloud.vertices = math.loadPointCloud("pointCloud.txt");
    PointCloud.Setup();

    PlayerMesh.globalPosition = glm::vec3(0.0f, 0.5f, 0.0f);
    PlayerMesh.globalScale = glm::vec3(0.2f, 0.2f, 0.2f);

#pragma region OtherMeshes


    CameraMesh = Mesh(Cube, 0.5f, colors.white, nullptr);
    CameraMesh.globalPosition = MainCamera.cameraPos;

#pragma endregion
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
    Shader ourShader("VertShaderOld.vert", "FragShaderOld.frag"); // you can name your shader files however you like

    shaderPrograms.push_back(ourShader.ID);



    /// SETUP MESHES HER
    EntitySetup();
    SetupMeshes();


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

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
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
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        //make all spheres move
        for (auto ballsphere : sphereMeshes)
        {
            ballsphere->velocity = glm::vec3(math.RandomVec3(-2, 2).x, 0.0f, math.RandomVec3(-2, 2).z);

        }
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        //stop all velocity
        for (Mesh* sphere : sphereMeshes)
        {
            sphere->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        Attack();
    }


    float cameraSpeed = 2.5f * deltaTime;

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

void CollisionChecking()
{

    int p = 0;
    for (int i = 0; i < enemyEntities.size(); ++i)
    {
        auto sphereMeshComponent1 = componentManager.GetComponent<Mesh>(enemyEntities[i]->GetId());

        if (sphereMeshComponent1->markedForDeletion) break;

        for (int j = p + 1; j < enemyEntities.size(); ++j)
        {
            auto sphereMeshComponent2 = componentManager.GetComponent<Mesh>(enemyEntities[j]->GetId());
            collision.SphereCollision(sphereMeshComponent1.get(), sphereMeshComponent2.get());
        }
        p++;
    }


    int W = 0;



    // Marking spheres for deletion
    std::set<int> spheresToDelete;
    auto playerMeshComponent = componentManager.GetComponent<Mesh>(playerEntity->GetId());
    for (auto it = enemyEntities.begin(); it != enemyEntities.end();)
    {
        auto sphereMeshComponent = componentManager.GetComponent<Mesh>((*it)->GetId());
        if (collision.SphereToAABBCollision(sphereMeshComponent.get(), &PlayerMesh) && !sphereMeshComponent->markedForDeletion)
        {
            std::cout << "Collision with player" << std::endl;
            // Knockback
            glm::vec3 knockbackDirection = glm::normalize(PlayerMesh.globalPosition - sphereMeshComponent->globalPosition);
            float smoothingFactor = 0.1f; // Adjust this value to control the smoothing
            if (CameraMode == 2) {
                MainCamera.cameraPos = math.lerp(MainCamera.cameraPos, MainCamera.cameraPos + knockbackDirection * 0.1f, smoothingFactor);
            } else {
                PlayerMesh.globalPosition = math.lerp(PlayerMesh.globalPosition, PlayerMesh.globalPosition + knockbackDirection * 0.1f, smoothingFactor);
            }

            // Mark sphere for deletion
            sphereMeshComponent->markedForDeletion = true;
            spheresToDelete.insert((*it)->GetId());

            // Remove sphere entity from the enemyEntities vector
            it = enemyEntities.erase(it);

            // Update lives and player color
            lives--;

            if (lives <= 0) {
                std::cout << "Game Over" << std::endl;
                playerMeshComponent->SetColor(colors.red);
            } else if (lives <= 3) {
                playerMeshComponent->SetColor(colors.orange);
            } else {
                playerMeshComponent->SetColor(colors.magenta);
            }
        }
        else
        {
            ++it;
        }
    }

    // Delete the marked spheres
    for (int entityId : spheresToDelete)
    {
        componentManager.RemoveComponent<TransformComponent>(entityId);
        componentManager.RemoveComponent<Mesh>(entityId);
        entityManager.DestroyEntity(Entity(componentManager, entityId));
    }

    for (auto it = healthPotions.begin(); it != healthPotions.end();)
    {
        auto potionMeshComponent = componentManager.GetComponent<Mesh>((*it)->GetId());
        if (collision.AABBCollision(&PlayerMesh, potionMeshComponent.get()))
        {
            std::cout << "Health potion collected" << std::endl;
            componentManager.RemoveComponent<TransformComponent>((*it)->GetId());
            componentManager.RemoveComponent<Mesh>((*it)->GetId());
            //entityManager.DestroyEntity((*it)->GetId());
            it = healthPotions.erase(it);
            lives = 6;
            playerMeshComponent->SetColor(colors.magenta);
        }
        else
        {
            ++it;
        }
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

void Attack()
{
    float attackRadius = 1.7f;
    float knockbackDistance = 0.8f;

    for (auto& entity : enemyEntities)
    {
        auto sphereMeshComponent = componentManager.GetComponent<Mesh>(entity->GetId());
        auto sphereTransformComponent = componentManager.GetComponent<TransformComponent>(entity->GetId());

        if (sphereMeshComponent && sphereTransformComponent)
        {
            float distance = glm::distance(PlayerMesh.globalPosition, sphereTransformComponent->position);
            if (distance <= attackRadius)
            {
                // Apply knockback
                glm::vec3 knockbackDirection = glm::normalize(sphereTransformComponent->position - PlayerMesh.globalPosition);
                sphereTransformComponent->position += knockbackDirection * knockbackDistance;

                // Decrease health
                sphereMeshComponent->health -= 1;
                if (sphereMeshComponent->health <= 0)
                {
                    // Mark sphere for deletion
                    sphereMeshComponent->markedForDeletion = true;
                }
            }
        }
    }

    // Remove marked spheres
    enemyEntities.erase(
        std::remove_if(enemyEntities.begin(), enemyEntities.end(), [](const std::shared_ptr<Entity>& entity) {
            auto sphereMeshComponent = componentManager.GetComponent<Mesh>(entity->GetId());
            return sphereMeshComponent && sphereMeshComponent->markedForDeletion;
        }),
        enemyEntities.end()
    );
}

