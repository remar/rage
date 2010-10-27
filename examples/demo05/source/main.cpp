/*
  RAGE demo 05

  DEBUG
*/

#include "rage.h"
#include <stdio.h>
#include <time.h>

Rage rage;

void displayError()
{
  consoleDemoInit();

  printf("[Rage] %s\n", rage.getErrorString());

  while(1){}
}

#define TRY(x) {if(RAGE_FAILED(x)) displayError();}

int main(void)
{
  // setup default VRAM mappings and screen modes
  TRY(rage.init(Rage::BG_MAPMEM_SIZE_16K, Rage::BG_MAPMEM_SIZE_16K));

  srand(time(0));

  TRY(rage.setupBackground(Rage::MAIN, 0, Rage::BG_MAP_512x512, 16, 16));
  TRY(rage.setupBackground(Rage::MAIN, 1, Rage::BG_MAP_512x512, 16, 16));
  //TRY(rage.setupBackground(Rage::SUB, 0, Rage::BG_MAP_512x512, 16, 16));
  //TRY(rage.setupBackground(Rage::SUB, 1, Rage::BG_MAP_512x512, 16, 16));

#include "blockdef.h"
  TRY(rage.loadTileSet(Rage::MAIN, &blockDef));
  //TRY(rage.loadTileSet(Rage::SUB, &blockDef));

#include "metroiddef.h"
  TRY(rage.loadTileSet(Rage::MAIN, &metroidDef));
  //TRY(rage.loadTileSet(Rage::SUB, &metroidDef));

  for(int y = 0;y < 32;y++)
    for(int x = 0;x < 32;x++)
      {
	if(y > 15)
	  {
	    TRY(rage.setTile(Rage::MAIN, 0, x, y, BLOCK_TILESET, rand()%16+1));
	    //TRY(rage.setTile(Rage::SUB, 0, x, y, BLOCK_TILESET, rand()%16+1));
	  }
	 else
	   {
	     TRY(rage.setTile(Rage::MAIN, 0, x, y, METROID_TILESET, rand()%4));
	     //TRY(rage.setTile(Rage::SUB, 0, x, y, METROID_TILESET, rand()%4));
	   }

	TRY(rage.setTile(Rage::MAIN, 1, x, y, BLOCK_TILESET, rand()%16+1));
	//TRY(rage.setTile(Rage::SUB, 1, x, y, BLOCK_TILESET, rand()%16+1));
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

      //TRY(rage.setBackgroundScroll(Rage::SUB, 0, y, y));
      //TRY(rage.setBackgroundScroll(Rage::SUB, 1, 0, 256 - y));

      TRY(rage.redraw());
    }

  return 0;
}
