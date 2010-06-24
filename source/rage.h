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

#ifndef RAGE_INCLUDE_H
#define RAGE_INCLUDE_H

#include <nds.h>

/**
   This is the API class to use when you want to use Rage.
*/
class Rage
{
 public:
  /** Current version of Rage */
  static const int RAGE_VERSION = 1;

  /** Fill this struct with what grit outputs in the .h file. */
  struct ImageDefinition
    {
      /** Pointer to tiles. */
      const u16 *gfx;
      /** Length in bytes of tiles. */
      u16 gfxLen;
      /** Pointer to palette. */
      const u16 *pal;
      /** Length in bytes of palette. */
      u16 palLen;
    };

  /** Defines a tileset. The tileSetID is used to reference this
      tileset, so only one tileset with this tileSetID can be loaded
      into a screen at a time. The tileSetID must be between 0 and 15
      (inclusive). */
  struct TileSetDefinition
  {
    /** Rage version. */
    int version;

    /** A unique tileset ID between 0 and 15 (inclusive). */
    int tileSetID; 

    /** The image that contains the graphics for this tileset. */
    ImageDefinition image;
  };

  /** One frame of animation. If you set index to -1 this frame is
      'empty' which means the sprite will be hidden during the
      duration. Useful for blinking sprites and sprites that should
      disappear when an animation is done. */
  struct Frame
  {
    /** Index into the image found in Rage::Animation. */
    int index;

    /** How many screen updates this frame should be visible. Each
	screen update is ~1/60 second. */
    int duration;
  };

  /** Defines an animation. */
  struct Animation
  {
    /** The image that contains the graphics for this animation. */
    ImageDefinition image;
     
    /** The size of each frame. Can be one of: SpriteSize_8x8,
	SpriteSize_16x16, SpriteSize_32x32, SpriteSize_64x64,
	SpriteSize_16x8, SpriteSize_32x8, SpriteSize_32x16,
	SpriteSize_64x32, SpriteSize_8x16, SpriteSize_8x32,
	SpriteSize_16x32, SpriteSize_32x64. */
    SpriteSize size;

    /** Indicates if this animation should loop. */
    bool looping;
    
    /** Number of frames in animation. */
    u16 frameCount;

    /** Array of frames for this animation. */
    Frame *frames;
  };

  /** Defines a sprite. */
  struct SpriteDefinition
  {
    /** Rage version. */
    int version;

    /** Unique sprite ID that should be provided to the
	createSpriteInstance and unloadSprite methods. */
    u16 spriteID;
    
    /** Number of animations in sprite. */
    u16 animationCount;

    /** Array of animations for this sprite. */
    Animation *animations;
  };

  /** Error codes returned by getErrorCode. */
  enum ErrorCode
    {
      // IMPLEMENTATION NOTE: Make sure that the char* array in
      // getErrorString matches if you add a new error code.
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

  /** Struct to provide to setMap. */
  struct Tile
  {
    /** Tileset ID of desired tileset. */
    u16 tileSet;

    /** Index into the tileset. */
    u16 tile;
  };

  /** Definitions used to decide which screen to use. */
  enum Screen {MAIN = 0, SUB = 1};

  /** Definitions used to point out which VRAM to use. */
  enum Type {SPRITE = 0, BG = 1};

  // In general, a method will return 0 to indicate failure
#define RAGE_FAILED(x) ((x) == 0)

  /* -------------------- General methods -------------------- */

  /** Default constructor that resets values. */
  Rage();

  /** Sets up the two screens and initializes the oam structs. */
  int init();

  /** Returns the error code, use after a method indicates an
      error. */
  ErrorCode getErrorCode();

  /** Call this once each frame to update the background and sprite
      OAM. */
  int redraw();

  /** Select which screen to show on the top display. Either
      Rage::MAIN or Rage::SUB. */
  int selectOnTop(Screen s);

  /* -------------------- Background methods -------------------- */

