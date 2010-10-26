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

  TRY(rage.setupBackground(Rage::MAIN, 0, Rage::BG_MAP_256x256, 16, 16));
  TRY(rage.setupBackground(Rage::MAIN, 1, Rage::BG_MAP_256x256, 16, 16));

#include "blockdef.h"
  TRY(rage.loadTileSet(Rage::MAIN, &blockDef));

#include "metroiddef.h"
  TRY(rage.loadTileSet(Rage::MAIN, &metroidDef));

  for(int y = 0;y < 12;y++)
    for(int x = 0;x < 16;x++)
      {
	TRY(rage.setTile(Rage::MAIN, 1, x, y, BLOCK_TILESET, rand()%16+1));

	TRY(rage.setTile(Rage::MAIN, 0, x, y, METROID_TILESET, rand()%4));
      }

  while(1)
    {
      rage.redraw();
    }

  return 0;
}
