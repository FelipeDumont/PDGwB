
#include "Constants.h"
#include "Room.h"
#include "AStar.h"
#include "Dungeon.h"

#include <random>
#include <fstream>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <chrono>

enum class MutationOp { insertChild, removeLeaf };

int firstTest(){

	// Create a Dungeon
	Dungeon newDungeon; 	// Create the new dungeon, with the initial 0,0 Normal room ...

	// TESTING DUNGEON
	Room room1(0, 0, 0, 0, RoomType::normal, nullptr);
	Room room2(-1, 0, -1, 1,  RoomType::key, &room1);
	Room room3(0, -1, 0, 2, RoomType::normal, &room1);
	Room room4(-1, -1, -2, 3, RoomType::key, &room2);
	Room room5(0, -2, 1, 4, RoomType::locked, &room3);
	Room room6(1, -1, 0, 5, RoomType::normal, &room3);	
	Room room7(2, -1, 0, 6, RoomType::normal, &room6);
	Room room8(1, 0, 2, 7, RoomType::locked, &room6);

	newDungeon.roomList.push_back(&room1);
	newDungeon.roomList.push_back(&room2);
	newDungeon.roomList.push_back(&room3);
	newDungeon.roomList.push_back(&room4);
	newDungeon.roomList.push_back(&room5);
	newDungeon.roomList.push_back(&room6);
	newDungeon.roomList.push_back(&room7);
	newDungeon.roomList.push_back(&room8);
	
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
void ReadSequence(std::string fileName){
	Constants::isForced = true;
	// read file
	std::ifstream ifs(fileName);
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

void Tournament(std::vector<Dungeon*> pop, int &parent1, int &parent2) {
    std::unordered_set<int> posHash;
    std::vector<int> parentPosL;
    do {
        int pos = Constants::Next(pop.size());
        if (posHash.insert(pos).second) {
            parentPosL.push_back(pos);
        }
    } while (posHash.size() != 4);

    parent1 = pop[parentPosL[0]]->fitness < pop[parentPosL[1]]->fitness ? parentPosL[0] : parentPosL[1];
    parent2 = pop[parentPosL[2]]->fitness < pop[parentPosL[3]]->fitness ? parentPosL[2] : parentPosL[3];
}

void ChangeChildren(Room* cut1, Room* cut2) {
    Room* parent = cut1->Parent;
	std::cout << "Change Children ..." << (cut1->Parent == nullptr) << " | " << (cut2->Parent == nullptr) << std::endl;
    try {
        switch (cut1->parentDirection) {
            case Constants::Direction::right:
                (*parent).rightChild = cut2;
                break;
            case Constants::Direction::down:
                (*parent).bottomChild = cut2;
                break;
            case Constants::Direction::left:
                (*parent).leftChild = cut2;
                break;
            default:
                std::cout << "Something went wrong in crossover!." << std::endl;
                std::cout << "Direction not supported:\n\tOnly Right, Down, and Left are allowed.\n\n" << std::endl;
                break;
        }
    } catch (std::exception &e) {
        std::cout << "Something went wrong while changing the children!" << std::endl;
        throw e;
    }
	std::cout << "Finished ..." << std::endl;
}

std::vector<int>  FindNKLR(Room *root, int *nRooms, int *specialCount) {
    std::queue<Room*> toVisit;
    std::vector<int> specialRooms;
    toVisit.push(root);
    *nRooms = 0;
	*specialCount = 0;
	
    while (!toVisit.empty()) {
        (*nRooms)++;
        Room *actualRoom = toVisit.front();
        toVisit.pop();
        RoomType type = actualRoom->type;
        if (type == RoomType::key){
            specialRooms.push_back(actualRoom->keyToOpen);
			(*specialCount)++;
		}
        else if (type == RoomType::locked){
            specialRooms.push_back(-actualRoom->keyToOpen);
			(*specialCount)++;
		}
        if (actualRoom->leftChild != nullptr){
            toVisit.push(actualRoom->leftChild);
		}
        if (actualRoom->bottomChild != nullptr){
            toVisit.push(actualRoom->bottomChild);
		}
        if (actualRoom->rightChild != nullptr){
            toVisit.push(actualRoom->rightChild);
		}
    }
	return specialRooms;
}

void Mutation(Dungeon* dun) {
    try {
        // Mutate keys, adding or removing a pair
        bool willMutate = Constants::Next(101) <= Constants::MUTATION_RATE;
        MutationOp op;
        if (willMutate) {
            op = Constants::Next(101) <= Constants::MUTATION0_RATE ? MutationOp::insertChild : MutationOp::removeLeaf;
            switch (op) {
                case MutationOp::insertChild:
                    // dun->AddLockAndKey();
                    break;
                case MutationOp::removeLeaf:
                    // dun->RemoveLockAndKey();
                    break;
            }
            dun->FixRoomList();
        }

    } catch (const std::exception& e) {
        throw e;
    }
}

void Crossover(Dungeon* indOriginal1, Dungeon* indOriginal2) {
    Dungeon *ind1, *ind2;
    Room *roomCut1, *roomCut2;
    std::vector<Room*> failedRooms;
    int prob = Constants::Next(100);
	// std::cout << "1" << std::endl;
    std::vector<int> specialRooms1, specialRooms2;
    std::vector<int> newSpecial1, newSpecial2;
    int nRooms1 = 0, nRooms2 = 0;
	int nSpecial1 = 0, nSpecial2 = 0;
	int newNSpecial1 = 0, newNSpecial2 = 0;
    bool isImpossible = false;
	std::vector<Room*>::iterator it;

    if (prob < Constants::CROSSOVER_RATE) {
        do {
            ind1 = indOriginal1->Copy();
            ind2 = indOriginal2->Copy();

            int cutPosition = Constants::Next(1, ind1->roomList.size());
			// std::cout << "cutPosition " << cutPosition << std::endl;

            roomCut1 = ind1->roomList[cutPosition];
            specialRooms1 = FindNKLR(roomCut1, &nRooms1, &nSpecial1);

            std::cout << "R1 = (" << nRooms1 << " | " << nSpecial1 << ") IDS [" << cutPosition  << " | " << ind1->roomList[cutPosition]->roomId << " | " << roomCut1->roomId<< "]"<< std::endl;
			
            failedRooms.clear();

            do {
                do {
                    cutPosition = Constants::Next(1, ind2->roomList.size());
                    roomCut2 = ind2->roomList[cutPosition];
					it = std::find_if(failedRooms.begin(), failedRooms.end(),
                               [roomCut2](Room* room) { return room->Equal(roomCut2); });
					// std::cout << "3+ IDS[" << roomCut2->roomId << "|" << ind2->roomList[cutPosition]->roomId << "] | "<< (std::find(failedRooms.begin(), failedRooms.end(), roomCut2->roomId) != failedRooms.end()) << std::endl;	
                } while (it != failedRooms.end());
                failedRooms.push_back(roomCut2);
                if (failedRooms.size() == ind2->roomList.size() - 1)
                    isImpossible = true;
                specialRooms2 = FindNKLR(roomCut2, &nRooms2, &nSpecial2);
				// std::cout << "special check " << nRooms2 << " | "<< nSpecial2  << " test " << (nSpecial2 > nRooms1 ) << " || " << (nSpecial1 > nRooms2)<< std::endl;	
            } while ((nSpecial2 > nRooms1 || nSpecial1 > nRooms2) && !isImpossible);
			std::cout << "R2 = " << nRooms2 << std::endl;

            // Implement the remaining part of the logic here
            
            if (!isImpossible) {
				std::cout << "A" << std::endl;
				// Change parents to point this rooms
				ChangeChildren(roomCut1, roomCut2);
				std::cout << "A2" << std::endl;
				ChangeChildren(roomCut2, roomCut1);
				std::cout << "A2" << std::endl;
				// Now change the other way arround
                Room* auxRoom;
                auxRoom = roomCut1->Parent;
                (*roomCut1).Parent = roomCut2->Parent;
                (*roomCut2).Parent = auxRoom;

				std::cout << "B" << std::endl;
				// Remove from grid and list !
				ind1->RemoveFromGrid(roomCut1);
                ind2->RemoveFromGrid(roomCut2);

				int x = roomCut1->X;
                int y = roomCut1->Y;
                Constants::Direction dir = roomCut1->parentDirection;
				// cut 1
                int rotation = roomCut1->rotation;
                (*roomCut1).X = roomCut2->X;
                (*roomCut1).Y = roomCut2->Y;
                (*roomCut1).parentDirection = roomCut2->parentDirection;
                (*roomCut1).rotation = roomCut2->rotation;
				// cut 2
                (*roomCut2).X = x;
                (*roomCut2).Y = y;
                (*roomCut2).parentDirection = dir;
                (*roomCut2).rotation = rotation;

				std::cout << "C" << std::endl;
                ind1->RefreshGrid(roomCut2);
                ind2->RefreshGrid(roomCut1);

                newSpecial1.clear();
                newSpecial2.clear();
				
                newSpecial2 = FindNKLR(roomCut2, &nRooms2, &newNSpecial2);
                newSpecial1 = FindNKLR(roomCut1, &nRooms1, &newNSpecial1);
            }
			std::cout << "Repair? (" << (newNSpecial1 != nSpecial1) << " || " << (newNSpecial2 != nSpecial2) << " || " << (nSpecial1 > nRooms2) << " || " << ( nSpecial2 > nRooms1) << ") &&" << (!isImpossible) << std::endl;
        } while ((newNSpecial1 != nSpecial1 || newNSpecial2 != nSpecial2 || nSpecial1 > nRooms2 || nSpecial2 > nRooms1) && !isImpossible);

        if (!isImpossible) {
			// std::cout << "YAY ? "<< std::endl;
			/*
			std::cout << "Fix Branch " << std::endl;
			for(Room* r : ind1->roomList){
				std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]" << r->keyToOpen << std::endl;
			}
			std::cout << "B" << std::endl;
			for(Room* r : ind2->roomList){
				std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]"<< r->keyToOpen<< std::endl;
			}
			*/

			roomCut2->FixBranch(specialRooms1);
            roomCut1->FixBranch(specialRooms2);
            
			ind1->FixRoomList();
            ind2->FixRoomList();


			std::cout << "Final Parent A " << std::endl;
			for(Room* r : ind1->roomList){
				std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]"<< r->keyToOpen<< std::endl;
			}
			std::cout << "Final Parent B" << std::endl;
			for(Room* r : ind2->roomList){
				std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]"<< r->keyToOpen<< std::endl;
			}
			
			// Just change them when its needed :3
			// Does not matter id copy or normal it does not return the ones we want ...
            (*indOriginal1) = *ind1;
            (*indOriginal2) = *ind2;
        }
		
    }
	// Clean the current in function *
	/*
	delete ind1;
	delete ind2;
	delete roomCut1;
	delete roomCut2;
	*/
}

