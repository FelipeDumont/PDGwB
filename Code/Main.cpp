#include <iostream>
#include <random>
#include <algorithm>
#include <string.h>
#include <cmath> //absolute (abs())
using namespace std;

class Individual {

public:
	// variables* 
	int* nodes;
	int* connections;
	int* keys;
	int* barriers;
	
	// variables
	Individual(){}
	
	
	Individual(int totalRooms){
		assignMemory(totalRooms);
	}
		
	void assignMemory(int totalRooms){
		// For now are the same size, later the base
		nodes = (int*)malloc(totalRooms*sizeof(int));
		connections = (int*)malloc(totalRooms*sizeof(int));
		keys = (int*)malloc(totalRooms*sizeof(int));
		// This needs check, for now the same size ...
		barriers = (int*)malloc(totalRooms*sizeof(int) * 4);
	}
	
	void freeMemory(){

		free(nodes);
		free(connections);
		free(keys);
		free(barriers);
	} 
};

class GeneticAlg {

public:
	// variables*
	
	
	// variables
	Individual baseSol;
	Individual bestChromosome;
	
	int totalRooms;
	int mapRow;
	int mapSize;
	
	int iRoom;
	int eRoom;
	int maxNumber;
	mt19937 randGen;
	
	// Assigned
	uniform_real_distribution<double> prob = uniform_real_distribution<>(0.0, 1.0);
	uniform_int_distribution<int> sides = uniform_int_distribution<>(0, 3);
	
	//Main function
	GeneticAlg(mt19937 gen, int roomNumber, int finalRoomX, int finalRoomY):
	randGen(gen), totalRooms(roomNumber){
		// First assign all the values needed
		mapRow = (roomNumber * 2) - 1;
		mapSize = mapRow * mapRow;
		iRoom = (totalRooms-1) + mapRow * (totalRooms-1);
		eRoom = ((totalRooms-1) + finalRoomX) + (mapRow) * ((totalRooms-1) + finalRoomY);
		maxNumber = numeric_limits<int>::max();
		// Finally assign the memory space
		assignMemory();
	}
	
	/// MEMORY CONTROL
	void assignMemory(){
		// For now are the same size, later the base
		// baseSol = (int*)malloc(totalRooms*sizeof(int));
		// bestChromosome = (int*)malloc(totalRooms*sizeof(int));
		baseSol = Individual(totalRooms);
		bestChromosome = Individual(totalRooms);
	}

	void freeMemory(){
		baseSol.freeMemory();
		bestChromosome.freeMemory();
		// free(baseSol);
		// free(bestChromosome);
	}	
	
	/// MAIN FUNCTION
	void evolvePop() {
		greedyDungeon();
		printChromosome(baseSol);
	}
	
	
	void greedyDungeon(){
		// Basic room positioning ( first room in the center)
		// cout << "Map Center " << iRoom << endl;
		for(int i = 0; i < totalRooms; i++){
			baseSol.nodes[i] = maxNumber;
		}
		
		baseSol.nodes[0] = iRoom;
		baseSol.nodes[totalRooms - 1] = eRoom;
		
		/*
		cout << baseSol.nodes[0] << " " <<
			baseSol.nodes[1] << " " <<
			baseSol.nodes[totalRooms -1]  << endl;
		*/
	}
	
	Individual getBest(){
		return bestChromosome;
	}
	
	// Recieves a chromosome organized from less to max and print their map interpretations
	void printChromosome(Individual indv) {
	
		int first = baseSol.nodes[0];
		int last = baseSol.nodes[totalRooms - 1];
		int map[totalRooms];
		for(int i = 0; i < totalRooms; i++){
			map[i] = baseSol.nodes[i];
		}
		sort(map, map + totalRooms);
		
        	int currentInnerIndex = 0;
        	int current = map[currentInnerIndex];
        	int currentLine = 0;
		for (int j = 0; j < mapSize; ++j)
		{
			if (j == current){
				if (j == first){
					cout << "▫ ";
				}
				else{
					if(j == last){
						cout << "⟏ ";
					}
					else{
						cout << "□ ";
					}
					
				}
				currentInnerIndex += 1;
				current = map[currentInnerIndex];
			}
			else{
				cout << "▣ ";
			}
			
			if((j + 1) / mapRow > currentLine){
				currentLine += 1;
				cout << endl;
			}
		}
		
    	}

};

GeneticAlg runGA(int seed,int rooms,int finalNodeX,int finalNodeY){
	// Seed assignation and first cout
	static mt19937  gen(seed);
	
	// Calculations
	int roomDistance = abs(finalNodeX) + abs(finalNodeY);
	
	// Minimal requirement to be able to reach the end position
	if( roomDistance + 1 > rooms) {
		rooms = roomDistance + 1;
	}

	
	// Run the Algorithm
	GeneticAlg genAlg(gen, rooms, finalNodeX, finalNodeY);
	genAlg.evolvePop();
	
	// Obtain the best solutions of all the posibles !!!
	Individual solution = genAlg.getBest();
	// cout << solution << endl;
	return genAlg;
} 

extern "C"
void ga(double *warray) {
	// Variable Assignation
	// Considering initial node (0,0)
	int seed = int(warray[0]);
	int rooms = int(warray[1]);
	
	int finalRoomX = int(warray[2]);
	int finalRoomY = int(warray[3]);
	
	GeneticAlg genAlg = runGA(seed, rooms, finalRoomX, finalRoomY);
	
	// Save data 
	
	// Free data
	genAlg.freeMemory();
	
}


int main() {
	// Variables for testing
	// Considering initial node (0,0)
	int seed = 1;
	int rooms = 5;
	
	int finalRoomX = 2;
	int finalRoomY = 2;
	
	GeneticAlg genAlg = runGA(seed, rooms, finalRoomX, finalRoomY);
	
	// Save data 
	
	// Free data
	genAlg.freeMemory();
	
	return 0;
}



/*
▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ 
▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ 
▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ 
▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ 
▣ ▣ ▣ ▣ ▫ □ □ □ ⟏ 
▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ 
▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ 
▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ 
▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ 
*/
