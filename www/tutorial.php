<html>
  <head>
    <title>Remar's Abstract Graphics Engine for Nintendo DS</title>
    <link rel="stylesheet" type="text/css" href="style.css" />
  </head>
<body>

<?php include('menu.html'); ?>

<h2>Introduction</h2>

<p>
This tutorial will show you how to create a simple graphical demo
using Rage. As Rage hides the details of the Nintendo DS graphics
system, you should easily be able to get some graphics on the screen,
without learning anything about tilemap offsets or VRAM mappings.
</p>

<p>
This tutorial will step by step implement a modified version of demo04
found in the Rage distribution, from creating the graphics and grit
files to having a working .nds ready for testing. Have fun!
</p>

<h2>Assumptions</h2>

<p>
I will assume that you've already installed devkitARM and libnds, and
that you're able to compile the examples. If that's not the case, you
can have a look at Drunken Coders <a
href="http://drunkencoders.com">introduction</a> to get you started on
that.
</p>

<p>
I will also assume that you know how to program in C and/or C++ as
that will not be covered in this tutorial. If you've never programmed
before, and desperatly want to learn how to make DS programs, there
are a couple of good sources found in books and on the net. Some of
these are:
</p>

<ul>
<li>Bjarne Stroustrup - The C++ Programming Language (recommended book)</li>
<li>Some tutorial page on the net.</li>
<li>Some other tutorial page on the net.</li>
</ul>

<h2>Authors angle</h2>

<table><tr><td><img src="gfx/tux.png" /></td>
<td>

I think it's important to tell you that I run Linux (or GNU/Linux if
you're a Stallmanite (why yes, I am)), but hopefully the differences
aren't that big between Linux and whatever you're running. Rage itself
is developed in Linux, compiled in Linux, tested in Linux, and so on,
so there might be initial issues when trying to use it in Windows, but
I hope not. If any issues arise because of this, please <a
href="contact.php">contact</a> me, I might actually fix it.

</td></tr></table>

<h2>Installing Rage</h2>

<p>Installing Rage should be pretty simple, follow these easy steps:</p>

<ul>
<li>Download <a href="files/rage-1.tar.bz2">rage-1.tar.bz2</a> from this site</li>
<li>Unpack somewhere</li>
<li>Open up a shell, go into the rage-1 directory, and type 'make install'</li>
</ul>

<p>
That last step will hopefully copy rage.h into libnds/include and
librage.a into libnds/lib. If you don't want to copy anything into
libnds (because you want it to be really clean) you can add the path
to the Rage directory to the LIBDIRS line in the Makefile later on,
instead of running 'make install' here.
</p>

<h2>Starting out</h2>

<p>Ok, let's get started with setting up our project!</p>

<h3>Copy template</h3>
<p>
First, copy the arm9 template from the libnds examples (you can find
it in examples/templates/arm9). Name it something nice, like
'tutorial' or some other imaginative name.
</p>

<h3>Edit Makefile</h3>
<p>
Now, to be able to link this project against Rage, you will have to
edit the Makefile. So open up 'tutorial/Makefile' in your favourite
editor and add -lrage to the LIBS line. Make sure to add it before
-lnds9. When you're done, the LIBS line will look something like this:
</p>

<div class="code"><pre>LIBS	:= 	-lrage -lnds9</pre></div>

<p>Allrighty then!</p>

<p>
If you skipped the 'make install' step earlier, you will also want to
add the path to Rage to the LIBDIRS line, so it will look like this:
</p>

<div class="code"><pre>LIBDIRS	:=	$(LIBNDS) PATH_TO_RAGE</pre></div>

<h3>Renaming</h3>

<p>
To be able to link in Rage, which is implemented in C++, you must now
rename main.c to main.cpp. I hope I don't have to show you how to
perform a rename. 8-)
</p>

<p>
Please note that you won't have to use any features of C++ if you
don't want to when using Rage, you can just use the C subset if that's
enough for you. Ok, I know, it's not a real subset, but it's pretty
close.
</p>

<p>
You can now try to compile this empty project by going into your
'tutorial' directory and typing 'make'. Hopefully you will get a
tutorial.nds.
</p>

<h3>Graphics files</h3>