float Fitness(Dungeon ind, int nV, int nK, int nL, float lCoef, int matrixOffset) {
        float avgUsedRoom = 0.0f;
        AStar astar;
        float indSym = 0;
        
        if (ind.nLocks > 0) {
            ind.neededLocks = astar.FindRoute(ind, matrixOffset);
			
            if (ind.neededRooms > ind.roomList.size()) {
                std::cout << "SOMETHING IS REALLY WRONG! Nrooms: " << std::to_string(ind.roomList.size()) << "  Used: " << std::to_string(ind.neededRooms)<< std::endl;
            }
            if (ind.neededLocks > ind.nLocks) {
                std::cout << "SOMETHING IS REALLY WRONG!"<< std::endl;
            }
			// std::cout << "needed Locks ... " <<  ind.neededLocks << std::endl;
			return (2 * (std::abs(nV - ((int)ind.roomList.size())) + std::abs(nK - ind.nKeys) + std::abs(nL - ind.nLocks) + std::abs(lCoef - ind.avgChildren)) + (ind.nLocks - ind.neededLocks));
			
        } else {
            return (2 * (std::abs(nV - ((int)ind.roomList.size())) + std::abs(nK - ind.nKeys) + std::abs(nL - ind.nLocks) + std::abs(lCoef - ind.avgChildren)));
        }
		return 0;
    }

