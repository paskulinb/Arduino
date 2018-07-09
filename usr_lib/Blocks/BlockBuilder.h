#ifndef BlockBuilder_h
#define BlockBuilder_h

#include <stddef.h>
#include <inttypes.h>
#include <alloca.h>
#include "BlockBase.h"


class BlockBuilder
{
    uint8_t blocks_count;
    //BlockRef* BlockIndex = (BlockRef*)alloca(sizeof(BlockRef)*blocks_count); //max. 100 blocks
public:
    BlockBuilder(void);
    ~BlockBuilder(void);
};
#endif
