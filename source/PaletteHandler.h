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

#ifndef PALETTE_HANDLER_H
#define PALETTE_HANDLER_H

#include "rage.h"
#include <map>

struct SavedPalette
{
  int length;
  u16 *data;
};

class PaletteHandler
{
 public:
  PaletteHandler();
  int mergePalette(Rage::Screen s, Rage::Type type,
		   Rage::ImageDefinition *imageDefinition,
		   u16 **transTable, int *transTableLen);
  int unmergePalette(Rage::Screen s, Rage::Type type,
		     Rage::ImageDefinition *imageDefinition);

 private:
  void copyPalette(Rage::Screen s, Rage::Type type);

  // [2][2] == [Rage::Screen][Rage::Type]
  u16 palette[2][2][256];
  int paletteCount[2][2][256];
  std::map<short unsigned int*, SavedPalette*> savedPalettes[2][2];
};

#endif
