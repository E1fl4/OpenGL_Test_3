#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Blocks.h"
#include "Chunk.h"
#include "Shader.h"
#include "Camera.h"
#include "glm/common.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);
unsigned int initTextures();
void setupBlockFaces(unsigned int* VAOs, unsigned int* VBOs);

Camera camera;

const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 700;

float lastX = SCREEN_WIDTH/2.0f;
float lastY = SCREEN_HEIGHT/2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool doSunlight = true;
glm::vec3 sunlightDirection = glm::normalize(glm::vec3(0.5f, -1.0f, -0.3f));
glm::vec4 pointLightPositions[0] = {
    // glm::vec4(0.0f, 2.0f, -9.0f, 1.0f),
    // glm::vec4(18.0f, 2.0f, -9.0f, 1.0f)
};


std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash> chunks;

uint16_t hotbar[9] = {
    Blocks::DIRT,
    Blocks::STONE,
    Blocks::VOID,
    Blocks::VOID,
    Blocks::VOID,
    Blocks::VOID,
    Blocks::VOID,
    Blocks::VOID,
    Blocks::VOID
};
unsigned int activeHotbarSlot = 0;

struct ImageData {
    int width;
    int height;
    int channels;
    unsigned char* pixels;
};
ImageData loadImage(const std::string& path);

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
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader blockShader("../resources/shaders/block.vert", "../resources/shaders/block.frag");
    Shader crosshairShader("../resources/shaders/crosshair.vert", "../resources/shaders/crosshair.frag");

    unsigned int textures = initTextures();
    Blocks::init();

    for (int x = 0; x < 32; x++) {
        for (int z = 0; z < 32; z++) {
            ChunkCoord coord{x, z};
            chunks.emplace(coord, std::make_unique<Chunk>(coord));
        }
    }

    float crosshairVertices[] = {
        -0.006f, -0.05f,
        0.006f, 0.05f,
        -0.006f, 0.05f,

        -0.006f, -0.05f,
        0.006f, -0.05f,
        0.006f, 0.05f,

        -0.05f, -0.006f,
        0.05f, 0.006f,
        -0.05f, 0.006f,

        -0.05f, -0.006f,
        0.05f, -0.006f,
        0.05f, 0.006f
    };
    unsigned int crosshairVAO, crosshairVBO;
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 2000.0f);
    blockShader.use();
    blockShader.setMat4("projection", glm::value_ptr(projection));

    blockShader.setInt("material.texture_diffuse1", 0);
    blockShader.setVec3("material.specular", glm::vec3(1.0f));
    blockShader.setFloat("material.shininess", 8.0f);

    blockShader.setBool("DoSunlight", doSunlight);
    if (doSunlight) {
        blockShader.setVec3("Sunlight.ambient", glm::vec3(0.38f));
        blockShader.setVec3("Sunlight.diffuse", glm::vec3(1.0f));
        blockShader.setVec3("Sunlight.specular", glm::vec3(0.1f));
    }

    for (unsigned int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
        blockShader.setVec3("PointLights[" + std::to_string(i) + "].ambient", glm::vec3(0.3f));
        blockShader.setVec3("PointLights[" + std::to_string(i) + "].diffuse", glm::vec3(1.0f));
        blockShader.setVec3("PointLights[" + std::to_string(i) + "].specular", glm::vec3(0.3f));
        blockShader.setFloat("PointLights[" + std::to_string(i) + "].constant", 1.0f);
        blockShader.setFloat("PointLights[" + std::to_string(i) + "].linear", 0.09f);
        blockShader.setFloat("PointLights[" + std::to_string(i) + "].quadratic", 0.032f);
    }

    crosshairShader.use();
    crosshairShader.setFloat("aspectRatio", (float)SCREEN_WIDTH/SCREEN_HEIGHT);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        // glClearColor(31.0f/255.0f, 30.0f/255.0f, 51.0f/255.0f, 1.0f);
        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
        // glClearColor(0.8f, 0.2f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 view = camera.getViewMatrix();

        blockShader.use();
        blockShader.setVec3("material.specular", glm::vec3(1.0f));
        blockShader.setFloat("material.shininess", 8.0f);

        blockShader.setBool("DoSunlight", doSunlight);
        if (doSunlight) {
            blockShader.setVec3("Sunlight.direction", glm::mat3(view) * sunlightDirection);
        }

        for (unsigned int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
            blockShader.setVec3("PointLights[" + std::to_string(i) + "].position", view * pointLightPositions[i]);
        }

        blockShader.setMat4("view", glm::value_ptr(view));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textures);

        for (auto& [coord, chunkPtr] : chunks) {
            chunkPtr->draw(blockShader);
        }

        crosshairShader.use();
        glBindVertexArray(crosshairVAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(blockShader.ID);
    glDeleteProgram(crosshairShader.ID);
    glDeleteTextures(1, &textures);
    glDeleteVertexArrays(1, &crosshairVAO);
    glDeleteBuffers(1, &crosshairVBO);
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

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        activeHotbarSlot = 0;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        activeHotbarSlot = 1;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        activeHotbarSlot = 2;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        activeHotbarSlot = 3;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        activeHotbarSlot = 4;
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        activeHotbarSlot = 5;
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
        activeHotbarSlot = 6;
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
        activeHotbarSlot = 7;
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
        activeHotbarSlot = 8;

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

void tryFindBlock(const int& button) {
    glm::ivec3 block = glm::floor(camera.position);
    glm::ivec3 step = glm::sign(camera.front);
    glm::vec3 tDelta(
        std::abs(1.0f / camera.front.x),
        std::abs(1.0f / camera.front.y),
        std::abs(1.0f / camera.front.z)
    );
    glm::vec3 tMax(
        (step.x>0 ? (block.x+1-camera.position.x) : (camera.position.x-block.x)) * tDelta.x,
        (step.y>0 ? (block.y+1-camera.position.y) : (camera.position.y-block.y)) * tDelta.y,
        (step.z>0 ? (block.z+1-camera.position.z) : (camera.position.z-block.z)) * tDelta.z
    );
    while (std::min({tMax.x, tMax.y, tMax.x}) < 6.0f) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            auto it = chunks.find({block.x/16, block.z/16});
            if (it == chunks.end()) continue;
            Chunk& chunk = *it->second;
            int localX = (block.x % 16 + 16) % 16;
            int localZ = (block.z % 16 + 16) % 16;
            if (!chunk.blockIsAir(glm::ivec3(localX, block.y, localZ))) {
                chunk.setBlock(chunk.indexFromPos({localX, block.y, localZ}), Blocks::AIR);
                chunk.buildMesh();
                break;
            }
        }
        glm::ivec3 thisStep;
        if (std::min({tMax.x, tMax.y, tMax.z}) == tMax.x) {
            thisStep = glm::ivec3(step.x, 0, 0);
            tMax.x += tDelta.x;
        } else if (std::min({tMax.x, tMax.y, tMax.z}) == tMax.y) {
            thisStep = glm::ivec3(0, step.y, 0);
            tMax.y += tDelta.y;
        } else if (std::min({tMax.x, tMax.y, tMax.z}) == tMax.z) {
            thisStep = glm::ivec3(0, 0, step.z);
            tMax.z += tDelta.z;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            auto it = chunks.find({(block.x+thisStep.x)/16, (block.z+thisStep.z)/16});
            if (it == chunks.end()) continue;
            Chunk& chunk = *it->second;
            int localX = ((block.x + thisStep.x) % 16 + 16) % 16;
            int localZ = ((block.z + thisStep.z) % 16 + 16) % 16;
            if (!chunk.blockIsAir(glm::ivec3(localX, block.y + thisStep.y, localZ))) {
                auto it2 = chunks.find({block.x/16, block.z/16});
                if (it2 == chunks.end()) continue;
                Chunk& chunk2 = *it2->second;
                localX = (block.x % 16 + 16) % 16;
                localZ = (block.z % 16 + 16) % 16;
                chunk2.setBlock(chunk2.indexFromPos({localX, block.y, localZ}), Blocks::STONE);
                chunk2.buildMesh();
                break;
            }
        }
        block += thisStep;
    }
}

