#ifndef PALETTE_HANDLER_H
#define PALETTE_HANDLER_H

#include "rage.h"

struct PaletteEntry
{
  u16 color;
  int count;
};

class PaletteHandler
{
 public:
  PaletteHandler();
  int mergePalettes(Rage::Screen s, Rage::Type type,
		    Rage::ImageDefinition *imageDefinition);

 private:
  PaletteEntry palette[2][2][256];
};

#endif
