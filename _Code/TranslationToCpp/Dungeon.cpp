
#include "RoomFactory.h"
#include "Constants.h"
#include "Dungeon.h"
#include <unordered_map>

Dungeon::Dungeon() {
	roomList.clear();
    Room* root = RoomFactory::CreateRoot();
	roomList.push_back(root);
    roomGrid.SetRoom(0,0,root);
	
	neededRooms = 0;
	neededLocks = 0;
	fitness = std::numeric_limits<float>::max();
}

Dungeon::~Dungeon(){
    CleanDungeon();
}

void Dungeon::CleanDungeon(){
    
    // std::cout << "clean To visit" << std::endl;
    while (!toVisit.empty()) {
        Room* roomPtr = toVisit.front(); // or back() depending on your needs
        toVisit.pop();
        delete roomPtr; // Delete the object
    }

    // std::cout << "clean roomlist " << roomList.size() << std::endl;
    
    for(Room* roomPtr : roomList){
        roomPtr->CleanRoom();
    }
    
    // Clean memory of roomList
    while (!roomList.empty()) {
        Room* roomPtr = roomList.back(); // or front() depending on your needs
        delete roomPtr;
        roomList.pop_back();
    }

    // std::cout << "Clean Grid "<< std::endl;
    roomGrid.ClearGrid();
    
}

Dungeon* Dungeon::Copy() {
    Dungeon* copyDungeon = new Dungeon();
    
    copyDungeon->CleanDungeon();  // Make sure this effectively removes any existing data

    copyDungeon->nKeys = nKeys;
    copyDungeon->nLocks = nLocks;
    copyDungeon->desiredKeys = desiredKeys;
    copyDungeon->avgChildren = avgChildren;
    copyDungeon->fitness = fitness;

    std::unordered_map<Room*, Room*> roomMap;  // Map to store old Room pointers to corresponding new Room pointers
    roomMap[nullptr] = nullptr;

    // First create all the rooms
    for (Room* oldRoom : roomList) {
        Room* newRoom = oldRoom->Copy();
        copyDungeon->roomList.push_back(newRoom);
        roomMap[oldRoom] = newRoom;  // Store the mapping
    }

    // Need to use the grid to copy the neighbors, children, and parent
    for (Room* newRoom : copyDungeon->roomList) {
        Room* oldRoom = newRoom;  // Reuse the variable for readability

        // Update links (Parent, leftChild, rightChild, bottomChild) using the mapping...
        if (oldRoom->Parent)
            newRoom->Parent = roomMap[oldRoom->Parent];
        if (oldRoom->leftChild)
            newRoom->leftChild = roomMap[oldRoom->leftChild];
        if (oldRoom->rightChild)
            newRoom->rightChild = roomMap[oldRoom->rightChild];
        if (oldRoom->bottomChild)
            newRoom->bottomChild = roomMap[oldRoom->bottomChild];

        // Once they are cleaned of the old room data
        copyDungeon->roomGrid.SetRoom(newRoom->X, newRoom->Y, newRoom);
    }

    return copyDungeon;
}



void Dungeon::CalcAvgChildren() {
    avgChildren = 0.0f;
    int childCount;
    int childLess = 0;
    for (Room* room : roomList) {
        childCount = 0;
        if (room->rightChild != nullptr && room->rightChild->Parent != nullptr)
            childCount += 1;
        if (room->leftChild != nullptr && room->leftChild->Parent != nullptr)
            childCount += 1;
        if (room->bottomChild != nullptr && room->bottomChild->Parent != nullptr)
            childCount += 1;
        if (childCount == 0)
            childLess++;
        avgChildren += childCount;
    }
    avgChildren = avgChildren / (roomList.size() - childLess);
}