  /** Sets up an abstract background for a layer. tileWidth and
      tileHeight must be divisible by 8, so valid values are 8, 16,
      24, 32, and so on. */
  int setupBackground(Screen s, u16 layer, u16 tileWidth, u16 tileHeight);

  /** Load a tileset into VRAM. */
  int loadTileSet(Screen s, TileSetDefinition *def);

  /** Unload a tileset from VRAM. */
  int unloadTileSet(Screen s, u16 tileSet);

  /** Unload all the tilesets loaded into the indicated screen. */
  int unloadAllTileSets(Screen s);

  /** Set a tile in the abstract tilemap. */
  int setTile(Screen s, u16 layer, u16 x, u16 y, u16 tileSet, u16 tile);

  /** Set all the tiles in one go. The tilemap provided must have the
      correct dimension, so if you have 16x16 tiles, the tilemap will
      have size 16*12. In general, to calculate the size of the
      tilemap you can use this formula:
      (32/(tileWidth/8))*(24/(tileHeight/8)). If the tiles doesn't fit
      perfectly into the 32*24 map, then the tiles will be clipped at
      the edges, so these tiles must also be provided. E.g. if you use
      24*24 pixel tiles, the rightmost tiles will only show
      2/3rds. You must still provide these tiles, so the tilemap will
      be 11*8. */
  int setMap(Screen s, u16 layer, Tile *map);

  /** Set all the tiles in one go. Works the same as setMap that takes
      an array of Tile, but this variant sets all the tiles using the
      same tileset.

      @see setMap for a discussion on the correct dimension for the
      tilemap. */
  int setMap(Screen s, u16 layer, u16 tileSet, u16 *map);

  /* -------------------- Sprite methods -------------------- */

  /** Load a sprite into the sprite engine. This method does not
      actually load any graphics into VRAM, that happens when an
      animation is started. 

      @return 0 on failure, 1 on success. */
  int loadSprite(Screen s, SpriteDefinition *def);

  /** Unloads a sprite from the sprite engine. Make sure that there
      are no instances of this sprite left. */
  int unloadSprite(Screen s, int sprite);

  /** Create an instance of a sprite. The sprite will start the first
      animation defined in the sprite, be visible, and be located at
      0,0.

      @return The key to use in the sprite methods below
      (e.g. selectAnimation, moveSpriteAbs). Returns 0 on failure. */
  int createSpriteInstance(Screen s, int sprite);

  /** Remove a sprite instance. 

      @return 0 on failure, 1 on success. */
  int removeSpriteInstance(Screen s, int sprite);

  /** Select animation for the sprite.

      @return 0 on failure, 1 on success. */
  int selectAnimation(Screen s, int sprite, int animation);

  /** Decide if this sprite should be hidden or visible. 

      @return 0 on failure, 1 on success. */
  int showSprite(Screen s, int sprite, bool show);

  /** Move a sprite to an absolute coordinate.

      @return 0 on failure, 1 on success. */
  int moveSpriteAbs(Screen s, int sprite, int x, int y);

  /** Move a sprite relative to the current position.

      @return 0 on failure, 1 on success. */
  int moveSpriteRel(Screen s, int sprite, int dx, int dy);

  /* -------------------- Memory info methods -------------------- */

  /** Get amount of free bytes in VRAM.

      @return Amount of free bytes. */
  int getFreeMem(Screen s, Type t);

  /** Get largest consecutive block of free bytes in VRAM. 

      @return Largest block in bytes. */
  int getLargestFreeBlock(Screen s, Type t);

  /** Get how many more sprites that can be instantiated.

      @return Available sprite indices. */
  int getAvailableSprites(Screen s);

  /* -------------------- Debug methods -------------------- */

  /** Get the error code as a string representation, useful for
      debugging. 

      @return Descriptive error string. */
  const char *getErrorString();

  /** Prints a list of free blocks in VRAM. Before using this function
      you must call consoleInit or consoleDemoInit. Useful for
      debugging. */
  void listFreeBlocks();
};

#endif
