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

  u16 currentFrameIndex;
};

#endif
