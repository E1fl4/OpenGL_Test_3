#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path);

Camera camera;

const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 700;

float lastX = 400;
float lastY = 300;
bool firstMouse = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool doSunlight = false;
glm::vec3 sunlightDirection = glm::normalize(glm::vec3(0.5f, -1.0f, -0.3f));
glm::vec4 pointLightPositions[] = {
    glm::vec4(0.0f, 2.0f, -9.0f, 1.0f),
    glm::vec4(20.0f, 2.0f, -9.0f, 1.0f)
};

int main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Test 3", NULL, NULL);
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

    Shader blockShader("../resources/shaders/shader.vert", "../resources/shaders/blockShader.frag");
    Shader lightSourceShader("../resources/shaders/shader.vert", "../resources/shaders/lightSourceShader.frag");

    float vertices[] = {
        -0.5f,  0.5f, 0.5f,     0.0f, 1.0f,     0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.5f,     1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.5f,     1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f,     0.0f, 0.0f, 1.0f,

        0.5f,  0.5f,  0.5f,     0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,     1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,     0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,     1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,     1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,     0.0f, 0.0f,     1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, -0.5f,     0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
         0.5f, 0.5f, -0.5f,     1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f,  0.5f,     0.0f, 0.0f,     0.0f, 1.0f, 0.0f,
         0.5f, 0.5f, -0.5f,     1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
         0.5f, 0.5f,  0.5f,     1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f,  0.5f,     0.0f, 0.0f,     0.0f, 1.0f, 0.0f,

         0.5f,  0.5f, -0.5f,     0.0f, 1.0f,     0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,     1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,     0.0f, 0.0f,     0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,     1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,     1.0f, 0.0f,     0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,     0.0f, 0.0f,     0.0f, 0.0f, -1.0f,

        -0.5f,  0.5f, -0.5f,     0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,     1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,     1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,     1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,     0.0f, 1.0f,     0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     1.0f, 1.0f,     0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     1.0f, 1.0f,     0.0f, -1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,     1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,     0.0f, -1.0f, 0.0f
    };

    stbi_set_flip_vertically_on_load(true);
    unsigned int texture = loadTexture("../resources/textures/diamond_ore.png");

    unsigned int VAO, VBO, lightVAO;
    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    blockShader.use();
    blockShader.setInt("material.diffuse", 0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        // glClearColor(31.0f/255.0f, 30.0f/255.0f, 51.0f/255.0f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // lightSourcePos.x = sin(glfwGetTime());
        // lightSourcePos.z = cos(glfwGetTime());

        // lightSourceColor.x = sin(glfwGetTime() * 0.14f) / 2.0f + 0.5f;
        // lightSourceColor.y = sin(glfwGetTime() * 0.32f) / 2.0f + 0.5f;
        // lightSourceColor.z = sin(glfwGetTime() * 0.02f) / 2.0f + 0.5f;

        glm::mat4 view = camera.getViewMatrix();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);

        glm::mat4 blockModel(1.0f);
        // glm::mat4 lightSourceModel(1.0f);
        // lightSourceModel = glm::translate(lightSourceModel, lightPos);
        // lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.2f));

        blockShader.use();
        blockShader.setVec3("material.specular", glm::vec3(1.0f));
        blockShader.setFloat("material.shininess", 16.0f);

        blockShader.setBool("DoSunlight", doSunlight);
        if (doSunlight) {
            blockShader.setVec3("Sunlight.direction", sunlightDirection);
            blockShader.setVec3("Sunlight.ambient", glm::vec3(0.3f));
            blockShader.setVec3("Sunlight.diffuse", glm::vec3(1.0f));
            blockShader.setVec3("Sunlight.specular", glm::vec3(0.3f));
        }
        for (unsigned int i = 0; i < 2; i++) {
            blockShader.setVec3("PointLights[" + std::to_string(i) + "].position", view * pointLightPositions[i]);
            blockShader.setVec3("PointLights[" + std::to_string(i) + "].ambient", glm::vec3(0.3f));
            blockShader.setVec3("PointLights[" + std::to_string(i) + "].diffuse", glm::vec3(1.0f));
            blockShader.setVec3("PointLights[" + std::to_string(i) + "].specular", glm::vec3(0.3f));
            blockShader.setFloat("PointLights[" + std::to_string(i) + "].constant", 1.0f);
            blockShader.setFloat("PointLights[" + std::to_string(i) + "].linear", 0.09f);
            blockShader.setFloat("PointLights[" + std::to_string(i) + "].quadratic", 0.032f);
        }

        blockShader.setMat4("view", glm::value_ptr(view));
        blockShader.setMat4("projection", glm::value_ptr(projection));
        blockShader.setMat4("model", glm::value_ptr(blockModel));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

        for (unsigned int i = 0; i < 10; i++) {
            for (unsigned int j = 0; j < 10; j++) {
                blockModel = glm::translate(glm::mat4(1.0f), glm::vec3((float)i * 2, 0.0f, (float)j * -2));
                blockShader.setMat4("model", glm::value_ptr(blockModel));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // lightSourceShader.use();
        // lightSourceShader.setVec3("LightColor", lightSourceColor);
        // lightSourceShader.setMat4("view", glm::value_ptr(view));
        // lightSourceShader.setMat4("projection", glm::value_ptr(projection));
        // lightSourceShader.setMat4("model", glm::value_ptr(lightSourceModel));

        // glBindVertexArray(lightVAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(blockShader.ID);
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.processKeyboard(DOWN, deltaTime);
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

    camera.processMouseMovement(xOffset, yOffset);
}

unsigned int loadTexture(char const * path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

