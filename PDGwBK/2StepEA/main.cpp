
#include "Constants.h"
#include "GA.h"
#include <cstdlib> // For std::stoi and std::stof



// Arrange the date to call the Creation
Dungeon* EvolveDungeons(int seed, int numVertices, int numKeys, int numLocks, float lCoefficient, float neededLocks,
                     bool testingMode, std::string fileName,
                     int generationsFORM, int generationsBK, int populationSize,
                     float mf1, float mf2, float cf, float se, float mbk1, float mbk2,
                     float sCoef, float lCoefW, float sCoefW,
                     double *aarray){
    if(testingMode){
        std::cout << " Testing mode, Display input parameters: "<< std::endl;
        std::cout << " SEED: " << seed << " nV: " << numVertices << " nK: " << numKeys << " nL: " << numLocks << " LCoef: " << lCoefficient << " Sym: " << se << " NeededL: " <<  neededLocks << std::endl;
        std::cout << " TestMode: " << testingMode << " TestFS: " << fileName << std::endl;
        std::cout << " Gen: " << generationsFORM << " GenBK: " << generationsBK << " PopSize: " << populationSize << std::endl;
        std::cout << " ALL probs: [" << mf1 << ", " << mf2 << ", " << cf << ", " << se << "] [" << mbk1 << ", "<< mbk2 << "]"  << std::endl;
    }

    GA ga;
    // Inner Configuration
    Constants::useDFS = false;
    Constants::useSymmetry = true;
    // Test this later :s
    Constants::ID = 0;

    // Testign Values
    Constants::FitnessSymmetry = se;
    Constants::NeededLocks = neededLocks;

    // SINTONIZATION PARAMETERS
    Constants::GENERATIONSFORM = generationsFORM;
    Constants::GENERATIONSBK = generationsBK;
    Constants::POP_SIZE = populationSize;
    // FORM
    Constants::MUTATION0_RATE_FORM = mf1; // Recieves 0.025 (2.5%)
    Constants::MUTATION1_RATE_FORM = mf2; // Recieves 0.025 (2.5%)
    Constants::SYMMETRIC_RATE_FORM = se ; // Recieves 0.025 (2.5%)
    Constants::CROSSOVER_RATE_FORM = cf ; // Recieves 0.9   (90%)
    
    // BK
    Constants::MUTATION0_RATE_BK = mbk1 ; // Recieves 0.025 (2.5%)
    Constants::MUTATION1_RATE_BK = mbk2 ; // Recieves 0.025 (2.5%)


    // Other
    Constants::MAX_DEPTH = numVertices; // Will be limited to the room quantity
    Constants::nV = numVertices;
    Constants::nK = numKeys;
    Constants::nL = numLocks;
    Constants::lCoef = lCoefficient;
    Constants::seed = seed;
    Constants::randGen = std::mt19937(Constants::seed);
    Constants::testingMode = testingMode;

    Constants::FitnessSymmetry = sCoef;
    Constants::lCoefW = lCoefW;
    Constants::sCoefW = sCoef;

    Constants::analisis = fileName == "Analisis";
    //std::cout << "Create dungeon ..." << std::endl;
    Dungeon* resultDungeon =  ga.CreateDungeon();
    //std::cout << "finished ... dungeon ..." << std::endl;

    if(Constants::analisis && aarray != nullptr){
        for (size_t i = 0; i < ga.analisisData.size(); i++)
        {
            // std::cout << ga.analisisData[i] << std::endl;
            aarray[i] =  ga.analisisData[i];
        }
    }
    return resultDungeon;
}

