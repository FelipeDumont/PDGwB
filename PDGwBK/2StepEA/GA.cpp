
#include "GA.h"

/**********************************************************************/
/************************      Helper       ***************************/
/**********************************************************************/

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

/**********************************************************************/
/**********************      Fitness       ****************************/
/**********************************************************************/
float GA::FitnessSymmetry(Dungeon* ind, int nV, int nK, int nL, float lCoef, float sCoef, int matrixOffset, bool useKB, bool final) {
    AStar astar;
    float indSym = 0;
    float fitness = 0;
    bool displayInfo = Constants::testingMode && final;

    if(useKB){
        // Symmetry will not be updated from now on ( only Keys and Barriers)
        if (ind->nLocks > 0) {
            ind->neededLocks = astar.FindRouteAStar(ind, matrixOffset);
            fitness = (2 * (std::abs(nV - ((int)ind->roomList.size()))));
            fitness += (2 * (std::abs(nK - ind->nKeys) + std::abs(nL - ind->nLocks)));
            fitness += (std::abs(lCoef - ind->avgChildren)) * 2  * Constants::lCoefW;
            // fitness += (std::abs(sCoef - ind->sCoef)) * Constants::sCoefW;
            fitness += (ind->nLocks - ind->neededLocks);

            if(displayInfo){
                std::cout << "NV " << nV << " - " << ((int)ind->roomList.size()) << std::endl;
                std::cout << "nK " << nK << " - " << ind->nKeys << std::endl;
                std::cout << "nL " << nL << " - " << ind->nLocks << std::endl;
                std::cout << "lCoef " << lCoef << " - " << ind->avgChildren << std::endl;
                std::cout << "Needed Locks " << ind->nLocks  << " - " << ind->neededLocks << std::endl;
            }

        } else {
            fitness = (2 * (std::abs(nV - ((int)ind->roomList.size()))));
            fitness += (2 * (std::abs(nK - ind->nKeys) + std::abs(nL)));
            fitness += (std::abs(lCoef - ind->avgChildren)) * 2  * Constants::lCoefW;
            // fitness += (std::abs(sCoef - ind->sCoef)) * Constants::sCoefW;
            fitness += (ind->nLocks);
            if(displayInfo){
                std::cout << "NV " << nV << " - " << ((int)ind->roomList.size()) << std::endl;
                std::cout << "nK " << nK << " - " << ind->nKeys << std::endl;
                std::cout << "nL " << nL << " - " << ind->nLocks << std::endl;
                std::cout << "lCoef " << lCoef << " - " << ind->avgChildren << std::endl;
            }
        }
    }
    else{
        // Could be used Later !
        // ind->CalculateSymmetry();
        fitness = (2 * (std::abs(nV - ((int)ind->roomList.size()))));
        fitness += (2 * (std::abs(nK) + std::abs(nL)));
        fitness += (std::abs(lCoef - ind->avgChildren)) * 2 * Constants::lCoefW;
        // fitness += (std::abs(sCoef - ind->sCoef)) * Constants::sCoefW;
        fitness += (ind->nLocks);
        if(displayInfo){
            std::cout << "NV " << nV << " - " << ((int)ind->roomList.size()) << std::endl;
            std::cout << "lCoef " << lCoef << " - " << ind->avgChildren << std::endl;
        }
    }
    
    return fitness;
}


/**********************************************************************/
/********************      Main Evolution      ************************/
/**********************************************************************/

void GA::Tournament(std::vector<Dungeon*> pop, int &parent1, int &parent2) {
    std::vector<int> parentPosL;
    std::vector<int> result;

    for (int i = 0; i < pop.size(); i += 1) {
        result.push_back(i);
    }

    for (int i = 0; parentPosL.size() < 4; i += 1) {
        int pos = Constants::Next(result.size());
        
        parentPosL.push_back(result[pos]);
        result.erase(result.begin() + pos);
    }
    parent1 = pop[parentPosL[0]]->fitness < pop[parentPosL[1]]->fitness ? parentPosL[0] : parentPosL[1];
    parent2 = pop[parentPosL[2]]->fitness < pop[parentPosL[3]]->fitness ? parentPosL[2] : parentPosL[3];
}


