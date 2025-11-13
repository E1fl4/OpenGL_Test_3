#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 700;

float lastX = 400;
float lastY = 300;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

float yaw = -90.0f;
float pitch = 0.0f;

int main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader shader("../resources/shaders/shader.vert", "../resources/shaders/shader.frag");
    
    float vertices[] = {
        -0.5f,  0.5f, 0.5f,     0.0f, 1.0f,
         0.5f,  0.5f, 0.5f,     1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f,
         0.5f,  0.5f, 0.5f,     1.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f,

        0.5f,  0.5f,  0.5f,     0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,     1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,     0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,     1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,     1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,     0.0f, 0.0f,

        -0.5f, 0.5f, -0.5f,     0.0f, 1.0f,
         0.5f, 0.5f, -0.5f,     1.0f, 1.0f,
        -0.5f, 0.5f,  0.5f,     0.0f, 0.0f,
         0.5f, 0.5f, -0.5f,     1.0f, 1.0f,
         0.5f, 0.5f,  0.5f,     1.0f, 0.0f,
        -0.5f, 0.5f,  0.5f,     0.0f, 0.0f,

         0.5f,  0.5f, -0.5f,     0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,     1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,     1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,     1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,     0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,     0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,     1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,     1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,     1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,     0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,     1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    // TEXTURE
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_set_flip_vertically_on_load(true);
    int texWidth, texHeight, nrChannels;
    unsigned char *textureData = stbi_load("../resources/textures/diamond_ore.png", &texWidth, &texHeight, &nrChannels, 0);
    if (textureData) {
        GLenum colorFormat = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, texWidth, texHeight, 0, colorFormat, GL_UNSIGNED_BYTE, textureData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(textureData);
    //

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(31.0f/255.0f, 30.0f/255.0f, 51.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        shader.use();

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);

        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", glm::value_ptr(view));

        glm::mat4 projection(1.0);
        projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", glm::value_ptr(projection));
        
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model(1.0);
            model = glm::translate(model, cubePositions[i]);
            // model = glm::rotate(model, (float)glfwGetTime()*((float)(i+1)/2), glm::normalize(glm::vec3(1.0f, 2.0f, 0.0f)));
            shader.setMat4("model", glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader.ID);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    const float cameraSpeed = 2.5f * deltaTime;
    const glm::vec3 cameraFrontHorizontal(cos(glm::radians(yaw)), 0.0f, sin(glm::radians(yaw)));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraFrontHorizontal;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraFrontHorizontal;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cameraPos += 1.5f * cameraSpeed * worldUp;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraPos -= 1.5f * cameraSpeed * worldUp;
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    if(pitch > 89.0f) {
        pitch = 89.0f;
    }
    if(pitch < -89.0f) {
        pitch = -89.0f;
    }

    yaw += xOffset;
    pitch += yOffset;
}
