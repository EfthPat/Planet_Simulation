#include "Camera.h"
#include <iostream>
#include <cmath>

// Constructor: Initializes camera with position, orientation, and control parameters
Camera::Camera(GLFWwindow* win, glm::vec3 startPosition, glm::vec3 startUp, float startYaw, float startPitch)
    : window(win), worldUp(startUp), yaw(startYaw), pitch(startPitch), orbitRadius(3.0f), movementSpeed(1000.0f), rotationSpeed(50.0f), lastFrame(0.0f) {

    // Set initial position
    position = startPosition;

    // Update camera vectors based on initial parameters
    updateCameraVectors();
}

// Calculates delta time and processes camera movement and rotation
void Camera::update() {

    // Get current time to calculate delta time
    float currentFrame = static_cast<float>(glfwGetTime());

    // Calculate time difference between the current frame and the last frame
    float deltaTime = currentFrame - lastFrame;

    // Update lastFrame time to the current frame time
    lastFrame = currentFrame;

    // Process keyboard inputs for camera rotation
    processKeyboardInput(deltaTime);

    // Update the camera's position and orientation vectors
    updateCameraVectors();
}


// Get view matrix representing the camera's point of view
glm::mat4 Camera::getViewMatrix() const {

    // Constructs a view matrix directed at the center of the scene
    return glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f), up);

}

// Process keyboard inputs to adjust the camera's orientation
void Camera::processKeyboardInput(float deltaTime) {

    // Adjust yaw (horizontal rotation) with right arrow key
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        yaw -= rotationSpeed * deltaTime;
    }

    // Adjust yaw (horizontal rotation) with left arrow key
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        yaw += rotationSpeed * deltaTime;
    }

    // Adjust pitch (vertical rotation) with up arrow key
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        pitch += rotationSpeed * deltaTime;
    }

    // Adjust pitch (vertical rotation) with down arrow key
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        pitch -= rotationSpeed * deltaTime;
    }

    // Limit pitch to prevent camera to flip-over
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    // Recalculate camera vectors after input processing
    updateCameraVectors();
}

// Update camera's basis vectors based on current yaw, pitch, and orbitRadius
void Camera::updateCameraVectors() {

    // Calculate new camera position using spherical coordinates
    position.x = orbitRadius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    position.y = orbitRadius * sin(glm::radians(pitch));
    position.z = orbitRadius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    // Set target position and calculate front vector
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    front = glm::normalize(target - position);

    // Recalculate right and up vectors based on new front vector
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}


