/*
    Rage - Remar's Abstract Graphics Engine for Nintendo DS
    Copyright 2010 Andreas Remar

    This file is part of Rage.

    Rage is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    Rage is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Rage. If not, see <http://www.gnu.org/licenses/>.

    The licenses are located in doc/license in the Rage package, see
    the files COPYING (GPL) and COPYING.LESSER (LGPL).
*/

#include "rage.h"

#include <stdio.h>

// +INTERNAL+
#include "SpriteInstance.h"
#include "ImageCache.h"
#include "Allocator.h"
#include "PaletteHandler.h"
#include "internalstructures.h"

// 128k image buffer used when copying images from RAM to VRAM. While
// image is in this buffer, it is modified to point to the merged
// palette.
#define BUFFER_SIZE 128 * 1024
u8 imageBuffer[BUFFER_SIZE];

Allocator allocator;
PaletteHandler paletteHandler;
ImageCache imageCache(&allocator, &paletteHandler, imageBuffer);

Rage::ErrorCode errorCode;

TileMap tileMap[2][4]; // 2 screens, 4 layers per screen

Rage::BGMapMemSize mapMemSize[2];

u16 tileMapInternal[64*64];

#define SPRITE_PLUS 2048

#define VALID_SCREEN_CHECK(x) {if(!(x == MAIN || x == SUB)) \
      {errorCode = BAD_SCREEN; return 0;}}
#define VALID_LAYER_CHECK(x) {if(x > 3) \
      {errorCode = BAD_LAYER; return 0;}}
#define VALID_VERSION_CHECK(x) {if(x != RAGE_VERSION) \
      {errorCode = BAD_VERSION; return 0;}}
#define VALID_TILESET_CHECK(x) {if(x < 0 || x >= MAX_TILESETS) \
      {errorCode = Rage::BAD_TILESET_ID; return 0;}}
#define VALID_SPRITEID_CHECK(x) {if(x >= MAX_SPRITES) \
      {errorCode = BAD_SPRITE_ID; return 0;}}
#define VALID_SPRITEINSTANCE_CHECK(x) {if(x < SPRITE_PLUS \
                                          || x >= MAX_SPRITES + SPRITE_PLUS) \
      {errorCode = BAD_SPRITE_INDEX; return 0;}}
#define SPRITE_INDEX(x) (x & 0x7f)
#define SPRITE_LOADED_CHECK(x) {if(spriteInstances[s][SPRITE_INDEX(x)].loaded \
				   == false) \
      {errorCode = BAD_SPRITE_INDEX; return 0;}}
#define TILEMAP_LOADED_CHECK(s,layer) {if(tileMap[s][layer].loaded == false) \
      {errorCode = TILEMAP_NOT_INITIALIZED; return 0;}}

bool bgNeedsUpdate;

#define MAX_TILESETS 16 // quite ad hoc number for now...

TileSetInternal tileSets[2][MAX_TILESETS];

#define MAX_SPRITES 128

SpriteDefinitionInternal spriteDefinitions[2][MAX_SPRITES];

struct SpriteInstanceContainer
{
  bool loaded;
  SpriteInstance *instance;
};

SpriteInstanceContainer spriteInstances[2][MAX_SPRITES];

int freeSpriteInstance[2];
bool outOfSpriteIndices[2];
int spriteInstancesLeft[2];

int mainBGVram;
int subBGVram;

void locateNextSpriteIndex(Rage::Screen s)
{
  int initialIndex = freeSpriteInstance[s];

  // Go the complete round to locate a free index
  for(int i = 0;i < MAX_SPRITES;i++)
    {
      freeSpriteInstance[s]++;
      if(freeSpriteInstance[s] == MAX_SPRITES)
	freeSpriteInstance[s] = 0;

      if(spriteInstances[s][freeSpriteInstance[s]].loaded == false)
	break; // Found a free sprite index
    }

  if(initialIndex == freeSpriteInstance[s])
    {
      // Found no free sprite index,
      outOfSpriteIndices[s] = true;
    }
}

int mapSizeToWidth(Rage::BGMapSize bgMapSize)
{
  int width[] = {32, 64, 32, 64};

  return width[bgMapSize];
}

int mapSizeToHeight(Rage::BGMapSize bgMapSize)
{
  int height[] = {32, 32, 64, 64};

  return height[bgMapSize];
}

enum Location {HARDWARE, INTERNAL_MAP}; // where to set tile

