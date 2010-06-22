/*
  RAGE demo 03

  A first simple demo to show sprite handling.
*/

#include <nds.h>
#include <stdio.h>

#include "rage.h"

#include "CaptainGood.h"

Rage rage;

void displayError()
{
  consoleDemoInit();

  printf("ERROR\n--------------------------------\n"
	 "%s\n", rage.getErrorString());

  while(1){}
}

#define TRY(x) {if(RAGE_FAILED(x)) displayError();}

void loadGfx()
{
#include "bonusdef.h" // define bonusDef, 24x24 tileset

  // load in tileset on both screens
  TRY(rage.loadTileSet(Rage::MAIN, &bonusDef));
  TRY(rage.loadTileSet(Rage::SUB, &bonusDef));

#include "cpngooddef.h" // define Captain Good sprite

  // load the Captain Good sprite into both screens
  TRY(rage.loadSprite(Rage::MAIN, &cpngoodDef));
  TRY(rage.loadSprite(Rage::SUB, &cpngoodDef));
}

CaptainGood *cpnGood[2][12];

void init()
{
  TRY(rage.init());
  TRY(rage.setupBackground(Rage::MAIN, 0, 24, 24));
  TRY(rage.setupBackground(Rage::SUB, 0, 24, 24));

  loadGfx();

  // use setMap to display a background on both screens
  Rage::Tile map[11*8];
  for(int i = 0;i < 11*8;i++)
      {
	map[i].tileSet = BONUS_TILESET;
	map[i].tile = 0;
      }
  TRY(rage.setMap(Rage::MAIN, 0, map));
  TRY(rage.setMap(Rage::SUB, 0, map));

  // Initialize the Captain Goods
  for(int i = 0;i < 12;i++)
    {
      cpnGood[Rage::MAIN][i] = new CaptainGood(Rage::MAIN, &rage);
      cpnGood[Rage::MAIN][i]->moveAbs(i>5?11*16-i*16:i*16, i*16);

      cpnGood[Rage::SUB][i] = new CaptainGood(Rage::SUB, &rage);
      cpnGood[Rage::SUB][i]->moveAbs(240-(i>5?(11*16-i*16):i*16), i*16);
      cpnGood[Rage::SUB][i]->setDirection(-1);
    }
}

int main(void)
{
  init();

  while(1)
    {
      for(int i = 0;i < 12;i++)
	{
	  cpnGood[Rage::MAIN][i]->update();
	  cpnGood[Rage::SUB][i]->update();
	}

      rage.redraw();
    }

  return 0;
}
