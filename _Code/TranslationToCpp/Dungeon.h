#ifndef DUNGEON_H
#define DUNGEON_H

#include "Room.h"
#include <vector>
#include <queue>

class Dungeon;

class Dungeon {
public:
    int nKeys, nLocks, neededLocks;
    float neededRooms;
    int desiredKeys;
    float avgChildren;
    Boundaries boundaries;
    Dimensions dimensions;
    std::queue<Room> toVisit;
    std::vector<Room> roomList;
    RoomGrid roomGrid;
    float fitness;
    
    Dungeon();


    Dungeon Copy();

    void CalcAvgChildren();

    void InstantiateRoom(Room* child, Room* actualRoom, Constants::Direction dir);

    void RemoveFromGrid(Room* cut);
    
    void GenerateRooms();
    
/*
    void RefreshGrid(Room*& newRoom);

    void FixRoomList();

    void AddLockAndKey();

    void RemoveLockAndKey();

    void SetBoundariesFromRoomList();

    void SetDimensionsFromBoundaries();
*/
};

#endif

