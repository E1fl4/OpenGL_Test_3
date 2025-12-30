#pragma once

#include "BlockType.h"
#include <cstdint>

namespace Blocks {
    void init();
    const BlockType& byId(const uint16_t& id);
    uint16_t byName(const char* name);

    extern uint16_t VOID;
    extern uint16_t AIR;
    extern uint16_t BEDROCK;
    extern uint16_t DIRT;
    extern uint16_t GRASS_BLOCK;
    extern uint16_t STONE;
}

