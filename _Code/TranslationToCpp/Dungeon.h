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


    Dungeon* Copy();

    void CalcAvgChildren();
/*
    void InstantiateRoom(Room*& child, Room*& actualRoom, int dir);

    void RemoveFromGrid(Room* cut);

    void RefreshGrid(Room*& newRoom);

    void GenerateRooms();

    void FixRoomList();

    void AddLockAndKey();

    void RemoveLockAndKey();

    void SetBoundariesFromRoomList();

    void SetDimensionsFromBoundaries();
*/
};

#endif

