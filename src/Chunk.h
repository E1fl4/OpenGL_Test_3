#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include "Shader.h"
#include "glm/fwd.hpp"
#include <cstddef>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoords;
    glm::vec3 normal;
    glm::uint8_t texIndex;
};

struct ChunkCoord {
    int x;
    int z;
    bool operator==(const ChunkCoord& other) const {
        return x == other.x && z == other.z;
    }
};

struct ChunkCoordHash {
    size_t operator()(const ChunkCoord& c) const {
        return std::hash<int>()(c.x) ^ std::hash<int>()(c.z) << 1;
    }
};

class Chunk {
public:
    ChunkCoord pos;
    Chunk(ChunkCoord pos, const bool& generate = true);
    ~Chunk();
    void draw(Shader& shader);
    void buildMesh();
    int indexFromPos(const glm::ivec3& pos);
    bool blockIsAir(const glm::ivec3& pos);
    void setBlock(const int& block, const uint16_t& blockType);

    Chunk(const Chunk&) = delete;
    Chunk& operator=(const Chunk&) = delete;
    Chunk(Chunk&&) noexcept;
    Chunk& operator=(Chunk&&) noexcept;
private:
    std::vector<uint16_t> blocks;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    unsigned int indexCount;
    unsigned int VAO, VBO, EBO;
    void initMesh();
    void genChunk();
};
