#include "Block.hpp"

IMPLEMENT_SUBCLASS(Block, "Block")
EMPTY_DELETEDATA(Block)
EMPTY_SLOTTABLE(Block)
EMPTY_COPYDATA(Block)

Block::Block() {
    STANDARD_CONSTRUCTOR()
}

