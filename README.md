# TileLess
a map client for offline usage

This is just about the client.
Packaging the mapdata in PostGIS is a separate project: https://github.com/TilelessMap/pg_tileless 

#### What is this? ####

This project tries to target the vacuum behind the enormous evolution of online mapping the last decade.

There are situations when keeping a lot of structured map data in a device for the field is nessecary. This project tries to explore some possibilities about that.

This is is still just a proof of concept project, but it starts to show some potential.

#### What is working now? ####

The map client is tested on Debian and Android, Windows and raspberry Pi. There is some issue on the Pi making it crash after a while.

For Android I haven't yet found how to share the right parts of the Android Studio project, but that will come soon. It is basicly the Android project from libsdl.org

Some map data can be found at:
https://twkb.jordogskog.no/maps/

compiled android apk
https://twkb.jordogskog.no/downloads/




## Install and run ##

#### Dependencies ####

To compile you need:
freetype
sdl2
sdl2-image (for orthophotos)
glew

sqlite is part of the source-code for simplicity.

To install all dependencies on a clean debian system:

    apt-get install git build-essential pkg-config libfreetype6-dev libsdl2-dev libsdl2-image-dev libglew-dev libmxml-dev

#### Compile ####



    make

#### Get some test map data ####


    wget https://twkb.jordogskog.no/maps/solor.tileless

#### Run ###

    ./tileless -f solor.tileless -d ./

the -d option is to set the working directory. Since a map project can be spread over many sqlite files the working directory is where the client searches for the other data-bases.

## Some notes ##

The map data is packed in sqlite databases. Databases with project information (like layers and styles) are called .tileless as a convention. Pure map data databases are called .sqlite.

Licensce GPL v2
