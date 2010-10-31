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
