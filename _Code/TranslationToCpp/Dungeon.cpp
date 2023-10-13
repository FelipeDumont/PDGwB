
#include "RoomFactory.h"
#include "Constants.h"
#include "Dungeon.h"


Dungeon::Dungeon() {
    while (!toVisit.empty()) {
        toVisit.pop();
    }
    roomList.clear();
    Room root = RoomFactory::CreateRoot();
    roomList.push_back(root);
    toVisit.push(root);
    roomGrid = RoomGrid();
    neededRooms = 0;
    neededLocks = 0;
    fitness = std::numeric_limits<float>::max();;
}

Dungeon* Dungeon::Copy() {
    Dungeon* copyDungeon = new Dungeon();
    copyDungeon->toVisit = std::queue<Room>(toVisit);
    copyDungeon->roomList = std::vector<Room>(roomList);
    copyDungeon->roomGrid = RoomGrid(roomGrid);
    copyDungeon->nKeys = nKeys;
    copyDungeon->nLocks = nLocks;
    copyDungeon->desiredKeys = desiredKeys;
    copyDungeon->avgChildren = avgChildren;
    copyDungeon->fitness = fitness;
    Room* aux;
    for (Room oldRoom : roomList) {
        aux = new Room(oldRoom);
        copyDungeon->roomList.push_back(*aux);
        copyDungeon->roomGrid.grid[oldRoom.X][oldRoom.Y] = aux;
    }
    // Need to use the grid to copy the neighbors, children, and parent
    for (Room newRoom : copyDungeon->roomList) {
        if (newRoom.Parent != nullptr) {
            newRoom.Parent = copyDungeon->roomGrid.grid[newRoom.Parent->X][newRoom.Parent->Y];
        }
        if (newRoom.rightChild != nullptr) {
            newRoom.rightChild = copyDungeon->roomGrid.grid[newRoom.rightChild->X][newRoom.rightChild->Y];
        }
        if (newRoom.bottomChild != nullptr) {
            newRoom.bottomChild = copyDungeon->roomGrid.grid[newRoom.bottomChild->X][newRoom.bottomChild->Y];
        }
        if (newRoom.leftChild != nullptr) {
            newRoom.leftChild = copyDungeon->roomGrid.grid[newRoom.leftChild->X][newRoom.leftChild->Y];
        }
    }
    return copyDungeon;
}

void Dungeon::CalcAvgChildren() {
    avgChildren = 0.0f;
    int childCount;
    int childLess = 0;
    for (Room room : roomList) {
        childCount = 0;
        if (room.rightChild != nullptr && room.rightChild->Parent != nullptr)
            childCount += 1;
        if (room.leftChild != nullptr && room.leftChild->Parent != nullptr)
            childCount += 1;
        if (room.bottomChild != nullptr && room.bottomChild->Parent != nullptr)
            childCount += 1;
        if (childCount == 0)
            childLess++;
        avgChildren += childCount;
    }
    avgChildren = avgChildren / (roomList.size() - childLess);
}

/*
void Dungeon::InstantiateRoom(Room*& child, Room*& actualRoom, int dir) {
    if (actualRoom->ValidateChild(dir, roomGrid)) {
        child = new Room(); // You may want to customize the Room constructor according to your needs
        //std::cout << "Created! ID = " << child->RoomId << std::endl;
        actualRoom->InsertChild(static_cast<Util::Direction>(dir), child, roomGrid);
        //std::cout << "Inserted! ID = " << child->RoomId << std::endl;
        child->ParentDirection = static_cast<Util::Direction>(dir);
        toVisit.push(child);
        roomList.push_back(child);
        roomGrid.grid[child->X][child->Y] = child;
    }
}
*/

