#ifndef DUNGEON_H
#define DUNGEON_H

#include "Room.h"
#include <vector>
#include <queue>
#include <iostream>
class Dungeon;

class Dungeon {
public:
    int nKeys = 0, nLocks = 0, neededLocks = 0;
    float neededRooms = 0;
    int desiredKeys = 0;
    float avgChildren = 0;
    float sCoef;
    std::queue<Room*> toVisit;
    std::vector<Room*> roomList;
    RoomGrid roomGrid;
    int finalRoomX = 0;
    int finalRoomY = 0;
    float fitness;
    
    Dungeon();
    ~Dungeon();

    Dungeon* Copy();

    void CleanDungeon();

    void CalcAvgChildren();
    void CalculateSymmetry();

    void InstantiateRoom(Room* child, Room* actualRoom, Constants::Direction dir);

    void RemoveFromGrid(Room* cut);

    void RemoveRoomsRecursive(Room* actualRoom);
    
    void GenerateRooms();

    void RefreshGridSymmetric(Room* newRoom, bool isFirst, bool willMutateSymmetric);  
    bool ValidateAndInsert(Room* newRoom, Room* auxRoom, Constants::Direction currentDirection, bool willMutateSymmetric);
    
    void FixRoomList();

    void DisplayDungeon();
    
    // Mutation
    void AddLockAndKey(int quantity);
    void RemoveLockAndKey();

    // Mutation
    void AddRooms();
    void RemoveRooms();

    // Way to slow, removed ...
    void SymmetricMutation();
};

#endif