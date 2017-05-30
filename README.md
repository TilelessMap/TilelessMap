# TileLess
a map client for offline usage


#### What is this? ####

This project tries to target the vacuum behind the enormous evolution of online mapping the last decade.

There are situations when keeping a lot of structured map data in a device for the field is nessecary. This project tries to explore some possibilities about that.

This is is still just a proof of concept project, but it starts to show some potential.

#### What is working now? ####

The map client is tested on Debian and Android. It compiles on windows,  with Visual Studio, but I only have that on a virtual machine, where I haven't succeeded to reach the gpu. So I haven't tried running it.

For Android I haven't yet found how to share the right parts of the Android Studio project, but that will come soon. It is basicly the Android project from libsdl.org

Some map data and android apk can be found at:
http://twkb.jordogskog.no/




## Install and run ##

#### Dependencies ####

To compile you need:
freetype
sdl2
glew

sqlite is part of the source-code for simplicity.

To install all dependencies on a clean debian system:

    apt-get install git build-essential pkg-config libfreetype6-dev libsdl2-dev libglew-dev

#### Compile ####



    make

#### Get map data ####


    wget http://twkb.jordogskog.no/boston.tileless

#### Run ###

    ./tileless -f boston.tileless -d ./

the -d option is to set the working directory. Since a map project can be spread over many sqlite files the working directory is where the client searches for the other data-bases.

## Some notes ##

The map data is packed in sqlite databases. Databases with project information (like layers and styles) are called .tileless as a convention. Pure map data dataabses are called .sqlite.

Licensce GPL v2
