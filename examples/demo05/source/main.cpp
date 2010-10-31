/*
  RAGE demo 05

  Example of background functions, scrolling and tile set unloading.
*/

#include "rage.h"
#include <stdio.h>
#include <time.h>

//#define DEBUG_ME 0xDEADBEEF

Rage rage;

void displayError(int line)
{
  consoleDemoInit();

  printf("ERROR %d\n--------------------------------\n"
	 "Line %d: %s\n", rage.getErrorCode(), line, rage.getErrorString());

  while(1){}
}

#define TRY(x) {if(RAGE_FAILED(x)) displayError(__LINE__);}

int main(void)
{
  // setup default VRAM mappings and screen modes
  TRY(rage.init(Rage::BG_MAPMEM_SIZE_16K, Rage::BG_MAPMEM_SIZE_16K));

#ifdef DEBUG_ME
  consoleDemoInit();
#endif

  srand(time(0));

  TRY(rage.setupBackground(Rage::MAIN, 0, Rage::BG_MAP_512x512, 16, 16));
  TRY(rage.setupBackground(Rage::MAIN, 1, Rage::BG_MAP_512x512, 16, 16));
#ifndef DEBUG_ME
  TRY(rage.setupBackground(Rage::SUB, 0, Rage::BG_MAP_512x512, 16, 16));
  TRY(rage.setupBackground(Rage::SUB, 1, Rage::BG_MAP_512x512, 16, 16));
#endif

#include "blockdef.h"
  TRY(rage.loadTileSet(Rage::MAIN, &blockDef));
  TRY(rage.unloadTileSet(Rage::MAIN, BLOCK_TILESET));
  TRY(rage.loadTileSet(Rage::MAIN, &blockDef));
#ifndef DEBUG_ME
  TRY(rage.loadTileSet(Rage::SUB, &blockDef));
#endif

#include "metroiddef.h"
  TRY(rage.loadTileSet(Rage::MAIN, &metroidDef));
  TRY(rage.unloadTileSet(Rage::MAIN, METROID_TILESET));
  TRY(rage.loadTileSet(Rage::MAIN, &metroidDef));
#ifndef DEBUG_ME
  TRY(rage.loadTileSet(Rage::SUB, &metroidDef));
#endif

  for(int y = 0;y < 32;y++)
    for(int x = 0;x < 32;x++)
      {
	if(y > 15)
	  {
	    TRY(rage.setTile(Rage::MAIN, 0, x, y, BLOCK_TILESET, rand()%16+1));
#ifndef DEBUG_ME
	    TRY(rage.setTile(Rage::SUB, 0, x, y, BLOCK_TILESET, rand()%16+1));
#endif

	  }
	 else
	   {
	     TRY(rage.setTile(Rage::MAIN, 0, x, y, METROID_TILESET, rand()%4));
#ifndef DEBUG_ME
	     TRY(rage.setTile(Rage::SUB, 0, x, y, METROID_TILESET, rand()%4));
#endif
	   }

	TRY(rage.setTile(Rage::MAIN, 1, x, y, BLOCK_TILESET, rand()%16+1));
#ifndef DEBUG_ME
	TRY(rage.setTile(Rage::SUB, 1, x, y, BLOCK_TILESET, rand()%16+1));
#endif
      }

  int y = 0;
  int dy = 1;

  while(1)
    {
      y += dy;

      if(y < 0 || y > 256)
       	dy = -dy;

      TRY(rage.setBackgroundScroll(Rage::MAIN, 0, y, y));
      TRY(rage.setBackgroundScroll(Rage::MAIN, 1, 0, 256 - y));

#ifndef DEBUG_ME
      TRY(rage.setBackgroundScroll(Rage::SUB, 0, y, y));
      TRY(rage.setBackgroundScroll(Rage::SUB, 1, 0, 256 - y));
#endif

      TRY(rage.redraw());
    }

  return 0;
}
