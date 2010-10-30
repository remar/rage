/*
  RAGE demo 01

  The first introductory demo of Remar's Abstract Graphics Engine.
*/

#include "rage.h"
#include <stdio.h>
#include <time.h>

Rage rage;

void displayError()
{
  consoleDemoInit();

  printf("%s\n", rage.getErrorString());

  while(1){}
}

#define TRY(x) {if(RAGE_FAILED(x)) displayError();}

int main(void)
{
  srand(time(0));

  TRY(rage.init(Rage::BG_MAPMEM_SIZE_16K, Rage::BG_MAPMEM_SIZE_16K)); // setup default VRAM mappings and screen modes

  TRY(rage.setupBackground(Rage::MAIN, /* screen, Rage::MAIN or Rage::SUB */
			   0  /* layer, 0-3, 0 in front, 3 in back */,
			   Rage::BG_MAP_256x256, /* map size, in pixels */
			   32 /* tile width, must be divisible by 8 */,
			   32 /* tile height, must be divisible by 8 */));

  TRY(rage.setupBackground(Rage::SUB, 0, Rage::BG_MAP_256x256, 16, 16));

#include "bonusdef.h"   // define bonusDef, 32x32 tileset
#include "blockdef.h"   // define blockDef, 16x16 tileset
#include "metroiddef.h" // define metroidDef, 16x16 tileset

  // load in large (32x32) tileset on the main screen
  TRY(rage.loadTileSet(Rage::MAIN, &bonusDef));

  // load in two different 16x16 tilesets on the sub screen
  TRY(rage.loadTileSet(Rage::SUB, &blockDef));
  TRY(rage.loadTileSet(Rage::SUB, &metroidDef));

  for(int y = 0;y < 6;y++)
    for(int x = 0;x < 8;x++)
      {
	TRY(rage.setTile(Rage::MAIN, 0, x, y, BONUS_TILESET, rand()%16));
      }

  for(int y = 0;y < 12;y++)
    for(int x = 0;x < 16;x++)
      {
	int tileset = rand()%2?BLOCK_TILESET:METROID_TILESET;
	int tile = tileset == BLOCK_TILESET ? (rand()%16)+1 : (rand()%3)+1;
	TRY(rage.setTile(Rage::SUB, 0, x, y, tileset, tile));
      }

  while(1)
    {
      rage.redraw();
    }

  return 0;
}