void Dungeon::GenerateRooms() {

    int prob;
    toVisit.push(roomList[0]);
    while (!toVisit.empty()) {
        Room* actualRoom = toVisit.front();
        toVisit.pop();
        int actualDepth = actualRoom->depth;
        // Si se ha alcanzado la profundidad máxima permitida, se deja de crear hijos
        if (actualDepth > Constants::MAX_DEPTH) {
                while (!toVisit.empty()) {
                    toVisit.pop();
                }
                break;
        }	
                
        // Comprobar cuántos hijos tendrá el nodo, si los tiene.
        prob = Constants::Next(100);
        // std::cout << "GenerateRooms A (prob)" << prob << std::endl;
        // La probabilidad de que el nodo padre tenga hijos es del 100%, luego, en cada altura, se añade un 10% de probabilidad de que NO tenga hijos.
        // Si un nodo tiene un hijo, se crea con RoomFactory, se inserta como hijo del nodo actual en el lugar correcto
        // También se encola para ser visitado más tarde y se agrega a la lista de habitaciones de este calabozo.
        if (prob <= (Constants::PROB_HAS_CHILD * (1 - actualDepth / (Constants::MAX_DEPTH + 1)))) {
            Room* child = new Room(RoomType::normal, -1, 0);
            int prob = Constants::Next(3);
            // std::cout << "GenerateRooms B (dir)" << prob << std::endl;
            Constants::Direction dir = static_cast<Constants::Direction>(prob);

            prob = Constants::Next(101);
            // std::cout << "GenerateRooms C (prob)" << prob << std::endl;
            if (prob < Constants::PROB_1_CHILD) {
                // Enqueue internamente
                InstantiateRoom(child, actualRoom, dir);
            } else if (prob < (Constants::PROB_1_CHILD + Constants::PROB_2_CHILD)) {
                // Enqueue internamente
                InstantiateRoom(child, actualRoom, dir);
                Constants::Direction dir2;
                do {
                    prob = Constants::Next(3);
                    // std::cout << "GenerateRooms D (dir)" << prob << std::endl;
                    dir2 = static_cast<Constants::Direction>(prob);
                } while (dir == dir2);
                InstantiateRoom(child, actualRoom, dir2);
            } else {
                InstantiateRoom(child, actualRoom, Constants::Direction::right);
                InstantiateRoom(child, actualRoom, Constants::Direction::down);
                InstantiateRoom(child, actualRoom, Constants::Direction::left);
            }
        }
            
    }

    nKeys = RoomFactory::AvailableLockId.size() + RoomFactory::UsedLockId.size();
	nLocks = RoomFactory::UsedLockId.size();
}



void Dungeon::InstantiateRoom(Room* child, Room* actualRoom, Constants::Direction dir) {
	
	if (actualRoom->ValidateChild(dir, roomGrid)) {
		Room* newRoom = RoomFactory::CreateRoom();
		child = newRoom;
		actualRoom->InsertChild(dir, child, roomGrid);
        //std::cout<< "inserting child [" << actualRoom->X << ", " << actualRoom->Y << "] = " << (actualRoom->Parent == nullptr) << " | " << (actualRoom->leftChild == nullptr) << (actualRoom->bottomChild == nullptr) << (actualRoom->rightChild == nullptr) << std::endl;
		child->parentDirection = dir;
		toVisit.push(child);
		roomList.push_back(child);
		roomGrid.SetRoom(child->X,child->Y,child);
	}
}



void Dungeon::RemoveFromGrid(Room* cut) {
    // std::cout << "try to remove " << cut->roomId << std::endl;
    if (cut != nullptr) {
        // std::cout << "LBR [" << (cut->leftChild != nullptr)<< "," << (cut->bottomChild != nullptr) << ", " << (cut->rightChild != nullptr) << "]"<< std::endl;
        // Look at their childs !
        if (cut->leftChild != nullptr && cut->leftChild->Parent != nullptr && cut->leftChild->Parent->Equal(cut)) {
            RemoveFromGrid(cut->leftChild);
        }
        if (cut->bottomChild != nullptr && cut->bottomChild->Parent != nullptr && cut->bottomChild->Parent->Equal(cut)) {
            RemoveFromGrid(cut->bottomChild);
        }

        if (cut->rightChild != nullptr && cut->rightChild->Parent != nullptr && cut->rightChild->Parent->Equal(cut)) {
            RemoveFromGrid(cut->rightChild);
        }
        
        // Clean them then ...
        roomGrid.SetRoom(cut->X, cut->Y, nullptr);
        for (auto it = roomList.begin(); it != roomList.end(); ++it) {
            if ((*it)->Equal(cut)) {
                roomList.erase(it);
                break;  // break out of the loop once the element is erased
            }
        }
    }
}

