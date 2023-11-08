
#include "Constants.h"
#include "Room.h"
#include "AStar.h"
#include "Dungeon.h"
#include "GA.h"


int main(){
    GA program;

    std::vector<Dungeon> dungeons =  program.GenerateDungeon();
    return 0;

}