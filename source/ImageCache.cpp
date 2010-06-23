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

#include "ImageCache.h"
#include <map>

struct CacheEntry
{
  int *vramOffsets; // offsets into VRAM for the frames and
  int vramBlocks; // blocks in VRAM taken up per block, used by allocator

  int frameCount;
  u16 **frames;
  int refCount; // keep track of references to this entry
};

std::map<const u16*,CacheEntry *> cache[2];

ImageCache::ImageCache(Allocator *allocator)
  : allocator(allocator)
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
  int *vramOffsets = new int[frameCount];

  OamState *oam;
  oam = s == Rage::MAIN ? &oamMain : &oamSub;

  const u16 *gfx = imageDef->gfx;

  u16 *screenOffset = (s == Rage::MAIN ? (u16*)0x06400000 : (u16*)0x06600000);

  for(int i = 0;i < frameCount;i++)
    {
      vramOffsets[i] = allocator->allocateVRAM(s, Rage::SPRITE, size >> 6)>>6;
      frames[i] = (vramOffsets[i]<<6) + screenOffset;

      dmaCopy(gfx, frames[i], size);

      gfx += size >> 1; // gfx is a u16, hence the divide by 2 (>> 1)
    }

  void *pal = s == Rage::MAIN ? SPRITE_PALETTE : SPRITE_PALETTE_SUB;

  dmaCopy(imageDef->pal, pal, imageDef->palLen);

  entry = new CacheEntry;
  entry->frameCount = frameCount;
  entry->frames = frames;
  entry->refCount = 1;

  entry->vramOffsets = vramOffsets;
  entry->vramBlocks = size >> 6;

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
	      allocator->addFreeBlock(s, Rage::SPRITE, entry->vramOffsets[i],
				      entry->vramBlocks);
	    }
	  delete [] entry->frames;
	  delete [] entry->vramOffsets;
	  delete entry;

	  cache[s][imageDef->gfx] = 0;
	}
    }
}