// Call from .so or .dll depending on where you want to use it !
extern "C"
void CreateDungeons(double *rarray, double *aarray, int size) {
    
    int seed = static_cast<int>(rarray[0]);
    int numVertices = static_cast<int>(rarray[1]);
    int numKeys = static_cast<int>(rarray[2]);
    int numLocks = static_cast<int>(rarray[3]);
    float lCoefficient = static_cast<float>(rarray[4]);
    float neededLocks = static_cast<float>(rarray[5]);
    bool testingMode = static_cast<int>(rarray[6]) == 1;
    // Two options Doing nothing OR do the analisis
    std::string fileName = static_cast<int>(rarray[7]) == 0 ? "NO" : "Analisis";
    // Algorithm Parameters
    int generationsFORM = static_cast<int>(rarray[8]);
    int generationsBK = static_cast<int>(rarray[9]);
    int populationSize = static_cast<int>(rarray[10]);
    // All probabilities
    float mf1 = static_cast<float>(rarray[11]);
    float mf2 = static_cast<float>(rarray[12]);
    float cf = static_cast<float>(rarray[13]);
    float se = static_cast<float>(rarray[14]);
    float mbk1 = static_cast<float>(rarray[15]);
    float mbk2 = static_cast<float>(rarray[16]);

    float sCoef = static_cast<float>(rarray[17]);
    float lCoefW = static_cast<float>(rarray[18]);
    float sCoefW = static_cast<float>(rarray[19]);

    
    Dungeon* result = EvolveDungeons(seed, numVertices, numKeys, numLocks, lCoefficient, neededLocks, 
                                        testingMode, fileName, 
                                        generationsFORM, generationsBK, populationSize,
                                        mf1, mf2, cf, se, mbk1, mbk2,
                                        sCoef, lCoefW, sCoefW, 
                                        aarray);
    // Prepare for Python to visualice it !
    std::vector<int> keys;
    if(testingMode == true){
        std::cout << "Best Fitness :" << result->fitness  << std::endl;
    }
    for (int i = 0; i < (int) result->roomList.size(); i++)
    {
        // std::cout << "Room id: " << result->roomList[i]->roomId << " POS: "<< result->roomList[i]->X << ", " << result->roomList[i]->Y << " = " << result->roomList[i]->keyToOpen << "[" << i << "]"<< std::endl;
        for (int j = 0; j < (int) result->roomList.size(); j++)
        {
            // The only one with authority to change the result is the one that has the key, no other room
            if(result->roomList[i]->type == RoomType::key && result->roomList[i]->roomId ==  result->roomList[j]->keyToOpen){
                result->roomList[j]->keyToOpen = i;
            }
        }
        result->roomList[i]->roomId = i;
        
    }
    
    // When testing we need to check the max size it can go, coz there's going to be bigger dungeons !
    // size = (nV * 2 * 7) + 2
    //std::cout << "TOTAL ROOMS " << (int) result->roomList.size() << std::endl;
    for (int i = 0; i < (int) result->roomList.size(); i++)
    {
        int basePos = (i*7);
        // The problem is the IDS later :s
        rarray[basePos+ 0] = result->roomList[i]->roomId;
        rarray[basePos+ 1] = result->roomList[i]->X;
        rarray[basePos + 2] = result->roomList[i]->Y;
        rarray[basePos + 3] = result->roomList[i]->leftChild != nullptr? result->roomList[i]->leftChild->roomId: -1;
        rarray[basePos + 4] = result->roomList[i]->bottomChild != nullptr? result->roomList[i]->bottomChild->roomId: -1;
        rarray[basePos + 5] = result->roomList[i]->rightChild != nullptr? result->roomList[i]->rightChild->roomId: -1;
        rarray[basePos + 6] = result->roomList[i]->keyToOpen;
        
    }

    rarray[size-2] = result->finalRoomX;
    rarray[size-1] = result->finalRoomY;

    //td::cout << size << " Final room " << result->finalRoomX << ", " << result->finalRoomY << std::endl;
    delete result;
}


// Call from console
int main(int argc, char* argv[]){

    if(argc >= 18){
        // Problem 6 parameters
        int seed = std::stoi(argv[1]);
        int numVertices = std::stoi(argv[2]);
        int numKeys = std::stoi(argv[3]);
        int numLocks = std::stoi(argv[4]);
        float lCoefficient = std::stof(argv[5]);
        float neededLocks = std::stof(argv[6]);
        // Testing 2 parameters
        bool testingMode = std::string(argv[7]) == "true";
        std::string fileName = std::string(argv[8]);
        // Algorithm Parameters ( 9 parameters )
        int generationsFORM = std::stoi(argv[9]);
        int generationsBK = std::stof(argv[10]);
        int populationSize = std::stoi(argv[11]);
        float mf1 = std::stof(argv[12]);
        float mf2 = std::stof(argv[13]);
        float cf = std::stof(argv[14]);
        float se = std::stof(argv[15]);
        float mbk1 = std::stof(argv[16]);
        float mbk2 = std::stof(argv[17]);

        float sCoef = 0;
        float lCoefW = 1;
        float sCoefW = 0;

        int size = numVertices;
        int maxRange = numVertices * 10;
        size = (maxRange * 7) + 2;
    
        double* rarray = new double[size];
        rarray[0] = seed;
        rarray[1] = numVertices;
        rarray[2] = numKeys;
        rarray[3] = numLocks;
        rarray[4] = lCoefficient;
        
        rarray[5] = neededLocks;
        rarray[6] = testingMode;
        rarray[7] = 0;

        rarray[8] = generationsFORM;
        rarray[9] = generationsBK;
        rarray[10] = populationSize;

        rarray[11] = mf1;
        rarray[12] = mf2;
        rarray[13] = cf;
        rarray[14] = se;
        rarray[15] = mbk1;
        rarray[16] = mbk2;

        rarray[17] = sCoef;
        rarray[18] = lCoefW;
        rarray[19] = sCoefW;
        // Create Dungeon !
        CreateDungeons(rarray, nullptr ,size);

        // Free the array !
        delete[] rarray;
        rarray = nullptr;
               
    }
    else{
        std::cout << "Wrong quantity of Arguments !";
    }
    return 0;
}