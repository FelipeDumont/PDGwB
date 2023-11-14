#!/bin/bash


clear


# Compile with the main file
# Executable file and then
# .so Compilation (shared library)


if [ "$1" = "true" ]; then
	g++ -fPIC -O3 main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp RoomFactory.cpp -o pdgwb 
else
    echo "Execute !"
fi


# Arguments =  Gen Deph pop_size nV nK nL lCoef seed testingMode testingBasics useRandomSequence fileName
# time ./example 10 5 4 5 1 1 1.2 10 false false false false
# bash build.sh true 10 5 4 5 1 1 1.2 10 false false false false
# bash build.sh false 10 5 4 5 1 1 1.2 10 false false false false
time ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}