int setTileInternal(Rage::Screen s, u16 layer, u16 x, u16 y,
		    u16 tileSet, u16 tile, Location loc)
{
  VALID_TILESET_CHECK(tileSet);

  if(tileSets[s][tileSet].loaded == false)
    {
      errorCode = Rage::BAD_TILESET_ID;
      return 0;
    }

  if(x >= tileMap[s][layer].mapWidth
     || y >= tileMap[s][layer].mapHeight)
    {
      errorCode = Rage::BAD_TILE_COORDINATES;
      return 0;
    }

  int tileWidth = tileMap[s][layer].tileWidth;
  int tileHeight = tileMap[s][layer].tileHeight;

  if(tile > tileSets[s][tileSet].size / (tileWidth * tileHeight))
    {
      errorCode = Rage::BAD_TILE_INDEX;
      return 0;
    }

  u16 *mapPtr;

  if(loc == HARDWARE)
    mapPtr = bgGetMapPtr(tileMap[s][layer].bgID);
  else if(loc == INTERNAL_MAP)
    mapPtr = tileMapInternal;
  else
    {
      errorCode = Rage::BAD_PARAMETER;
      return 0;
    }

  int mapx, mapy;

  int mapWidth = mapSizeToWidth(tileMap[s][layer].mapSize);
  int mapHeight = mapSizeToHeight(tileMap[s][layer].mapSize);

  int mapOffset;

  int mapOffsetY = tileMap[s][layer].mapOffsetY;

  // TODO: Could calculate mapOffset in advance if tile doesn't cross
  // chunk borders

  for(int yi = 0;yi < tileHeight;yi++)
    {
      for(int xi = 0;xi < tileWidth;xi++)
	{
	  mapOffset = 0;

	  mapx = x*tileWidth+xi;
	  if(mapx > mapWidth - 1) // edge reached, next line
	    break;

	  mapy = y*tileHeight+yi;
	  if(mapy > mapHeight - 1) // edge reached, tile done
	    break;

	  if(mapx > 31)
	    {
	      mapOffset = 32 * 32;

	      mapx -= 32;
	    }

	  if(mapy > 31)
	    {
	      mapOffset += mapOffsetY;

	      mapy -= 32;
	    }

	  mapPtr[mapx+mapy*32 + mapOffset]
	    = tile*tileWidth*tileHeight + xi + yi*tileWidth 
	    + tileSets[s][tileSet].offset;
	}
    }

  return 1;
}
// -INTERNAL-

Rage::Rage()
{
  errorCode = NO_ERROR;
  bgNeedsUpdate = false;

  for(int i = 0;i < MAX_TILESETS;i++)
    {
      tileSets[MAIN][i].loaded = false;
      tileSets[SUB][i].loaded = false;
    }

  for(int i = 0;i < MAX_SPRITES;i++)
    {
      spriteInstances[MAIN][i].loaded = false;
      spriteInstances[SUB][i].loaded = false;
    }

  freeSpriteInstance[MAIN] = 0;
  freeSpriteInstance[SUB]  = 0;
  outOfSpriteIndices[MAIN] = false;
  outOfSpriteIndices[SUB]  = false;
  spriteInstancesLeft[MAIN] = 128;
  spriteInstancesLeft[SUB]  = 128;

  for(int i = 0;i < 4;i++)
    {
      tileMap[MAIN][i].loaded = false;
      tileMap[SUB][i].loaded = false;
    }
}

int
Rage::init(BGMapMemSize mainBGSize, BGMapMemSize subBGSize)
{
  videoSetMode(MODE_0_2D);
  vramSetBankA(VRAM_A_MAIN_BG);
  vramSetBankB(VRAM_B_MAIN_SPRITE_0x06400000);

  videoSetModeSub(MODE_0_2D);
  vramSetBankC(VRAM_C_SUB_BG_0x06200000);
  vramSetBankD(VRAM_D_SUB_SPRITE);

  oamInit(&oamMain, SpriteMapping_1D_32, false);
  oamInit(&oamSub, SpriteMapping_1D_32, false);

  mapMemSize[MAIN] = mainBGSize;
  mapMemSize[SUB] = subBGSize;

  // Tell allocator how much memory is available for background
  // maps
  allocator.init(mainBGSize, subBGSize);

  mainBGVram = (mainBGSize == BG_MAPMEM_SIZE_16K ? 0x06004000 : 0x06008000);
  subBGVram = (subBGSize == BG_MAPMEM_SIZE_16K ? 0x06204000 : 0x06208000);

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

  // animate sprites
  for(int i = 0;i < MAX_SPRITES;i++)
    {
      if(spriteInstances[Rage::MAIN][i].loaded)
	{
	  spriteInstances[Rage::MAIN][i].instance->animate();
	}
      if(spriteInstances[Rage::SUB][i].loaded)
	{
	  spriteInstances[Rage::SUB][i].instance->animate();
	}
    }

  swiWaitForVBlank();

  // oamUpdate stuff here
  oamUpdate(&oamMain);
  oamUpdate(&oamSub);

  return 1;
}

