# pong

A simple clone of Pong written in C using [raylib](https://github.com/raysan5/raylib). <br>
Currently, only Linux is supported by the included build script.

## How To Play

To start playing or restart the game, press `R`. <br>
To move the left paddle up and down, press `W` and `S`. <br>
To move the right paddle up and down, press `Up Arrow` and `Down Arrow`. <br>
To exit, press `ESC`. <br>

## Dependancies

The only dependancy is [raylib](https://github.com/raysan5/raylib). If you do not
already have it installed, follow it's 
[install instructions for your operating system](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux).

## Build and Run

Ensure you have the required dependancies installed. <br>
By default, the build scipts attempt to compile using GCC. If you wish to use another compiler,
modify the build scripts as needed.<br>
<br>

To build and run: <br>

    git clone --recursive https://github.com/aaronmcmicking/pong
    cd pong
    ./build.sh
    ./pong