<p>
So, you want to actually display some graphics on the DS, and you
think that maybe Rage can help you? Hehe, sorry. It will just fuck up
your project and force you to learn every nitty-gritty detail of the
complicated DS hardware. HAHAHAH!
</p>

<p>Oh, sorry, I don't know what got into me, here's some graphics for
you to use:</p>

<img src="gfx/goodleft.png"> 
<img src="gfx/goodright.png">
<img src="gfx/metroid.png"> 

<p>Create a 'gfx' directory in your 'tutorial' directory, and put the
images there. The images are already in the correct graphics format
(paletted) so you won't have to edit them. If you want to replace the
graphics files with your own creations, make sure that they are
indexed (paletted) images with the correct dimensions (height and
width divisible by 8).</p>

<p>Now your 'gfx' directory should look like this:</p>

<div class="code">
<pre>
[andreas:~]$ ls tutorial/gfx
goodleft.png  goodright.png  metroid.png
</pre>
</div>

<h3>Grit files</h3>

<p>To tell <a href="http://www.coranac.com/projects/grit/">grit</a>
(the GBA Raster Image Transmogrifier) how to convert the .png files
into objects that can be linked into the code, you will have to create
a grit file for each image. Each grit file consists of a list of
arguments to the grit command that is automatically run when you type
'make' in the 'tutorial' directory.</p>

<p>So, create the file tutorial/gfx/goodleft.grit and open it in your
$EDITOR. Here's what the grit file should look like:</p>

<div class="code">
<pre>
[andreas:gfx]$ cat tutorial/gfx/goodleft.grit 
# Include palette
-p

# Include gfx data
-g

# Gfx data type
-gu16

# 8bpp
-gB8

# Exclude map data
-m!

# Metatile height (tiles are 16x16 pixels)
-Mh2

# Metatile width (tiles are 16x16 pixels)
-Mw2
</pre>
</div>

<p>This grit file includes alot of options that really are the default
(e.g. -p, -g, -m!), but I include them anyway to be explicit. The most
interesting thing to note is the metatile switches. This tells grit to
not just linearly extract 8x8 pixel tiles from goodleft.png but
instead group them in 16x16 pixel tiles. I'll show you with an
example.</p>

<p>If you don't use metatiling, the tiles will be indexed like this:</p>

<img src="gfx/notiling.png"/>

<p>But if you metatile with -Mh2 and -Mw2 (16x16 tiles), the tiles
will be indexed like this instead:</p>

<img src="gfx/tiling.png"/>

<p>So send lots of love to the maker(s) of grit for sparing you the
awful work of doing this yourself. 8-)</p>

<p>Now, to complete this part, copy goodleft.grit to goodright.grit
and metroid.grit. That's right, they all use the same format. So now
your 'gfx' dir will contain this:</p>

<div class="code">
<pre>
[andreas:~]$ ls tutorial/gfx/
goodleft.grit  goodright.grit  metroid.grit
goodleft.png   goodright.png   metroid.png
</pre>
</div>

<p>As you might have noticed, the .grit files are named the same as
the .png files. I think you get the hint.</p>

<h2>Entering code</h2>

<p>Finally it's time to start coding!</p>

<h3>Initial main.cpp</h3>

The default main.c file found in the arm9 template is not really a
clean template, so replace the contents with this:

<div class="code">
<pre>
#include &lt;nds.h&gt;

int main(void)
{

  return 0;
}
</pre>
</div>

<p>It might be a good idea to actually make this change in the arm9
template as well, so that copying the template will start you out with
a clean main.c.</p>

<h3>Adding Rage</h3>

<p>To use Rage, you have to include the Rage definitions found in
rage.h. Add this after the inclusion of nds.h:</p>

<div class="code"><pre>#include "rage.h"</pre></div>

<p>You will also have to create a Rage object that will work as the
interface to the functions in Rage. Add this after the previous line:</p>

<div class="code"><pre>Rage rage;</pre></div>

<p>This will create a Rage object in the global environment. If you
don't want it to be global, there's nothing stopping you from placing
it somewhere else, as long as you know what you're doing.</p>

<h3>Initializing Rage</h3>

<p>Now we're going to enter code in the main() function. Add this to
the top of main:</p>

<div class="code"><pre>rage.init()</pre></div>

