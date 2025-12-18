#include "Block.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <string>
#include <unordered_map>

glm::vec3 normals[6] = {
    glm::vec3(0, 1, 0),
    glm::vec3(0, -1, 0),
    glm::vec3(0, 0, -1),
    glm::vec3(0, 0, 1),
    glm::vec3(1, 0, 0),
    glm::vec3(-1, 0, 0),
};

std::unordered_map<glm::ivec3, Block*, vecHash> blockByPos;

bool blockExists(const glm::ivec3& pos) {
    return blockByPos.find(pos) != blockByPos.end();
}

Block::Block(const glm::vec3& position, BlockType& blockType) {
    this->position = position;
    this->blockType = &blockType;
    blockByPos[glm::ivec3(position)] = this;
}

Block::~Block() {
    blockByPos.erase(glm::ivec3(position));
}

void Block::draw(const unsigned int* VAOs, const Shader& shader, const Camera& camera) {
    // bool hasModel = false;

    // for (unsigned int i = 0; i < 6; i++) {
    //     if (!blockExists(position + normals[i]) && glm::dot(normals[i], camera.position - position) > 0) {
    //         if (!hasModel) {
    //             glm::mat4 model(1.0f);
    //             model = glm::translate(model, position);
    //             shader.setMat4("model", glm::value_ptr(model));
    //             hasModel = true;
    //         }
    //         glBindTexture(GL_TEXTURE_2D, blockType->textures[i]);
    //         glBindVertexArray(VAOs[i]);
    //         glDrawArrays(GL_TRIANGLES, 0, 6);
    //     }
    // }
}

