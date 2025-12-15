#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <string>

class Block {
public:
    glm::vec3 position;
    unsigned int texture;
    std::string name = "diamond_ore";
    Block(glm::vec3 position, unsigned int texture) {
        this->position = position;
        this->texture = texture;
        setupFaces();
    }
    ~Block() {
        glDeleteVertexArrays(1, &topVAO);
        glDeleteVertexArrays(1, &bottomVAO);
        glDeleteVertexArrays(1, &northVAO);
        glDeleteVertexArrays(1, &southVAO);
        glDeleteVertexArrays(1, &eastVAO);
        glDeleteVertexArrays(1, &westVAO);
        glDeleteBuffers(1, &topVBO);
        glDeleteBuffers(1, &bottomVBO);
        glDeleteBuffers(1, &northVBO);
        glDeleteBuffers(1, &southVBO);
        glDeleteBuffers(1, &eastVBO);
        glDeleteBuffers(1, &westVBO);
        glDeleteTextures(1, &texture);
    }
    Block(const Block&&) = delete;
    Block &operator = (const Block&) = delete;
    void draw(const Shader &shader) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, position);
        shader.setMat4("model", glm::value_ptr(model));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(topVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(bottomVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(northVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(southVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(eastVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(westVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
private:
    unsigned int topVAO, bottomVAO, northVAO, southVAO, eastVAO, westVAO;
    unsigned int topVBO, bottomVBO, northVBO, southVBO, eastVBO, westVBO;
    void setupFaces() {
        float southVertices[] = {
            0.0f, 1.0f, 1.0f,     0.0f, 1.0f,     0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,     1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
        };

        float eastVertices[] = {
            1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,     1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f,     0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,     1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,     1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f,     0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
        };

        float topVertices[] = {
            0.0f, 1.0f, 0.0f,     0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,     1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,     1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 1.0f, 0.0f,
        };

        float northVertices[] = {
            1.0f, 1.0f, 0.0f,     0.0f, 1.0f,     0.0f, 0.0f, -1.0f,
            0.0f, 1.0f, 0.0f,     1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
            1.0f, 0.0f, 0.0f,     0.0f, 0.0f,     0.0f, 0.0f, -1.0f,
            0.0f, 1.0f, 0.0f,     1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, 0.0f,     1.0f, 0.0f,     0.0f, 0.0f, -1.0f,
            1.0f, 0.0f, 0.0f,     0.0f, 0.0f,     0.0f, 0.0f, -1.0f,
        };

        float westVertices[] = {
            0.0f, 1.0f, 0.0f,     0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 1.0f,     1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,     0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 1.0f,     1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,     1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,     0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
        };

        float bottomVertices[] = {
            0.0f, 0.0f, 1.0f,     0.0f, 1.0f,     0.0f, -1.0f, 0.0f,
            1.0f, 0.0f, 1.0f,     1.0f, 1.0f,     0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 0.0f,     0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
            1.0f, 0.0f, 1.0f,     1.0f, 1.0f,     0.0f, -1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,     1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 0.0f,     0.0f, 0.0f,     0.0f, -1.0f, 0.0f
        };

        glGenVertexArrays(1, &topVAO);
        glGenBuffers(1, &topVBO);
        glGenVertexArrays(1, &bottomVAO);
        glGenBuffers(1, &bottomVBO);
        glGenVertexArrays(1, &northVAO);
        glGenBuffers(1, &northVBO);
        glGenVertexArrays(1, &southVAO);
        glGenBuffers(1, &southVBO);
        glGenVertexArrays(1, &eastVAO);
        glGenBuffers(1, &eastVBO);
        glGenVertexArrays(1, &westVAO);
        glGenBuffers(1, &westVBO);

        glBindVertexArray(topVAO);
        glBindBuffer(GL_ARRAY_BUFFER, topVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(topVertices), topVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(bottomVAO);
        glBindBuffer(GL_ARRAY_BUFFER, bottomVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(bottomVertices), bottomVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(northVAO);
        glBindBuffer(GL_ARRAY_BUFFER, northVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(northVertices), northVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(southVAO);
        glBindBuffer(GL_ARRAY_BUFFER, southVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(southVertices), southVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(eastVAO);
        glBindBuffer(GL_ARRAY_BUFFER, eastVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(eastVertices), eastVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(westVAO);
        glBindBuffer(GL_ARRAY_BUFFER, westVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(westVertices), westVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
};
