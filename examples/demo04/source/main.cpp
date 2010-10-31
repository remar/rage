/*
  RAGE demo 04

  A simplified sprite demo without the class abstraction found in demo 03.
 */

#include "rage.h"
#include <time.h>
#include <stdio.h>

Rage rage;

void displayError(int line)
{
  consoleDemoInit();

  printf("ERROR\n--------------------------------\n"
	 "%d: %s\n", line, rage.getErrorString());

  while(1){}
}

#define TRY(x) {if(RAGE_FAILED(x)) displayError(__LINE__);}

int main(void)
{
  TRY(rage.init(Rage::BG_MAPMEM_SIZE_16K, Rage::BG_MAPMEM_SIZE_16K));

  srand(time(0));

  TRY(rage.setupBackground(Rage::MAIN, 0, Rage::BG_MAP_256x256, 16, 16));

#include "metroiddef.h"

  TRY(rage.loadTileSet(Rage::MAIN, &metroidDef));

  u16 map[16*16];
  for(int i = 0;i < 16*16;i++)
    map[i] = i/16==5?0:(i/16==6?rand()%3+1:rand()%4);

  TRY(rage.setMap(Rage::MAIN, 0, METROID_TILESET, map));

#include "cpngooddef.h"

  TRY(rage.loadSprite(Rage::MAIN, &cpngoodDef));

  int good = rage.createSpriteInstance(Rage::MAIN, GOOD_SPRITE);

  TRY(rage.selectAnimation(Rage::MAIN, good, WALK_RIGHT));

  int x = -16;
  TRY(rage.moveSpriteAbs(Rage::MAIN, good, x, 5*16));

  while(1)
    {
      TRY(rage.moveSpriteRel(Rage::MAIN, good, 1, 0));

      if(++x > 255)
	{
	  x = -16;
	  TRY(rage.moveSpriteAbs(Rage::MAIN, good, x, 5*16));
	}

      TRY(rage.redraw());
    }

  return 0;
}
