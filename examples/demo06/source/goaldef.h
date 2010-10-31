#include "goal.h"

#define GOAL_SPRITE 2
enum {CLOSED_GOAL, OPEN_GOAL};

Rage::SpriteDefinition goalDef;
goalDef.version = 1;
goalDef.spriteID = GOAL_SPRITE;
goalDef.animationCount = 2;

Rage::Animation goalAnimations[2];

goalDef.animations = goalAnimations;

// Goal image
Rage::ImageDefinition goalImage;
goalImage.gfx = goalTiles;
goalImage.gfxLen = goalTilesLen;
goalImage.pal = goalPal;
goalImage.palLen = goalPalLen;

// Closed goal animation
Rage::Frame closedgoal[1];
closedgoal[0].index = 0; closedgoal[0].duration = 100;

goalAnimations[CLOSED_GOAL].image = goalImage;
goalAnimations[CLOSED_GOAL].size = SpriteSize_16x16;
goalAnimations[CLOSED_GOAL].looping = false;
goalAnimations[CLOSED_GOAL].frameCount = 1;
goalAnimations[CLOSED_GOAL].frames = closedgoal;

// Open goal animation
Rage::Frame opengoal[4];
opengoal[0].index = 0; opengoal[0].duration = 10;
opengoal[1].index = 1; opengoal[1].duration = 10;
opengoal[2].index = 2; opengoal[2].duration = 10;
opengoal[3].index = 3; opengoal[3].duration = 10;

goalAnimations[OPEN_GOAL].image = goalImage;
goalAnimations[OPEN_GOAL].size = SpriteSize_16x16;
goalAnimations[OPEN_GOAL].looping = true;
goalAnimations[OPEN_GOAL].frameCount = 4;
goalAnimations[OPEN_GOAL].frames = opengoal;
