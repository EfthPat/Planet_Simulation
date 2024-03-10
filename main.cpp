#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "./code/sun/SunModel.h"
#include "./code/moon/MoonModel.h"
#include "./code/planet/PlanetModel.h"
#include "./code/earth/EarthModel.h"
#include "./code/camera/Camera.h"

int main() {

    // Initialize GLFW
    glfwInit();

    // Configure GLFW (OpenGL version 3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Get the primary monitor and its video mode
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // Create a full-screen window with the monitor's resolution
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "OpenGL Project: Solar System", primaryMonitor, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);

    // Set the viewport to cover the full window
    glViewport(0, 0, mode->width, mode->height);

    // Create an instance of SunModel
    SunModel sunModel("./assets/sun/sun.obj", "./code/sun/SunVertexShader.glsl", "./code/sun/SunFragmentShader.glsl", "./assets/sun/sun.jpg");

    // Create an instance of EarthModel
    EarthModel earthModel("./assets/earth/Earth.obj", "./code/earth/EarthVertexShader.glsl", "./code/earth/EarthFragmentShader.glsl", "./assets/earth/Earth.png");
    
    // Create an instance of MoonModel
    MoonModel moonModel("./assets/moon/Moon.obj", "./code/moon/MoonVertexShader.glsl", "./code/moon/MoonFragmentShader.glsl", "./assets/moon/Moon.png");


    // Create an array to store the skins of the stars
    std::vector<std::string> planetLinks = { "./assets/planet/Planet_1.png", "./assets/planet/Planet_2.png", "./assets/planet/Planet_3.png" };
    
    // Create an array to store the planets
    std::vector<PlanetModel> planets;
    unsigned int totalPlanets = 5;
    srand(static_cast<unsigned int>(time(nullptr)));
    for (unsigned int i = 0; i < totalPlanets; ++i) {
        // Pass the vector of texture paths to the constructor
        PlanetModel planetModel("./assets/planet/Planet.obj", "./code/planet/PlanetVertexShader.glsl", "./code/planet/PlanetFragmentShader.glsl", planetLinks);
        planets.push_back(planetModel);
    }

    // Create an instance for the camera - window , initial position , initial up-vector, initial yaw (x-axis angle) , initial pitch (y-axis angle)
    Camera camera(window, glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

    // Render loop
    while (!glfwWindowShouldClose(window)) {

        // If ESCAPE was pressed..
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            // Exit the render loop
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        }

        // Clear color and depth buffers to prevent old data from affecting the new frame
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // Update the camera's position
        camera.update();
        glm::mat4 viewMatrix = camera.getViewMatrix();

        // Render the sun, earth, moon and the random planets, given the camera's current position
        sunModel.render(viewMatrix);
        earthModel.render(viewMatrix);
        moonModel.render(earthModel.getEarthPosition(), viewMatrix);
        for (PlanetModel& planet : planets) {
            planet.render(viewMatrix); 
        }

        // Swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    // Terminate the program, clearing all the previously allocated GLFW resources
    glfwTerminate();
    return 0;

}
