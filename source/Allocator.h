#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "rage.h"
#include <list>

struct MemoryBlock
{
  int offset;
  int length;
};

#define BACKGROUND_BLOCKS 1792
#define SPRITE_BLOCKS 2048

class Allocator
{
 public:
  Allocator();

  /* Returns offset into VRAM where 'size' blocks will fit. One block
     is 64 bytes (8x8 pixels at 8 bpp). */
  int allocateVRAM(Rage::Screen s, Rage::Type t, int size);
  void addFreeBlock(Rage::Screen s, Rage::Type t, int offset, int length);
  int sumFreeBlocks(Rage::Screen s, Rage::Type t);
  int largestFreeBlock(Rage::Screen s, Rage::Type t);
  void listFreeBlocks(Rage::Screen s, Rage::Type t);

 private:
  std::list<MemoryBlock> freeBlocks[2][2];
};

#endif
