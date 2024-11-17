#version 330 core
out vec4 FragColor;

in vec3 ourNormal; // Normal at the fragment
in vec3 ourColor;  // Vertex color
in vec3 ourPos;    // Position of the fragment

uniform vec3 lightPos;   // Position of the light
uniform vec3 viewPos;    // Position of the camera
uniform vec3 lightColor; // Color of the light

void main() {
    // Normalize input vectors
    vec3 norm = normalize(ourNormal);
    vec3 lightDir = normalize(lightPos - ourPos);
    vec3 viewDir = normalize(viewPos - ourPos);

    // Ambient component
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse component
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular component
    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // Shininess factor
    vec3 specular = specularStrength * spec * lightColor;

    // Combine all components and apply fragment color
    vec3 result = (ambient + diffuse + specular) * ourColor;
    FragColor = vec4(result, 1.0);
}