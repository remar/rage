/*
  RAGE demo 06

  Sprite example, merging palettes automatically. Look at them
  bouncing sprites!!
*/

#include "rage.h"
#include <stdio.h>
#include <time.h>

Rage rage;

void displayError(int line)
{
  consoleDemoInit();

  printf("ERROR %d\n--------------------------------\n"
	 "Line %d: %s\n", rage.getErrorCode(), line, rage.getErrorString());

  while(1){}
}

#define TRY(x) {if(RAGE_FAILED(x)) displayError(__LINE__);}

struct Sprite
{
  int id;
  int x;
  int y;
  int dx;
  int dy;
  int delay;
  int delayCounter;
};

Sprite sprite[2][128];

int main(void)
{
  // setup default VRAM mappings and screen modes
  TRY(rage.init());

  srand(time(0));

  TRY(rage.setupBackground(Rage::MAIN, 0, Rage::BG_MAP_256x256, 16, 16));
  TRY(rage.setupBackground(Rage::SUB, 0, Rage::BG_MAP_256x256, 16, 16));

#include "blockdef.h"
  TRY(rage.loadTileSet(Rage::MAIN, &blockDef));
  TRY(rage.loadTileSet(Rage::SUB, &blockDef));

#include "coindef.h"
  TRY(rage.loadSprite(Rage::MAIN, &coinDef));
  TRY(rage.loadSprite(Rage::SUB, &coinDef));

#include "goaldef.h"
  TRY(rage.loadSprite(Rage::MAIN, &goalDef));
  TRY(rage.loadSprite(Rage::SUB, &goalDef));

  // Set up sprites
  for(int s = 0;s < 2;s++)
    for(int i = 0;i < 128;i++)
      {
	if(rand()%2)
	  {
	    sprite[s][i].id = rage.createSpriteInstance((Rage::Screen)s, GOAL_SPRITE);
	    rage.selectAnimation((Rage::Screen)s, sprite[s][i].id, OPEN_GOAL);
	  }
	else
	  sprite[s][i].id = rage.createSpriteInstance((Rage::Screen)s, COIN_SPRITE);

	sprite[s][i].x = rand()%(255-32-17)+17;
	sprite[s][i].y = rand()%(255-32-17)+17;
	sprite[s][i].dx = rand()%2 ? -1 : 1;
	sprite[s][i].dy = rand()%2 ? -1 : 1;
	sprite[s][i].delay = rand()%3 + 1;
	sprite[s][i].delayCounter = sprite[s][i].delay;
      }

  // Create map
  u16 map[16*16];

  for(int y = 1;y < 15;y++)
    for(int x = 1;x < 15;x++)
      {
	map[x+y*16] = 0;
      }

  for(int i = 0;i < 16;i++)
    {
      map[i+0*16] = 2;
      map[i+15*16] = 2;
      map[0+i*16] = 3;
      map[15+i*16] = 3;
    }

  map[0+0*16] = 4;
  map[15+0*16] = 5;
  map[0+15*16] = 6;
  map[15+15*16] = 7;

  // Load in map on both screens
  TRY(rage.setMap(Rage::MAIN, 0, BLOCK_TILESET, map));
  TRY(rage.setMap(Rage::SUB, 0, BLOCK_TILESET, map));

  int y = 0;
  int dy = 1;

#define SCROLL_DELAY 3
  int scrollCounter = SCROLL_DELAY;

  while(1)
    {
      if(--scrollCounter == 0)
	{
	  y += dy;

	  if(y < 1 || y > (255-192))
	    dy = -dy;

	  scrollCounter = SCROLL_DELAY;
	}

      for(int s = 0;s < 2;s++)
	for(int i = 0;i < 128;i++)
	  {
	    if(--sprite[s][i].delayCounter == 0)
	      {
		sprite[s][i].x += sprite[s][i].dx;
		sprite[s][i].y += sprite[s][i].dy;

		if(sprite[s][i].x < 17 || sprite[s][i].x > 255 - 16 - 16)
		  sprite[s][i].dx = -sprite[s][i].dx;

		if(sprite[s][i].y < 17 || sprite[s][i].y > 255 - 16 - 16)
		  sprite[s][i].dy = -sprite[s][i].dy;

		sprite[s][i].delayCounter = sprite[s][i].delay;

	      }
	    TRY(rage.moveSpriteAbs((Rage::Screen)s, sprite[s][i].id, sprite[s][i].x, sprite[s][i].y - y));
	  }

      TRY(rage.setBackgroundScroll(Rage::MAIN, 0, 0, y));
      TRY(rage.setBackgroundScroll(Rage::SUB, 0, 0, y));

      TRY(rage.redraw());
    }

  return 0;
}
