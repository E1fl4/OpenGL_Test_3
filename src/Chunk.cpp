#include "Chunk.h"
#include "FaceData.h"
#include "Blocks.h"
#include "Shader.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glad/glad.h>

Chunk::Chunk(ChunkCoord pos, const bool& generate) {
    this->pos = pos;
    if (generate) genChunk();
    initMesh();
    buildMesh();
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Chunk::draw(Shader& shader) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(pos.x*16, 0, pos.z*16));
    shader.setMat4("model", glm::value_ptr(model));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Chunk::initMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, texIndex));
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
}

void Chunk::buildMesh() {
    vertices.clear();
    indices.clear();
    for (int i = 0; i < blocks.size(); i++) {
        if (blocks[i] == Blocks::AIR) continue;
        glm::vec3 pos(i%16, i/(16*16), (i/16)%16);
        for (int face = 0; face < 6; face++) {
            if (blockIsAir(glm::ivec3(pos) + c_faces[face].normal)) {
                for (int corner = 0; corner < 4; corner++) {
                    vertices.push_back({
                        pos + c_faces[face].vertices[corner],
                        c_faces[face].texCoords[corner],
                        c_faces[face].normal,
                        Blocks::byId(blocks[i]).textures[face]
                    });
                }
                uint32_t baseIndex = vertices.size() - 4;
                indices.push_back(baseIndex + 0);
                indices.push_back(baseIndex + 1);
                indices.push_back(baseIndex + 2);
                indices.push_back(baseIndex + 2);
                indices.push_back(baseIndex + 3);
                indices.push_back(baseIndex + 0);
            }
        }
    }
    indexCount = indices.size();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

int Chunk::indexFromPos(const glm::ivec3& pos) {
    return pos.x + pos.z * 16 + pos.y * 16 * 16;
}

bool Chunk::blockIsAir(const glm::ivec3& pos) {
    if (pos.x > 15 || pos.x < 0
        || pos.z > 15 || pos.z < 0
        || pos.y > 255 || pos.y < 0
    ) return true;

    return blocks[indexFromPos(pos)] == Blocks::AIR;
}

void Chunk::setBlock(const int& block, const uint16_t& blockType) {
    blocks[block] = blockType;
}

void Chunk::genChunk() {
    blocks.resize(16 * 16 * 256, Blocks::AIR);
    for (int i = 0; i < 256; i++) {
        blocks[i] = Blocks::BEDROCK;
    }
    for (int i = 256; i < 256*28; i++) {
        blocks[i] = Blocks::STONE;
    }
    for (int i = 256*28; i < 256*31; i++) {
        blocks[i] = Blocks::DIRT;
    }
    for (int i = 256*31; i < 256*32; i++) {
        blocks[i] = Blocks::GRASS_BLOCK;
    }
}

