#ifndef INTERNAL_STRUCTURES_H
#define INTERNAL_STRUCTURES_H

#include "rage.h"

struct TileSetInternal
{
  bool loaded;
  int offset; // offset into allocated tilemem for this tileset, in blocks
  int size; // size of this tileset in blocks (each block 64 bytes)
};

struct SpriteDefinitionInternal
{
  bool loaded;
  int animationCount;
  Rage::Animation *animations;
};

#endif