void Dungeon::RefreshGrid(Room* newRoom) {
    bool hasInserted;
    if (newRoom != nullptr) {
        // std::cout << "Insert it all !!!" << newRoom->roomId << std::endl;
        roomGrid.SetRoom(newRoom->X,newRoom->Y,newRoom);
        roomList.push_back(newRoom);
        Room* aux = newRoom->leftChild;
        if (aux != nullptr && aux->Parent != nullptr && aux->Parent->Equal(newRoom)) {
            // std::cout << "Check " << aux->roomId << ": " << aux->rotation << " | " << newRoom->roomId << ": " << newRoom->rotation << std::endl;
            hasInserted = newRoom->ValidateChild(Constants::Direction::left, roomGrid);
            if (hasInserted) {
                newRoom->InsertChild(Constants::Direction::left, aux, roomGrid);
                RefreshGrid(aux);
            } else {
                newRoom->leftChild = nullptr;
            }
        }
        aux = newRoom->bottomChild;
        if (aux != nullptr && aux->Parent != nullptr && aux->Parent->Equal(newRoom)) {
            // std::cout << "Check " << aux->roomId << ": " << aux->rotation << " | " << newRoom->roomId << ": " << newRoom->rotation << std::endl;
            hasInserted = newRoom->ValidateChild(Constants::Direction::down, roomGrid);
            if (hasInserted) {
                newRoom->InsertChild(Constants::Direction::down, aux, roomGrid);
                RefreshGrid(aux);
            } else {
                newRoom->bottomChild = nullptr;
            }
        }
        aux = newRoom->rightChild;
        if (aux != nullptr && aux->Parent != nullptr && aux->Parent->Equal(newRoom)) {
            // std::cout << "Check " << aux->roomId << ": " << aux->rotation << " | " << newRoom->roomId << ": " << newRoom->rotation << std::endl;
            hasInserted = newRoom->ValidateChild(Constants::Direction::right, roomGrid);
            if (hasInserted) {
                newRoom->InsertChild(Constants::Direction::right, aux, roomGrid);
                RefreshGrid(aux);
            } else {
                newRoom->rightChild = nullptr;
            }
        }
    }
}
void Dungeon::AddLockAndKey() {

    Room* actualRoom;
    Room* child;
    actualRoom = roomList[0];
    std::queue<Room*> toVisit;
    toVisit.push(actualRoom);
    bool hasKey = false;
    bool hasLock = false;
    int lockId = -1;

    while (!toVisit.empty() && !hasLock) {
        actualRoom = toVisit.front();
        toVisit.pop();
        if (actualRoom->type == RoomType::normal && actualRoom->Equal(roomList[0]) != true) {
            if (Constants::Next(101) <= Constants::PROB_KEY_ROOM + Constants::PROB_LOCKER_ROOM) {
                if (!hasKey) {
                    (*actualRoom).type = RoomType::key;
                    (*actualRoom).roomId = Constants::getNextId();
                    (*actualRoom).keyToOpen = actualRoom->roomId;
                    lockId = actualRoom->roomId;
                    hasKey = true;
                    roomGrid.SetRoom(actualRoom->X, actualRoom->Y, actualRoom);
                } else {
                    (*actualRoom).type = RoomType::locked;
                    (*actualRoom).keyToOpen = lockId;
                    hasLock = true;
                    roomGrid.SetRoom(actualRoom->X, actualRoom->Y, actualRoom);
                }
            }
        }
        child = actualRoom->leftChild;
        if (child != nullptr && actualRoom->Equal(child->Parent)) {
            toVisit.push(child);
        }
        child = actualRoom->bottomChild;
        if (child != nullptr && actualRoom->Equal(child->Parent)) {
            toVisit.push(child);
        }
        child = actualRoom->rightChild;
        if (child != nullptr && actualRoom->Equal(child->Parent)) {
            toVisit.push(child);
        }
    }
}

