#include "Allocator.h"
#include <stdio.h>

Allocator::Allocator()
{
  addFreeBlock(Rage::MAIN, Rage::SPRITE, 0, SPRITE_BLOCKS);
  addFreeBlock(Rage::SUB,  Rage::SPRITE, 0, SPRITE_BLOCKS);
  addFreeBlock(Rage::MAIN, Rage::BG,     0, BACKGROUND_BLOCKS);
  addFreeBlock(Rage::SUB,  Rage::BG,     0, BACKGROUND_BLOCKS);  
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
  const char *type_string[] = {"SPR", "BG "};

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
  
  printf("total free %d, %d kB\n", totalFree, totalFree * 64 / 1024);
}
