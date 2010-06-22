#ifndef RAGE_INCLUDE_H
#define RAGE_INCLUDE_H

#include <nds.h>

class Rage
{
 public:
  static const int RAGE_VERSION = 1;

  struct ImageDefinition
    {
      const u16 *gfx;
      u16 gfxLen;
      const u16 *pal;
      u16 palLen;
    };

  struct TileSetDefinition
  {
    int version;
    int tileSetID;
    ImageDefinition image;
  };

  struct Frame
  {
    int index;
    int duration; /* 1/60 s */
  };

  struct Animation
  {
    ImageDefinition image;
    
    SpriteSize size; /* size of frames */
    bool looping;
    
    u16 frameCount;
    Frame *frames;
  };

  struct SpriteDefinition
  {
    int version;

    u16 spriteID; /* unique sprite ID that should be provided to the
		     createSpriteInstance and unloadSprite methods */
    
    u16 animationCount;
    Animation *animations;
  };

  enum ErrorCode
    {
      // Make sure that the char* array in getErrorString matches if
      // you add a new error code.
      NO_ERROR,
      OUT_OF_VRAM,
      FAILED_LOAD_TILESET,
      FAILED_LOAD_SPRITE,
      BAD_PARAMETER,
      DUPLICATE_TILESET_ID,
      DUPLICATE_SPRITE_ID,
      OUT_OF_SPRITE_INDEXES,
      BAD_SCREEN,
      BAD_LAYER,
      BAD_TILE_DIMENSION,
      BAD_VERSION,
      BAD_TILESET_ID,
      BAD_TILE_INDEX,
      BAD_TILE_COORDINATES,
      BAD_SPRITE_ID,
      BAD_SPRITE_DIMENSION,
      BAD_SPRITE_INDEX,
      SPRITE_NOT_LOADED,
      LAST_ERROR_CODE
    };

  struct Tile
  {
    u16 tileSet;
    u16 tile;
  };

  enum Screen {MAIN = 0, SUB = 1};
  enum Type {SPRITE = 0, BG = 1};

  // In general, a method will return 0 to indicate failure
#define RAGE_FAILED(x) ((x) == 0)

  Rage();
  int init();
  ErrorCode getErrorCode();
  const char *getErrorString();
  int redraw();
  int selectOnTop(Screen s);

  /* Background methods */
  int setupBackground(Screen s, u16 layer, u16 tileWidth, u16 tileHeight);
  int loadTileSet(Screen s, TileSetDefinition *def);
  int unloadTileSet(Screen s, u16 tileSet);
  int unloadAllTileSets(Screen s);
  int setTile(Screen s, u16 layer, u16 x, u16 y, u16 tileSet, u16 tile);
  int setMap(Screen s, u16 layer, Tile *map);
  int setMap(Screen s, u16 layer, u16 tileSet, u16 *map);

  /* Sprite methods */
  int loadSprite(Screen s, SpriteDefinition *def);
  int unloadSprite(Screen s, int sprite);
  int createSpriteInstance(Screen s, int sprite);
  int removeSpriteInstance(Screen s, int sprite);
  int selectAnimation(Screen s, int sprite, int animation);
  int showSprite(Screen s, int sprite, bool show);
  int moveSpriteAbs(Screen s, int sprite, int x, int y);
  int moveSpriteRel(Screen s, int sprite, int dx, int dy);

  /* Memory info methods */
  int getFreeMem(Screen s, Type t);
  int getLargestFreeBlock(Screen s, Type t);
  int getAvailableSprites(Screen s);

  void listFreeBlocks();
};

#endif
