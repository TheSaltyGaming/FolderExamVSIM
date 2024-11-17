#version 330 core

layout (location = 0) in vec3 aPos;   // Vertex position
layout (location = 1) in vec3 aColor; // Vertex color
layout (location = 2) in vec3 aNormal; // Vertex normal

out vec3 ourColor;  // Pass vertex color to fragment shader
out vec3 ourNormal; // Pass transformed normal to fragment shader
out vec3 ourPos;    // Pass position in world space to fragment shader

uniform mat4 model;      // Model matrix
uniform mat4 view;       // View matrix
uniform mat4 projection; // Projection matrix

void main() {
    // Transform vertex position into clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Transform normal to world space (handles non-uniform scaling)
    ourNormal = mat3(transpose(inverse(model))) * aNormal;

    // Pass vertex color
    ourColor = aColor;

    // Transform position to world space
    ourPos = vec3(model * vec4(aPos, 1.0));
}