#ifndef RAGE_INCLUDE_H
#define RAGE_INCLUDE_H

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
    
    int animationID; /* this is the unique number to provide to
			specify this animation */
    
    int width, height; /* width and height of frames */
    bool looping;
    
    int frameCount;
    Frame *frames;
  };

  struct SpriteDefinition
  {
    int version;
    
    int animationCount;
    Animation *animations;
  };

  enum ErrorCode
    {
      NO_ERROR,
      OUT_OF_VRAM,
      FAILED_LOAD_TILESET,
      FAILED_LOAD_SPRITE,
      BAD_PARAMETER,
      ANIMATION_ID_COLLISION,
      OUT_OF_SPRITE_INDEXES,
      BAD_ENGINE,
      BAD_LAYER,
      BAD_TILE_DIMENSION,
      BAD_VERSION,
      BAD_TILESET_ID,
      BAD_TILE_INDEX,
    };

  struct Tile
  {
    u16 tileSet;
    u16 tile;
  };

  enum Engine {MAIN = 0, SUB = 1};
  enum Type {SPRITE, BG};

  // In general, a method will return 0 to indicate failure
#define FAILED(x) ((x) == 0)

  Rage();
  int init();
  ErrorCode getErrorCode();
  int redraw();
  int selectOnTop(Engine e);
  int setupBackground(Engine e, int layer, int tileWidth, int tileHeight);
  int loadTileSet(Engine e, TileSetDefinition *def);
  int unloadTileSet(Engine e, int tileSet);
  int setTile(Engine e, int layer, int x, int y, int tileSet, int tile);
  int setMap(Engine e, int layer, Tile *map);
};

#endif
