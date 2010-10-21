#ifndef MAP_ALLOCATOR_H
#define MAP_ALLOCATOR_H

#include "rage.h"

class MapAllocator
{
 public:
  MapAllocator();
  void initialize(Rage::Screen screen, Rage::BGMapMemSize mapMemSize);
  int allocateMap(Rage::Screen screen, Rage::BGMapSize mapSize);
};

#endif