int
Rage::selectOnTop(Screen s)
{
  VALID_SCREEN_CHECK(s);
  
  if(s == MAIN)
    lcdMainOnTop();
  else
    lcdMainOnBottom();

  return 1;
}

int
Rage::setupBackground(Screen s, u16 layer, BGMapSize bgMapSize,
		      u16 tileWidth, u16 tileHeight)
{
  VALID_SCREEN_CHECK(s);
  VALID_LAYER_CHECK(layer);

  if(!(bgMapSize == BG_MAP_256x256
       || bgMapSize == BG_MAP_512x256
       || bgMapSize == BG_MAP_256x512
       || bgMapSize == BG_MAP_512x512))
    {
      errorCode = BAD_MAP_SIZE;
      return 0;
    }
  
  if(tileWidth < 8 || (tileWidth % 8) != 0
     || tileHeight < 8 || (tileHeight % 8) != 0)
    {
      errorCode = BAD_TILE_DIMENSION;
      return 0;
    }

  if(tileMap[s][layer].loaded)
    {
      // Free up map memory
      releaseBackground(s, layer);
    }

  tileMap[s][layer].mapSize = bgMapSize;

  tileWidth /= 8;
  tileHeight /= 8;

  int mapWidth = mapSizeToWidth(bgMapSize);
  int mapHeight = mapSizeToHeight(bgMapSize);

  tileMap[s][layer].tileWidth = tileWidth;
  tileMap[s][layer].tileHeight = tileHeight;
  tileMap[s][layer].mapWidth = (mapWidth / tileWidth) + !!(mapWidth % tileWidth);
  tileMap[s][layer].mapHeight = (mapHeight / tileHeight) + !!(mapHeight % tileHeight);

  tileMap[s][layer].mapOffsetY = 32 * 32;
  if(bgMapSize == BG_MAP_512x512)
    tileMap[s][layer].mapOffsetY += 32 * 32;

  int offset = allocator.allocateMap(s, bgMapSize);

  tileMap[s][layer].offset = offset;

  if(offset == -1) // Out of map memory?
    {
      errorCode = OUT_OF_MAP_MEMORY;
      return 0;
    }

  BgSize bgSizeMap[4] = {BgSize_T_256x256, BgSize_T_512x256,
			 BgSize_T_256x512, BgSize_T_512x512};

  int tileBase = mapMemSize[s] == BG_MAPMEM_SIZE_16K ? 1 : 2;

  if(s == MAIN)
    {
      tileMap[MAIN][layer].bgID = bgInit(layer, BgType_Text8bpp, bgSizeMap[bgMapSize],
					 offset,
					 tileBase);
    }
  else
    {
      tileMap[SUB][layer].bgID = bgInitSub(layer, BgType_Text8bpp, bgSizeMap[bgMapSize],
					   offset,
					   tileBase);
    }

  tileMap[s][layer].loaded = true;

  return 1;
}

int
Rage::releaseBackground(Screen s, u16 layer)
{
  VALID_SCREEN_CHECK(s);
  VALID_LAYER_CHECK(layer);

  if(tileMap[s][layer].loaded == false)
    {
      errorCode = BAD_LAYER;
      return 0;
    }

  allocator.releaseMap(s, tileMap[s][layer].mapSize, tileMap[s][layer].offset);

  tileMap[s][layer].loaded = false;

  return 1;
}

