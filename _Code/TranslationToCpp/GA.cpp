
#include "GA.h"

void GA::Tournament(std::vector<Dungeon*> pop, int &parent1, int &parent2) {
    std::unordered_set<int> posHash;
    std::vector<int> parentPosL;
    do {
        int pos = Constants::Next(pop.size());
        // std::cout << "Select: " << pos << std::endl;
        if (posHash.insert(pos).second) {
            parentPosL.push_back(pos);
        }
    } while (posHash.size() != 4);
    // std::cout << "Parent Selection " << pop[parentPosL[0]]->fitness << " <" <<  pop[parentPosL[1]]->fitness << std::endl;
    // std::cout << "Parent Selection " << pop[parentPosL[2]]->fitness << " <" <<  pop[parentPosL[3]]->fitness << std::endl;
    parent1 = pop[parentPosL[0]]->fitness < pop[parentPosL[1]]->fitness ? parentPosL[0] : parentPosL[1];
    parent2 = pop[parentPosL[2]]->fitness < pop[parentPosL[3]]->fitness ? parentPosL[2] : parentPosL[3];
}

void GA::Crossover(Dungeon*& indOriginal1, Dungeon*& indOriginal2) {
    Dungeon *ind1 = nullptr, *ind2 = nullptr;
    Room *roomCut1, *roomCut2;
    std::vector<Room*> failedRooms;
    int prob = Constants::Next(100);
    std::vector<int> specialRooms1, specialRooms2;
    std::vector<int> newSpecial1, newSpecial2;
    int nRooms1 = 0, nRooms2 = 0;
    int nSpecial1 = 0, nSpecial2 = 0;
    int newNSpecial1 = 0, newNSpecial2 = 0;
    bool isImpossible = false;
    bool cleanInd = false;
    std::vector<Room*>::iterator it;

    if (prob < Constants::CROSSOVER_RATE) {
        do {
            if (cleanInd) {
                delete ind1;
                delete ind2;
                ind1 = ind2 = nullptr; // Ensure pointers are null after deletion
            }

            ind1 = indOriginal1->Copy();
            ind2 = indOriginal2->Copy();
            cleanInd = true;

            int cutPosition = Constants::Next(1, ind1->roomList.size());
            roomCut1 = ind1->roomList[cutPosition];
            specialRooms1 = FindNKLR(roomCut1, &nRooms1, &nSpecial1);

            failedRooms.clear();
            do {
                do {
                    cutPosition = Constants::Next(1, ind2->roomList.size());
                    roomCut2 = ind2->roomList[cutPosition];
                    it = std::find_if(failedRooms.begin(), failedRooms.end(),
                                      [roomCut2](Room* room) { return room->Equal(roomCut2); });
                } while (it != failedRooms.end());

                failedRooms.push_back(roomCut2);
                if (failedRooms.size() == ind2->roomList.size() - 1)
                    isImpossible = true;
                specialRooms2 = FindNKLR(roomCut2, &nRooms2, &nSpecial2);
            } while ((nSpecial2 > nRooms1 || nSpecial1 > nRooms2) && !isImpossible);

            if (!isImpossible) {
                ChangeChildren(roomCut1, roomCut2);
                ChangeChildren(roomCut2, roomCut1);

                // Swap the parents of roomCut1 and roomCut2
                Room* tempParent = roomCut1->Parent;
                roomCut1->Parent = roomCut2->Parent;
                roomCut2->Parent = tempParent;

                ind1->RemoveFromGrid(roomCut1);
                ind2->RemoveFromGrid(roomCut2);

                // Swap positions and other properties
                int x = roomCut1->X;
                int y = roomCut1->Y;
                Constants::Direction dir = roomCut1->parentDirection;
                int rotation = roomCut1->rotation;

                roomCut1->X = roomCut2->X;
                roomCut1->Y = roomCut2->Y;
                roomCut1->parentDirection = roomCut2->parentDirection;
                roomCut1->rotation = roomCut2->rotation;

                roomCut2->X = x;
                roomCut2->Y = y;
                roomCut2->parentDirection = dir;
                roomCut2->rotation = rotation;

                ind1->RefreshGrid(roomCut2);
                ind2->RefreshGrid(roomCut1);

                newSpecial1.clear();
                newSpecial2.clear();

                newSpecial2 = FindNKLR(roomCut2, &nRooms2, &newNSpecial2);
                newSpecial1 = FindNKLR(roomCut1, &nRooms1, &newNSpecial1);
            }
        } while ((newNSpecial1 != nSpecial1 || newNSpecial2 != nSpecial2 || nSpecial1 > nRooms2 || nSpecial2 > nRooms1) && !isImpossible);

        if (!isImpossible) {
            roomCut2->FixBranch(specialRooms1);
            roomCut1->FixBranch(specialRooms2);

            ind1->FixRoomList();
            ind2->FixRoomList();

            delete indOriginal1;
            delete indOriginal2;

            indOriginal1 = ind1;
            indOriginal2 = ind2;
            ind1 = ind2 = nullptr; // Avoid double deletion
        }
    }
    delete ind1; // Safe deletion if not null
    delete ind2; // Safe deletion if not null
}


