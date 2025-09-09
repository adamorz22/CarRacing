#version 460 core

out vec4 FragColor;
in vec3 Normal;  
in vec3 FragPos; 
in vec3 vertexColor;

vec3 lightPos = vec3(-1000.0f, -200.8f, -1.0f);
vec3 lightColor = vec3(1.0f, 1.0f, 1.0f); 

void main() 
{

    float ambientStrength = 0.4f;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(abs(FragPos - lightPos));

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 1.1f;
    vec3 viewDir = normalize(lightPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * vertexColor;
    FragColor = vec4(result, 1.0);

}