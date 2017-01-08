# noTile
a map client for offline usage

This is a project in a very early stage

Licensce GPL v2

## To compile ##

I have only compiled it on Linux, but it should be no problem compiling at other platforms.

You will need the gcc compiler. Easiest way to get the common build tools in Debian and it's children is to install build-essential

apt-get install build-essential

Dependencies:
SDL2
glew

So on Debian system install dependencies with:

apt-get install libglew1.10 libsdl2-dev

Building:
make


## Maps to use ##

prepacked maps can be found at: http://twkb.jordogskog.no/maps

## To test things

download the data db norge.sqlite (it is approx 2.4 GB) into the root folder of this project.

Then open a terminal and run:

./noTile norge_proj.sqlite

If everything works as expected you should get a window with an inzoomed map.

You can zoom in the map by scrolling the mouse and pan by click and drag