void Dungeon::RemoveLockAndKey() {
    //std::cout << "Select between " << nKeys << std::endl;
    int removeKey = Constants::Next(nKeys);
    //std::cout << "Select between " << nKeys << std::endl;
    Room* actualRoom = roomList[0];
    std::queue<Room*> toVisit;
    toVisit.push(actualRoom);

    // Allways just the pair !
    bool hasKey = true;
    bool hasLock = true;
    int keyCount = 0;
    int lockId = -1;
    

    // Find the key to remove and get its corresponding lockId
    for (Room* r : roomList) {
        if (r->type == RoomType::key) {
            //std::cout << "KEYS "<< r->keyToOpen << std::endl;
            if (removeKey == keyCount) {
                lockId = r->keyToOpen;
                break;
            }
            keyCount++;
        }
    }

    //std::cout << "selected key ??? " << removeKey  << " | " << lockId << "/" << nKeys << std::endl;
    if (lockId != -1) {
        // Remove the key and its corresponding lock
        while (!toVisit.empty() && (hasLock || hasKey)) {
            actualRoom = toVisit.front();
            toVisit.pop();

            if ((actualRoom->type == RoomType::key && actualRoom->roomId == lockId) && hasKey) {
                //std::cout << " Removed key " << nKeys << std::endl;
                (*actualRoom).type = RoomType::normal;
                hasKey = false;
                (*actualRoom).keyToOpen = -1;
                // Update the grid
                roomGrid.SetRoom(actualRoom->X, actualRoom->Y, actualRoom);
                nKeys--;
            }

            if ((actualRoom->type == RoomType::locked && actualRoom->keyToOpen == lockId) && hasLock) {
                //std::cout << " Removed lock " << nLocks << std::endl;
                (*actualRoom).type = RoomType::normal;
                (*actualRoom).keyToOpen = -1;
                // Update the grid
                roomGrid.SetRoom(actualRoom->X, actualRoom->Y, actualRoom);
                nLocks--;
                hasLock = false;
                
            }


            // Add child rooms to the visit queue
            if (actualRoom->leftChild != nullptr && actualRoom->Equal(actualRoom->leftChild->Parent))
                toVisit.push(actualRoom->leftChild);

            if (actualRoom->bottomChild != nullptr && actualRoom->Equal(actualRoom->bottomChild->Parent))
                toVisit.push(actualRoom->bottomChild);

            if (actualRoom->rightChild != nullptr && actualRoom->Equal(actualRoom->rightChild->Parent))
                toVisit.push(actualRoom->rightChild);
        }
    }
    // std::cout << "Locks and keys removed ! " << nLocks  << " / " << nKeys  << std::endl;
}

void Dungeon::FixRoomList() {
    std::queue<Room*> toVisit;
    toVisit.push(roomList[0]);

    // Reset counts to 0
    nKeys = 0;
    nLocks = 0;    
    roomList.clear();

    // Perform BFS to reconstruct roomList
    while (!toVisit.empty()) {
        Room* actualRoom = toVisit.front();
        toVisit.pop();

        // Push the current room into the vector
        roomList.push_back(actualRoom);
        roomGrid.SetRoom(actualRoom->X, actualRoom->Y, actualRoom);

        // Increment counts based on room type
        if (actualRoom->type == RoomType::key)
            nKeys++;
        else if (actualRoom->type == RoomType::locked)
            nLocks++;

        // Add child rooms to the visit queue
        if (actualRoom->leftChild != nullptr)
            toVisit.push(actualRoom->leftChild);
        if (actualRoom->bottomChild != nullptr)
            toVisit.push(actualRoom->bottomChild);
        if (actualRoom->rightChild != nullptr)
            toVisit.push(actualRoom->rightChild);
    }
}




