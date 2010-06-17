#include <nds.h>
#include <stdio.h>
#include "rage.h"

// +INTERNAL+
enum {TILE_WIDTH, TILE_HEIGHT, MAP_WIDTH, MAP_HEIGHT};

int bgID[2][4];

Rage::ErrorCode errorCode;
Rage::Tile tileMap[2][4][32*24];
u16 tileMapDimensions[2][4][4];

#define VALID_ENGINE_CHECK(x) {if(!(x == MAIN || x == SUB)) \
      {errorCode = BAD_ENGINE; return 0;}}
#define VALID_LAYER_CHECK(x) {if(x < 0 || x > 3) \
      {errorCode = BAD_LAYER; return 0;}}
#define VALID_VERSION_CHECK(x) {if(x != RAGE_VERSION) \
      {errorCode = BAD_VERSION; return 0;}}

bool bgNeedsUpdate;
bool spriteMainNeedsUpdate;
bool spriteSubNeedsUpdate;

struct TileSetInternal
{
  bool loaded;
  int offset; // offset into allocated tilemem for this tileset, in blocks
  int size; // size of this tileset in blocks (each block 64 bytes
};

#define MAX_TILESETS 16

TileSetInternal tileSets[2][MAX_TILESETS];

#define BACKGROUND_BLOCKS 1920
#define SPRITE_BLOCKS 2048

char allocationBg[2][BACKGROUND_BLOCKS];
char allocationSprite[2][SPRITE_BLOCKS];

/* Return offset into memory where graphics of size 'size'*64 bytes
   will fit. */
int allocateVRAM(Rage::Engine e, Rage::Type t, int size)
{
  int offset = -1;
  int blocks_size = 0;

  // find 'size' consecutive blocks in the allocation structure
  if(t == Rage::SPRITE)
    {

    }
  else if(t == Rage::BG)
    {
      for(int i = 0;i < BACKGROUND_BLOCKS;i++)
	{
	  offset = i;
	  blocks_size = 0;
	  for(int j = offset;j < BACKGROUND_BLOCKS;j++)
	    {
	      if(allocationBg[e][j] == 1)
		{
		  blocks_size = 0;
		  break;
		}
	      blocks_size++;

	      if(blocks_size == size)
		{
		  for(int k = offset;k < offset + size;k++)
		    allocationBg[e][k] = 1;

		  return offset * 64;
		}
	    }
	}
    }
  
  return -1;
}
// -INTERNAL-

Rage::Rage()
{
  errorCode = NO_ERROR;
  bgNeedsUpdate = false;
  spriteMainNeedsUpdate = false;
  spriteSubNeedsUpdate = false;

  for(int i = 0;i < BACKGROUND_BLOCKS;i++)
    {
      allocationBg[MAIN][i] = allocationBg[SUB][i] = 0;
    }
  for(int i = 0;i < SPRITE_BLOCKS;i++)
    {
      allocationSprite[MAIN][i] = allocationSprite[SUB][i] = 0;
    }

  for(int i = 0;i < MAX_TILESETS;i++)
    {
      tileSets[MAIN][i].loaded = false;
      tileSets[SUB][i].loaded = false;
    }
}

int
Rage::init()
{
  videoSetMode(MODE_0_2D);
  vramSetBankA(VRAM_A_MAIN_BG);
  vramSetBankB(VRAM_B_MAIN_SPRITE_0x06400000);

  videoSetModeSub(MODE_0_2D);
  vramSetBankC(VRAM_C_SUB_BG_0x06200000);
  vramSetBankD(VRAM_D_SUB_SPRITE);

  return 1;
}

Rage::ErrorCode
Rage::getErrorCode()
{
  return errorCode;
}

int
Rage::redraw()
{
  if(bgNeedsUpdate)
    {
      bgUpdate();

      bgNeedsUpdate = false;
    }

  return 1;
}

int
Rage::selectOnTop(Engine e)
{
  VALID_ENGINE_CHECK(e);
  
  if(e == MAIN)
    lcdMainOnTop();
  else if(e == SUB)
    lcdMainOnBottom();

  return 1;
}

