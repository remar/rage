<html>
  <head>
    <title>Remar's Abstract Graphics Engine for Nintendo DS</title>
    <link rel="stylesheet" type="text/css" href="style.css" />
  </head>
<body>

<?php include('menu.html'); ?>

<h2>Introduction</h2>

<p>Rage is an abstraction of the Nintendo DS graphics engines
implemented in C++. It is currently quite limited, but it is
intentional, to simplify the first implementation.</p>

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
select from.</p>

<h2>Future features</h2>

<ul>
</ul>

<h2>Licensing</h2>

<p>Rage is licensed with the <a
href="http://fsf.org/licenses/lgpl.html">Lesser General Public
License</a>, LGPL. In short, this license allows you to link against
Rage without making your source free. If you modify Rage, you must
provide the source for the modified Rage version.</p>

</body>
</html>
