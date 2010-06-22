#include "ImageCache.h"
#include <map>

struct CacheEntry
{
  int frameCount;
  u16 **frames;
  int refCount; // keep track of references to this entry
};

std::map<const u16*,CacheEntry *> cache[2];

ImageCache::ImageCache()
{

}

int getFrameWidth(SpriteSize frameSize)
{
  switch(frameSize)
    {
      case SpriteSize_8x8:
      case SpriteSize_8x16:
      case SpriteSize_8x32:
      return 8;
      break;

      case SpriteSize_16x8:
      case SpriteSize_16x16:
      case SpriteSize_16x32:
      return 16;
      break;

      case SpriteSize_32x8:
      case SpriteSize_32x16:
      case SpriteSize_32x32:
      case SpriteSize_32x64:
      return 32;
      break;

      case SpriteSize_64x32:
      case SpriteSize_64x64:
      return 64;
      break;
    }

  return 0; /* Should never happen */
}

int getFrameHeight(SpriteSize frameSize)
{
  switch(frameSize)
    {
      case SpriteSize_8x8:
      case SpriteSize_16x8:
      case SpriteSize_32x8:
      return 8;
      break;

      case SpriteSize_8x16:
      case SpriteSize_16x16:
      case SpriteSize_32x16:
      return 16;
      break;

      case SpriteSize_8x32:
      case SpriteSize_16x32:
      case SpriteSize_32x32:
      case SpriteSize_64x32:
      return 32;
      break;

      case SpriteSize_32x64:
      case SpriteSize_64x64:
      return 64;
      break;
    }

  return 0; /* Should never happen */
}

u16 **
ImageCache::get(Rage::Screen s, Rage::ImageDefinition *imageDef,
		SpriteSize frameSize)
{
  // If loaded, return loaded gfx
  CacheEntry *entry;
  if((entry = cache[s][imageDef->gfx]))
    {
      entry->refCount++;

      return entry->frames;
    }

  int size = getFrameWidth(frameSize) * getFrameHeight(frameSize);
  int frameCount = imageDef->gfxLen / size;

  u16 **frames = new u16*[frameCount];

  OamState *oam;
  oam = s == Rage::MAIN ? &oamMain : &oamSub;

  const u16 *gfx = imageDef->gfx;

  for(int i = 0;i < frameCount;i++)
    {
      frames[i] = oamAllocateGfx(oam, frameSize, SpriteColorFormat_256Color);
      dmaCopy(gfx, frames[i], size);

      gfx += size >> 1; // gfx is a u16, hence the divide by 2 (>> 1)
    }

  void *pal = s == Rage::MAIN ? SPRITE_PALETTE : SPRITE_PALETTE_SUB;

  dmaCopy(imageDef->pal, pal, imageDef->palLen);

  entry = new CacheEntry;
  entry->frameCount = frameCount;
  entry->frames = frames;
  entry->refCount = 1;

  cache[s][imageDef->gfx] = entry;

  return frames;
}

void
ImageCache::unload(Rage::Screen s, Rage::ImageDefinition *imageDef)
{
  // Decrease counter by 1; if counter is 0, unload from VRAM
  CacheEntry *entry;
  if((entry = cache[s][imageDef->gfx]))
    {
      if(--entry->refCount == 0)
	{
	  OamState *oam;
	  oam = s == Rage::MAIN ? &oamMain : &oamSub;

	  for(int i = 0;i < entry->frameCount;i++)
	    {
	      oamFreeGfx(oam, entry->frames[i]);
	    }
	  delete [] entry->frames;
	  delete entry;

	  cache[s][imageDef->gfx] = 0;
	}
    }
}
