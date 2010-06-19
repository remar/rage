/*
  RAGE demo 02

  This is a demo to try out tilesizes that doesn't really fit. E.g. if
  you use 24x24 pixel tiles, you can fit 10 and 2/3 of a tile in one
  row, so the last one will be clipped.
*/

#include <nds.h>
#include <stdio.h>

#include "rage.h"

Rage rage;

void displayError()
{
  consoleDemoInit();

  printf("Error, code %d\n", rage.getErrorCode());

  while(1){}
}

#define TRY(x) {if(RAGE_FAILED(x)) displayError();}

int main(void)
{
  TRY(rage.init()); // setup default VRAM mappings and screen modes

  TRY(rage.setupBackground(Rage::MAIN,
			   0  /* layer, 0-3, 0 in front, 3 in back */,
			   24 /* tile width, must be divisible by 8 */,
			   24 /* tile height, must be divisible by 8 */));

#include "bonusdef.h"   // define bonusDef, 24x24 tileset

  // load in tileset on the main screen
  TRY(rage.loadTileSet(Rage::MAIN, &bonusDef));

  for(int y = 0;y < 8;y++)
    for(int x = 0;x < 11;x++)
      {
	TRY(rage.setTile(Rage::MAIN, 0, x, y, BONUS_TILESET, 0));
      }

  while(1)
    {
      rage.redraw();
    }

  return 0;
}
