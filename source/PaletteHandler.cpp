#include "PaletteHandler.h"

u16 mergedPal[256];

PaletteHandler::PaletteHandler()
{
  PaletteEntry nil;
  nil.color = 0;
  nil.count = 0;

  for(int i = 0;i < 256;i++)
    {
      palette[Rage::MAIN][Rage::SPRITE][i] = nil;
      palette[Rage::SUB][Rage::SPRITE][i] = nil;
      palette[Rage::MAIN][Rage::BG][i] = nil;
      palette[Rage::SUB][Rage::BG][i] = nil;
    }
}

struct MergeEntry
{
  u16 color;
  u16 originalIndex;
};

int
PaletteHandler::mergePalettes(Rage::Screen s, Rage::Type type,
			      Rage::ImageDefinition *imageDefinition)
{
  // If first entry in existing palettes is empty, just copy the
  // images palette into the empty palette.
  if(palette[s][type][0].count == 0)
    {
      for(int i = 0;i < 256;i++)
	{
	  // Locate the end of the palette. Color 0 (all black)
	  // indicates the end if we're at index 2 or later.
	  if(imageDefinition->pal[i] == 0 && i > 1)
	    return 1;

	  palette[s][type][i].color = imageDefinition->pal[i];
	  palette[s][type][i].count++;
	}
    }


  // Translation table, from trans[i] to i
  u16 trans[256];

  bool paletteEntryMatched[256];

  for(int i = 0;i < 256;i++)
    paletteEntryMatched[i] = false;

  PaletteEntry *pal = palette[s][type];

  int palEntries = 0;

  u16 *entry = (u16*)imageDefinition->pal;
  for(int i = 0;i < 256;i++)
    {
      palEntries++;
      entry++;
      if(*entry == 0 && i > 1)
	break;
    }

  // Go through the provided palette to see if any of the colors is
  // already found in the existing palette.
  for(int i = 0;i < 256;i++)
    {
      for(int j = 1;j < palEntries;j++)
	if(pal[i].count > 0 && pal[i].color == imageDefinition->pal[j])
	  {
	    pal[i].count++;

	    trans[j] = i;

	    paletteEntryMatched[j] = true;
	  }
    }

  // Build palette to append to existing palette
  MergeEntry paletteToMerge[256];
  int j = 0;
  for(int i = 1;i < palEntries;i++)
    {
      if(!paletteEntryMatched[i])
	{
	  paletteToMerge[j].color = imageDefinition->pal[i];
	  paletteToMerge[j].originalIndex = i;

	  j++;
	}
    }
  paletteToMerge[j].color = 0; // null terminate

  // Merge in palette with existing palette
  j = 0;

  for(int i = 0;i < 256;i++)
    {
      if(paletteToMerge[j].color == 0)
	break;

      if(pal[i].count == 0)
	{
	  pal[i].color = paletteToMerge[j].color;
	  pal[i].count++;

	  trans[paletteToMerge[j].originalIndex] = i;

	  j++;
	}
      else if(i == 255)
	{
	  // TODO: Palette full! Warn user somehow.
	}
    }

  // Modify image to point to the merged palette
  u8 *data = (u8*)imageDefinition->gfx;

  for(int i = 0;i < imageDefinition->gfxLen;i++)
    {
      data[i] = trans[data[i]];
    }

  // Copy merged palette to hardware
  for(int i = 0;i < 256;i++)
    mergedPal[i] = pal[i].color;

  dmaCopy(mergedPal, BG_PALETTE, 512);

  return 1;
}
