/*
  RAGE demo 04

  A simplified sprite demo without the class abstraction found in demo 03.
 */

#include "rage.h"

Rage rage;

int main(void)
{
  rage.init();

  rage.setupBackground(Rage::MAIN, 0, 16, 16);

#include "metroiddef.h"

  rage.loadTileSet(Rage::MAIN, &metroidDef);

  u16 map[16*12];
  for(int i = 0;i < 16*12;i++)
    map[i] = i/16==5?0:(i/16==6?rand()%3+1:rand()%4);

  rage.setMap(Rage::MAIN, 0, METROID_TILESET, map);

#include "cpngooddef.h"

  rage.loadSprite(Rage::MAIN, &cpngoodDef);

  int good = rage.createSpriteInstance(Rage::MAIN, GOOD_SPRITE);

  rage.selectAnimation(Rage::MAIN, good, WALK_RIGHT);

  int x = -16;
  rage.moveSpriteAbs(Rage::MAIN, good, x, 5*16);

  while(1)
    {
      rage.moveSpriteRel(Rage::MAIN, good, 1, 0);

      if(++x > 255)
	{
	  x = -16;
	  rage.moveSpriteAbs(Rage::MAIN, good, x, 5*16);
	}

      rage.redraw();
    }

  return 0;
}
