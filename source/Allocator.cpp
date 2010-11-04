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

#include "Allocator.h"
#include <stdio.h>

Allocator::Allocator()
{
  addFreeBlock(Rage::MAIN, Rage::SPRITE, 0, SPRITE_BLOCKS);
  addFreeBlock(Rage::SUB,  Rage::SPRITE, 0, SPRITE_BLOCKS);
}

void
Allocator::init(Rage::BGMapMemSize mainBGSize, Rage::BGMapMemSize subBGSize)
{
  addFreeBlock(Rage::MAIN, Rage::BG,     0, BACKGROUND_BLOCKS - mainBGSize * MAP_BLOCKS);
  addFreeBlock(Rage::SUB,  Rage::BG,     0, BACKGROUND_BLOCKS - subBGSize * MAP_BLOCKS);

  addFreeBlock(Rage::MAIN, Rage::MAP, 0,
	       mainBGSize == Rage::BG_MAPMEM_SIZE_16K ? MAP_CHUNKS_16K : MAP_CHUNKS_32K);
  addFreeBlock(Rage::SUB, Rage::MAP, 0,
	       subBGSize == Rage::BG_MAPMEM_SIZE_16K ? MAP_CHUNKS_16K : MAP_CHUNKS_32K);
}

int
Allocator::allocateVRAM(Rage::Screen s, Rage::Type t, int size)
{
  std::list<MemoryBlock>::iterator it = freeBlocks[s][t].begin();

  for(;it != freeBlocks[s][t].end();it++)
    {
      if((*it).length >= size)
	{
	  // Found free block
	  int offset = (*it).offset;
	  int length = (*it).length;

	  // Remove block from list
	  freeBlocks[s][t].erase(it);
	  
	  // Add new block of size 'length - size' and offset 'offset + size'
	  if(length - size > 0)
	    addFreeBlock(s, t, offset + size, length - size);

	  // Return offset into VRAM
	  return offset << 6; // multiply by 64
	}
    }

  return -1;
}

int
Allocator::allocateMap(Rage::Screen s, Rage::BGMapSize mapSize)
{
  std::list<MemoryBlock>::iterator it = freeBlocks[s][Rage::MAP].begin();

  // Map from map size to chunk requirement
  int memReq[4] = {1, 2, 2, 4};

  for(;it != freeBlocks[s][Rage::MAP].end();it++)
    {
      if((*it).length >= memReq[mapSize])
	{
	  // Found free block
	  int offset = (*it).offset;
	  int length = (*it).length;

	  // Remove block from list
	  freeBlocks[s][Rage::MAP].erase(it);
	  
	  // Add new block of size 'length - size' and offset 'offset + size'
	  if(length - memReq[mapSize] > 0)
	    addFreeBlock(s, Rage::MAP, offset + memReq[mapSize], length - memReq[mapSize]);

	  return offset;
	}
    }
  
  return -1;
}

void
Allocator::releaseMap(Rage::Screen s, Rage::BGMapSize mapSize, int offset)
{
  // Map from map size to chunk requirement
  int memReq[4] = {1, 2, 2, 4};

  addFreeBlock(s, Rage::MAP, offset, memReq[mapSize]);
}

void
Allocator::addFreeBlock(Rage::Screen s, Rage::Type t, int offset, int length)
{
  // locate blocks before and after the new block and merge blocks
  MemoryBlock oldBlockBefore;
  MemoryBlock oldBlockAfter;

  oldBlockBefore.offset = -1;
  oldBlockBefore.length = 0;
  oldBlockAfter.offset = -1;
  oldBlockAfter.length = 0;

  std::list<MemoryBlock>::iterator it = freeBlocks[s][t].begin();

  bool incIt; // only increase iterator if we've not removed an element

  for(;it != freeBlocks[s][t].end();)
    {
      incIt = true;

      if((*it).offset + (*it).length == offset)
	{
	  oldBlockBefore = (*it);

	  it = freeBlocks[s][t].erase(it);
	  incIt = false;
	}
      if((*it).offset == offset + length)
	{
	  oldBlockAfter = (*it);

	  it = freeBlocks[s][t].erase(it);
	  incIt = false;
	}

      if(oldBlockBefore.offset != -1 && oldBlockAfter.offset != -1)
	break; // found blocks before and after

      if(incIt)
	it++;
    }

  MemoryBlock freeBlock;
  freeBlock.offset = offset;
  freeBlock.length = length;

  if(oldBlockBefore.offset != -1)
    {
      freeBlock.offset = oldBlockBefore.offset;
      freeBlock.length += oldBlockBefore.length;
    }
  if(oldBlockAfter.offset != -1)
    {
      freeBlock.length += oldBlockAfter.length;
    }

  freeBlocks[s][t].push_front(freeBlock);
}

int
Allocator::sumFreeBlocks(Rage::Screen s, Rage::Type t)
{
  int sum = 0;

  std::list<MemoryBlock>::iterator it = freeBlocks[s][t].begin();

  for(;it != freeBlocks[s][t].end();it++)
    {
      sum += (*it).length;
    }

  return sum;
}

int
Allocator::largestFreeBlock(Rage::Screen s, Rage::Type t)
{
  int largest = 0;

  std::list<MemoryBlock>::iterator it = freeBlocks[s][t].begin();

  for(;it != freeBlocks[s][t].end();it++)
    {
      if((*it).length > largest)
	largest = (*it).length;
    }

  return largest;
}

void
Allocator::listFreeBlocks(Rage::Screen s, Rage::Type t)
{
  const char *engine_string[] = {"MAIN", "SUB "};
  const char *type_string[] = {"SPR", "BG ", "MAP"};

  printf("%s %s --------------------\n",
	 engine_string[(int)s], type_string[(int)t]);

  int totalFree = 0;

  std::list<MemoryBlock>::iterator it = freeBlocks[s][t].begin();
  for(;it != freeBlocks[s][t].end();it++)
    {
      int offset = (*it).offset;
      int length = (*it).length;
      printf("offset %d, length %d\n", offset, length);
      totalFree += length;
    }
  
  printf("total free %d, %d kB\n", totalFree, t == Rage::MAP ? totalFree * 2 : totalFree / 16);
}
