#include "rage.h"
#include "SpriteInstance.h"
#include <stdio.h>

SpriteInstance::SpriteInstance(SpriteDefinitionInternal *spriteDef,
			       ImageCache *imageCache, Rage::Screen screen,
			       int spriteIndex)
  : x(0), y(0), spriteDefinition(spriteDef), imageCache(imageCache),
    screen(screen), spriteIndex(spriteIndex), visible(true)
{
  setInitialAnimation();
}

int
SpriteInstance::setAnimation(u16 animationID)
{
  if(animationID >= spriteDefinition->animationCount)
    {
      return 0;
    }

  if(animationID != currentAnimation)
    {
      // Unload previous animation graphics from VRAM
      imageCache->unload(screen, &spriteDefinition->animations[currentAnimation].image);

      Rage::Animation *a = &spriteDefinition->animations[animationID];

      // Load in graphics in animation into VRAM
      animationFrames = imageCache->get(screen, &a->image, a->size);
    }

  if(animationFrames == 0)
    {
      // Out of VRAM
      return 0;
    }

  // Set up animation values
  currentAnimation = animationID;
  currentFrame = 0;
  timeSpentInFrame = 0;

  currAnim = &spriteDefinition->animations[currentAnimation];
  currentFrameIndex = currAnim->frames[currentFrame].index;

  stopAnimation = false;

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
      currentFrame++;

      if(currentFrame >= currAnim->frameCount)
	{
	  if(currAnim->looping)
	    currentFrame = 0;
	  else
	    stopAnimation = true; // reached end of non-looping animation
	}

      currentFrameIndex = currAnim->frames[currentFrame].index;

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
	     -1, false, !visible /* hide */, false, false, false);
}
