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
  // consoleDemoInit();

  printf("%s\n", rage.getErrorString());

  while(1){}
}

#define TRY(x) {if(RAGE_FAILED(x)) displayError();}

int main(void)
{
  // setup default VRAM mappings and screen modes
  TRY(rage.init(Rage::BG_MAPMEM_SIZE_32K, Rage::BG_MAPMEM_SIZE_16K));

  srand(time(0));

  TRY(rage.setupBackground(Rage::MAIN, /* screen, Rage::MAIN or Rage::SUB */
			   0  /* layer, 0-3, 0 in front, 3 in back */,
			   Rage::BG_MAP_512x512, /* map size in pixels;
						    256x256, 512x256,
						    256x512, or 512x512*/
			   32 /* tile width, must be divisible by 8 */,
			   32 /* tile height, must be divisible by 8 */));

#include "bonusdef.h"   // define bonusDef, 32x32 tileset

  // load in large (32x32) tileset on the main screen
  TRY(rage.loadTileSet(Rage::MAIN, &bonusDef));

  for(int y = 0;y < 6;y++)
    for(int x = 0;x < 8;x++)
      {
	TRY(rage.setTile(Rage::MAIN, 0, x, y, BONUS_TILESET, rand()%16));
      }

  while(1)
    {
      rage.redraw();
    }

  return 0;
}
