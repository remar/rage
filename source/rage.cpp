#include "rage.h"

#include <stdio.h>
#include <list>

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
#define VALID_TILESET_CHECK(x) {if(x < 0 || x >= MAX_TILESETS) \
      {errorCode = BAD_TILESET_ID; return 0;}}

bool bgNeedsUpdate;
bool spriteMainNeedsUpdate;
bool spriteSubNeedsUpdate;

struct TileSetInternal
{
  bool loaded;
  int offset; // offset into allocated tilemem for this tileset, in blocks
  int size; // size of this tileset in blocks (each block 64 bytes)
};

#define MAX_TILESETS 16 // quite ad hoc number for now...

TileSetInternal tileSets[2][MAX_TILESETS];

#define BACKGROUND_BLOCKS 1792
#define SPRITE_BLOCKS 2048

struct MemoryBlock
{
  int offset;
  int length;
};

std::list<MemoryBlock> freeBlocks[2][2];

void listFreeBlocksInternal(Rage::Engine e, Rage::Type t)
{
  const char *engine_string[] = {"MAIN", "SUB "};
  const char *type_string[] = {"SPR", "BG "};

  printf("%s %s --------------------\n",
	 engine_string[(int)e], type_string[(int)t]);

  int totalFree = 0;

  std::list<MemoryBlock>::iterator it = freeBlocks[e][t].begin();
  for(;it != freeBlocks[e][t].end();it++)
    {
      int offset = (*it).offset;
      int length = (*it).length;
      printf("offset %d, length %d\n", offset, length);
      totalFree += length;
    }
  
  printf("total free %d, %d kB\n", totalFree, totalFree * 64 / 1024);
}

void addFreeBlock(Rage::Engine e, Rage::Type t, int offset, int length)
{
  // locate blocks before and after the new block and merge blocks
  MemoryBlock oldBlockBefore;
  MemoryBlock oldBlockAfter;

  oldBlockBefore.offset = -1;
  oldBlockBefore.length = 0;
  oldBlockAfter.offset = -1;
  oldBlockAfter.length = 0;

  std::list<MemoryBlock>::iterator it = freeBlocks[e][t].begin();

  bool incIt; // only increase iterator if we've not removed an element

  for(;it != freeBlocks[e][t].end();)
    {
      incIt = true;

      if((*it).offset + (*it).length == offset)
	{
	  oldBlockBefore = (*it);

	  it = freeBlocks[e][t].erase(it);
	  incIt = false;
	}
      if((*it).offset == offset + length)
	{
	  oldBlockAfter = (*it);

	  it = freeBlocks[e][t].erase(it);
	  incIt = false;
	}

      if(oldBlockBefore.offset != -1 && oldBlockAfter.offset != -1)
	break; // found blocks before and after

      if(incIt)
	it++;
    }

  MemoryBlock freeBlock;
  freeBlock.offset = offset;
  freeBlock.length = length;

  if(oldBlockBefore.offset != -1)
    {
      freeBlock.offset = oldBlockBefore.offset;
      freeBlock.length += oldBlockBefore.length;
    }
  if(oldBlockAfter.offset != -1)
    {
      freeBlock.length += oldBlockAfter.length;
    }

  freeBlocks[e][t].push_back(freeBlock);
}

