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

/**
   \mainpage Rage API Documentation

   \section intro Introduction

   This is the API documentation for Rage - Remar's Abstract Graphics
   Engine for Nintendo DS, version 2. See rage.h for a listing of the
   main include file, and Rage for a listing of the Rage class with
   all the methods and the structs.


   \section init Initialization

   To initialize Rage, simply call Rage::init(BGMapMemSize mainBGSize,
   BGMapMemSize subBGSize). This will set up the standard VRAM
   mappings, set both screens to 2D mode 0, and initialize the OAM
   (sprite hardware) structures. The VRAM mappings gives you 128k VRAM
   for sprites and either 96k or 112k VRAM for backgrounds per
   screen. The amount of VRAM available for backgrounds depends on how
   much memory is allocated for maps. You can either allocate 16k or
   32k for maps per screen. A 256x256 pixel map needs 2k memory, a
   256x512 or 512x256 pixel map needs 4k memory, and a 512x512 pixel
   map needs 8k memory. This means that, to be able to create 4
   512x512 pixel maps, you must allocate 32k to maps. BGMapMemSize is
   either BG_MAPMEM_SIZE_16K or BG_MAPMEM_SIZE_32K.


   \section background Background handling

   \subsection setup Background setup

   To use the background methods you must first set up an abstract
   tilemap for a layer. You do this with a call to
   Rage::setupBackground(Screen s, u16 layer, BGMapSize bgMapSize, u16
   tileWidth, u16 tileHeight).  Screen can be one of Rage::MAIN and
   Rage::SUB, layer is a number between 0 and 3, bgMapSize is one of
   Rage::BG_MAP_256x256, Rage::BG_MAP_512x256, Rage::BG_MAP_256x512,
   or Rage::BG_MAP_512x512, and tileWidth and tileHeight are numbers
   divisible by 8 (8, 16, 24, 32, and so on).

   Layer 0 is in the foreground and layer 3 is in the background. This
   means that layer 0 will cover layer 3 (and 1, and 2), so you must
   use transparent tiles in layers 0, 1, and 2, to be able to see
   layer 3. Palette index 0 is transparent, use this to make
   transparent tiles.

   The colormode of backgrounds is 8 bpp, so make sure that the
   graphics you load in is paletted 8 bpp.

   If you're done with a background and want to free up its memory,
   you can do this with a call to Rage::releaseBackground(Screen s,
   u16 layer). Screen is as usual Rage::MAIN or Rage::SUB, and layer
   is between 0 and 3 (inclusive).

   \subsection loadingtileset Loading a tile set

   To load a tile set you must first define it by declaring a
   Rage::TileSetDefinition. This struct consists of a version, an ID,
   and a Rage::ImageDefinition. Set the version to the version of Rage
   (1 for now). The ID should be unique among the loaded tile sets,
   and must be between 0 and 15. The ImageDefinition is filled with
   the values that are generated by grit. Please set all datatypes in
   grit to be u16 (-gu16) and color format to 8 bpp (-gB8).

   Then, when you've defined your tile set, you can load it in with a
   call to Rage::loadTileSet. You can load the same tile set into both
   screens if you wish, there won't be a tile set ID collision.

   To unload a tile set, e.g. to make room for other tile sets, you
   can use Rage::unloadTileSet or Rage::unloadAllTileSets.

   \subsection tileset Setting tiles
   
   There are two main ways to set tiles, either one by one, or all in
   one go.

   To set only one tile, use Rage::setTile(Screen s, u16 layer, u16 x,
   u16 y, u16 tileSet, u16 tile). Screen is as usual Rage::MAIN or
   Rage::SUB, and layer is between 0 and 3. The x and y coordinates
   are coordinates into the abstract tilemap, so if you have 16x16
   pixel tiles, valid values for x is 0 to 15, and valid values for y
   is 0 to 11. The tileSet parameter should be the tile set ID of a
   loaded tile set and the tile parameter is an index into the loaded
   tile set.

   To set all the tiles in one go, there are two variants of the
   Rage::setMap function. Either you provide an array of the struct
   Rage::Tile, or you can provide an array of integers to set all the
   tiles using the same tile set. See Rage::setMap for a discussion on
   the correct dimensions for the provided array.


   \section sprite Sprite handling

   \subsection loadingsprite Loading a sprite

   Just as with a tile set, you must declare a struct that holds the
   definition of a sprite. You do this by declaring a
   Rage::SpriteDefinition and filling it in. This struct consists of a
   version, an ID, the number of animations, and an array of
   Rage::Animation. As with a tile set definition, version should be
   the version of Rage (1 for now) and the ID should be unique among
   the loaded sprites. The number of animations should of course be
   set to the size of the array of animations assigned to the
   SpriteDefinition.

   A Rage::Animation consists of a Rage::ImageDefinition, the size of
   each frame, an indicator of if the animation is looping, the number
   of frames, and an array of Rage::Frame. The ImageDefinition is
   filled in with the details found in the include file generated by
   grit. The size of each frame is one of the valid sprite sizes on
   the Nintendo DS, see Rage::Animation::size for a list of valid
   sizes. The looping attribute is hopefully self evident. The number
   of frames is set to the size of the array of Frames.

   A Rage::Frame consist of an index into the animations image, and a
   duration measured in screen updates (frames). The index can either
   be set to a valid offset into the animation image, or -1 to
   indicate an empty frame. This empty frame means that the sprite is
   hidden during the frames duration, useful for blinking sprites or
   hiding a sprite when an animation is done.

   When you have defined a sprite, you can load it into Rage with
   Rage::loadSprite. Make sure to check the return value to catch any
   errors found in your sprite definition.

   Loading a sprite will not actually load any graphics into VRAM,
   that happens when you create a sprite instance and start an
   animation.

   To unload a sprite you can use the Rage::unloadSprite method.

   \subsection createspriteinstance Creating a sprite instance

   To actually show a sprite on the screen, you have to create an
   instance of the sprite. You do this with a call to
   Rage::createSpriteInstance. The return value from this call is a
   handle that you should provide in the sprite control methods
   explained below, or 0 on failure. The created sprite instance will
   be visible, play the first animation defined, and be positioned at
   0,0.

   To remove a sprite instance, call Rage::removeSpriteInstance.

   \subsection spritefunctionality Sprite control methods

   There are a couple of methods that you can use to control your
   sprites.

   To select an animation for a sprite instance, call
   Rage::selectAnimation. This call will unload the previous animation
   image from VRAM and load in the new animation image.

   To hide a sprite, or show it again after hiding, use
   Rage::showSprite.

   You can move sprites in two different ways, either giving an
   absolute position to move the sprite to, or move the sprite
   relative to the current position. You do this with either
   Rage::moveSpriteAbs or Rage::moveSpriteRel.


   \section resource Resource information

   There are a couple of methods you can use to get information about
   the availability of resources.

   To get the amount of free VRAM, in bytes, use Rage::getFreeMem. You
   can get information about either sprite VRAM or background VRAM.

   If fragmentation is an issue in your program you can use
   Rage::getLargestFreeBlock to get the largest block, in bytes, of
   consecutive free VRAM. This is an indication of how big tileset or
   animation you currently can load in.

   You can have a maximum of 128 sprites per screen. To get
   information about how many more sprite instances you can create,
   call Rage::getAvailableSprites.


   \section debug Debugging help

   There are a couple of methods that can be useful when debugging a
   program.

   Use Rage::getErrorCode to get the current error code. Call this
   method immediately after getting an error code from a Rage method
   call, to see what went wrong. In general, a return value of 0
   indicates an error.

   Use Rage::getErrorString to get the current error as a string
   instead of an integer. This is useful to output the error to a
   console.

   To show a list of free VRAM blocks, call Rage::listFreeBlocks. You
   must first initialize a console with consoleInit or
   consoleDemoInit.

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
  /** Current version of the Rage specification */
  static const int RAGE_VERSION = 2;

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
	createSpriteInstance and unloadSprite methods. The spriteID
	must be between 0 and 127 (inclusive). */
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
      BAD_ANIMATION_ID,
      BAD_MAP_SIZE,
      OUT_OF_MAP_MEMORY,
      FAILED_PALETTE_MERGE,
      FAILED_PALETTE_UNMERGE,
      TILEMAP_NOT_INITIALIZED,
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
  enum Screen
    {MAIN = 0, /**< The main DS screen. */
     SUB = 1   /**< The secondary DS screen. */};

  /** Definitions used to point out which VRAM to use. */
  enum Type {SPRITE = 0, BG = 1, MAP = 2};

  /** The two available choices for map memory size. */
  enum BGMapMemSize {BG_MAPMEM_SIZE_16K = 0, /**< 16k map memory, 112k tile memory */
		     BG_MAPMEM_SIZE_32K = 1  /**< 32k map memory, 96k tile memory */};

  /** The various map sizes, in pixels. */
  enum BGMapSize {BG_MAP_256x256 = 0,
		  BG_MAP_512x256 = 1,
		  BG_MAP_256x512 = 2,
		  BG_MAP_512x512 = 3};

  // In general, a method will return 0 to indicate failure