int
Rage::setupBackground(Engine e, int layer, int tileWidth, int tileHeight)
{
  VALID_ENGINE_CHECK(e);
  VALID_LAYER_CHECK(layer);
  
  if(tileWidth < 8 || (tileWidth % 8) != 0
     || tileHeight < 8 || (tileHeight % 8) != 0)
    {
      errorCode = BAD_TILE_DIMENSION;
      return 0;
    }

  int w, h;
  w = tileWidth / 8;
  h = tileHeight / 8;

  if(e == MAIN)
    {
      tileMapDimensions[MAIN][layer][TILE_WIDTH] = w;
      tileMapDimensions[MAIN][layer][TILE_HEIGHT] = h;
      tileMapDimensions[MAIN][layer][MAP_WIDTH] = 32 / w + !!(32 % w);
      tileMapDimensions[MAIN][layer][MAP_HEIGHT] = 32 / h + !!(32 % h);

      bgID[MAIN][layer] = bgInit(layer, BgType_Text8bpp, BgSize_T_256x256,
				 layer,
				 1);
    }
  else
    {
      tileMapDimensions[SUB][layer][TILE_WIDTH] = tileWidth / 8;
      tileMapDimensions[SUB][layer][TILE_HEIGHT] = tileHeight / 8;
      tileMapDimensions[SUB][layer][MAP_WIDTH] = 32 / w + !!(32 % w);
      tileMapDimensions[SUB][layer][MAP_HEIGHT] = 32 / h + !!(32 % h);

      bgID[SUB][layer] = bgInitSub(layer, BgType_Text8bpp, BgSize_T_256x256,
				   layer,
				   1);
    }

  return 1;
}

int
Rage::loadTileSet(Engine e, TileSetDefinition *def)
{
  VALID_ENGINE_CHECK(e);
  VALID_VERSION_CHECK(def->version);

  // allocate VRAM
  int blocks = def->image.gfxLen / (8*8);
  int offset = allocateVRAM(e, BG, blocks);

  if(offset == -1)
    {
      errorCode = OUT_OF_VRAM;
      return 0;
    }

  if(def->tileSetID < 0 || def->tileSetID >= MAX_TILESETS)
    {
      errorCode = BAD_TILESET_ID;
      return 0;
    }

  // copy tile graphics to VRAM and palette to palette area
  if(e == MAIN)
    {
      dmaCopy(def->image.gfx, (u16*)(0x06004000 + offset), def->image.gfxLen);
      dmaCopy(def->image.pal, BG_PALETTE, def->image.palLen);
    }
  else
    {
      dmaCopy(def->image.gfx, (u16*)(0x06204000 + offset), def->image.gfxLen);
      dmaCopy(def->image.pal, BG_PALETTE_SUB, def->image.palLen);
    }

  tileSets[e][def->tileSetID].loaded = true;
  tileSets[e][def->tileSetID].offset = offset / 64;
  tileSets[e][def->tileSetID].size = blocks;

  return 1;
}

int
Rage::setTile(Engine e, int layer, int x, int y, int tileset, int tile)
{
  VALID_ENGINE_CHECK(e);
  VALID_LAYER_CHECK(layer);

  if(tileset < 0 || tileset > 15 || tileSets[e][tileset].loaded == false)
    {
      errorCode = BAD_TILESET_ID;
      return 0;
    }

  int tileWidth = tileMapDimensions[e][layer][TILE_WIDTH];
  int tileHeight = tileMapDimensions[e][layer][TILE_HEIGHT];

  if(tile > tileSets[e][tileset].size / (tileWidth * tileHeight))
    {
      errorCode = BAD_TILE_INDEX;
      return 0;
    }

  Tile t;
  t.tileSet = tileset;
  t.tile = tile;

  tileMap[e][layer][x + y * tileMapDimensions[e][layer][MAP_WIDTH]] = t;

  u16 *mapPtr = bgGetMapPtr(bgID[e][layer]);

  for(int yi = 0;yi < tileHeight;yi++)
    {
      for(int xi = 0;xi < tileWidth;xi++)
	{
	  mapPtr[x*tileWidth+xi+(y*tileHeight+yi)*32] = tile*tileWidth*tileHeight + xi + yi*tileWidth;
	}
    }

  bgNeedsUpdate = true;

  return 1;
}
