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

#include "PaletteHandler.h"

// Translation table, from trans[i] to i
u16 trans[256];

PaletteHandler::PaletteHandler()
{
  for(int i = 0;i < 256;i++)
    for(int j = 0;j < 2;j++)
      for(int k = 0;k < 2;k++)
	{
	  palette[j][k][i] = 0;
	  paletteCount[j][k][i] = 0;
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
	    {
	      SavedPalette *saved = new SavedPalette();
	      saved->length = i-1;
	      saved->data = new u16[saved->length];
	      
	      for(int j = 0;j < saved->length;j++)
		{
		  saved->data[j] = j+1; // Straight copy of images
					// palette into combined palette
		}
	      savedPalettes[s][type][(short unsigned int *)imageDefinition->gfx] = saved;

	      // copy palette to hardware
	      copyPalette(s, type);

	      return 1;
	    }

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

  // Nullify translation table
  dmaFillWords(0, trans, 512);

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

  // Add 1 to index 0 (the transparent color, not added in loop
  // above).
  palCount[0]++;

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

  // "return" translation table
  *transTable = trans;
  *transTableLen = palEntries;

  // copy palette to hardware
  copyPalette(s, type);

  // Generate "saved palette", used when unmerging palette
  SavedPalette *saved = new SavedPalette();
  saved->length = palEntries - 1;
  saved->data = new u16[saved->length];

  for(int i = 0;i < saved->length;i++)
    {
      saved->data[i] = trans[i+1];
    }
  savedPalettes[s][type][(short unsigned int *)imageDefinition->gfx] = saved;

  saved = savedPalettes[s][type][(short unsigned int *)imageDefinition->gfx];

  return 1;
}

int
PaletteHandler::unmergePalette(Rage::Screen s, Rage::Type type,
			       Rage::ImageDefinition *imageDefinition)
{
  SavedPalette *saved = savedPalettes[s][type][(short unsigned int *)imageDefinition->gfx];

  if(saved == 0)
    return 0;

  paletteCount[s][type][0]--; // All images first index is
			      // transparent, so it's a special case
  for(int i = 0;i < saved->length;i++)
    {
      paletteCount[s][type][saved->data[i]]--;
    }

  savedPalettes[s][type].erase((short unsigned int *)imageDefinition->gfx);
  delete saved;

  return 1;
}

void
PaletteHandler::copyPalette(Rage::Screen s, Rage::Type type)
{
  void *palAdress[] = {SPRITE_PALETTE, SPRITE_PALETTE_SUB, BG_PALETTE, BG_PALETTE_SUB};

  DC_FlushRange(palette[s][type], 512);

  dmaCopy(palette[s][type], palAdress[s + type * 2], 512);
}
