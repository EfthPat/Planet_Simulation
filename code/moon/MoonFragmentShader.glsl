#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
uniform sampler2D textureSampler;
out vec4 FragColor;

void main() {
    vec3 lightPos = vec3(0.0, 0.0, 0.0);

    // Ambient light
    float ambientStrength = 0.2;
    vec3 ambientColor = vec3(1.0, 1.0, 1.0);
    vec3 ambientLight = ambientStrength * ambientColor;

    // Normalize the normal vector of the surface and light direction vectors
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // Calculate the angle between the normal and light direction
    float angle = max(dot(norm, lightDir), 0.0);

    // Diffuse light
    vec3 diffuseColor = vec3(1.0, 1.0, 1.0);
    vec3 diffuseLight = angle * diffuseColor;

    // Specular light
    float specularStrength = 1.0;
    vec3 viewDir = normalize(vec3(0.0, 0.0, 0.0) - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specularLight = specularStrength * spec * vec3(1.0, 1.0, 1.0) * angle;

    // Combine lighting components and texture
    vec3 result = (ambientLight + diffuseLight + specularLight) * texture(textureSampler, TexCoord).rgb;
    FragColor = vec4(result, 1.0);
}