Dungeon* GA::CreateDungeon() {
    
    std::vector<Dungeon*> dungeons;

    // First Generation
    for (int dungeonID = 0; dungeonID < Constants::POP_SIZE; dungeonID++) {
        Dungeon* individual = new Dungeon();
        individual->GenerateRooms();
        dungeons.push_back(individual);
    }


    // Evolve Form of the map
    dungeons = EvolveForm(dungeons);

    // Evolve Barrier and Keys of the map !!!
    // std::cout << "Forced no EvolveBK"<< std::endl;
    dungeons = EvolveBK(dungeons);
    
    if (Constants::testingMode) {
        std::cout <<  std::endl << "FINAL INDIVIDUALS "  << std::endl;
    }
    float auxFitness = dungeons[0]->fitness;
    int id = 0;
    // std::cout << "FINAL DUNGEONS" << std::endl << std::endl;
    for (int j = 0; j < dungeons.size(); j++) {
        if (Constants::testingMode) {
            std::cout << "Dungeon " << j <<  std::endl;
        }
        float fitness = 0;
        fitness = FitnessSymmetry(dungeons[j], Constants::nV, Constants::nK, Constants::nL, Constants::lCoef, Constants::FitnessSymmetry, Constants::MATRIXOFFSET, true, true);
        dungeons[j]->fitness = fitness;

        if (Constants::testingMode) {
            dungeons[j]->DisplayDungeon();
        }

        if (auxFitness > dungeons[j]->fitness) {
            auxFitness = dungeons[j]->fitness;
            id = j;
        }
    }
    Dungeon* aux = dungeons[id]->Copy();

    if(Constants::analisis){
        analisisData.push_back(((int)aux->roomList.size()));
        analisisData.push_back(aux->nKeys );
        analisisData.push_back(aux->nLocks );
        analisisData.push_back(aux->avgChildren );
        analisisData.push_back(aux->neededLocks );
    }

    if (Constants::testingMode) {
        std::cout << std::endl << std::endl << "BEST OF ALL" << std::endl;
        std::cout << "fitness " << aux->fitness << std::endl;

        for (Room* r : aux->roomList) {
            std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]" << ((int)r->type) << ": " << r->keyToOpen << std::endl;
        }

    }
    else if(Constants::analisis == false){
        std::cout << aux->fitness<< std::endl;
    }
    

    for (Dungeon* dungeon : dungeons) {
        delete dungeon;
    }
    dungeons.clear();
    return aux;
}

/**********************************************************************/
/************************      Evolve       ***************************/
/**********************************************************************/



/**********************************************************************/
/************************      FORM       *****************************/
/**********************************************************************/
/*                              STEP 1                                */
/// @brief Evolve the Form of the dungeon
/// @param dungeons Generation to evolve
/// @return Evolved dungeons next Generation
std::vector<Dungeon*>  GA::EvolveForm(std::vector<Dungeon*> dungeons){
    Dungeon* aux;
    float lastBestFitness = 0;
    for (int gen = 0; gen < Constants::GENERATIONSFORM; gen++) {
		std::vector<Dungeon*> nexGeneration;

        if (Constants::testingMode) {
            std::cout << "GEN " << gen << std::endl;
        }
        
        int counter = 0;
        float max = std::numeric_limits<double>::infinity() * -1;
        float min = std::numeric_limits<double>::infinity();;
        float prom = 0;

        for (Dungeon* dungeon : dungeons) {
            if (Constants::testingMode) {
                std::cout << "Dungeon " << counter <<  std::endl;
            }
            float fitness = 0;
            fitness = FitnessSymmetry(dungeon, Constants::nV, Constants::nK, Constants::nL, Constants::lCoef, Constants::FitnessSymmetry, Constants::MATRIXOFFSET, false, true);
            dungeon->fitness = fitness;

            if(max < fitness){
                max = fitness;
            }
            if(min > fitness){
                min = fitness;
            }
            if(min != lastBestFitness){
                lastBestFitness = min;
                // std::cout << "current Best Individual " << min << std::endl;
            }
            prom += fitness;
            
            
            if(Constants::testingMode){
               dungeon->DisplayDungeon();
            }
            counter++;
        }

        if(Constants::analisis){
            prom /= counter;
            analisisData.push_back(min);
            analisisData.push_back(max);
            analisisData.push_back(prom);
        }
        
		
        float auxFitness = dungeons[0]->fitness;
        int id = 0;
        for (int j = 0; j < dungeons.size(); j++) {
            if (auxFitness > dungeons[j]->fitness) {
                auxFitness = dungeons[j]->fitness;
                id = j;
            }
        }
        aux = dungeons[id]->Copy();
		
		// NEXT GENERATION Preparation !
		for (int i = 0; i < Constants::POP_SIZE/2; i++)
		{
			// Select parents
			int parentA = 0;
			int parentB = 1;
			
			Tournament(dungeons, parentA, parentB);
            // std::cout << "Parents " << parentA  << ", " << parentB << std::endl;
			Dungeon* parent1 = dungeons[parentA]->Copy();
			Dungeon* parent2 = dungeons[parentB]->Copy();
            CrossoverForm(parent1,parent2);
            
            // Chech parents if their fitness is better than the best
            float fitness1 = FitnessSymmetry(parent1, Constants::nV, Constants::nK, Constants::nL, Constants::lCoef, Constants::FitnessSymmetry, Constants::MATRIXOFFSET, false, true);
            float fitness2 = FitnessSymmetry(parent2, Constants::nV, Constants::nK, Constants::nL, Constants::lCoef, Constants::FitnessSymmetry, Constants::MATRIXOFFSET, false, true);
            
            if(fitness1 < lastBestFitness){
                lastBestFitness = fitness1;
                delete aux;
                aux = parent1->Copy();
            }
            if (fitness2 < lastBestFitness){
                lastBestFitness = fitness2;
                delete aux;
                aux = parent2->Copy();
            }
            MutationForm(parent1);
			MutationForm(parent2);

            
			nexGeneration.push_back(parent1);
			nexGeneration.push_back(parent2);
		}

        // std::cout << "Clean old pop " << dungeons.size() << " NEXT GEN " <<nexGeneration.size()  << std::endl;
		// Clean current generation
		while (!dungeons.empty()) {
            // std::cout << "Removing dungeon ..." << std::endl;
			Dungeon* dungeon = dungeons.back(); // or front() depending on your needs
			dungeons.pop_back();
			delete dungeon; // Delete the object
		}
	
		// Prepare the next generation
		for(Dungeon* d : nexGeneration){
			dungeons.push_back(d);
		}
		
		// Elitism
		delete dungeons[0];
		dungeons[0] = aux;
    }
    return dungeons;
}

