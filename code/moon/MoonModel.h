#ifndef MOON_MODEL_H
#define MOON_MODEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class MoonModel {

public:

    // Constructor: Initializes a new instance of SunModel with paths for model, shaders, and texture
    MoonModel(const std::string& modelPath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& texturePath);

    // Renders the moon model
    void render(const glm::vec3& earthPosition, const glm::mat4& viewMatrix);

    // Destructor: Cleans up resources
    ~MoonModel();

private:

    // Stores the vertex data of the model
    std::vector<float> vertices;

    // OpenGL identifiers for Vertex Array Object and Vertex Buffer Object
    unsigned int VAO, VBO;

    // Number of vertices, set during processMesh() and used in render()
    unsigned int vertexCount;

    // OpenGL identifier for the texture
    unsigned int texture;

    // Identifier for the compiled and linked shader program
    unsigned int shaderProgram;

    // Timestamp of the last update for animations
    float lastUpdateTime;

    // Angle of Moon's rotation
    float rotationAngle;

    // Speed of Moon's self-rotation
    float rotationSpeed;

    // Radius of Moon's orbit around the Earth
    float orbitRadius;

    // Speed of Moon's orbit around the Earth
    float orbitSpeed;

    // Current angle of Moon's orbit
    float orbitAngle;

    // Flag to toggle pause-state of the animation
    bool isPaused;

    // Tracks if the space key was pressed in the last frame
    bool wasSpacePressed;

   
    // Processes an individual mesh of the model
    void processMesh(aiMesh* mesh, const aiScene* scene);

    // Sets up the vertex buffers and attributes
    void setupBuffers();

    // Loads the model from a given file path
    void loadModel(const std::string& path);

    // Compiles and links the vertex and fragment shaders
    void compileShaders(const std::string& vertexPath, const std::string& fragmentPath);
    
    // Loads a texture from a given file path
    void loadTexture(const std::string& texturePath);
    
    // Sets up the transformation matrices for the model
    void setupMatrices();

};

#endif