<p>This will set up a standard video mode on the DS, and also set up
the standard VRAM mappings. (If you don't know what VRAM mappings are,
you don't have to learn anything about it, don't worry.)</p>

<h3>Setting up a background</h3>

<p>Before you can start displaying the background you will have to set
up an abstract tile map. What I mean by abstract is that the tiles in
the map can have other dimensions than 8x8 pixels, e.g. 16x16 or
24x8. To set up a 16x16 tile map in layer 0 on the main screen, type in:</p>

<div class="code"><pre>rage.setupBackground(Rage::MAIN, 0, 16, 16);</pre></div>

<p>When you set up a tile map, the width and height of tiles must be
divisible by 8, e.g. 8, 16, 24, 32 and so on.</p>

<h3>Loading background graphics</h3>

<p>Finally, actually loading some graphics into the VRAM of the DS. To
be able to load background graphics you will have to define a tile
set. You do this by filling in a simple data structure, that looks
like this in rage.h:</p>

<div class="code"><pre>
struct TileSetDefinition
  {
    int version;
    int tileSetID; 
    ImageDefinition image;
  };
</pre></div>

<p>The version attribute should be set to the current version of Rage,
1 for now. The tileSetID attribute should be set to an integer between
0 and 15 (inclusive). Yes, this means that, at the moment, you can
only load in 16 different tile sets per screen, this might change in
the future. You will use this tileSetID later on to reference this
tile set when setting tiles.</p>

<p>The image attribute is an ImageDefinition, which looks like this:</p>

<div class="code"><pre>
struct ImageDefinition
  {
    const u16 *gfx;
    u16 gfxLen;
    const u16 *pal;
    u16 palLen;
  };
</pre></div>

<p>If you've used grit before, you will see that this matches what
grit outputs in the .h file when it's done. So, fill this in with what
you find in the .h file created by grit. 8-)</p>

<p>Ok, I'll show you how to create this tile set definition. I like to
keep the main.cpp source file clean, so I'm going to put this in a
separate file and then include it. I'll call it metroiddef.h as it
defines a tile set definition called metroidDef. This is what
metroiddef.h contains:</p>

<div class="code"><pre>
#include "metroid.h" // generated by grit from gfx/metroid.png

#define METROID_TILESET 2

Rage::TileSetDefinition metroidDef;
metroidDef.version = 1;
metroidDef.tileSetID = METROID_TILESET;
metroidDef.image.gfx = metroidTiles;
metroidDef.image.gfxLen = metroidTilesLen;
metroidDef.image.pal = metroidPal;
metroidDef.image.palLen = metroidPalLen;
</pre></div>

<p>Notice that I make a #define METROID_TILESET that I then set the
tileSetID to. I will then later on use this METROID_TILESET in the
code when setting tiles. This is a good way to avoid 'magic
numbers'.</p>

<p>Now, to actually load this tile set into Rage, add this to main:</p>

<div class="code"><pre>
#include "metroiddef.h"
rage.loadTileSet(Rage::MAIN, &metroidDef);
</pre></div>

<p>I know, you might be thinking 'WTF? You shouldn't have include
files in your functions!', and I agree... but this is how I'm doing it
right now, OK? If and when I learn about some file system on the DS,
I'll create a nice file format for tile set definitions.</p>

<p>Another thing to note is that the palette for the background will
be taken from the last loaded tile set, so make sure that you use the
same palette for all your different tile set images, or at least
provide the correct palette in the last tile set image
loaded. Automatic palette handling is in the wishlist.</p>

<h3>Setting tiles</h3>

<p>Phew, still with me after the grunt work of defining a tile set?
Good! Now let's draw a nice background with our tile set! I'm going to
use a method in Rage that sets all the tiles in one go, so first
declare a u16 array of the correct dimension:</p>

<div class="code"><pre>u16 map[16*12];</pre></div>

<p>Why 16*12? Because when using 16x16 pixel tiles, you will fit 16
tiles in a row and 12 tiles in a column. So there. To calculate the
map size, you can use this simple formula:
(32/(tileWidth/8))*(24/(tileHeight/8)). Hmm, ok, this isn't the whole
truth. If your tiles doesn't fit exactly on the screen, they will be
clipped. E.g. if you use 24x24 pixel tiles, one row will contain 10
and 2/3 of a tile, so the row will be 11 tiles wide. I hope this makes
sense.</p>

