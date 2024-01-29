#ifndef DFS_H
#define DFS_H

#include "AStar.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>


class DFS {
public:
    Location* start;
    std::vector<Location*> openList;
    std::vector<Location*> ClosedList;
    std::vector<Location*> path;
    std::vector<int> keys;
    std::vector<int> lockedRooms;
    std::vector<Location> locksLocation;
    std::vector<Location> allLocksLocation;
    Location* target;

    int FindRouteDFS(Dungeon* dun);
    void ValidateKeyRoom(Location* current);
    void InitiatePathFinding(Dungeon* dun);
    int FindIndex(const std::vector<int> vec, int valueToFind);
    std::vector<Location*> GetWalkableAdjacentSquares(int X, int Y, int sizeX, int sizeY, std::vector<std::vector<int>> map);

    // Reseted at start !
    int neededLocks;
    int nVisitedRooms;
    int minX;
    int minY;
    int maxX;
    int maxY;
    int sizeX;
    int sizeY;

    std::vector<std::vector<int>> map;
};

#endif