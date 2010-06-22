#ifndef CAPTAIN_GOOD_H
#define CAPTAIN_GOOD_H

#include "rage.h"

class CaptainGood
{
 public:
  CaptainGood(Rage::Screen screen, Rage *gfx);
  ~CaptainGood();
  void moveAbs(int x, int y);
  void setDirection(int dx);
  void update();

 private:
  Rage::Screen screen;
  Rage *gfx;
  int sprite;
  int x, y;

  int dx;
};

#endif
