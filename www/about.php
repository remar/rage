<html>
  <head>
    <title>Remar's Abstract Graphics Engine for Nintendo DS</title>
    <link rel="stylesheet" type="text/css" href="style.css" />
  </head>
<body>

<?php include('menu.html'); ?>

<h2>Introduction</h2>

<p>Rage is an abstraction of the Nintendo DS graphics implemented in
C++. It is currently quite limited, but it is intentional, to simplify
the first implementation.</p>

<h2>Features</h2>

<ul>
<li>Abstract background handling</li>
<li>Animated sprites</li>
</ul>

<p>Abstract backgrounds means that you can use tiles of other
dimensions than 8x8, e.g. 16x16 or 24x8, as long as they are divisible
by 8. Rage will take care of setting the correct 8x8 tiles when you
set an abstract tile. There's currently no way to scroll the
backgrounds.</p>

<p>In Rage, you load in sprites and then create instances of these
sprites. A sprite can have several different animations that you can
select from. Defining a sprite is done by filling in a data structure
with information about the animations that it contains.</p>

<p>See the examples found in the Rage package and the <a
href="tutorial.php">tutorial</a> on this site to learn how to use
these features.</p>

<h2>Future features</h2>

<p>The first version of the graphics engine does not support the
following features, but many will be included in later versions.</p>

<ul>
<li>Automatic palette blending/merging</li>
<li>Scrolling backgrounds (vertical, horizontal, multidir)</li>
<li>Sprite rotation, scaling, and mirroring</li>
<li>Better control of the VRAM mappings</li>
<li>Being able to choose color depth (4bpp, 8bpp, 16bpp)</li>
<li>Usage of rotation and extended rotation backgrounds</li>
<li>Abstraction for scrolling "views" (large maps)</li>
<li>Different ways of reading in graphics (file system, included in code)</li>
<li>Reading some graphics format (png?)</li>
<li>Provide a console for debugging the graphics engine</li>
<li>Provide a diagnostics screen showing memory allocation/fragmentation</li>
<li>Better control of background tiles (e.g. mirroring)</li>
</ul>

<h2>Licensing</h2>

<p>Rage is licensed with the <a
href="http://gnu.org/licenses/lgpl.html">Lesser General Public
License</a>, LGPL. In short, this license allows you to link against
Rage without making your source free. If you modify Rage, you must
provide the source for the modified Rage version.</p>

</body>
</html>
