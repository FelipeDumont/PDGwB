#!/bin/bash


clear


# Compile with the main file
# Executable file and then
# .so Compilation (shared library)
g++ -fPIC -O3 GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp RoomFactory.cpp -o example 

time ./example

