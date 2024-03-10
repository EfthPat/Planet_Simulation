#version 330 core

// Position vector of each vertex (x, y, z coordinates)
layout (location = 0) in vec3 aPos;

// Texture coordinate of each vertex (u, v coordinates)
layout (location = 1) in vec2 aTexCoord;

// Normal vector of each vertex (x, y, z coordinates)
layout (location = 2) in vec3 aNormal;

// Model matrix for transforming model coordinates to world coordinates
uniform mat4 model;

// View matrix for transforming world coordinates to camera coordinates
uniform mat4 view;

// Projection matrix for projecting 3D coordinates onto a 2D plane
uniform mat4 projection;     

// Passed to fragment shader: texture coordinate for texturing
out vec2 TexCoord;     

// Passed to fragment shader: normal vector transformed to world space
out vec3 Normal;             

// Passed to fragment shader: fragment position in world coordinates
out vec3 FragPos;            

void main() {

    TexCoord = aTexCoord;

    // Converts normal vector from model to world coordinates for correct lighting
    Normal = mat3(transpose(inverse(model))) * aNormal; 

    // Transforms vertex position from model to world coordinates
    FragPos = vec3(model * vec4(aPos, 1.0)); 

    // Calculates final position of vertex on screen, combining all transformations
    gl_Position = projection * view * model * vec4(aPos, 1.0);

}
