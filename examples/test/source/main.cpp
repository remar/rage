#include <nds.h>
#include <stdio.h>

#include "rage.h"
#include "block.h"

Rage rage;

void displayError()
{
  consoleDemoInit();

  switch(rage.getErrorCode())
    {
    case Rage::BAD_ENGINE:
      printf("Bad engine provided\n");
      break;

    case Rage::BAD_LAYER:
      printf("Bad layer provided\n");
      break;

    case Rage::BAD_TILE_DIMENSION:
      printf("Bad tile dimension provided\n");
      break;

    default:
      printf("Unknown error, code %d\n", rage.getErrorCode());
      break;
    }

  while(1){}
}

int main(void)
{

  if(FAILED(rage.init()))
    {
      displayError();
    }

  if(FAILED(rage.setupBackground(Rage::MAIN, 0, 16, 16)))
    {
      displayError();
    }

#include "blockdef.h" // define blockDef, our tileset

  rage.loadTileSet(Rage::MAIN, &blockDef);

  for(int i = 0;i < 16*12;i++)
    rage.setTile(Rage::MAIN, 0, i%16, i/16, 1, i%17);

  while(1)
    {
      rage.redraw();
    }

  return 0;
}
