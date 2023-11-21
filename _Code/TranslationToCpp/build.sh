#!/bin/bash


clear


# Compile with the main file
# Executable file and then
# .so Compilation (shared library)


if [ "$1" = "0" ]; then
	echo "Just Build"
	g++ -fPIC -O3 main.cpp GA.cpp Dungeon.cpp AStar.cpp DFS.cpp Constants.cpp Room.cpp RoomFactory.cpp -o pdgwb 
elif [ "$1" = "1" ]; then
	echo "Just Build and Excecute"
	g++ -fPIC -O3 main.cpp GA.cpp Dungeon.cpp AStar.cpp DFS.cpp Constants.cpp Room.cpp RoomFactory.cpp -o pdgwb
	# Arguments =  Gen Deph pop_size nV nK nL lCoef seed testingMode testingBasics useRandomSequence fileName
	# bash build.sh 1 10 5 4 5 1 1 1.2 10 false false false RandomSequence.txt
	# bash build.sh 1 10 5 4 5 1 1 1.2 10 false false false RandomSequence.txt
	time ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}
elif [ "$1" = "2" ]; then
	echo "Just Run"
	time ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}
else
	echo "Just Build DLL and SO"
	g++ -shared -fPIC main.cpp GA.cpp Dungeon.cpp AStar.cpp DFS.cpp Constants.cpp Room.cpp RoomFactory.cpp -o pdgwb.so
	g++ -shared -fPIC main.cpp GA.cpp Dungeon.cpp AStar.cpp DFS.cpp Constants.cpp Room.cpp RoomFactory.cpp -o pdgwb.dll
fi



