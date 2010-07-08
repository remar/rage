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

#include "rage.h"
#include "SpriteInstance.h"

SpriteInstance::SpriteInstance(SpriteDefinitionInternal *spriteDef,
			       ImageCache *imageCache, Rage::Screen screen,
			       int spriteIndex)
  : x(0), y(0), spriteDefinition(spriteDef), imageCache(imageCache),
    screen(screen), spriteIndex(spriteIndex), visible(true), emptyFrame(false)
{
  setInitialAnimation();
}

int
SpriteInstance::setAnimation(u16 animationID)
{
  if(animationID >= spriteDefinition->animationCount)
    {
      return -1;
    }

  // Check if we need to switch graphics
  if(animationID != currentAnimation
     && (spriteDefinition->animations[animationID].image.gfx
	 != spriteDefinition->animations[currentAnimation].image.gfx))
    {
      // Unload previous animation graphics from VRAM
      imageCache->unload(screen, &spriteDefinition->animations[currentAnimation].image);
      
      Rage::Animation *a = &spriteDefinition->animations[animationID];
      
      // Load in graphics in animation into VRAM
      animationFrames = imageCache->get(screen, &a->image, a->size);
      
      if(animationFrames == 0)
	{
	  // Out of VRAM
	  return 0;
	}
    }

  // Set up animation values
  currentAnimation = animationID;
  currentFrame = 0;
  timeSpentInFrame = 0;

  currAnim = &spriteDefinition->animations[currentAnimation];
  currentFrameIndex = currAnim->frames[currentFrame].index;

  stopAnimation = false;

  setOAM();

  return 1;
}

bool
SpriteInstance::animate()
{
  if(stopAnimation)
    return false;

  timeSpentInFrame++;
  if(timeSpentInFrame >= currAnim->frames[currentFrame].duration)
    {
      if(emptyFrame == true)
	{
	  /* If previous frame was empty, show sprite again */
	  emptyFrame = false;
	}

      currentFrame++;

      if(currentFrame >= currAnim->frameCount)
	{
	  if(currAnim->looping)
	    currentFrame = 0;
	  else
            {
              stopAnimation = true; // reached end of non-looping animation

              setOAM();

              return true;
            }
	}

      currentFrameIndex = currAnim->frames[currentFrame].index;

      if(currentFrameIndex == -1)
	{
	  /* Hide sprite during the frames duration */
	  emptyFrame = true;

	  /* Show a valid frame, but hide it */
	  currentFrameIndex = 0;
	}

      timeSpentInFrame = 0;

      setOAM();

      return true; // Need redraw (updating of OAM)
    }

  return false; // No redraw needed
}

void
SpriteInstance::setVisible(bool visible)
{
  this->visible = visible;

  setOAM();
}

void
SpriteInstance::moveAbs(int x, int y)
{
  this->x = x;
  this->y = y;

  setOAM();
}

void
SpriteInstance::moveRel(int x, int y)
{
  this->x += x;
  this->y += y;

  setOAM();
}

int
SpriteInstance::setInitialAnimation()
{
  currentAnimation = 0;
  Rage::Animation *a = &spriteDefinition->animations[currentAnimation];
  animationFrames = imageCache->get(screen, &a->image, a->size);

  if(animationFrames == 0)
    return 0; // OOM

  currentFrame = 0;
  timeSpentInFrame = 0;

  currAnim = &spriteDefinition->animations[currentAnimation];

  stopAnimation = false;

  return 1;
}

void
SpriteInstance::setOAM()
{
      OamState *oam;
      oam = screen == Rage::MAIN ? &oamMain : &oamSub;

      oamSet(oam, spriteIndex, x, y, 0, 0, currAnim->size,
	     SpriteColorFormat_256Color, animationFrames[currentFrameIndex],
	     -1, false, !visible || emptyFrame /* hide */,
	     false, false, false);
}
