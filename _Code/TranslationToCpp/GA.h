#ifndef GA_H
#define GA_H

#include "Constants.h"
#include "Room.h"
#include "AStar.h"
#include "Dungeon.h"
#include "DFS.h"

#include <fstream>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <chrono> // Not used ...

class GA {
public:
    bool testingMode;;
    enum class MutationOp { insertChild, removeLeaf };
    float Fitness(Dungeon* ind, int nV, int nK, int nL, float lCoef, int matrixOffset);
    std::vector<int>  FindNKLR(Room *root, int *nRooms, int *specialCount);
    std::vector<Dungeon*> CreateDungeon();
    std::vector<Dungeon> GenerateDungeon();

    void Tournament(std::vector<Dungeon*> pop, int &parent1, int &parent2);
    void ChangeChildren(Room* cut1, Room* cut2);
    void Crossover(Dungeon* indOriginal1, Dungeon* indOriginal2);
    void Mutation(Dungeon* dun);

    // Testing
    void ReadSequence(std::string fileName);
    int Testing(); // OLD int main()
    void FirstTest();
};

#endif