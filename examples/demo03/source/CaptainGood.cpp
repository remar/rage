#include "CaptainGood.h"

#include "gooddefines.h"

CaptainGood::CaptainGood(Rage::Screen screen, Rage *gfx)
  : screen(screen), gfx(gfx)
{
  sprite = gfx->createSpriteInstance(screen, GOOD_SPRITE);

  dx = 1;
  gfx->selectAnimation(screen, sprite, WALK_RIGHT);
}

CaptainGood::~CaptainGood()
{
  gfx->removeSpriteInstance(screen, sprite);
}

void
CaptainGood::moveAbs(int x, int y)
{
  this->x = x;
  this->y = y;

  gfx->moveSpriteAbs(screen, sprite, x, y);
}

void
CaptainGood::setDirection(int dx)
{
  this->dx = dx;
  gfx->selectAnimation(screen, sprite, dx==1?WALK_RIGHT:WALK_LEFT);
}

void
CaptainGood::update()
{
  x += dx;

  if(x <= 0 || x >= 256-16)
    {
      setDirection(-dx);
    }

  gfx->moveSpriteAbs(screen, sprite, x, y);
}