// void tryFindBlock(int button) {
//     glm::ivec3 currentBlock = glm::floor(camera.position);
//     glm::ivec3 step = glm::sign(camera.front);
//     glm::vec3 totalSteps(
//         step.x==1 ? 1-(camera.position.x-currentBlock.x) : camera.position.x-currentBlock.x,
//         step.y==1 ? 1-(camera.position.y-currentBlock.y) : camera.position.y-currentBlock.y,
//         step.z==1 ? 1-(camera.position.z-currentBlock.z) : camera.position.z-currentBlock.z
//     );
//     for (int i = 0; i < 5; i++) {
//         std::cout << currentBlock.x << ", " << currentBlock.y << ", " << currentBlock.z << ", \n";
//         glm::vec3 lengths(
//             glm::vec3(totalSteps.x, totalSteps.x*(camera.front.y/camera.front.x), totalSteps.x*(camera.front.z/camera.front.x)).length(),
//             glm::vec3(totalSteps.y*(camera.front.x/camera.front.y), totalSteps.y, totalSteps.y*(camera.front.z/camera.front.y)).length(),
//             glm::vec3(totalSteps.z*(camera.front.x/camera.front.z), totalSteps.z*(camera.front.y/camera.front.z), totalSteps.z).length()
//         );
//         if (std::min({lengths.x, lengths.y, lengths.z}) == lengths.x) {
//             currentBlock.x += step.x;
//             totalSteps.x += step.x;
//         } else if (std::min({lengths.x, lengths.y, lengths.z}) == lengths.y) {
//             currentBlock.y += step.y;
//             totalSteps.y += step.y;
//         } else if (std::min({lengths.x, lengths.y, lengths.z}) == lengths.z) {
//             currentBlock.z += step.z;
//             totalSteps.z += step.z;
//         }
//         auto it = chunks.find({currentBlock.x/16, currentBlock.z/16});
//         if (it == chunks.end()) continue;
//         Chunk& chunk = *it->second;
//         if (!chunk.blockIsAir(glm::ivec3(currentBlock.x % 16, currentBlock.y, currentBlock.z % 16))) {
//             chunk.setBlock(chunk.indexFromPos({currentBlock.x % 16, currentBlock.y, currentBlock.z % 16}), Blocks::AIR);
//             chunk.buildMesh();
//             break;
//         }
//     }
// }

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        tryFindBlock(button);
    }
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

