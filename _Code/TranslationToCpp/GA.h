#ifndef GA_H
#define GA_H

#include "Constants.h"
#include "Room.h"
#include "AStar.h"
#include "Dungeon.h"

#include <random>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <limits>

class GA {
public:
    int FirstTest();
    void ReadSequence(std::string fileName);
    float Fitness(Dungeon ind, int nV, int nK, int nL, float lCoef, int matrixOffset);
    std::vector<Dungeon> CreateDungeon();
    std::vector<Dungeon> GenerateDungeon();
};

#endif 