int allocateVRAM(Rage::Engine e, Rage::Type t, int size)
{
  std::list<MemoryBlock>::iterator it = freeBlocks[e][t].begin();

  for(;it != freeBlocks[e][t].end();it++)
    {
      if((*it).length >= size)
	{
	  // Found free block
	  int offset = (*it).offset;
	  int length = (*it).length;

	  // Remove block from list
	  freeBlocks[e][t].erase(it);
	  
	  // Add new block of size 'length - size' and offset 'offset + size'
	  if(length - size > 0)
	    addFreeBlock(e, t, offset + size, length - size);

	  // Return offset into VRAM
	  return offset << 6; // multiply by 64
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

  for(int i = 0;i < MAX_TILESETS;i++)
    {
      tileSets[MAIN][i].loaded = false;
      tileSets[SUB][i].loaded = false;
    }
}

void initAllocator()
{
  // Initialize list of free blocks in VRAM
  addFreeBlock(Rage::MAIN, Rage::SPRITE, 0, SPRITE_BLOCKS);
  addFreeBlock(Rage::SUB, Rage::SPRITE, 0, SPRITE_BLOCKS);
  addFreeBlock(Rage::MAIN, Rage::BG, 0, BACKGROUND_BLOCKS);
  addFreeBlock(Rage::SUB, Rage::BG, 0, BACKGROUND_BLOCKS);
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

  initAllocator();

  return 1;
}

Rage::ErrorCode
Rage::getErrorCode()
{
  return errorCode;
}

const char *
Rage::getErrorString()
{
  const char *errorStrings[LAST_ERROR_CODE] =
    {
      "No error",
      "Out of VRAM",
      "Failed to load tileset",
      "Failed to load sprite",
      "Bad parameter",
      "Tileset ID already in use",
      "Animation ID already in use",
      "Out of sprite indexes",
      "Bad engine provided",
      "Bad layer provided",
      "Bad tile dimension",
      "Bad version",
      "Bad tileset ID",
      "Bad tile index",
      "Bad tile coordinates",
    };

  if(errorCode < NO_ERROR || errorCode >= LAST_ERROR_CODE)
    return "Major bug in RAGE found (1)";

  return errorStrings[errorCode];
}

int
Rage::redraw()
{
  if(bgNeedsUpdate)
    {
      bgUpdate();

      bgNeedsUpdate = false;
    }

  swiWaitForVBlank();

  // oamUpdate stuff here

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

  tileMapDimensions[e][layer][TILE_WIDTH] = w;
  tileMapDimensions[e][layer][TILE_HEIGHT] = h;
  tileMapDimensions[e][layer][MAP_WIDTH] = 32 / w + !!(32 % w);
  tileMapDimensions[e][layer][MAP_HEIGHT] = 32 / h + !!(32 % h);

  if(e == MAIN)
    {
      bgID[MAIN][layer] = bgInit(layer, BgType_Text8bpp, BgSize_T_256x256,
				 layer,
				 1);
    }
  else
    {
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

  if(tileSets[e][def->tileSetID].loaded == true)
    {
      errorCode = DUPLICATE_TILESET_ID;
      return 0;
    }

  // allocate VRAM
  int blocks = def->image.gfxLen >> 6; // divide by 64;
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
  tileSets[e][def->tileSetID].offset = offset >> 6; // divide by 64;
  tileSets[e][def->tileSetID].size = blocks;

  return 1;
}

int
Rage::unloadTileSet(Engine e, u16 tileSet)
{
  VALID_ENGINE_CHECK(e);
  VALID_TILESET_CHECK(tileSet);

  if(tileSets[e][tileSet].loaded == false)
    {
      errorCode = BAD_TILESET_ID;
      return 0;
    }

  tileSets[e][tileSet].loaded = false;

  // deallocate VRAM
  addFreeBlock(e, Rage::BG, 
	       tileSets[e][tileSet].offset, tileSets[e][tileSet].size);

  return 1;
}

int
Rage::unloadAllTileSets(Engine e)
{
  VALID_ENGINE_CHECK(e);

  for(int i = 0;i < MAX_TILESETS;i++)
    if(tileSets[e][i].loaded)
      unloadTileSet(e, i);

  return 1;
}

int
Rage::setTile(Engine e, int layer, u16 x, u16 y, u16 tileSet, u16 tile)
{
  VALID_ENGINE_CHECK(e);
  VALID_LAYER_CHECK(layer);
  VALID_TILESET_CHECK(tileSet);

  if(tileSets[e][tileSet].loaded == false)
    {
      errorCode = BAD_TILESET_ID;
      return 0;
    }

  if(x >= tileMapDimensions[e][layer][MAP_WIDTH]
     || y >= tileMapDimensions[e][layer][MAP_HEIGHT])
    {
      errorCode = BAD_TILE_COORDINATES;
      return 0;
    }

  int tileWidth = tileMapDimensions[e][layer][TILE_WIDTH];
  int tileHeight = tileMapDimensions[e][layer][TILE_HEIGHT];

  if(tile > tileSets[e][tileSet].size / (tileWidth * tileHeight))
    {
      errorCode = BAD_TILE_INDEX;
      return 0;
    }

  u16 *mapPtr = bgGetMapPtr(bgID[e][layer]);

  int mapx, mapy;

  for(int yi = 0;yi < tileHeight;yi++)
    {
      for(int xi = 0;xi < tileWidth;xi++)
	{
	  mapx = x*tileWidth+xi;
	  if(mapx > 31) // edge reached, next line
	    break;

	  mapy = y*tileHeight+yi;
	  if(mapy > 23) // edge reached, tile done
	    break;

	  mapPtr[mapx+mapy*32]
	    = tile*tileWidth*tileHeight + xi + yi*tileWidth 
	    + tileSets[e][tileSet].offset;
	}
    }

  bgNeedsUpdate = true;

  return 1;
}

void
Rage::listFreeBlocks()
{
  consoleDemoInit();
  listFreeBlocksInternal(MAIN, SPRITE);
  listFreeBlocksInternal(MAIN, BG);
  listFreeBlocksInternal(SUB, SPRITE);
  listFreeBlocksInternal(SUB, BG);
  while(1);
}
