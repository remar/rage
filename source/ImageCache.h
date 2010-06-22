#ifndef IMAGE_CACHE_H
#define IMAGE_CACHE_H

#include <nds.h>
#include "rage.h"

class ImageCache
{
 public:
  ImageCache();
  u16 **get(Rage::Screen s, Rage::ImageDefinition *imageDef,
	    SpriteSize frameSize);
  void unload(Rage::Screen s, Rage::ImageDefinition *imageDef);
};

#endif