ImageData loadImage(const std::string& path) {
    ImageData img;
    img.pixels = stbi_load(path.c_str(), &img.width, &img.height, &img.channels, 4);
    return img;
}

unsigned int initTextures() {
    stbi_set_flip_vertically_on_load(true);
    std::vector<std::string> texturePaths = {
        "../resources/textures/bedrock.png",
        "../resources/textures/coal_ore.png",
        "../resources/textures/diamond_ore.png",
        "../resources/textures/dirt.png",
        "../resources/textures/grass_carried.png",
        "../resources/textures/grass_side_carried.png",
        "../resources/textures/iron_ore.png",
        "../resources/textures/log_oak.png",
        "../resources/textures/log_oak_top.png",
        "../resources/textures/netherrack.png",
        "../resources/textures/planks_oak.png",
        "../resources/textures/stone.png"
    };
    unsigned int textures;
    glGenTextures(1, &textures);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textures);
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_RGBA8,
        16,
        16,
        texturePaths.size(),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );
    std::vector<ImageData> images;
    for (const std::string& path : texturePaths) {
        images.push_back(loadImage(path));
    }
    for (unsigned int i = 0; i < images.size(); i++) {
        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0, 0, 0, i,
            16, 16, 1,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            images[i].pixels
        );
        stbi_image_free(images[i].pixels);
    }
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return textures;
}