<p>Let's fill the map with tiles:</p>

<div class="code"><pre>
for(int i = 0;i < 16*12;i++)
  {
    if(i/16 == 5)
      map[i] = 0;
    else if(i/16 == 6)
      map[i] = rand() % 3 + 1;
    else
      map[i] = rand() % 4;
  }
</pre></div>

<p>The different cases simply means that row 6 (index 5) should be
empty and row 7 (index 6) should definitely not be empty. All other
tiles will be random.</p>

<p>Now, to display this elegant tile map, add this to your main function:</p>

<div class="code"><pre>
rage.setMap(Rage::MAIN, 0, METROID_TILESET, map);
</pre></div>

<p>Notice that I use the METROID_TILESET that I defined in the
metroiddef.h earlier.</p>

<h3>The main loop</h3>

<p>As always when making a game, we need a main loop. Our main loop
will be very simple right now as we're simply displaying a
background. It looks like this:</p>

<div class="code"><pre>
while(1)
  {
    rage.redraw();
  }
</pre></div>

<p>This will loop forever and update the screen at a rate of 60
frames per second.</p>

<p>Ok, let's take a break here and try to compile this (it's always
good to compile now and then to catch the simple mistakes that always
creeps into code). If the compilation succeeds, you will now have a
tutorial.nds that actually does something! If you're not able to
compile, here's how my main.cpp looks like at this stage:</p>

<div class="code"><pre>
#include &lt;nds.h&gt;
#include "rage.h"

Rage rage;

int main(void)
{
  rage.init();
  rage.setupBackground(Rage::MAIN, 0, 16, 16);

#include "metroiddef.h"
  rage.loadTileSet(Rage::MAIN, &metroidDef);

  u16 map[16*12];

  for(int i = 0;i &lt; 16*12;i++)
    {
      if(i/16 == 5)
	map[i] = 0;
      else if(i/16 == 6)
	map[i] = rand() % 3 + 1;
      else
	map[i] = rand() % 4;
    }

  rage.setMap(Rage::MAIN, 0, METROID_TILESET, map);

  while(1)
    {
      rage.redraw();
    }

  return 0;
}
</pre></div>

<h3>Loading a sprite</h3>

<p>Just as with tile sets, you will have to create a sprite definition
to load in a sprite. The sprite definition struct looks like this in
rage.h:</p>

<div class="code"><pre>
struct SpriteDefinition
{
  int version;
  u16 spriteID;
  u16 animationCount;
  Animation *animations;
};
</pre></div>

<p>The version attribute should be set to the current version of Rage,
right now this is 1. The spriteID attribute should be a number between
0 and 127 (inclusive). You will use this spriteID later on when
creating sprite instances of this sprite definition. The
animationCount attribute is simply the number of animations this
sprite contains.</p>

<p>The animations attribute is a pointer to an array of type
Animation. The Animation struct looks like this:</p>

<div class="code"><pre>
struct Animation
{
  ImageDefinition image;
  SpriteSize size;
  bool looping;
  u16 frameCount;
  Frame *frames;
};
</pre></div>

<p>Each animation has an image associated with it, fill in the
image attribute with the names found in the .h file produced by
grit.</p>

<p>The size attribute indicates the size of each frame of animation,
and uses the SpriteSize enum found in libnds. The valid values are:
SpriteSize_8x8, SpriteSize_16x16, SpriteSize_32x32, SpriteSize_64x64,
SpriteSize_16x8, SpriteSize_32x8, SpriteSize_32x16, SpriteSize_64x32,
SpriteSize_8x16, SpriteSize_8x32, SpriteSize_16x32, and
SpriteSize_32x64.</p>

<p>The looping attribute is used to indicate if this animation should
loop or not. Quite obvious, right?</p>

<p>The frameCount should be set to the number of frames in this
animation, and the frames attribute is a pointer to an array of type
Frame. Frame is defined like this:</p>

<div class="code"><pre>
struct Frame
{
  int index;
  int duration;
};
</pre></div>