void GA::Mutation(Dungeon* dun) {
    try {
        // Mutate keys, adding or removing a pair
        bool willMutate = Constants::Next(101) <= Constants::MUTATION_RATE;
        MutationOp op;
        if (willMutate) {
			
            op = Constants::Next(101) <= Constants::MUTATION0_RATE ? MutationOp::insertChild : MutationOp::removeLeaf;
            switch (op) {
                case MutationOp::insertChild:
					// std::cout << "ALK" << std::endl;
                    dun->AddLockAndKey();
                    break;
                case MutationOp::removeLeaf:
					// std::cout << "RlK" << std::endl;
                    dun->RemoveLockAndKey();
                    break;
            }
            dun->FixRoomList();
        }

    } catch (const std::exception& e) {
        throw e;
    }
}


void GA::ChangeChildren(Room* cut1, Room* cut2) {
    Room* parent = cut1->Parent;
	
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
}

std::vector<int>  GA::FindNKLR(Room *root, int *nRooms, int *specialCount) {
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




float GA::Fitness(Dungeon* ind, int nV, int nK, int nL, float lCoef, int matrixOffset) {
        AStar astar;
		DFS dfs1;
        DFS dfs2;
        DFS dfs3;
        float indSym = 0;
        float fitness = 0;

        if (ind->nLocks > 0) {
			
			float avgUsedRooms = 0;
			
            ind->neededLocks = astar.FindRoute(ind, matrixOffset);
            // std::cout << "Needed locks " << ind->neededLocks << std::endl;

			// Between 3 we calculate the avg
			
			// std::cout << "dfs ... " << Constants::currentValue << std::endl;
			avgUsedRooms += dfs1.FindRoute(ind);
            
			// std::cout << "dfs1 " << avgUsedRooms  << " | " << Constants::currentValue << std::endl;
			
            avgUsedRooms += dfs2.FindRoute(ind);
			// std::cout << "dfs2 " << avgUsedRooms  << " | " << Constants::currentValue << std::endl;
			avgUsedRooms += dfs3.FindRoute(ind);
			//std::cout << "dfs3 " << avgUsedRooms  << " | " << Constants::currentValue << std::endl;
			
            avgUsedRooms = round((avgUsedRooms / 3.0f) * 10000.0)/10000.0;
			
			
            if (ind->neededRooms > ind->roomList.size()) {
                std::cout << "SOMETHING IS REALLY WRONG! Nrooms: " << std::to_string(ind->roomList.size()) << "  Used: " << std::to_string(ind->neededRooms)<< std::endl;
            }
            if (ind->neededLocks > ind->nLocks) {
                std::cout << "SOMETHING IS REALLY WRONG!"<< std::endl;
            }
			
			// std::cout << "needed Locks ... " <<  (2 * (std::abs(nV - ((int)ind->roomList.size())) + std::abs(nK - ind->nKeys) + std::abs(nL - ind->nLocks) + (std::abs(lCoef - ind->avgChildren)*5))) << std::endl;
			// std::cout << " Other " << (ind->nLocks - ind->neededLocks) + std::abs(ind->roomList.size() * 0.8 - avgUsedRooms)<< std::endl;
			
			fitness = (2 * (std::abs(nV - ((int)ind->roomList.size())) + std::abs(nK - ind->nKeys) + std::abs(nL - ind->nLocks) + (std::abs(lCoef - ind->avgChildren)*5))) ;
			fitness += (ind->nLocks - ind->neededLocks) + std::abs(ind->roomList.size() * 0.8 - avgUsedRooms);
			// std::cout << "Final Fitness: " << fitness << std::endl;
			
        } else {
			fitness = (2 * (std::abs(nV - ((int)ind->roomList.size())) + std::abs(nK - ind->nKeys) + std::abs(nL - ind->nLocks) +
					std::abs(lCoef - ind->avgChildren)));
            // std::cout << "Final Fitness NL: " << fitness << std::endl;
        }

		return round(fitness * 10000.0)/10000.0;
    }

std::vector<Dungeon*> GA::CreateDungeon() {
    std::vector<Dungeon*> dungeons;
    

    for (int dungeonID = 0; dungeonID < Constants::POP_SIZE; dungeonID++) {
        Dungeon* individual = new Dungeon();
        individual->GenerateRooms();
        dungeons.push_back(individual->Copy());
    }

    if (testingMode) {
        std::cout << "Start To evolve with first fitness !: " << Constants::currentValue << std::endl;
    }

    for (int gen = 0; gen < Constants::GENERATIONS; gen++) {
		std::vector<Dungeon*> nexGeneration;
		Dungeon* aux;
		
		
        if (testingMode) {
            // std::cout << "GEN " << gen << std::endl;
        }

        int counter = 0;
        for (Dungeon* dungeon : dungeons) {
            if (testingMode) {
                
                // std::cout << "Dungeon: " << counter << std::endl;
                for (Room* r : dungeon->roomList) {
                    // std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]" << r->keyToOpen << std::endl;
                }
            }
            
            float fitness = Fitness(dungeon, Constants::nV, Constants::nK, Constants::nL, Constants::lCoef, Constants::MATRIXOFFSET);
            dungeon->fitness = fitness;

            if(testingMode){
                // std::cout << fitness << std::endl;
            }
            counter++;
        }

		
        float auxFitness = dungeons[0]->fitness;
        int id = 0;
        for (int j = 0; j < dungeons.size(); j++) {
            // std::cout << auxFitness << ">" << dungeons[j]->fitness << ((auxFitness > dungeons[j]->fitness)? "True":"False") << std::endl;
            if (auxFitness > dungeons[j]->fitness) {
                auxFitness = dungeons[j]->fitness;
                id = j;
            }
        }
        aux = dungeons[id]->Copy();
		
		
		// std::cout << "Prepare next gen " << std::endl;
		// NEXT GENERATION Preparation !
		for (int i = 0; i < Constants::POP_SIZE/2; i++)
		{
			// Select parents
			int parentA = 0;
			int parentB = 1;
			
			Tournament(dungeons, parentA, parentB);

			Dungeon* parent1 = dungeons[parentA]->Copy();
			Dungeon* parent2 = dungeons[parentB]->Copy();

			// Evolution !!!
			if ( testingMode){
				// std::cout << "Cross: " << parentA << " | " << parentB << " :::: " << Constants::currentValue << std::endl;
			}
			
			Crossover(parent1, parent2);
            if ( testingMode){
				// std::cout << "End Crossover: " << Constants::currentValue << std::endl;
			}
			Mutation(parent1);
            if ( testingMode){
				// std::cout << "End Mutation 1: " << Constants::currentValue << std::endl;
			}
			Mutation(parent2);
			if ( testingMode){
				// std::cout << "End Mutation 2: " << Constants::currentValue << std::endl;
			}

			parent1->FixRoomList();
			parent2->FixRoomList();
			
			parent1->CalcAvgChildren();
			parent2->CalcAvgChildren();
			
			nexGeneration.push_back(parent1->Copy());
			nexGeneration.push_back(parent2->Copy());

			delete parent1;
			delete parent2;
		}

		// Clean current generation
		while (!dungeons.empty()) {
			Dungeon* dungeon = dungeons.back(); // or front() depending on your needs
			dungeons.pop_back();
			delete dungeon; // Delete the object
		}
	
		
		// Prepare the next generation
		for(Dungeon* d : nexGeneration){
			dungeons.push_back(d);
		}
		
		// Elitism
		dungeons[0]->CleanDungeon();
		delete dungeons[0];
		dungeons[0] = aux;
    }
	
    float auxFitness = dungeons[0]->fitness;
    int id = 0;
    std::cout << "FINAL DUNGEONS" << std::endl << std::endl;
    for (int j = 0; j < dungeons.size(); j++) {
        float fitness = Fitness(dungeons[j], Constants::nV, Constants::nK, Constants::nL, Constants::lCoef, Constants::MATRIXOFFSET);
        dungeons[j]->fitness = fitness;

        if (testingMode) {
            std::cout << fitness << std::endl;

            for (Room* r : dungeons[j]->roomList) {
                std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]" << ((int)r->type) << ": " << r->keyToOpen << std::endl;
            }
        }

        if (auxFitness < dungeons[j]->fitness) {
            auxFitness = dungeons[j]->fitness;
            id = j;
        }
    }
	

    return dungeons;
}


// to test
void GA::ReadSequence(std::string fileName){
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