int
Rage::loadTileSet(Screen s, TileSetDefinition *def)
{
  VALID_SCREEN_CHECK(s);
  VALID_VERSION_CHECK(def->version);
  VALID_TILESET_CHECK(def->tileSetID);

  if(tileSets[s][def->tileSetID].loaded == true)
    {
      errorCode = DUPLICATE_TILESET_ID;
      return 0;
    }

  // allocate VRAM
  int blocks = def->image.gfxLen >> 6; // divide by 64;
  int offset = allocator.allocateVRAM(s, BG, blocks);

  if(offset == -1)
    {
      errorCode = OUT_OF_VRAM;
      return 0;
    }

  u16 *transTable;
  int transTableLen;

  int retval = 1;
  if(!paletteHandler.mergePalette(s, BG, &(def->image), &transTable, &transTableLen))
    {
      // Soft error, still copy image data to VRAM
      errorCode = FAILED_PALETTE_MERGE;
      retval = 0;
    }

  int bgVram = s == MAIN ? mainBGVram : subBGVram;

  if(transTableLen == 0)
    {
      // Just copy gfx to VRAM, no translation necessary
      dmaCopy(def->image.gfx, (u16*)(bgVram + offset), def->image.gfxLen);
    }
  else
    {
      // Loop through image data, copying & modifying entries as we go
      // along.
      u8 *imageData = (u8*)def->image.gfx;

      int i;
      for(i = 0;i < BUFFER_SIZE && i < def->image.gfxLen;i++)
	{
	  imageBuffer[i] = transTable[imageData[i]];
	}

      DC_FlushRange(imageBuffer, i);
      dmaCopy(imageBuffer, (u16*)(bgVram + offset), i);
    }

  tileSets[s][def->tileSetID].loaded = true;
  tileSets[s][def->tileSetID].offset = offset >> 6; // divide by 64;
  tileSets[s][def->tileSetID].size = blocks;
  tileSets[s][def->tileSetID].imageDef = def->image;

  return retval;
}

int
Rage::unloadTileSet(Screen s, u16 tileSet)
{
  VALID_SCREEN_CHECK(s);
  VALID_TILESET_CHECK(tileSet);

  if(tileSets[s][tileSet].loaded == false)
    {
      errorCode = BAD_TILESET_ID;
      return 0;
    }

  int retval = 1;

  if(!paletteHandler.unmergePalette(s, BG, &(tileSets[s][tileSet].imageDef)))
    {
      errorCode = FAILED_PALETTE_UNMERGE;
      retval = 0;
    }

  tileSets[s][tileSet].loaded = false;

  // deallocate VRAM
  allocator.addFreeBlock(s, Rage::BG, 
			 tileSets[s][tileSet].offset,
			 tileSets[s][tileSet].size);

  return retval;
}

int
Rage::unloadAllTileSets(Screen s)
{
  VALID_SCREEN_CHECK(s);

  for(int i = 0;i < MAX_TILESETS;i++)
    if(tileSets[s][i].loaded)
      unloadTileSet(s, i);

  return 1;
}

int
Rage::setBackgroundScroll(Screen s, u16 layer, int x, int y)
{
  VALID_SCREEN_CHECK(s);
  VALID_LAYER_CHECK(s);
  TILEMAP_LOADED_CHECK(s, layer);

  bgSetScroll(tileMap[s][layer].bgID, x, y);

  bgNeedsUpdate = true;

  return 1;
}

int
Rage::setTile(Screen s, u16 layer, u16 x, u16 y, u16 tileSet, u16 tile)
{
  VALID_SCREEN_CHECK(s);
  VALID_LAYER_CHECK(layer);

  return setTileInternal(s, layer, x, y, tileSet, tile, HARDWARE);
}

int
Rage::setMap(Screen s, u16 layer, Tile *map)
{
  // write into the internal tilemap, then dmaCopy it to hardware

  VALID_SCREEN_CHECK(s);
  VALID_LAYER_CHECK(layer);

  int width = tileMap[s][layer].mapWidth;
  int height = tileMap[s][layer].mapHeight;
  Tile t;

  for(int y = 0;y < height;y++)
    for(int x = 0;x < width;x++)
      {
	t = map[x + y * width];
	if(!setTileInternal(s, layer, x, y, t.tileSet, t.tile, INTERNAL_MAP))
	  return 0;
      }

  DC_FlushAll(); // make sure that the modifications are flushed to main ram
  dmaCopy(tileMapInternal, bgGetMapPtr(tileMap[s][layer].bgID),
	  sizeof(tileMapInternal));

  return 1;
}