void GA::CrossoverForm(Dungeon*& indOriginal1, Dungeon*& indOriginal2) {
    Dungeon *ind1 = nullptr, *ind2 = nullptr;
    
    Room *roomCut1, *roomCut2;
    int cutposition1, cutposition2;
    int prob = Constants::Next(1001);
    // std::cout << " Will cross ???"  << ((prob < Constants::CROSSOVER_RATE_FORM * 1000) == 1 ? "True":"False")<< ", " << 
    //             indOriginal1->roomList.size() << ", " << indOriginal2->roomList.size() << std::endl;
    if (prob < Constants::CROSSOVER_RATE_FORM * 1000 && indOriginal1->roomList.size() > 1 && indOriginal2->roomList.size() > 1) {
        // Randomly select Each parent 
        ind1 = indOriginal1;
        ind2 = indOriginal2;

        
        prob = Constants::Next(100);
        if(prob < 50){
            ind1 = indOriginal1;
            ind2 = indOriginal2;
        }
        else{
            ind2 = indOriginal1;
            ind1 = indOriginal2;
        }
        
        
        // Select the cut position for individual1
        cutposition1 = Constants::Next(1, ind1->roomList.size());
        roomCut1 = ind1->roomList[cutposition1];

        // Search for the other individual "Same Deph"
        std::vector<int> otherDephRooms;
        for (int i =1; i < (int)ind2->roomList.size(); i++)
        {
            otherDephRooms.push_back(i);
        }
        
        // Select one of those !
        if(otherDephRooms.size() > 0){

            
            // Select a node in the same deph
            int otherRoomID = Constants::Next(otherDephRooms.size());
            cutposition2 = otherDephRooms[otherRoomID];
            roomCut2 = ind2->roomList[cutposition2];

            // Meaningless If there is only leafs changed we interrupt the move 
            if(roomCut1->IsLeafNode() && roomCut2->IsLeafNode()){
                return;
            }

            // CHANGE parent link to son (from cut1 -> cut2)
            ChangeChildren(roomCut1, roomCut2);
            ChangeChildren(roomCut2, roomCut1);

            // Swap the parents from roomCut1 and roomCut2
            Room* tempParent = roomCut1->Parent;
            roomCut1->Parent = roomCut2->Parent;
            roomCut2->Parent = tempParent;

            // Remove the whole branch from the tree, and grid
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
            
            // Mutation add
            int mutationProb = Constants::Next(1001);
            bool willMutateSymmetric = mutationProb <= Constants::SYMMETRIC_RATE_FORM * 1000;

            // Refresh the grid, fix and fix the room lists
            ind1->RefreshGridSymmetric(roomCut2, true, willMutateSymmetric);
            ind2->RefreshGridSymmetric(roomCut1, true, willMutateSymmetric);
            

            ind1->FixRoomList();
            ind2->FixRoomList();

            ind1->CalcAvgChildren();
            ind2->CalcAvgChildren();    
            
        }
        else{
            // std::cout << "CROSS FAILED :C" << std::endl;
        }   
    }
    else{
        // std::cout << "No Cross over " << std::endl;
    }
}


