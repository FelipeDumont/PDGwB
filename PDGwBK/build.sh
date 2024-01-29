#!/bin/bash


clear

# Directly from c++ or in python to visualice it
if [ "$1" = "0" ]; then
	echo "Just Build and Excecute"
	cd 2StepEA
	g++ -fPIC -O3 main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp -o pdgwb
	cd ..
	# Call example
	# bash build.sh 0 1010 15 3 2 2.0 2 false no 180 100 0.50 0.495 0.722 0.734 0.5 0.700 0.300 false
	# bash build.sh 0 1010 15 3 2 2.0 2 true no 180 100 0.50 0.495 0.722 0.734 0.5 0.700 0.300 false
	time ./2StepEA/pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19}
	#    ./pdgwb Seed nV nK nL Lcoef neededL Testing FileName genForm genBK popS mf1 mf2 cf se mbk1 mbk2
	#           |_________________________| |______________| |__________________________________________|
	# 	           6 variables (problem) 	2 variables	    9 Fixed
	
	# valgrind --leak-check=full ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}
elif [ "$1" = "1" ]; then
	echo "Just Build and Excecute USING Valgrind"
	cd 2StepEA
	g++ -fPIC -O3 main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp -o pdgwb
	cd ..
	# Call example
	# bash build.sh 1 1010 15 3 2 2.0 2 false no 180 100 0.50 0.495 0.722 0.734 0.5 0.700 0.300 false
	valgrind --leak-check=full -s ./2StepEA/pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19}
	#    						./pdgwb Seed nV nK nL Lcoef neededL Testing FileName genForm genBK popS mf1 mf2 cf se mbk1 mbk2
	#    						       |_________________________| |______________| |__________________________________________|
	#	          				           6 variables (problem) 	2 variables	    9 Fixed
	
	# valgrind --leak-check=full ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}

elif [ "$1" = "2" ]; then
	echo "Build pdgwbSO run in python"
	cd 2StepEA
	g++ -fPIC -O3 -shared main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp -o pdgwbSO
	cd ..
	# Call example
	# bash build.sh 2 1010 15 3 2 2.0 2 false no 180 100 0.50 0.495 0.722 0.734 0.5 0.700 0.300 false
	# bash build.sh 2 1010 15 3 2 2.0 2 false no 180 100 0.50 0.495 0.722 0.734 0.5 0.700 0.300 true
	time python3 2StepEA/TestDungeons.py $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19}
	
	#    python3 TestDungeons.py Seed nV nK nL Lcoef neededL Testing FileName genForm genBK popS mf1 mf2 cf se mbk1 mbk2  DisplayDungeon
	#           				|_________________________| |______________| |__________________________________________| true/false
	#          				           6 variables (problem) 	2 variables	    9 Fixed

elif [ "$1" = "Analisis" ]; then
	# Call Example
	# bash build.sh Analisis 1011 15 3 2 2.0 2 false Analisis 134 100 49 0.3522 0.3062 0.3295 0.2974 0.700 0.3000 true NewResults
	# The problem settings will be ignored, so you can use "Almost" the same line to run trh eproyect
	
	# If there are enough arguments add all the results to that folder
	endingFolder=""
	if [ "$#" -gt 19 ]; then
		endingFolder="${20}/"
		mkdir ${20}
	fi

	# Just to check, we are doing a linear test, no multiple instances
	date +"%D %T" 
	# Used folders to place the analisis
	folders=("Analisis_GASE" "Analisis_GANOSE")
	defaultFolder="Analisis"
	
	# Number of analisis for each problem
	N=2
	
	# To understand each problem
	# #Room, #Keys, #Barriers, LCoeficient, #NeededLocks (<=Locks)
	problems=("15 3 2 2.0 2"  "20 4 4 1.0 4" "20 4 4 2.0 4" "25 8 8 1.0 8" "30 4 4 2.0 4" "30 6 6 1.5 6" "100 20 20 1.5 20" "500 100 100 1.5 100")
	# problems=("20 4 4 1.0 4" "20 4 4 1.5 4" "20 4 4 2.0 4" "20 4 4 2.5 4" "20 4 4 3.0 4")


	problems_length=${#problems[@]}
	folders_length=${#folders[@]}
	
	
	# First remove the folders if they exist
	echo "Removing old folders"
	if [ -d $defaultFolder ]; then
		rm -rf "$endingFolder$defaultFolder"
	fi

	for item in "${folders[@]}"; do
		if [ -d $item ]; then
			rm -rf $endingFolder$item
		fi
	done
	

	# Build the program
	echo "Build current proyect"
	cd 2StepEA
	g++ -fPIC -O3 -shared main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp -o pdgwbSO
	cd ..
	
	# Build original One
	cd Original
	g++ -fPIC -O3 -shared main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp RoomFactory.cpp DFS.cpp -o pdgwbSO
	cd ..
	
	
	# Do the analisis on the current version of the problem
	currentFolder=$endingFolder"Analisis_GASE"
	mkdir $currentFolder
	
	# Run all the versions of the problem (indicated on the "0" value)
	for ((p = 0; p < problems_length; p++)); do
		problem="${problems[p]}"
		echo "Current Problem [" $problem "] " $currentFolder
		# echo "python3 Analisis.py $2 $problem $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19}"
		python3 Analisis.py $2 $problem $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19} 0 $currentFolder $N ${20} ${21} ${22}
	done
	
	# Do the analisis on the originaal version of the problem without the DFS (indicated on the "3" value)
	currentFolder="Analisis_GAOR_NODFS"
	# Create the folder for the current analisis
	mkdir $currentFolder

	for ((p = 0; p < problems_length; p++)); do
		problem="${problems[p]}"
		echo "Current Problem [" $problem "] " $currentFolder
		# echo "python3 Analisis.py $2 $problem $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19}"
		python3 Analisis.py $2 $problem $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19} 3 $currentFolder $N ${20} ${21} ${22}
	done
	
	date +"%D %T" 
else
	echo "There's no action assigned to "$1 "Input"
fi
