#!/usr/bin/bash

CFLAGS="-Wall -Wextra -std=c11"

gcc $CFLAGS -o snake snake.c `pkg-config --libs sdl3`