std::vector<Dungeon*> CreateDungeon(){
	
	float minFitness = std::numeric_limits<float>::max();
	std::vector<Dungeon*> dungeons;
	Dungeon* aux = nullptr;
	
	// std::cout << "initial ind "<< Constants::POP_SIZE << std::endl;
	for(int dungeonID = 0; dungeonID < Constants::POP_SIZE; dungeonID++){
		std::cout << "Create dungeon ... "<< dungeonID << std::endl;
		Dungeon* individual = new Dungeon();
		individual->GenerateRooms();
		dungeons.push_back(individual);
	}

	// Evolve !!!
	for (int gen = 0; gen < Constants::GENERATIONS; gen++)
	{
		// FITNESS CALCULATION
		
		// std::cout << "Generation: " << gen << std::endl;
		for(int dungeonID = 0; dungeonID < Constants::POP_SIZE; dungeonID++){
			float fitness = Fitness(*dungeons[dungeonID], Constants::nV, Constants::nK, Constants::nL, Constants::lCoef, Constants::MATRIXOFFSET);
			dungeons[dungeonID]->fitness = fitness;

			
			std::cout << fitness << std::endl;
			// Show dungeon with all the room information ...
			for(Room* r : dungeons[dungeonID]->roomList){
				std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]" << r->keyToOpen << std::endl;
			}
				
		}

		// ELITISM
		int auxFitness = dungeons[0]->fitness;
		int id = 0;
		for (int j = 0; j < dungeons.size(); j++)
		{
			if(auxFitness > dungeons[j]->fitness){
				auxFitness = dungeons[j]->fitness;
				id = j;
			}
		}
		
		aux = dungeons[id]->Copy();
		
		// NEXT GENERATION
		std::vector<Dungeon*> nexGeneration;
		for (int i = 0; i < Constants::POP_SIZE/2; i++)
		{
			
			int parentA = 0;
			int parentB = 1;
			
			Tournament(dungeons, parentA, parentB);
			// std::cout << parentA << " | " << parentB << std::endl;

			Dungeon* parent1 = dungeons[parentA]->Copy();
			Dungeon* parent2 = dungeons[parentB]->Copy();


			std::cout << "Pre Evolution seedID: " <<  Constants::currentValue << std::endl;
			// CROSSOVER
			// Do not return the right values !!!
			Crossover(parent1, parent2);
			
			std::cout << "Parent Result P1" << std::endl;
			/*
			
			for(Room* r : parent1->roomList){
				std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]" << r->keyToOpen << std::endl;
			}
			std::cout << "Parent Result P2" << std::endl;
			for(Room* r : parent2->roomList){
				std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]" << r->keyToOpen << std::endl;
			}
			*/
			// std::cout << "Mutation !" << std::endl;
			// MUTATION ...
			// Mutation(parent1);
			// Mutation(parent2);
			// std::cout << "END Mutation !" << std::endl;
			// currentValue
			// std::cout << "Late Evolution seedID: " <<  Constants::currentValue << std::endl;

			// std::cout << "Fix and prepare next gen !" << std::endl;
			parent1->FixRoomList();
			parent2->FixRoomList();
			
			parent1->CalcAvgChildren();
			parent2->CalcAvgChildren();

			nexGeneration.push_back(parent1);
			nexGeneration.push_back(parent2);
		}

		nexGeneration[0] = aux;
		dungeons = nexGeneration;
	}

	// Best individual
	int auxFitness = dungeons[0]->fitness;
	int id = 0;
	std::cout << "Final Generation ..." << std::endl;
	for (int j = 0; j < dungeons.size(); j++)
	{
		float fitness = Fitness(*dungeons[j], Constants::nV, Constants::nK, Constants::nL, Constants::lCoef, Constants::MATRIXOFFSET);
		dungeons[j]->fitness = fitness;
		std::cout << fitness << std::endl;
		for(Room* r : dungeons[j]->roomList){
			std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]" << ((int)r->type) << ": " << r->keyToOpen << std::endl;
		}
		if(auxFitness < dungeons[j]->fitness){
			auxFitness = dungeons[j]->fitness;
			id = j;
		}
	}
	aux = dungeons[id]->Copy();

	// Free *
	delete aux;

	// Return all the posible dungeons in testing !
	return dungeons;
}