<p>The index attribute is an index into the animation image, and the
duration attribute is the number of screen updates that this frame
should be visible. So if you set duration to 3, this frame will be
visible for 3 screen updates (1/20th of a second). A useful feature
when defining a frame is to set the index to -1, this will indicate
that the frame is 'empty', which means that the sprite will be hidden
during the duration of the frame. This can be useful if you want
blinking sprites or to hide a sprite when its animation has
played.</p>

<p>Mmmmkay, now we have enough information to create our first sprite
definition. It will be a Captain Good sprite with two animations, one
for walking left and one for walking right. The sprite definition
found in demo04 actually contains animations for standing still (left
and right) and jumping (left and right), but I feel like making the
sprite definition a little shorter for this tutorial. Have a look in
'rage-1/examples/demo04/source/cpngooddef.h' if you want to see a
sprite definition with six animations.</p>

<p>Ok, let's get crackin'! As with the tile set definition, I'm going
to put the sprite definition in a separate file, this time called
'cpngooddef.h'. So create this file in the 'source' directory so we
can get started.</p>

<p>Let's start with including the files produced by grit:</p>

<div class="code"><pre>
#include "goodleft.h"
#include "goodright.h"
</pre></div>

<p>Next let's add some definitions and enums that will come in handy:</p>

<div class="code"><pre>
#define GOOD_SPRITE 0
enum {WALK_LEFT, WALK_RIGHT};
</pre></div>

<p>These constants will be used in this file, and in main.cpp as well.</p>

<p>Now, you can create the various parts of the sprite definition in
different orders, first all the frame definitions, or first the sprite
definition and fill it in further down, and so on. I'll go with the
latter. So let's declare the SpriteDefinition and fill in the basics:</p>

<div class="code"><pre>
Rage::SpriteDefinition cpngoodDef;
cpngoodDef.version = 1;
cpngoodDef.spriteID = GOOD_SPRITE;
cpngoodDef.animationCount = 2;
</pre></div>

<p>Then add the array of animations:</p>

<div class="code"><pre>
Rage::Animation cpngoodAnimations[2];
cpngoodDef.animations = cpngoodAnimations;
</pre></div>

<p>Now we're ready to define the first animation, Captain Good walking
left. It will consist of 4 frames, and index 1 of the image is used
twice. First let's declare the image for the walking left
animation:</p>

<div class="code"><pre>
Rage::ImageDefinition goodleftImage;
goodleftImage.gfx = goodleftTiles;
goodleftImage.gfxLen = goodleftTilesLen;
goodleftImage.pal = goodleftPal;
goodleftImage.palLen = goodleftPalLen;
</pre></div>

<p>And then the array of frames for this animation:</p>

<div class="code"><pre>
Rage::Frame walkleftFrames[4];
walkleftFrames[0].index = 0; walkleftFrames[0].duration = 5;
walkleftFrames[1].index = 1; walkleftFrames[1].duration = 5;
walkleftFrames[2].index = 2; walkleftFrames[2].duration = 5;
walkleftFrames[3].index = 1; walkleftFrames[3].duration = 5;
</pre></div>

<p>Let's fill in index 0 (WALK_LEFT) of the animations array with the
animation details:</p>

<div class="code"><pre>
cpngoodAnimations[WALK_LEFT].image = goodleftImage;
cpngoodAnimations[WALK_LEFT].size = SpriteSize_16x16;
cpngoodAnimations[WALK_LEFT].looping = true;
cpngoodAnimations[WALK_LEFT].frameCount = 4;
cpngoodAnimations[WALK_LEFT].frames = walkleftFrames;
</pre></div>

<p>So, that's the first animation!</p>

<p>The walk right animation is almost identical, and I could actually
have reused the walkleftFrames array, but I choose not to to be more
complete:</p>

<div class="code"><pre>
Rage::ImageDefinition goodrightImage;
goodrightImage.gfx = goodrightTiles;
goodrightImage.gfxLen = goodrightTilesLen;
goodrightImage.pal = goodrightPal;
goodrightImage.palLen = goodrightPalLen;

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
</pre></div>

<p>Pheeew, that's alot of code. But when you're making your own sprite
definitions, you can copy-paste alot of the lines and just edit a
little. For example, when assigning values to the frame array, the
lines are almost identical.</p>

<p>Back to main.cpp! To load the defined sprite into Rage, add the
following code below the call to setMap:</p>

