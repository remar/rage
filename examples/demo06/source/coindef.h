#include "coin.h"

#define COIN_SPRITE 3

Rage::SpriteDefinition coinDef;
coinDef.version = 2;
coinDef.spriteID = COIN_SPRITE;
coinDef.animationCount = 1;

Rage::Animation coinAnimations[1];

coinDef.animations = coinAnimations;

// Coin image
coinAnimations[0].image.gfx = coinTiles;
coinAnimations[0].image.gfxLen = coinTilesLen;
coinAnimations[0].image.pal = coinPal;
coinAnimations[0].image.palLen = coinPalLen;

// Blink animation
Rage::Frame blink[2];
blink[0].index = 0; blink[0].duration = 20;
blink[1].index = 1; blink[1].duration = 20;

coinAnimations[0].size = SpriteSize_8x16;
coinAnimations[0].looping = true;
coinAnimations[0].frameCount = 2;
coinAnimations[0].frames = blink;
