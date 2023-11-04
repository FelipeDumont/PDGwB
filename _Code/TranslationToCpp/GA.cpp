
#include "Constants.h"
#include "Room.h"
#include "AStar.h"
#include "Dungeon.h"

#include <random>
#include <fstream>
#include <string>

int firstTest(){

	// Create a Dungeon
	Dungeon newDungeon; 	// Create the new dungeon, with the initial 0,0 Normal room ...

	// TESTING DUNGEON
	Room room1(0, 0, 0, RoomType::normal, nullptr);
	Room room2(-1, 0, -1, RoomType::key, &room1);
	Room room3(0, -1, 0, RoomType::normal, &room1);
	Room room4(-1, -1, -2, RoomType::key, &room2);
	Room room5(0, -2, 1, RoomType::locked, &room3);
	Room room6(1, -1, 0, RoomType::normal, &room3);	
	Room room7(2, -1, 0, RoomType::normal, &room6);
	Room room8(1, 0, 2, RoomType::locked, &room6);

	newDungeon.roomList.push_back(room1);
	newDungeon.roomList.push_back(room2);
	newDungeon.roomList.push_back(room3);
	newDungeon.roomList.push_back(room4);
	newDungeon.roomList.push_back(room5);
	newDungeon.roomList.push_back(room6);
	newDungeon.roomList.push_back(room7);
	newDungeon.roomList.push_back(room8);
	
	// Create full grid
	newDungeon.roomGrid.SetRoom(room1.X,room1.Y, &room1);
	newDungeon.roomGrid.SetRoom(room2.X,room2.Y, &room2);
	newDungeon.roomGrid.SetRoom(room3.X,room3.Y, &room3);
	newDungeon.roomGrid.SetRoom(room4.X,room4.Y, &room4);
	newDungeon.roomGrid.SetRoom(room5.X,room5.Y, &room5);
	newDungeon.roomGrid.SetRoom(room6.X,room6.Y, &room6);
	newDungeon.roomGrid.SetRoom(room7.X,room7.Y, &room7);
	newDungeon.roomGrid.SetRoom(room8.X,room8.Y, &room8);


	AStar a;

	// std::cout << "room pos " << dungeon.roomGrid.grid[0][0].X << "," << dungeon.roomGrid.grid[0][0].Y << std::endl;
	int route = a.FindRoute(newDungeon,Constants::MATRIXOFFSET);
	std::cout << "Needed locks " << route << std::endl;

	// CLEAN
	// newDungeon.Clean();

	return 0;
}

// to test
void ReadSequence(){
	Constants::isForced = true;
	// read file
	std::ifstream ifs("RandomSequence.txt");
    std::vector<int> intVector;
    std::string line;
	Constants::forcedRandomSequence.clear();
	
    if (ifs.is_open()) {
        while (std::getline(ifs, line)) {
            // Convert the line to an integer and save it in the vector
            int number = std::stoi(line);
			//std::cout << number << std::endl;
            Constants::forcedRandomSequence.push_back(number);
        }
        ifs.close();
    } else {
        std::cerr << "Unable to open file" << std::endl;
        Constants::isForced = false;
    }
}


std::vector<Dungeon> CreateDungeon(){
	
	float minFitness = std::numeric_limits<float>::max();
	std::vector<Dungeon> dungeons;
	for(int i = 0; i < Constants::POP_SIZE; i ++){
		Dungeon individual;
		individual.GenerateRooms();
		dungeons.push_back(individual);
	}
	// Rest of the generation ...
	
	// Display 1
	/*
	for(int i = 0; i < dungeons[1].roomList.size(); i++){
		std::cout << "[" << dungeons[1].roomList[i].X << ", " << dungeons[1].roomList[i].Y << "]" << std::endl; 
	}
	*/

	return dungeons;
}

int main() {
	// Constants constants;			// Constant and static values location
	bool test = false;
	bool useForcedRandomSequence = true;
	if(test){
		firstTest();
	}
	else{
		// Assign the internal Values !
		Constants::ID = 0;	    	// internal room ID
		Constants::MAX_DEPTH = 4;   // max tree deph
		Constants::POP_SIZE = 2;    // 2 individuals
		Constants::GENERATIONS = 0; // No real GA
		Constants::nV = 5; 			// Rooms
		Constants::nK = 1; 			// Keys
		Constants::nL = 1; 			// Barriers
		Constants::lCoef = 1.2; 		// Linearity !
		
		if(useForcedRandomSequence){
			ReadSequence();
		}
		// Create the dungeon !!!
		std::vector<Dungeon> results = CreateDungeon();
		// std::cout <<  results[1].roomList[0].roomId << "\n";
	}
	return 0;
}
