#ifndef INTERNAL_STRUCTURES_H
#define INTERNAL_STRUCTURES_H

#include "rage.h"

struct TileSetInternal
{
  bool loaded;
  int offset; // offset into allocated tilemem for this tileset, in blocks
  int size; // size of this tileset in blocks (each block 64 bytes)
  Rage::ImageDefinition imageDef;
};

struct SpriteDefinitionInternal
{
  bool loaded;
  int animationCount;
  Rage::Animation *animations;
};

struct TileMap
{
  bool loaded;

  Rage::BGMapSize mapSize;

  int tileWidth;
  int tileHeight;

  int mapWidth;
  int mapHeight;

  // keep track of background ID returned by libnds API
  int bgID;

  // 2k offset into VRAM where the map is located
  int offset;

  int mapOffsetY;
};

#endif
