#!/bin/sh

CFLAGS="-O3 -std=c18 -Wall -Werror -Wextra -pedantic -lm"
RAYLIB="-lraylib -lglfw -lGL -lm -lpthread -ldl -lrt -lX11"

gcc  $CFLAGS -o pong pong.c $RAYLIB