<div class="code"><pre>
#include "cpngooddef.h"
rage.loadSprite(Rage::MAIN, &cpngoodDef);
</pre></div>

<p>The Rage::MAIN points out the screen to load the sprite definition
into. As for tile sets, this can be either Rage::MAIN or
Rage::SUB.</p>

<p>Before you can actually display Captain Good on the screen you will
have to create an instance of the sprite. This is done with a call to
createSpriteInstance:</p>

<div class="code"><pre>
int good = rage.createSpriteInstance(Rage::MAIN, GOOD_SPRITE);
</pre></div>

<p>Note that the return value is stored in the 'good' integer. This
variable is used later on when refering to this sprite instance. Let's
use this immediately! This is how you set the animation for the sprite
instance:</p>

<div class="code"><pre>
rage.selectAnimation(Rage::MAIN, good, WALK_RIGHT);
</pre></div>

<p>If you don't select animation, it will simply start the first
animation defined in the sprite. Technical note: nothing is loaded
into VRAM before an animation is started, and only the image
associated with the current animations playing are loaded into
VRAM.</p>

<p>Let's make this demo a little interesting by making Captain Good
walk back and forth over the screen. To keep track of his position and
direction, define these variables:</p>

<div class="code"><pre>
int x = 0;
int dx = 1;
</pre></div>

<p>Now let's move the sprite instance to the starting position:</p>

<div class="code"><pre>
rage.moveSpriteAbs(Rage::MAIN, good, x, 5*16);
</pre></div>

<p>Next we'll add some code to move Captain Good one pixel each
frame. He'll move in the direction of dx, simply by adding dx to
x. Each frame we also check if we've reached the edge of the screen,
in that case Captain Good will change direction. This is what the main
loop looks like after these additions:</p>

<div class="code"><pre>
while(1)
  {
    x += dx;

    rage.moveSpriteAbs(Rage::MAIN, good, x, 5*16);

    if(x == 0 || x == 255 - 16)
      {
	dx = -dx;
	rage.selectAnimation(Rage::MAIN, good, dx==1 ? WALK_RIGHT : WALK_LEFT);
      }

    rage.redraw();
  }
</pre></div>

<h3>Compiling</h3>

