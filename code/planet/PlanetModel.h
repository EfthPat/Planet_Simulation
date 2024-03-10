#ifndef PLANET_MODEL_H
#define PLANET_MODEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>


class PlanetModel {

public:

    // Constructor: Initializes a new instance of SunModel with paths for model, shaders, and texture
    PlanetModel(const std::string& modelPath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::vector<std::string>& texturePaths);

    

    // Renders the planet model
    void render(const glm::mat4& viewMatrix);

    ~PlanetModel();

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

    // Loads the model from a given file path
    void loadModel(const std::string& path);

    // Compiles and links the vertex and fragment shaders
    void compileShaders(const std::string& vertexPath, const std::string& fragmentPath);

    // Loads a texture from a given file path
    void loadTexture(const std::string& texturePath);

    // Sets up the transformation matrices for the model
    void setupMatrices();

    // Processes an individual mesh of the model
    void processMesh(aiMesh* mesh, const aiScene* scene);

    // Sets up the vertex buffers and attributes
    void setupBuffers();

};

#endif

