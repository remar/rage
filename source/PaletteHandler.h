#ifndef PALETTE_HANDLER_H
#define PALETTE_HANDLER_H

#include "rage.h"

class PaletteHandler
{
 public:
  PaletteHandler();
  int mergePalette(Rage::Screen s, Rage::Type type,
		   Rage::ImageDefinition *imageDefinition,
		   u16 **transTable, int *transTableLen);

 private:
  u16 palette[2][2][256];
  int paletteCount[2][2][256];
};

#endif
