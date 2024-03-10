#version 330 core

// Position attribute
layout (location = 0) in vec3 aPos;     

// Texture coordinate attribute
layout (location = 1) in vec2 aTexCoord;

// Model matrix for transforming model space to world space
uniform mat4 model;

// View matrix for transforming world space to camera space
uniform mat4 view;      

// Projection matrix for transforming camera space to clip space
uniform mat4 projection;

// Output texture coordinate to fragment shader
out vec2 TexCoord;

void main() {

    // Calculate the final vertex position in clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Pass the texture coordinate to the fragment shader
    TexCoord = aTexCoord;

}
