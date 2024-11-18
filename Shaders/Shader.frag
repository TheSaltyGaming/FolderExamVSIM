#version 330 core
out vec4 FragColor;

#define MAX_POINT_LIGHTS 64


in vec3 ourNormal;
in vec3 ourColor;
in vec3 ourPos;
uniform vec4 Color;
uniform vec3 viewPos;
vec3 lightColor;
vec3 lightDir;
vec3 norm;
uniform vec3 lightPos;


void main()
{
    float specularStrength = 0.5;
    lightColor = vec3(1.0);
    norm = normalize(ourNormal);

    lightDir = normalize(lightPos - ourPos);

    vec3 viewDir = normalize(viewPos - ourPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = (ambient +diffuse + specular) * ourColor;
    FragColor = vec4(result, 1.0);
}