void setupBlockFaces(unsigned int* VAOs, unsigned int* VBOs) {
    const float vertices[6][48] = {
        { // Top
            0.0f,1.0f,0.0f, 0.0f,1.0f, 0.0f,1.0f,0.0f,
            1.0f,1.0f,0.0f, 1.0f,1.0f, 0.0f,1.0f,0.0f,
            0.0f,1.0f,1.0f, 0.0f,0.0f, 0.0f,1.0f,0.0f,
            1.0f,1.0f,0.0f, 1.0f,1.0f, 0.0f,1.0f,0.0f,
            1.0f,1.0f,1.0f, 1.0f,0.0f, 0.0f,1.0f,0.0f,
            0.0f,1.0f,1.0f, 0.0f,0.0f, 0.0f,1.0f,0.0f
        },
        { // Bottom
            0.0f,0.0f,1.0f, 0.0f,1.0f, 0.0f,-1.0f,0.0f,
            1.0f,0.0f,1.0f, 1.0f,1.0f, 0.0f,-1.0f,0.0f,
            0.0f,0.0f,0.0f, 0.0f,0.0f, 0.0f,-1.0f,0.0f,
            1.0f,0.0f,1.0f, 1.0f,1.0f, 0.0f,-1.0f,0.0f,
            1.0f,0.0f,0.0f, 1.0f,0.0f, 0.0f,-1.0f,0.0f,
            0.0f,0.0f,0.0f, 0.0f,0.0f, 0.0f,-1.0f,0.0f
        },
        { // North
            1.0f,1.0f,0.0f, 0.0f,1.0f, 0.0f,0.0f,-1.0f,
            0.0f,1.0f,0.0f, 1.0f,1.0f, 0.0f,0.0f,-1.0f,
            1.0f,0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f,-1.0f,
            0.0f,1.0f,0.0f, 1.0f,1.0f, 0.0f,0.0f,-1.0f,
            0.0f,0.0f,0.0f, 1.0f,0.0f, 0.0f,0.0f,-1.0f,
            1.0f,0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f,-1.0f
        },
        { // South
            0.0f,1.0f,1.0f, 0.0f,1.0f, 0.0f,0.0f,1.0f,
            1.0f,1.0f,1.0f, 1.0f,1.0f, 0.0f,0.0f,1.0f,
            0.0f,0.0f,1.0f, 0.0f,0.0f, 0.0f,0.0f,1.0f,
            1.0f,1.0f,1.0f, 1.0f,1.0f, 0.0f,0.0f,1.0f,
            1.0f,0.0f,1.0f, 1.0f,0.0f, 0.0f,0.0f,1.0f,
            0.0f,0.0f,1.0f, 0.0f,0.0f, 0.0f,0.0f,1.0f
        },
        { // East
            1.0f,1.0f,1.0f, 0.0f,1.0f, 1.0f,0.0f,0.0f,
            1.0f,1.0f,0.0f, 1.0f,1.0f, 1.0f,0.0f,0.0f,
            1.0f,0.0f,1.0f, 0.0f,0.0f, 1.0f,0.0f,0.0f,
            1.0f,1.0f,0.0f, 1.0f,1.0f, 1.0f,0.0f,0.0f,
            1.0f,0.0f,0.0f, 1.0f,0.0f, 1.0f,0.0f,0.0f,
            1.0f,0.0f,1.0f, 0.0f,0.0f, 1.0f,0.0f,0.0f
        },
        { // West
            0.0f,1.0f,0.0f, 0.0f,1.0f, -1.0f,0.0f,0.0f,
            0.0f,1.0f,1.0f, 1.0f,1.0f, -1.0f,0.0f,0.0f,
            0.0f,0.0f,0.0f, 0.0f,0.0f, -1.0f,0.0f,0.0f,
            0.0f,1.0f,1.0f, 1.0f,1.0f, -1.0f,0.0f,0.0f,
            0.0f,0.0f,1.0f, 1.0f,0.0f, -1.0f,0.0f,0.0f,
            0.0f,0.0f,0.0f, 0.0f,0.0f, -1.0f,0.0f,0.0f
        }
    };

    glGenVertexArrays(6, VAOs);
    glGenBuffers(6, VBOs);

    for (unsigned int i = 0; i < 6; i++) {
        glBindVertexArray(VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[i]), vertices[i], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    glBindVertexArray(0);
}

