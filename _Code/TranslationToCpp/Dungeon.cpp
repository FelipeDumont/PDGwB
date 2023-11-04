
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
    
    roomGrid.SetRoom(0,0,&root);
	
	neededRooms = 0;
	neededLocks = 0;
	fitness = std::numeric_limits<float>::max();
}

Dungeon Dungeon::Copy() {
    Dungeon copyDungeon;
    copyDungeon.toVisit = std::queue<Room>(toVisit);
    copyDungeon.roomList = std::vector<Room>(roomList);
    copyDungeon.roomGrid = roomGrid.Copy();
    copyDungeon.nKeys = nKeys;
    copyDungeon.nLocks = nLocks;
    copyDungeon.desiredKeys = desiredKeys;
    copyDungeon.avgChildren = avgChildren;
    copyDungeon.fitness = fitness;
    
    Room* aux;
    for (Room oldRoom : roomList) {
        aux = new Room(oldRoom);
        copyDungeon.roomList.push_back(*aux);
        copyDungeon.roomGrid.SetRoom(oldRoom.X,oldRoom.Y, aux);
    }
    // Need to use the grid to copy the neighbors, children, and parent
    for (Room newRoom : copyDungeon.roomList) {
        if (newRoom.Parent != nullptr) {
            newRoom.Parent = copyDungeon.roomGrid.GetRoom(newRoom.Parent->X,newRoom.Parent->Y);
        }
        if (newRoom.rightChild != nullptr) {
            newRoom.rightChild = copyDungeon.roomGrid.GetRoom(newRoom.rightChild->X,newRoom.rightChild->Y);
        }
        if (newRoom.bottomChild != nullptr) {
            newRoom.bottomChild = copyDungeon.roomGrid.GetRoom(newRoom.bottomChild->X,newRoom.bottomChild->Y);
        }
        if (newRoom.leftChild != nullptr) {
            newRoom.leftChild = copyDungeon.roomGrid.GetRoom(newRoom.leftChild->X,newRoom.leftChild->Y);
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

void Dungeon::GenerateRooms() {

    int prob;
    while (!toVisit.empty()) {
        Room actualRoom = toVisit.front();
        toVisit.pop();
        int actualDepth = actualRoom.depth;
        // Si se ha alcanzado la profundidad máxima permitida, se deja de crear hijos
        if (actualDepth > Constants::MAX_DEPTH) {
                while (!toVisit.empty()) {
                    toVisit.pop();
                }
                break;
        }	
                
        // Comprobar cuántos hijos tendrá el nodo, si los tiene.
        prob = Constants::Next100();
        // std::cout << "GenerateRooms A (prob)" << prob << std::endl;
        // La probabilidad de que el nodo padre tenga hijos es del 100%, luego, en cada altura, se añade un 10% de probabilidad de que NO tenga hijos.
        // Si un nodo tiene un hijo, se crea con RoomFactory, se inserta como hijo del nodo actual en el lugar correcto
        // También se encola para ser visitado más tarde y se agrega a la lista de habitaciones de este calabozo.
        if (prob <= (Constants::PROB_HAS_CHILD * (1 - actualDepth / (Constants::MAX_DEPTH + 1)))) {
            Room child;
            int prob = Constants::Next3();
            // std::cout << "GenerateRooms B (dir)" << prob << std::endl;
            Constants::Direction dir = static_cast<Constants::Direction>(prob);

            prob = Constants::Next101();
            // std::cout << "GenerateRooms C (prob)" << prob << std::endl;
            if (prob < Constants::PROB_1_CHILD) {
                // Enqueue internamente
                InstantiateRoom(&child, &actualRoom, dir);
            } else if (prob < (Constants::PROB_1_CHILD + Constants::PROB_2_CHILD)) {
                // Enqueue internamente
                InstantiateRoom(&child, &actualRoom, dir);
                Constants::Direction dir2;
                do {
                    prob = Constants::Next1To3();
                    // std::cout << "GenerateRooms D (dir)" << prob << std::endl;
                    dir2 = static_cast<Constants::Direction>(prob);
                } while (dir == dir2);
                InstantiateRoom(&child, &actualRoom, dir2);
            } else {
                InstantiateRoom(&child, &actualRoom, Constants::Direction::right);
                InstantiateRoom(&child, &actualRoom, Constants::Direction::down);
                InstantiateRoom(&child, &actualRoom, Constants::Direction::left);
            }
        }
            
    }
    nKeys = RoomFactory::AvailableLockId.size() + RoomFactory::UsedLockId.size();
	nLocks = RoomFactory::UsedLockId.size();
}



void Dungeon::InstantiateRoom(Room* child, Room* actualRoom, Constants::Direction dir) {
	
	if (actualRoom->ValidateChild(dir, roomGrid)) {
		Room newRoom = RoomFactory::CreateRoom();
		child = &newRoom;
		actualRoom->InsertChild(dir, child, roomGrid);
		child->parentDirection = dir;
		toVisit.push(*child);
		roomList.push_back(*child);
		roomGrid.SetRoom(child->X,child->Y,child);
	}
}

/*


void Dungeon::RemoveFromGrid(Room* cut) {
    if (cut != nullptr) {
        roomGrid.grid[cut->X][cut->Y] = nullptr;
        roomList.erase(std::remove(roomList.begin(), roomList.end(), *cut), roomList.end());
        if (cut->leftChild != nullptr && cut->leftChild->Parent != nullptr && cut->leftChild->Parent->Equal(cut)) {
            RemoveFromGrid(cut->leftChild);
        }
        if (cut->bottomChild != nullptr && cut->bottomChild->Parent != nullptr && cut->bottomChild->Parent->Equal(cut)) {
            RemoveFromGrid(cut->bottomChild);
        }

        if (cut->rightChild != nullptr && cut->rightChild->Parent != nullptr && cut->rightChild->Parent->Equal(cut)) {
            RemoveFromGrid(cut->rightChild);
        }
    }
}
*/



