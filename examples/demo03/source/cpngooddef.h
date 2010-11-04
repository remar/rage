/*
  This is the definition for the Captain Good sprite. It consists of 6
  animations that reference 2 different images (one for the left
  animations, one for the right animations). It's mostly the 'walking'
  animations that are interesting as they consist of more than 1 frame.

  Note that spriteID is set to a #defined constant, and this constant
  is used in the implementation of class CaptainGood as well.
*/

#include "goodleft.h"
#include "goodright.h"

#include "gooddefines.h"

Rage::SpriteDefinition cpngoodDef;
cpngoodDef.version = 2;
cpngoodDef.spriteID = GOOD_SPRITE;

Rage::Animation cpngoodAnimations[6];

cpngoodDef.animationCount = 6;
cpngoodDef.animations = cpngoodAnimations;

// LEFT //////////////////////////////////////////////////////////////////////
// walk left image
Rage::ImageDefinition goodleftImage;
goodleftImage.gfx = goodleftTiles;
goodleftImage.gfxLen = goodleftTilesLen;
goodleftImage.pal = goodleftPal;
goodleftImage.palLen = goodleftPalLen;

// walk left animation
Rage::Frame walkleftFrames[4];
walkleftFrames[0].index = 0; walkleftFrames[0].duration = 5;
walkleftFrames[1].index = 1; walkleftFrames[1].duration = 5;
walkleftFrames[2].index = 2; walkleftFrames[2].duration = 5;
walkleftFrames[3].index = 1; walkleftFrames[3].duration = 5;
cpngoodAnimations[WALK_LEFT].image = goodleftImage;
cpngoodAnimations[WALK_LEFT].size = SpriteSize_16x16;
cpngoodAnimations[WALK_LEFT].looping = true;
cpngoodAnimations[WALK_LEFT].frameCount = 4;
cpngoodAnimations[WALK_LEFT].frames = walkleftFrames;

// stand left animation
Rage::Frame standleftFrames[1];
standleftFrames[0].index = 0; standleftFrames[0].duration = 100;
cpngoodAnimations[STAND_LEFT].image = goodleftImage;
cpngoodAnimations[STAND_LEFT].size = SpriteSize_16x16;
cpngoodAnimations[STAND_LEFT].looping = true;
cpngoodAnimations[STAND_LEFT].frameCount = 1;
cpngoodAnimations[STAND_LEFT].frames = standleftFrames;

// jump left animation
Rage::Frame jumpleftFrames[1];
jumpleftFrames[0].index = 2; jumpleftFrames[0].duration = 100;
cpngoodAnimations[JUMP_LEFT].image = goodleftImage;
cpngoodAnimations[JUMP_LEFT].size = SpriteSize_16x16;
cpngoodAnimations[JUMP_LEFT].looping = true;
cpngoodAnimations[JUMP_LEFT].frameCount = 1;
cpngoodAnimations[JUMP_LEFT].frames = jumpleftFrames;

// RIGHT /////////////////////////////////////////////////////////////////////
// walk right image
Rage::ImageDefinition goodrightImage;
goodrightImage.gfx = goodrightTiles;
goodrightImage.gfxLen = goodrightTilesLen;
goodrightImage.pal = goodrightPal;
goodrightImage.palLen = goodrightPalLen;

// walk right animation
Rage::Frame walkrightFrames[4];
walkrightFrames[0].index = 0; walkrightFrames[0].duration = 5;
walkrightFrames[1].index = 1; walkrightFrames[1].duration = 5;
walkrightFrames[2].index = 2; walkrightFrames[2].duration = 5;
walkrightFrames[3].index = 1; walkrightFrames[3].duration = 5;
cpngoodAnimations[WALK_RIGHT].image = goodrightImage;
cpngoodAnimations[WALK_RIGHT].size = SpriteSize_16x16;
cpngoodAnimations[WALK_RIGHT].looping = true;
cpngoodAnimations[WALK_RIGHT].frameCount = 4;
cpngoodAnimations[WALK_RIGHT].frames = walkrightFrames;

// stand right animation
Rage::Frame standrightFrames[1];
standrightFrames[0].index = 0; standrightFrames[0].duration = 100;
cpngoodAnimations[STAND_RIGHT].image = goodrightImage;
cpngoodAnimations[STAND_RIGHT].size = SpriteSize_16x16;
cpngoodAnimations[STAND_RIGHT].looping = true;
cpngoodAnimations[STAND_RIGHT].frameCount = 1;
cpngoodAnimations[STAND_RIGHT].frames = standrightFrames;

// jump right animation
Rage::Frame jumprightFrames[1];
jumprightFrames[0].index = 2; jumprightFrames[0].duration = 100;
cpngoodAnimations[JUMP_RIGHT].image = goodrightImage;
cpngoodAnimations[JUMP_RIGHT].size = SpriteSize_16x16;
cpngoodAnimations[JUMP_RIGHT].looping = true;
cpngoodAnimations[JUMP_RIGHT].frameCount = 1;
cpngoodAnimations[JUMP_RIGHT].frames = jumprightFrames;
