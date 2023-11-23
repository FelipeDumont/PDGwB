
#include "Constants.h"
#include "GA.h"
#include <cstdlib> // For std::stoi and std::stof

extern "C" { 
	int Add(int a, int b)
	{
	    return a + b;
	}
}


int main(int argc, char* argv[]){

    
    if(argc >= 12){
        GA ga;

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
        // Assign internally
        ga.testingMode = testingMode;

        std::cout << "testing mode ? " << testingMode << std::endl;
        if(testingMode){
            std::cout << "popsize" << Constants::POP_SIZE << std::endl;
            std::cout << "Testing mode" << ga.testingMode << std::endl;
        }
        // Finally we check the random sequence
        if(argc == 13 && useForceRandomSequence){
            std::string fileName = argv[12];// "RandomSequence.txt";
            ga.ReadSequence(fileName);
            if(testingMode){
                std::cout << "Values assigned ??? "<< Constants::forcedRandomSequence.size()<< std::endl;
            }
        }

        std::vector<Dungeon*> results = ga.CreateDungeon();
    }
    else{
        std::cout << "Wrong quantity of Arguments !";
    }
    return 0;
}
