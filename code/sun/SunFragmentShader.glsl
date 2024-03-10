#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureSampler;

void main() {
    
    vec4 texColor = texture(textureSampler, TexCoord);

    const float emissiveStrength = 0.2;

    vec3 emissiveColor = vec3(emissiveStrength,emissiveStrength,emissiveStrength);

    // Add emissive color
    vec3 colorWithEmission = texColor.rgb + emissiveColor;

    FragColor = vec4(colorWithEmission, 1.0);


}
