#pragma once

#include <array>
#include <cstddef>
#include <glad/glad.h>
#include <glm/glm.hpp>

struct BlockType {
    uint16_t id;
    std::array<uint8_t, 6> textures;
};