int
Rage::setMap(Screen s, u16 layer, u16 tileSet, u16 *map)
{
  // write into the internal tilemap, then dmaCopy it to hardware

  VALID_SCREEN_CHECK(s);
  VALID_LAYER_CHECK(layer);

  int width = tileMap[s][layer].mapWidth;
  int height = tileMap[s][layer].mapHeight;

  for(int y = 0;y < height;y++)
    for(int x = 0;x < width;x++)
      {
	if(!setTileInternal(s, layer, x, y, tileSet, map[x + y * width],
			    INTERNAL_MAP))
	  return 0;
      }

  DC_FlushAll(); // make sure that the modifications are flushed to main ram
  int size = 2 * mapSizeToWidth(tileMap[s][layer].mapSize) * mapSizeToHeight(tileMap[s][layer].mapSize);
  dmaCopy(tileMapInternal, bgGetMapPtr(tileMap[s][layer].bgID),
	  size);

  return 1;
}

int
Rage::loadSprite(Screen s, SpriteDefinition *def)
{
  VALID_SCREEN_CHECK(s);
  VALID_VERSION_CHECK(def->version);
  VALID_SPRITEID_CHECK(def->spriteID);

  if(spriteDefinitions[s][def->spriteID].loaded == true)
    {
      errorCode = DUPLICATE_SPRITE_ID;
      return 0;
    }

  // copy sprite structure
  int animCount = def->animationCount;
  spriteDefinitions[s][def->spriteID].animations = new Animation[animCount];
  spriteDefinitions[s][def->spriteID].animationCount = animCount;

  for(int i = 0;i < animCount;i++)
    {
      // Nullify to be able to delete frames if load fails
      spriteDefinitions[s][def->spriteID].animations[i].frames = 0;
    }

  bool invalidSpriteDimension = false;

  for(int i = 0;i < animCount;i++)
    {
      int frmCount = def->animations[i].frameCount;
      Frame *frames = new Frame[frmCount];
      for(int j = 0;j < frmCount;j++)
	{
	  frames[j] = def->animations[i].frames[j];
	}

      spriteDefinitions[s][def->spriteID].animations[i].image
	= def->animations[i].image;
      spriteDefinitions[s][def->spriteID].animations[i].size
	= def->animations[i].size;
      spriteDefinitions[s][def->spriteID].animations[i].looping
	= def->animations[i].looping;
      spriteDefinitions[s][def->spriteID].animations[i].frameCount = frmCount;
      spriteDefinitions[s][def->spriteID].animations[i].frames = frames;
    }

  if(invalidSpriteDimension)
    {
      for(int i = 0;i < animCount;i++)
	delete [] spriteDefinitions[s][def->spriteID].animations[i].frames;
      delete [] spriteDefinitions[s][def->spriteID].animations;

      errorCode = BAD_SPRITE_DIMENSION;

      return 0;
    }

  spriteDefinitions[s][def->spriteID].loaded = true;

#if 0 // DEBUG
  consoleDemoInit();
  for(int i = 0;i < spriteDefinitions[s][def->spriteID].animationCount;i++)
    {
      Animation *a = &spriteDefinitions[s][def->spriteID].animations[i];
      printf("[animation %d (%dx%d) %slooping]\n", i, a->width,
	     a->height, a->looping?"":"!");
      printf("image 0x%X\n", (int)a->image.gfx);
      for(int j = 0;j < a->frameCount;j++)
	{
	  printf("frame %d: index %d, dur %d\n", j,
		 a->frames[j].index,
		 a->frames[j].duration);
	}
    }
  while(1);
#endif

  return 1;
}

int
Rage::unloadSprite(Screen s, int sprite)
{
  VALID_SCREEN_CHECK(s);
  VALID_SPRITEID_CHECK(sprite);

  SpriteDefinitionInternal *spr = &spriteDefinitions[s][sprite];

  if(spr->loaded == false)
    {
      errorCode = BAD_SPRITE_ID;
      return 0;
    }

  // go through animations deleting frames, then delete animations
  for(int i = 0;i < spr->animationCount;i++)
    {
      delete [] spr->animations[i].frames;
    }

  delete [] spr->animations;

  spr->loaded = false;

  return 1;
}

int
Rage::unloadAllSprites(Screen s)
{
  VALID_SCREEN_CHECK(s);

  for(int sprite = 0;sprite < 128;sprite++)
    if(spriteDefinitions[s][sprite].loaded)
      unloadSprite(s, sprite);

  return 1;
}

