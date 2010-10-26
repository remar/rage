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
#define MAP_BLOCKS 256

// Amount of 2k chunks available for maps
// One 32x32 map chunk needs 2k VRAM space
// A full 64x64 map requires 8k VRAM space
#define MAP_CHUNKS_16K 8
#define MAP_CHUNKS_32K 16


class Allocator
{
 public:
  Allocator();

  void init(Rage::BGMapMemSize mainBGSize, Rage::BGMapMemSize subBGSize);

  /* Returns offset into VRAM where 'size' blocks will fit. One block
     is 64 bytes (8x8 pixels at 8 bpp). Returns -1 on failure (OOM). */
  int allocateVRAM(Rage::Screen s, Rage::Type t, int size);

  /* Returns 2k offset into VRAM where map will fit. Returns -1 on
     failure (OOM). */
  int allocateMap(Rage::Screen, Rage::BGMapSize mapSize);

  void addFreeBlock(Rage::Screen s, Rage::Type t, int offset, int length);
  int sumFreeBlocks(Rage::Screen s, Rage::Type t);
  int largestFreeBlock(Rage::Screen s, Rage::Type t);
  void listFreeBlocks(Rage::Screen s, Rage::Type t);

 private:
  std::list<MemoryBlock> freeBlocks[2][3];
};

#endif
