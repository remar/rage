/*
    Rage - Remar's Abstract Graphics Engine for Nintendo DS
    Copyright 2010 Andreas Remar

    This file is part of Rage.

    Rage is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    Rage is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Rage. If not, see <http://www.gnu.org/licenses/>.

    The licenses are located in doc/license in the Rage package, see
    the files COPYING (GPL) and COPYING.LESSER (LGPL).
*/

#ifndef SPRITE_INSTANCE_H
#define SPRITE_INSTANCE_H

#include "internalstructures.h"
#include "ImageCache.h"

class SpriteInstance
{
 public:
  SpriteInstance(SpriteDefinitionInternal *spriteDef, ImageCache *imageCache,
		 Rage::Screen screen, int spriteIndex);
  int setAnimation(u16 animationID);

  /* Returns true if oam needs to be updated */
  bool animate();

  void setVisible(bool visible);
  void moveAbs(int x, int y);
  void moveRel(int x, int y);
  int x, y;

 private:
  int setInitialAnimation();
  void setOAM();

  SpriteDefinitionInternal *spriteDefinition;
  ImageCache *imageCache;
  Rage::Screen screen;
  int spriteIndex;

  bool visible;

  Rage::Animation *currAnim;
  u16 **animationFrames;
  u16 currentAnimation;
  u16 currentFrame;
  u16 timeSpentInFrame;
  bool stopAnimation;

  int currentFrameIndex;

  bool emptyFrame; /* Current frame is empty, hide sprite */
};

#endif
