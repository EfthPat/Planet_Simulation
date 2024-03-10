#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    
    // Constructor: initializes camera with position, orientation, and control parameters
    Camera(GLFWwindow* window, glm::vec3 startPosition, glm::vec3 startUp, float startYaw, float startPitch);

    // Update camera's position and orientation based on user input and time
    void update();

    // Get the view matrix, representing the camera's point of view
    glm::mat4 getViewMatrix() const;

private:
    
    // Reference to the GLFW window for input handling
    GLFWwindow* window;

    // Camera's position in world space
    glm::vec3 position;

    // The direction that the camera is facing
    glm::vec3 front;

    // The upward orientation of the camera
    glm::vec3 up;

    // Rightward orientation of the camera, perpendicular to 'up'
    glm::vec3 right;

    // World's upward orientation
    glm::vec3 worldUp;

    // Horizontal angle (left/right) of the camera's orientation
    float yaw;

    // Vertical angle (up/down) of the camera's orientation
    float pitch;

    // Distance of the camera from the point it orbits around
    float orbitRadius;

    // Speed at which the camera moves/zooms
    float movementSpeed;

    // Speed at which the camera rotates
    float rotationSpeed;

    // Timestamp of the last update frame for calculating delta time
    float lastFrame;

    // Update camera basis vectors: 'front', 'right', and 'up' based on current yaw and pitch
    void updateCameraVectors();

    // Process keyboard input to adjust yaw and pitch
    void processKeyboardInput(float deltaTime);
};

#endif