<p>Oh yeah, let's compile! Type 'make' in the 'tutorial' directory and
cross your fingers (or hold your thumbs if you're in Sweden). If all
goes well, the final demo will appear in your 'tutorial' dir, and look
something like this when running:</p>

<center><img src="gfx/tutorial.png"/></center>

<p>If all didn't go well, here's the complete main.cpp after the
addition of the sprite stuff, and also listings of the metroiddef.h
and cpngooddef.h files for completeness:</p>

<p><b>main.cpp</b></p>
<div class="code"><pre>
#include <nds.h>
#include "rage.h"

Rage rage;

int main(void)
{
  rage.init();
  rage.setupBackground(Rage::MAIN, 0, 16, 16);

#include "metroiddef.h"
  rage.loadTileSet(Rage::MAIN, &metroidDef);

  u16 map[16*12];

  for(int i = 0;i < 16*12;i++)
    {
      if(i/16 == 5)
	map[i] = 0;
      else if(i/16 == 6)
	map[i] = rand() % 3 + 1;
      else
	map[i] = rand() % 4;
    }

  rage.setMap(Rage::MAIN, 0, METROID_TILESET, map);

#include "cpngooddef.h"
  rage.loadSprite(Rage::MAIN, &cpngoodDef);

  int good = rage.createSpriteInstance(Rage::MAIN, GOOD_SPRITE);

  rage.selectAnimation(Rage::MAIN, good, WALK_RIGHT);

  int x = 0;
  int dx = 1;

  rage.moveSpriteAbs(Rage::MAIN, good, x, 5*16);

  while(1)
    {
      x += dx;

      rage.moveSpriteAbs(Rage::MAIN, good, x, 5*16);

      if(x == 0 || x == 255 - 16)
	{
	  dx = -dx;
	  rage.selectAnimation(Rage::MAIN, good, dx==1?WALK_RIGHT:WALK_LEFT);
	}

      rage.redraw();
    }

  return 0;
}
</pre></div>

<p><b>metroiddef.h</b></p>

<div class="code"><pre>
#include "metroid.h" // generated by grit from gfx/metroid.png

#define METROID_TILESET 2

Rage::TileSetDefinition metroidDef;
metroidDef.version = 1;
metroidDef.tileSetID = METROID_TILESET;
metroidDef.image.gfx = metroidTiles;
metroidDef.image.gfxLen = metroidTilesLen;
metroidDef.image.pal = metroidPal;
metroidDef.image.palLen = metroidPalLen;
</pre></div>

<p><b>cpngooddef.h</b></p>

<div class="code"><pre>
#include "goodleft.h"
#include "goodright.h"

#define GOOD_SPRITE 0
enum {WALK_LEFT, WALK_RIGHT};

Rage::SpriteDefinition cpngoodDef;
cpngoodDef.version = 1;
cpngoodDef.spriteID = GOOD_SPRITE;
cpngoodDef.animationCount = 2;

Rage::Animation cpngoodAnimations[2];
cpngoodDef.animations = cpngoodAnimations;


Rage::ImageDefinition goodleftImage;
goodleftImage.gfx = goodleftTiles;
goodleftImage.gfxLen = goodleftTilesLen;
goodleftImage.pal = goodleftPal;
goodleftImage.palLen = goodleftPalLen;

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


Rage::ImageDefinition goodrightImage;
goodrightImage.gfx = goodrightTiles;
goodrightImage.gfxLen = goodrightTilesLen;
goodrightImage.pal = goodrightPal;
goodrightImage.palLen = goodrightPalLen;

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
</pre></div>

<h2>Error checking</h2>

<p>Something I haven't covered so far in the tutorial is error
checking, but in a real program you should make sure to check the
return values of all calls to Rage. In general, if a method returns 0,
the call failed for some reason. To see why it failed, you can call
getErrorCode, or getErrorString to get the error as a descriptive
string (char array). There's a RAGE_FAILED macro defined in rage.h
that can be used for error checking. Use it like this:</p>

<div class="code"><pre>
if(RAGE_FAILED(rage.init()))
{
  // error handling
  switch(rage.getErrorCode())
  {
    ...
  }
}
</pre></div>

<p>In demo01 there's a macro defined that makes a check of return
value and calls an error handling function when a call fails. Here's
the code of the macro and how it's used:</p>

<div class="code"><pre>
#define TRY(x) {if(RAGE_FAILED(x)) displayError();}
TRY(rage.init());
</pre></div>

<p>The displayError function simply turns on the standard console in
libnds, displays the error message from Rage, and locks up in an
infinite loop. Not a very pretty error handling method, but useful
when developing.</p>

<h2>Keeping track of memory usage</h2>

<p>If you're having problems with running out of VRAM or sprite
instances, there are a couple of methods you can use. To get the
amount of free VRAM (in bytes) for either backgrounds or sprites, on
either the main or the sub screen, use getFreeMem. Like this:</p>

<div class="code"><pre>
int freeMem = rage.getFreeMem(Rage::MAIN, Rage::BG);
</pre></div>

<p>This will hopefully be useful while developing. If fragmentation is
an issue (loading and unloading a lot, in weird order) you can use
getLargestFreeBlock to get the largest free consecutive block of bytes
in a VRAM bank:</p>

<div class="code"><pre>
int freeMem = rage.getLargestFreeBlock(Rage::SUB, Rage::SPRITE);
</pre></div>

<p>To see how many more sprite instances you can create on a screen,
use getAvailableSprites:</p>

<div class="code"><pre>
int spritesAvailable = rage.getAvailableSprites(Rage::MAIN);
</pre></div>

<h2>Wrapping up</h2>

<p>I hope you've been able to follow along with this tutorial, and
hopefully that you see how you can use Rage to create cool games. If
you find any errors in this tutorial, or in Rage in general, be sure
to <a href="contact.php">tell me</a> about it.</p>

<p>You can read the documentation for Rage <a href="api">here</a>, and
get a listing of rage.h <a href="api/rage_8h-source.html">here</a>.</p>

<p>One last thing. Who's Captain Good? Oh, it's the main character
from <a href="http://remar.se/daniel/retrobattle.php">Retrobattle</a>
created by Daniel Remar. I've "borrowed" the sprite. 8-)</p>

<p>Peace out,<br>Andreas Remar</p>

</body>
</html>
