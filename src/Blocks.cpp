#include "Blocks.h"
#include "BlockType.h"
#include <cstdint>
#include <vector>

namespace Blocks {

static std::vector<BlockType> blockTypes;

uint16_t VOID;
uint16_t AIR;
uint16_t BEDROCK;
uint16_t DIRT;
uint16_t GRASS_BLOCK;
uint16_t STONE;

void init() {
    blockTypes.reserve(256);

    VOID = blockTypes.size();
    blockTypes.push_back({VOID});

    AIR = blockTypes.size();
    blockTypes.push_back({AIR});

    BEDROCK = blockTypes.size();
    blockTypes.push_back({
        BEDROCK,
        {0,0,0,0,0,0}
    });

    DIRT = blockTypes.size();
    blockTypes.push_back({
        DIRT,
        {3,3,3,3,3,3}
    });

    GRASS_BLOCK = blockTypes.size();
    blockTypes.push_back({
        GRASS_BLOCK,
        {4,3,5,5,5,5}
    });

    STONE = blockTypes.size();
    blockTypes.push_back({
        STONE,
        {11,11,11,11,11,11}
    });
}

const BlockType& byId(const uint16_t& id) {
    return blockTypes[id];
}

}

