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
    float Fitness(std::vector<int> specialRooms, Room *root, int *nRooms, int *specialCount) ;
    std::vector<int>  FindNKLR(Room *root, int *nRooms, int *specialCount);
    std::vector<Dungeon> CreateDungeon();
    std::vector<Dungeon> GenerateDungeon();
};

#endif 
