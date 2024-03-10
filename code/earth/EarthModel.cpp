#include "EarthModel.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include "stb_image.h"

// Constructor: Loads the model, compiles shaders, loads texture, and sets up matrices
EarthModel::EarthModel(const std::string & modelPath, const std::string & vertexShaderPath, const std::string & fragmentShaderPath, const std::string & texturePath) {

    loadModel(modelPath);

    compileShaders(vertexShaderPath, fragmentShaderPath);

    loadTexture(texturePath);

    setupMatrices();

    lastUpdateTime = static_cast<float>(glfwGetTime());


    // Initialize spin variables
    rotationAngle = 180.0f;
    rotationSpeed = 200.0f;

    // Initialize rotation variables
    orbitRadius = 1.4f;
    orbitSpeed = 50.0f; 
    orbitAngle = 10.0f;

    // Initialize animation variables
    isPaused = false;
    wasSpacePressed = false;

}



// Loads the model using Assimp and processes the mesh
void EarthModel::loadModel(const std::string& path) {

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    aiMesh* mesh = scene->mMeshes[0];
    processMesh(mesh, scene);

    setupBuffers();
}

// Processes the mesh and stores vertices, texture coordinates and normals 
void EarthModel::processMesh(aiMesh* mesh, const aiScene* scene) {

    // Clear previous data
    vertices.clear();

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {

        // Vertices
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);


        // Texture Coordinates
        if (mesh->mTextureCoords[0]) {
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        }

        // Normals
        if (mesh->HasNormals()) {
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);
        }

    }

    // Each point has 3 floats for vertice coordinates, 2 for texture coordinates, and 3 for normals
    vertexCount = static_cast<unsigned int>(vertices.size() / 8);
    
}

// Sets up the VAO and VBO for the model
void EarthModel::setupBuffers() {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    // Configure for the shader :
    // Vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Vertex normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error in setupBuffers: " << err << std::endl;
    }

}

// Initializes the model, view, and projection matrices
void EarthModel::setupMatrices() {

    // Get current window size
    int width, height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    // Create and set up the model matrix with translation
    glm::mat4 model = glm::mat4(1.0f);
    float translateX = -1.0f;
    float translateY = 0.0f;
    float translateZ = 0.0f; 
    model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));


    // Scale down the earth so that it doesn't appear too big
    model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

    // Set up the view matrix
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, upVector);

    // Create and set up the projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 100.0f);


    glUseProgram(shaderProgram);

    // Set the matrices as uniform variables, so the shaders can access them
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

}

// Compiles and links vertex and fragment shaders
void EarthModel::compileShaders(const std::string& vertexPath, const std::string& fragmentPath) {


    // Function to read shader source code from file
    auto readShaderFile = [](const std::string& filePath) -> std::string {
        std::ifstream shaderFile(filePath);
        if (!shaderFile) {
            std::cerr << "ERROR::SHADER::FILE_NOT_FOUND: " << filePath << std::endl;
            return "";
        }
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        return shaderStream.str();
        };

    // Read shader source code
    std::string vertexShaderCode = readShaderFile(vertexPath);
    std::string fragmentShaderCode = readShaderFile(fragmentPath);

    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vShaderCode = vertexShaderCode.c_str();
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fShaderCode = fragmentShaderCode.c_str();
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders into a program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Loads texture from a file and sets texture parameters
void EarthModel::loadTexture(const std::string& texturePath) {

    // Load image using stb_image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture at " << texturePath << std::endl;
        return;
    }

    // Generate and bind texture
    glGenTextures(1, &this->texture); // Use the class member variable 'texture'
    glBindTexture(GL_TEXTURE_2D, this->texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Assign image to texture
    // RGB image
    if (nrChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    // RGBA image
    else if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    // Free image memory
    stbi_image_free(data);
}

// Draws earth's model on the screen
void EarthModel::render(const glm::mat4& viewMatrix) {

    // Check if the spacebar is pressed
    bool isSpacePressed = (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS);

    // Toggle the paused state if the spacebar state changes
    if (isSpacePressed && !wasSpacePressed) {
        isPaused = !isPaused;

        // When the user resumes, update lastUpdateTime to the current time
        if (!isPaused) {
            lastUpdateTime = static_cast<float>(glfwGetTime());
        }
    }

    // Store the current spacebar state for the next frame
    wasSpacePressed = isSpacePressed;

    // Update earth's spin and orbit only if the application is not paused
    if (!isPaused) {

        // Update the orbit angle based on orbit speed
        float currentTime = static_cast<float>(glfwGetTime());;
        float deltaTime = currentTime - lastUpdateTime;
        lastUpdateTime = currentTime;
        orbitAngle += deltaTime * orbitSpeed;
        rotationAngle += deltaTime * rotationSpeed;
    }

    // Calculate earth's position, in orbit, around the sun
    float earthX = orbitRadius * cos(glm::radians(orbitAngle));
    float earthZ = orbitRadius * sin(glm::radians(orbitAngle));
    earthPosition = glm::vec3(earthX, 0.0f, earthZ);

    // Create the model matrix for earth :
    // Position Earth in its orbit
    glm::mat4 model = glm::translate(glm::mat4(1.0f), earthPosition);
    // Rotate Earth around its own axis
    model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    // Scale down Earth
    model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

    // Use shader program
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // Bind the Vertex Array Object (VAO)
    glBindVertexArray(VAO);

    // Draw the model
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    // Unbind the VAO and texture
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Unbind shader program
    glUseProgram(0);
}

// Returns Earth's position. Needed by Moon
glm::vec3 EarthModel::getEarthPosition() const {
    return earthPosition;
}

// Destructor: Clean up resources
EarthModel::~EarthModel() {

    // Delete the shader program
    if (shaderProgram)
        glDeleteProgram(shaderProgram);

    // Delete the texture
    if (texture)
        glDeleteTextures(1, &texture);

    // Delete the VAO and VBO
    if (VAO)
        glDeleteVertexArrays(1, &VAO);

    if (VBO)
        glDeleteBuffers(1, &VBO);

}
