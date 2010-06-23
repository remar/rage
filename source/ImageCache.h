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