// Minimal population = 4, for the type of tournament we are using !
int main() {
	
	bool test = false;
	bool useForcedRandomSequence = true;
	if(test){
		firstTest();
	}
	else{
		// Assign the internal Values !
		Constants::GENERATIONS = 5; // GENERATIONS >:3
		Constants::ID = 0;	    	// internal room ID
		Constants::MAX_DEPTH = 4;   // max tree deph
		Constants::POP_SIZE = 4;  // 4 individuals min !
		Constants::nV = 5; 			// Rooms
		Constants::nK = 1; 			// Keys
		Constants::nL = 1; 			// Barriers
		Constants::lCoef = 1.2; 	// Linearity !


		if(useForcedRandomSequence){
			std::string fileName = "RandomSequence.txt";
			ReadSequence(fileName);
			std::cout << "Values assigned ??? "<< Constants::forcedRandomSequence.size()<< std::endl;
		}

		//auto start = std::chrono::high_resolution_clock::now();

		// Create the dungeon !!!
		std::vector<Dungeon*> results = CreateDungeon();

		// auto stop = std::chrono::high_resolution_clock::now();
		// Calculate the duration in milliseconds
		// auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

		// Output the duration
		// std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;

		// std::cout <<  results[1].roomList[0].roomId << "\n";
	}
	return 0;
}
