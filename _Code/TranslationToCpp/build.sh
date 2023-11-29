#!/bin/bash


clear


# Compile with the main file
# Executable file and then
# .so Compilation (shared library)


if [ "$1" = "0" ]; then
	echo "Just Build"
	g++ -fPIC -O3 main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp RoomFactory.cpp -o pdgwb 
elif [ "$1" = "1" ]; then
	echo "Just Build and Excecute"
	g++ -fPIC -O3 main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp RoomFactory.cpp DFS.cpp -o pdgwb
	# Arguments =  Gen Deph pop_size nV nK nL lCoef seed testingMode testingBasics useRandomSequence fileName
	# bash build.sh 1 10 5 4 5 1 1 1.2 10 false false false RandomSequence.txt
	# bash build.sh 1 10 5 4 5 1 1 1.2 10 false false false RandomSequence.txt
	# valgrind --leak-check=full ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}
	time ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}
	# valgrind --leak-check=full ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}
elif [ "$1" = "2" ]; then
	echo "Just Run"
	time ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}
	# valgrind --leak-check=full ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}
elif [ "$1" = "Test" ]; then
	echo "TESTING ALL INSTANCES !!! First Build!"
	g++ -fPIC -O3 main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp RoomFactory.cpp DFS.cpp -o pdgwb
	echo "Test 1: ./pdgwb 200 8 8 8 2 2 1.2 10 true false true RandomSequence_200_8_8_8_2_2_1,2.txt"
	time ./pdgwb 200 8 8 8 2 2 1.2 10 true false true RandomSequence_200_8_8_8_2_2_1,2.txt
	echo "Test 2: ./pdgwb 200 10 8 12 4 4 1.3 10 true false true RandomSequence_200_10_8_12_4_4_1,3.txt"---
	time ./pdgwb 200 10 8 12 4 4 1.3 10 true false true RandomSequence_200_10_8_12_4_4_1,3.txt
	echo "Test 3: ./pdgwb 200 10 8 10 1 1 1.4 10 true false true RandomSequence_200_10_8_10_1_1_1,4.txt"
	time ./pdgwb 200 10 8 10 1 1 1.4 10 true false true RandomSequence_200_10_8_10_1_1_1,4.txt
	echo "Test 4: ./pdgwb 200 12 10 10 5 5 1.8 10 true false true RandomSequence_200_12_10_10_5_5_1,8.txt"
	time ./pdgwb 200 12 10 10 5 5 1.8 10 true false true RandomSequence_200_12_10_10_5_5_1,8.txt
	echo "Test 5: ./pdgwb 200 20 20 10 1 1 1.4 10 true false true RandomSequence_200_20_20_10_1_1_1,4.txt"
	time ./pdgwb 200 20 20 10 1 1 1.4 10 true false true RandomSequence_200_20_20_10_1_1_1,4.txt
	
else
	echo "Just Build DLL and SO"
	g++ -fPIC -O3 -shared main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp RoomFactory.cpp DFS.cpp -o pdgwb.so
	python3 TestDungeons.py 100 10 4 6 1 1 1.5 10
	# g++ -shared -fPIC main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp RoomFactory.cpp -o pdgwb.so
	# g++ -shared -fPIC main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp RoomFactory.cpp -o pdgwb.dll
fi