void GA::MutationForm(Dungeon* dun) {
    try {
        // Mutate keys, adding or removing a pair
        int mutationProb = Constants::Next(1001);
        bool willMutateA = mutationProb <= Constants::MUTATION0_RATE_FORM  * 1000;
        
        if (willMutateA) {
            // std::cout << "Mutation Remove "<< std::endl;
            dun->RemoveRooms();
            dun->CalcAvgChildren();
        }

        mutationProb = Constants::Next(1001);
        bool willMutateB = mutationProb <= Constants::MUTATION1_RATE_FORM  * 1000;
        if(willMutateB){
            // std::cout << "Mutation ADD "<< std::endl;
            dun->AddRooms();
            dun->CalcAvgChildren();
        }

    } catch (const std::exception& e) {
        throw e;
    }
}

/**********************************************************************/
/************************      BK       *******************************/
/**********************************************************************/

/// @brief Evolve the barrier and keys mechanics
/// @param dungeons Generation to evolve
/// @return Evolved dungeons next Generation
std::vector<Dungeon*>  GA::EvolveBK(std::vector<Dungeon*> dungeons){
    Dungeon* aux;
    
    for(Dungeon* d : dungeons){
        d->AddLockAndKey(Constants::nL);
        // Get their new fitness using the A*, b-k
        float fitness = 0;
        fitness = FitnessSymmetry(d, Constants::nV, Constants::nK, Constants::nL, Constants::lCoef, Constants::FitnessSymmetry, Constants::MATRIXOFFSET, true, false);
        d->fitness = fitness;
        d->FixRoomList(); // And NK NL
    }
    
    // EVOLUTION !!!
    for (int gen = 0; gen < Constants::GENERATIONSBK; gen++) {
		std::vector<Dungeon*> nexGeneration;
        if (Constants::testingMode) {
            std::cout << "GEN BK " << gen << std::endl;
        }
        // Fitness Current Generation
        int counter = 0;
        float max = std::numeric_limits<double>::infinity() * -1;
        float min = std::numeric_limits<double>::infinity();;
        float prom = 0;

        for (Dungeon* dungeon : dungeons) {
            if (Constants::testingMode) {
                std::cout << "Dungeon " << counter <<  std::endl;
            }
            float fitness = 0;
            fitness = FitnessSymmetry(dungeon, Constants::nV, Constants::nK, Constants::nL, Constants::lCoef, Constants::FitnessSymmetry, Constants::MATRIXOFFSET, true, false);
            dungeon->fitness = fitness;

            if(max < fitness){
                max = fitness;
            }
            if(min > fitness){
                min = fitness;
            }
            prom += fitness;

            if(Constants::testingMode){
                dungeon->DisplayDungeon();
            }
            counter++;
        }
        if(Constants::analisis){
            prom /= counter;
            analisisData.push_back(min);
            analisisData.push_back(max);
            analisisData.push_back(prom);
        }

        // elitism
        float auxFitness = dungeons[0]->fitness;
        int id = 0;
        for (int j = 0; j < dungeons.size(); j++) {
            if (auxFitness > dungeons[j]->fitness) {
                auxFitness = dungeons[j]->fitness;
                id = j;
            }
        }
        aux = dungeons[id]->Copy();

        // NEXT GENERATION Preparation !
		for (int i = 0; i < Constants::POP_SIZE/2; i++)
		{
            
			// Select parents
			int parentA = 0;
			int parentB = 1;
			
			Tournament(dungeons, parentA, parentB);

			Dungeon* parent1 = dungeons[parentA]->Copy();
			Dungeon* parent2 = dungeons[parentB]->Copy();

            EvolutionBK(parent1);
			EvolutionBK(parent2);

			nexGeneration.push_back(parent1);
			nexGeneration.push_back(parent2);
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
		delete dungeons[0];
		dungeons[0] = aux;
    }
		
    return dungeons;
}

void GA::EvolutionBK(Dungeon* dun) {
    int lastKey = dun->nKeys;
    int lastLocks = dun->nLocks;
    // dun->nLocks
    
    // Mutate keys, adding or removing a pair
    int mutationProb = Constants::Next(1001);
    bool wilRemoveLK = mutationProb <= Constants::MUTATION0_RATE_BK * 1000;
    MutationOp op;
    if (wilRemoveLK) {
        // std::cout << "Mutate Remove Lock" << std::endl;
        dun->RemoveLockAndKey();
        dun->FixRoomList();
    }

    // 
    mutationProb = Constants::Next(1001);
    bool willAddLK = mutationProb <= Constants::MUTATION1_RATE_BK * 1000;
    if(willAddLK){
        // std::cout << "Mutate Add Lock" << std::endl;
        dun->AddLockAndKey(1);
        dun->FixRoomList();
    }
    /*
    if(lastKey != dun->nKeys || lastLocks != dun->nLocks){
        std::cout << "Diff, new : [" << dun->nKeys << ", " << dun->nLocks << " ]" << std::endl;
    }
    */
}


// Get Analisis data 
std::vector<float> GA::GetAnalisisData(){
    return analisisData;
}