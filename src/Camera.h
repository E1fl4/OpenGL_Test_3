#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum movementDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 front;
    glm::vec3 frontHorizontal;
    glm::vec3 up;
    glm::vec3 right;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float speed = 4.0f;
    float sensitivity = 0.1f;

    Camera() {
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    void processKeyboard(movementDirection direction, float deltaTime) {
        if (direction == FORWARD)
            position += frontHorizontal * speed * deltaTime;
        if (direction == BACKWARD)
            position -= frontHorizontal * speed * deltaTime;
        if (direction == LEFT)
            position -= right * speed * deltaTime;
        if (direction == RIGHT)
            position += right * speed * deltaTime;
        if (direction == UP)
            position += worldUp * speed * 0.8f * deltaTime;
        if (direction == DOWN)
            position -= worldUp * speed * 0.8f * deltaTime;
    }

    void processMouseMovement(float xOffset, float yOffset) {
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw += xOffset;
        pitch += yOffset;

        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }

        updateCameraVectors();
    }

private:
    void updateCameraVectors() {
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);
        frontHorizontal = glm::vec3(cos(glm::radians(yaw)), 0.0f, sin(glm::radians(yaw)));
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
};

