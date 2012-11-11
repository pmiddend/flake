flake
----------------------------------------------------------------------------

This repository contains the code accompanying my diploma thesis (in
German) for the [University of Osnabrück](http://www.uos.de) called:

> Echtzeit-Simulation von Windfeldern mit OpenCL zur Modellierung von Schneefall

which translates to

> Real-time simulation of wind fields using OpenCL for modelling falling snow

The thesis can be downloaded
[here](http://pmiddend.github.com/flake/flake.pdf).

Abstract
--------

The thesis deals with the different aspects of modelling and rendering
snow. To give snow flakes realistic trajectories, an algorithm from
the field of computational fluid dynamics is implemented in
[OpenCL](http://www.khronos.org/opencl/). This algorithm creates a
vector field containing the speed and direction of the wind at each
point in space. Using this field, the drag force is calculated for
each snowflake.

The flakes are modelled as a particle system and are rendered using
<a href="http://msdn.microsoft.com/en-us/library/windows/desktop/bb147281(v=vs.85).aspx">point sprites</a>
in OpenGL or DirectX. Their properties, such as the position and the
velocity, are updated on the graphics card (GPU) using OpenCL, taking into account
gravity, drag and other forces.

The collision of a flake with the ground or an obstacle causes an
internal counter signifying the amount of snow at the point of
collision to be increased. At regular intervals, this "snow amount
field" is converted to a snow mesh using the [Marching Cubes
algorithm](http://en.wikipedia.org/wiki/Marching_cubes) (this is not
shown in the video below). Using [triplanar
texturing](http://http.developer.nvidia.com/GPUGems3/gpugems3_ch01.html),
this mesh is given a surface that mimics a real snow cover.

A video of the result can be seen here:

<iframe width="420" height="315"
src="http://www.youtube.com/embed/aO8EhyJvtZE" frameborder="0"
allowfullscreen></iframe>

flakelib
--------

The numerical algorithms to solve for the wind field have been
generalized, so they can be included in other applications. All the
necessary classes are in the fruitlib directory (the C++ code resides
in fruitlib/src and fruitlib/include, respectively).

flake tests
-----------

To try out flake and flakelib, you can either download the pre-built
binaries
[here](http://pmiddend.github.com/flake/flake_release_107.zip) or
build it yourself using the source. Note, however, that you need to
build its underlying engine,
[sge](http://github.com/freundlich/spacegameengine) (also hosted on github), first.
