#ifndef ROOM_H
#define ROOM_H


#include "Constants.h"
#include <queue>
#include <vector>

// Classes that will be created
class RoomGrid;
class Room;


enum class RoomType {
    normal,
    key,
    locked
};


class Room {
public:
    Room* rightChild = nullptr;
    Room* bottomChild = nullptr;
    Room* leftChild = nullptr;
    int roomId;
    int keyToOpen = -1;
    RoomType type;
    int X;
    int Y;
    int depth = 0;
    int rotation = 0;
    Room* Parent = nullptr;
    Constants::Direction parentDirection = Constants::Direction::down;

    Room();
    Room(int nX, int nY, int nK, RoomType nT, Room* nP);
    
    Room(RoomType roomType, int keyToOpen, int id);
    Room* Copy();
    
    
    void FixBranch(std::vector<int> specialRooms);
    
    bool ValidateChild(Constants::Direction dir, RoomGrid roomGrid);
    void InsertChild(Constants::Direction dir, Room*& child, RoomGrid& roomGrid);
    void SetParent(Room* parent);
    void FindChildren(std::vector<Room*>& roomList);
    bool IsLeafNode();
    bool Equal(Room* other);
    
};

class RoomGrid {
private:
    std::vector<std::vector<Room*>> grid;
public:
    

    RoomGrid();
    Room* GetRoom(int x, int y);
    void SetRoom(int x, int y, Room* value);
    
    RoomGrid* Copy();
};


#endif