int
Rage::createSpriteInstance(Screen s, int sprite)
{
  VALID_SCREEN_CHECK(s);
  VALID_SPRITEID_CHECK(sprite);

  if(spriteDefinitions[s][sprite].loaded == false)
    {
      errorCode = SPRITE_NOT_LOADED;
      return 0;
    }

  if(outOfSpriteIndices[s])
    {
      errorCode = OUT_OF_SPRITE_INDEXES;
      return 0;
    }

  // Locate a free sprite slot
  int spriteIndex = freeSpriteInstance[s];
  locateNextSpriteIndex(s);

  spriteInstances[s][spriteIndex].instance
    = new SpriteInstance(&spriteDefinitions[s][sprite], &imageCache, s,
			 spriteIndex);
  spriteInstances[s][spriteIndex].loaded = true;

  spriteInstancesLeft[s]--;

  return spriteIndex + SPRITE_PLUS; // return sprite handle
}

int
Rage::removeSpriteInstance(Screen s, int sprite)
{
  VALID_SCREEN_CHECK(s);
  VALID_SPRITEINSTANCE_CHECK(sprite);

  int index = SPRITE_INDEX(sprite);

  if(spriteInstances[s][index].loaded == false)
    {
      errorCode = BAD_SPRITE_INDEX;
      return 0;
    }

  spriteInstances[s][index].instance->setVisible(false);

  delete spriteInstances[s][index].instance;
  spriteInstances[s][index].loaded = false;

  if(outOfSpriteIndices[s])
    {
      freeSpriteInstance[s] = index;
      outOfSpriteIndices[s] = false;
    }

  spriteInstancesLeft[s]++;

  return 1;
}

int
Rage::selectAnimation(Screen s, int sprite, int animation)
{
  VALID_SCREEN_CHECK(s);
  VALID_SPRITEINSTANCE_CHECK(sprite);
  SPRITE_LOADED_CHECK(sprite);

  int index = SPRITE_INDEX(sprite);

  int code;
  if((code = spriteInstances[s][index].instance->setAnimation(animation)) != 1)
    {
      switch(code)
	{
	case 0:
	  errorCode = OUT_OF_VRAM;
	  break;

	case -1:
	  errorCode = BAD_ANIMATION_ID;
	  break;
	}
      return 0;
    }

  return 1;
}

int
Rage::showSprite(Screen s, int sprite, bool show)
{
  VALID_SCREEN_CHECK(s);
  VALID_SPRITEINSTANCE_CHECK(sprite);
  SPRITE_LOADED_CHECK(sprite);

  int index = SPRITE_INDEX(sprite);

  spriteInstances[s][index].instance->setVisible(show);

  return 1;
}

int
Rage::moveSpriteAbs(Screen s, int sprite, int x, int y)
{
  VALID_SCREEN_CHECK(s);
  VALID_SPRITEINSTANCE_CHECK(sprite);
  SPRITE_LOADED_CHECK(sprite);

  int index = SPRITE_INDEX(sprite);

  spriteInstances[s][index].instance->moveAbs(x, y);

  return 1;
}

int
Rage::moveSpriteRel(Screen s, int sprite, int x, int y)
{
  VALID_SCREEN_CHECK(s);
  VALID_SPRITEINSTANCE_CHECK(sprite);
  SPRITE_LOADED_CHECK(sprite);

  int index = SPRITE_INDEX(sprite);

  spriteInstances[s][index].instance->moveRel(x, y);

  return 1;
}

int
Rage::getFreeMem(Screen s, Type t)
{
  return allocator.sumFreeBlocks(s, t) << 6;
}

int
Rage::getLargestFreeBlock(Screen s, Type t)
{
  return allocator.largestFreeBlock(s, t) << 6;
}

int
Rage::getAvailableSprites(Screen s)
{
  return spriteInstancesLeft[s];
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
      "Sprite ID already in use",
      "Out of sprite indexes",
      "Bad screen provided",
      "Bad layer provided",
      "Bad tile dimension",
      "Bad version",
      "Bad tileset ID",
      "Bad tile index",
      "Bad tile coordinates",
      "Bad sprite ID",
      "Bad sprite dimension",
      "Bad sprite instance",
      "No such sprite loaded",
      "Bad animation ID",
      "Bad map size",
      "Out of map memory",
      "Failed palette merge",
      "Failed palette unmerge",
      "Tilemap not initialized"
    };

  if(errorCode < NO_ERROR || errorCode >= LAST_ERROR_CODE)
    return "Major bug in RAGE found (1)";

  return errorStrings[errorCode];
}

void
Rage::listFreeBlocks()
{
  allocator.listFreeBlocks(MAIN, SPRITE);
  allocator.listFreeBlocks(MAIN, BG);
  allocator.listFreeBlocks(SUB, SPRITE);
  allocator.listFreeBlocks(SUB, BG);
}
