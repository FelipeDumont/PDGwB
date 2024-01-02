#!/bin/bash


clear

# Directly from c++ or in python to visualice it
if [ "$1" = "0" ]; then
	echo "Just Build and Excecute"
	g++ -fPIC -O3 main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp -o pdgwb
	# Call example
	# bash build.sh 0 1010 6 1 1 1.8 1 false no 100 10 0.025 0.025 0.9 0.5 0.025 0.025 0.9 false
	# bash build.sh 0 1010 6 1 1 1.8 1 true no 100 10 0.025 0.025 0.9 0.5 0.025 0.025 0.9 false
	time ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19}
	#    ./pdgwb Seed nV nK nL Lcoef neededL Testing FileName gen popS mf1 mf2 cf se mbk1 mbk2 cbk
	#           |_________________________| |______________| |__________________________________|
	#                      6 variables	  2 variables	    9 Fixed   
	
	# valgrind --leak-check=full ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}
elif [ "$1" = "1" ]; then
	echo "Just Build and Excecute"
	g++ -fPIC -O3 main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp -o pdgwb
	# Call example
	# bash build.sh 0 1010 6 1 1 1.8 1 false no 100 10 0.025 0.025 0.9 0.5 0.025 0.025 0.9
	# bash build.sh 0 1010 6 1 1 1.8 1 true no 100 10 0.025 0.025 0.9 0.5 0.025 0.025 0.9
	valgrind --leak-check=full  ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19}
	#    			    ./pdgwb Seed nV nK nL Lcoef neededL Testing FileName gen popS mf1 mf2 cf se mbk1 mbk2 cbk
	#                            	   |_________________________| |______________| |__________________________________|
	#                               	6 variables	  	2 variables	    9 Fixed  
	
	# valgrind --leak-check=full ./pdgwb $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13}

else
	echo "Build pdgwbSO run in python (colab hates .so for some reason)"
	g++ -fPIC -O3 -shared main.cpp GA.cpp Dungeon.cpp AStar.cpp Constants.cpp Room.cpp -o pdgwbSO
	# Call example
	# bash build.sh python 1010 6 1 1 1.8 1 false no 262 34 0.8528 0.674 0.7703 0.6575 0.4407 0.0457 0.0088 false
	# bash build.sh python 1010 6 1 1 1.8 1 true no 262 34 0.8528 0.674 0.7703 0.6575 0.4407 0.0457 0.0088 false
	time python3 TestDungeons.py $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19}
	#    python3 TestDungeons.py Seed nV nK nL Lcoef neededL Testing FileName gen popS mf1 mf2 cf se mbk1 mbk2 cbk  DisplayDungeon
	#                           |_________________________| |______________| |__________________________________|	true/false
	#                           	6 variables	  	2 variables	    9 Fixed  				LAST

fi


