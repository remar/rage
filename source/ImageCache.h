#ifndef IMAGE_CACHE_H
#define IMAGE_CACHE_H

#include <nds.h>
#include "rage.h"
#include "Allocator.h"

class ImageCache
{
 public:
  ImageCache(Allocator *allocator);
  u16 **get(Rage::Screen s, Rage::ImageDefinition *imageDef,
	    SpriteSize frameSize);
  void unload(Rage::Screen s, Rage::ImageDefinition *imageDef);

 private:
  Allocator *allocator;
};

#endif
