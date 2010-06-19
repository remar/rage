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

  printf("ERROR\n--------------------------------\n"
	 "%s\n", rage.getErrorString());

  while(1){}
}

#define TRY(x) {if(RAGE_FAILED(x)) displayError();}

int main(void)
{
  TRY(rage.init()); // setup default VRAM mappings and screen modes

  TRY(rage.setupBackground(Rage::MAIN, /* screen, Rage::MAIN or Rage::SUB */
			   0  /* layer, 0-3, 0 in front, 3 in back */,
			   24 /* tile width, must be divisible by 8 */,
			   24 /* tile height, must be divisible by 8 */));
  TRY(rage.setupBackground(Rage::SUB, 0, 24, 24));

#include "bonusdef.h" // define bonusDef, 24x24 tileset

  // load in tileset on both screens
  TRY(rage.loadTileSet(Rage::MAIN, &bonusDef));
  TRY(rage.loadTileSet(Rage::SUB, &bonusDef));

  // use setMap to display a background on the main screen
  Rage::Tile map[11*8];
  for(int i = 0;i < 11*8;i++)
      {
	map[i].tileSet = BONUS_TILESET;
	map[i].tile = 0;
      }
  TRY(rage.setMap(Rage::MAIN /* screen */,
		  0          /* layer */,
		  map        /* Tile array of correct size */));

  // use the other setMap to display a background on the sub screen
  u16 map2[11*8];
  for(int i = 0;i < 11*8;i++)
    map2[i] = 0;
  TRY(rage.setMap(Rage::SUB, 0, BONUS_TILESET, map2));

  while(1)
    {
      rage.redraw();
    }

  return 0;
}
