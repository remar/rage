#include "PaletteHandler.h"

u16 mergedPal[256];

// Translation table, from trans[i] to i
u16 trans[256];

PaletteHandler::PaletteHandler()
{
  for(int i = 0;i < 256;i++)
    {
      palette[Rage::MAIN][Rage::SPRITE][i] = 0;
      palette[Rage::SUB][Rage::SPRITE][i] = 0;
      palette[Rage::MAIN][Rage::BG][i] = 0;
      palette[Rage::SUB][Rage::BG][i] = 0;
      paletteCount[Rage::MAIN][Rage::SPRITE][i] = 0;
      paletteCount[Rage::SUB][Rage::SPRITE][i] = 0;
      paletteCount[Rage::MAIN][Rage::BG][i] = 0;
      paletteCount[Rage::SUB][Rage::BG][i] = 0;
    }
}

struct MergeEntry
{
  u16 color;
  u16 originalIndex;
};

int
PaletteHandler::mergePalette(Rage::Screen s, Rage::Type type,
			     Rage::ImageDefinition *imageDefinition,
			     u16 **transTable, int *transTableLen)
{
  // TODO! Uh oh, can't merge the same image several times (i.e. on
  // both screens) because the image has been modified!! Maybe save
  // modified palette here? Save it in the form (index, color) to make
  // it possible to remerge palette. OR don't modify the image, copy
  // it through an intermediate buffer that is modified
  // instead. Should work for tiles at least. Might be too slow for
  // animations that change often.

  // Thoughts:
  // palettes is a map from 'pointer to ImageDefinition' to 'modified palette'
  // palettes[imageDefinition] = <the modified palette>
  // if(palettes[imagesDefinition]) ...

  // If first entry in existing palettes is empty, just copy the
  // images palette into the empty palette.
  if(paletteCount[s][type][0] == 0)
    {
      *transTableLen = 0;

      for(int i = 0;i < 256;i++)
	{
	  // Locate the end of the palette. Color 0 (all black)
	  // indicates the end if we're at index 2 or later.
	  if(imageDefinition->pal[i] == 0 && i > 1)
	    return 1;

	  palette[s][type][i] = imageDefinition->pal[i];
	  paletteCount[s][type][i]++;
	}
    }


  bool paletteEntryMatched[256];

  for(int i = 0;i < 256;i++)
    paletteEntryMatched[i] = false;

  u16 *pal = palette[s][type];
  int *palCount = paletteCount[s][type];

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
	if(palCount[i] > 0 && pal[i] == imageDefinition->pal[j])
	  {
	    palCount[i]++;

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

      if(palCount[i] == 0)
	{
	  pal[i] = paletteToMerge[j].color;
	  palCount[i]++;

	  trans[paletteToMerge[j].originalIndex] = i;

	  j++;
	}
      else if(i == 255)
	{
	  // TODO: Palette full! Warn user somehow.
	  return 0;
	}
    }

  // Modify image to point to the merged palette

  // // TODO: This will be performed by the user of this method instead.
  // u8 *data = (u8*)imageDefinition->gfx;
  // for(int i = 0;i < imageDefinition->gfxLen;i++)
  //   {
  //     data[i] = trans[data[i]];
  //   }

  *transTable = trans;
  *transTableLen = palEntries;

  if(s == Rage::MAIN && type == Rage::BG)
    dmaCopy(pal, BG_PALETTE, 512);
  if(s == Rage::SUB && type == Rage::BG)
    dmaCopy(pal, BG_PALETTE_SUB, 512);

  return 1;
}
