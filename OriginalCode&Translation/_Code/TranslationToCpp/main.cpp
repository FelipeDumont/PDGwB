
#include "Constants.h"
#include "GA.h"
#include <cstdlib> // For std::stoi and std::stof
#include "DFS.h"

int main(int argc, char* argv[]){

    
    if(argc >= 12){
        GA ga;
        // Inner Configuration
        Constants::useDFS = true;
        Constants::ID = 0;
        // ARGS
        Constants::GENERATIONS = std::stoi(argv[1]);// 10
        Constants::MAX_DEPTH = std::stoi(argv[2]);
        Constants::POP_SIZE = std::stoi(argv[3]);
        Constants::nV = std::stoi(argv[4]);
        Constants::nK = std::stoi(argv[5]);
        Constants::nL = std::stoi(argv[6]);
        Constants::lCoef = std::stof(argv[7]);
        Constants::seed = std::stoi(argv[8]);
        bool testingMode = std::string(argv[9]) == "true";
        bool testingBasics = std::string(argv[10]) == "true";
        bool useForceRandomSequence = std::string(argv[11]) == "true";
        Constants::randGen = std::mt19937(Constants::seed);
        // Assign internally
        ga.testingMode = testingMode;

        if(testingMode){
            // std::cout << "popsize" << Constants::POP_SIZE << std::endl;
            // std::cout << "Testing mode" << ga.testingMode << std::endl;
        }
        // Finally we check the random sequence
        if(argc == 13 && useForceRandomSequence){
            std::string fileName = argv[12];// "RandomSequence.txt";
            ga.ReadSequence(fileName);
            if(testingMode){
                std::cout << "Random sequence size: "<< Constants::forcedRandomSequence.size()<< std::endl;
            }
        }

        if(testingBasics)
        {
            ga.BasicTests();
        }   
        else{
            Dungeon* result = ga.CreateDungeon();

            // Prepare for the return to python
            std::cout << "Best Dungeon " << result->fitness << std::endl;
            // Need to delete them to test
            delete result;
        }
                   
    }
    else{
        std::cout << "Wrong quantity of Arguments !";
    }
    return 0;
}



extern "C"
void CreateDungeons(double *rarray) {
    GA ga;
    // Inner Configuration
    Constants::useDFS = true;
    Constants::ID = 0;
    // ARGS
    Constants::GENERATIONS = static_cast<int>(rarray[0]);
    Constants::MAX_DEPTH = static_cast<int>(rarray[1]);
    Constants::POP_SIZE = static_cast<int>(rarray[2]);
    Constants::nV = static_cast<int>(rarray[3]);
    Constants::nK = static_cast<int>(rarray[4]);
    Constants::nL = static_cast<int>(rarray[5]);
    Constants::lCoef = static_cast<float>(rarray[6]);
    Constants::seed = static_cast<int>(rarray[7]);
    Constants::randGen = std::mt19937(Constants::seed);

    Dungeon* result = ga.CreateDungeon();

    // Prepare for the return to Python
    // Need to delete them to test
    for (int i = 0; i < (int) result->roomList.size(); i++)
    {
        int basePos = (i*7);
        rarray[basePos+ 0] = result->roomList[i]->roomId;
        rarray[basePos+ 1] = result->roomList[i]->X;
        rarray[basePos + 2] = result->roomList[i]->Y;
        rarray[basePos + 3] = result->roomList[i]->leftChild != nullptr? result->roomList[i]->leftChild->roomId: -1;
        rarray[basePos + 4] = result->roomList[i]->bottomChild != nullptr? result->roomList[i]->bottomChild->roomId: -1;
        rarray[basePos + 5] = result->roomList[i]->rightChild != nullptr? result->roomList[i]->rightChild->roomId: -1;
        rarray[basePos + 6] = result->roomList[i]->keyToOpen;

        
    }

    delete result;
}
