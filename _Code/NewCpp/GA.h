#ifndef GA_H
#define GA_H

#include "Constants.h"
#include "Room.h"
#include "AStar.h"
#include "Dungeon.h"

#include <fstream>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <chrono> // Not used ...


class GA {
public:
    bool testingMode;;
    enum class MutationOp { insertChild, removeLeaf };


    /**********************************************************************/
    /************************      Helper       ***************************/
    /**********************************************************************/
    void ChangeChildren(Room* cut1, Room* cut2);


    /**********************************************************************/
    /**********************      Fitness       ****************************/
    /**********************************************************************/
    float FitnessSymmetry(Dungeon* ind, int nV, int nK, int nL, float lCoef, int matrixOffset, bool useKB, bool final);
    
    /**********************************************************************/
    /********************      Main Evolution      ************************/
    /**********************************************************************/
    void Tournament(std::vector<Dungeon*> pop, int &parent1, int &parent2);
    Dungeon* CreateDungeon();

    /**********************************************************************/
    /************************      TEST       *****************************/
    /**********************************************************************/
    void ReadSequence(std::string fileName);

    /**********************************************************************/
    /************************      Evolve       ***************************/
    /**********************************************************************/
    std::vector<Dungeon*> EvolveForm(std::vector<Dungeon*> dungeons);
    std::vector<Dungeon*> EvolveBK(std::vector<Dungeon*> dungeons);

    /**********************************************************************/
    /************************      FORM       *****************************/
    /**********************************************************************/
    void CrossoverForm(Dungeon*& indOriginal1, Dungeon*& indOriginal2);
    void MutationForm(Dungeon* dun);

    /**********************************************************************/
    /************************      BK       *******************************/
    /**********************************************************************/
    void EvolutionBK(Dungeon* dun);

};

#endif