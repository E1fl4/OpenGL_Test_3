#pragma once

#include <cstddef>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include "glm/fwd.hpp"

struct BlockType {
    unsigned int textures[6];
};

class Block {
public:
    glm::vec3 position;
    BlockType blockType;
    Block(const glm::vec3& position, const BlockType& blockType);
    ~Block();
    Block(const Block&&) = delete;
    Block &operator = (const Block&) = delete;
    void draw(const unsigned int* VAOs, const Shader& shader, const Camera& camera);
private:
};

struct vecHash {
    std::size_t operator()(const glm::ivec3& v) const noexcept {
        std::size_t h1 = std::hash<int>{}(v.x);
        std::size_t h2 = std::hash<int>{}(v.y);
        std::size_t h3 = std::hash<int>{}(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