#define RAGE_FAILED(x) ((x) == 0)

  /* -------------------- General methods -------------------- */

  /** Default constructor that sets up some of Rage's internal
      structures. */
  Rage();

  /** Sets up the two screens and initializes the OAM structs.

      @param mainBGSize How much memory that will be allocated for
      background maps for the main screen.

      @param subBGSize How much memory that will be allocated for
      background maps for the sub screen.
      
      @return 0 on failure, 1 on success. */
  int init(BGMapMemSize mainBGSize = BG_MAPMEM_SIZE_16K,
	   BGMapMemSize subBGSize = BG_MAPMEM_SIZE_16K);

  /** Returns the error code, use after a method indicates an
      error. 

      @return The last error encountered. */
  ErrorCode getErrorCode();

  /** Call this once each frame to update the background and sprite
      OAM.

      @return 0 on failure, 1 on success. */
  int redraw();

  /** Select which screen to show on the top display.
  
      @param s Which screen to show on top, either Rage::MAIN or
      Rage::SUB.

      @return 0 on failure, 1 on success. */
  int selectOnTop(Screen s);

  /* -------------------- Background methods -------------------- */

  /** Sets up an abstract background for a layer. tileWidth and
      tileHeight must be divisible by 8, so valid values are 8, 16,
      24, 32, and so on.

      Layer 0 is in the foreground and layer 3 is in the background. This
      means that layer 0 will cover layer 3 (and 1, and 2), so you must
      use transparent tiles in layers 0, 1, and 2, to be able to see
      layer 3. Palette index 0 is transparent, use this to make
      transparent tiles.

      @param s Screen to set up background on, Rage::MAIN or Rage::SUB.

      @param layer Which layer to set up, a value between 0 and 3.

      @param bgMapSize Size of this layer, Rage::BG_MAP_256x256,
      Rage::BG_MAP_512x256, Rage::BG_MAP_256x512, or
      Rage::BG_MAP_512x512.

      @param tileWidth The width of a tile. Must be divisible by 8.

      @param tileHeight The height of a tile. Must be divisible by 8.

      @return 0 on failure, 1 on success. */
  int setupBackground(Screen s, u16 layer, BGMapSize bgMapSize,
		      u16 tileWidth, u16 tileHeight);

  /** Releases a background, freeing up its map area.

      @param s Screen to release background on.

      @param layer Background layer to release.

      @return 0 on failure, 1 on success. */
  int releaseBackground(Screen s, u16 layer);

  /** Load a tileset into VRAM.
      
      @param s Screen to load tile set into, Rage::MAIN or
      Rage::SUB.

      @param def A valid tile set definition.

      @return 0 on failure, 1 on success. */
  int loadTileSet(Screen s, TileSetDefinition *def);

  /** Unload a tileset from VRAM. 

      @param s Screen to unload tile set from, Rage::MAIN or Rage::SUB.

      @param tileSet The tile set ID of a loaded tile set.

      @return 0 on failure, 1 on success. */
  int unloadTileSet(Screen s, u16 tileSet);

  /** Unload all the tilesets loaded into the indicated screen.

      @param s Screen to unload all tile sets from, Rage::MAIN or
      Rage::SUB.

      @return 0 on failure, 1 on success. */
  int unloadAllTileSets(Screen s);

  /** Set the scroll of the selected layer.

      @param s Screen where layer is located, Rage::MAIN or Rage::SUB.

      @param layer Layer to scroll.

      @param x The x position to set scroll to. Note that a positive
      value moves the layer left.

      @param y The y position to set scroll to. Note that a positive
      value moves the layer up.

      @return 0 on failure, 1 on success. */
  int setBackgroundScroll(Screen s, u16 layer, int x, int y);

  /** Set a tile in the abstract tilemap.

      @param s Screen to set tile on, Rage::MAIN or Rage::SUB. 

      @param layer Layer to set tile on, 0-3. 

      @param x X coordinate into the abstract tile map. 

      @param y Y coordinate into the abstract tile map. 

      @param tileSet The tile set ID of a loaded tile set.

      @param tile An index into the indicated tile set.

      @return 0 on failure, 1 on success. */
  int setTile(Screen s, u16 layer, u16 x, u16 y, u16 tileSet, u16 tile);

  /** Set all the tiles in one go. The tilemap provided must have the
      correct dimension, and must match the size you've chosen for the
      map.

      If you've chosen a 256x256 pixel map, with 16x16 pixel tiles,
      you can fit 16 tiles in a row and 16 tiles in a column, so the
      correct dimension for the map is 16*16. If you've selected a
      512x256 pixel map, with 32x32 pixel tiles, you will fit 512/32 =
      16 tiles in a row and 256/32 = 8 tiles in a column, so the
      correct dimension will be 16*8.

      In general, to find out the correct dimensions, you can use this
      simple formula: map size = roof(mapWidth/tileWidth) *
      roof(mapHeight/tileHeight). roof(x) means round up, so
      roof(256/24) = 11. Example: background is set up as a 512x512
      pixel map with 32x24 pixel tiles. The correct dimensions for the
      map will be: roof(512/32) * roof(512/24) = 16 * 22 = 352
      entries.

      @param s Screen to set map on, Rage::MAIN or Rage::SUB.

      @param layer Layer to set map on, 0-3.

      @param map Array of type Rage::Tile of the correct dimensions. 

      @return 0 on failure, 1 on success. */
  int setMap(Screen s, u16 layer, Tile *map);

  /** Set all the tiles in one go. Works the same as setMap that takes
      an array of Tile, but this variant sets all the tiles using the
      same tileset.

      @see setMap for a discussion on the correct dimensions for the
      tilemap.

      @param s Screen to set map on, Rage::MAIN or Rage::SUB.

      @param layer Layer to set map on, 0-3.

      @param tileSet Tile set ID of a loaded tile set.

      @param map Array of type u16 of the correct dimensions.

      @return 0 on failure, 1 on success. */
  int setMap(Screen s, u16 layer, u16 tileSet, u16 *map);

  /* -------------------- Sprite methods -------------------- */

  /** Load a sprite into the sprite engine. This method does not
      actually load any graphics into VRAM, that happens when an
      animation is started. 

      @param s Screen to load sprite into, Rage::MAIN or Rage::SUB.

      @param def A valid sprite definition.

      @return 0 on failure, 1 on success. */
  int loadSprite(Screen s, SpriteDefinition *def);

  /** Unloads a sprite from the sprite engine. Make sure that there
      are no instances of this sprite left. 

      @param s Screen to unload sprite from, Rage::MAIN or Rage::SUB.

      @param sprite Sprite ID of a loaded sprite. 

      @return 0 on failure, 1 on success. */
  int unloadSprite(Screen s, int sprite);

  /** Unloads all sprites from one screen. Make sure that there are no
      instances left of the sprites on this screen.

      @param s Screen to unload all sprites from, Rage::MAIN or
      Rage::SUB.

      @return 0 on failure, 1 on success. */
  int unloadAllSprites(Screen s);

  /** Create an instance of a sprite. The sprite will start the first
      animation defined in the sprite, be visible, and be located at
      0,0.

      @param s Screen where sprite is loaded, Rage::MAIN or Rage::SUB.

      @param sprite Sprite ID of a loaded sprite.

      @return The handle to use in the sprite methods below
      (e.g. selectAnimation, moveSpriteAbs). Returns 0 on failure. */
  int createSpriteInstance(Screen s, int sprite);

  /** Remove a sprite instance. 

      @param s Screen where sprite instance is located, Rage::MAIN or
      Rage::SUB.

      @param sprite Sprite handle returned from Rage::createSpriteInstance.

      @return 0 on failure, 1 on success. */
  int removeSpriteInstance(Screen s, int sprite);

  /** Select animation for the sprite.

      @param s Screen where sprite instance is located, Rage::MAIN or
      Rage::SUB.

      @param sprite Sprite handle returned from Rage::createSpriteInstance.

      @param animation The animation to select.

      @return 0 on failure, 1 on success. */
  int selectAnimation(Screen s, int sprite, int animation);

  /** Decide if this sprite should be hidden or visible. 

      @param s Screen where sprite instance is located, Rage::MAIN or
      Rage::SUB.

      @param sprite Sprite handle returned from Rage::createSpriteInstance.

      @param show Show sprite if true, hide sprite if false.

      @return 0 on failure, 1 on success. */
  int showSprite(Screen s, int sprite, bool show);

  /** Move a sprite to an absolute coordinate.

      @param s Screen where sprite instance is located, Rage::MAIN or
      Rage::SUB.

      @param sprite Sprite handle returned from Rage::createSpriteInstance.

      @param x New X coordinate of sprite instance.

      @param y New Y coordinate of sprite instance.

      @return 0 on failure, 1 on success. */
  int moveSpriteAbs(Screen s, int sprite, int x, int y);

  /** Move a sprite relative to the current position.

      @param s Screen where sprite instance is located, Rage::MAIN or
      Rage::SUB.

      @param sprite Sprite handle returned from Rage::createSpriteInstance.

      @param dx Relative movement in the x direction.

      @param dy Relative movement in the y direction.

      @return 0 on failure, 1 on success. */
  int moveSpriteRel(Screen s, int sprite, int dx, int dy);

  /* -------------------- Memory info methods -------------------- */

  /** Get amount of free bytes in VRAM.

      @param s Screen to get information about, Rage::MAIN or
      Rage::SUB.

      @param t Type of VRAM to get information about, Rage::SPRITE or
      Rage::BG.

      @return Amount of free bytes. */
  int getFreeMem(Screen s, Type t);

  /** Get largest consecutive block of free bytes in VRAM. 

      @param s Screen to get information about, Rage::MAIN or
      Rage::SUB.

      @param t Type of VRAM to get information about, Rage::SPRITE or
      Rage::BG.

      @return Largest block in bytes. */
  int getLargestFreeBlock(Screen s, Type t);

  /** Get how many more sprites that can be instantiated.

      @param s Screen to get information about.

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
