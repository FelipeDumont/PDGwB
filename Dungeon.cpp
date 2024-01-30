
#include "Constants.h"
#include "Dungeon.h"
#include <unordered_map>


Dungeon::Dungeon() {
	roomList.clear();
    Room* root = new Room(RoomType::normal, 0, 0);
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
    
    // Clean each room and delete them
    for (Room* roomPtr : roomList) {
        delete roomPtr;
    }

    // Clear the room list
    roomList.clear();

    // Clean The toVisit elements
    while (!toVisit.empty()) {
        toVisit.pop();
    }

    // Clear the grid elements
    roomGrid.ClearGrid();
}

Dungeon* Dungeon::Copy() {
    Dungeon* copyDungeon = new Dungeon();
    
    copyDungeon->CleanDungeon();  // Make sure this effectively removes any existing data

    copyDungeon->nKeys = nKeys;
    copyDungeon->nLocks = nLocks;
    copyDungeon->neededLocks = neededLocks;
    copyDungeon->desiredKeys = desiredKeys;
    copyDungeon->avgChildren = avgChildren;
    copyDungeon->sCoef = sCoef;
    copyDungeon->fitness = fitness;
    copyDungeon->finalRoomX = finalRoomX;
    copyDungeon->finalRoomY = finalRoomY;

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
        if (actualDepth > Constants::MAX_DEPTH || this->roomList.size() >= (Constants::nV * 2)) {
                while (!toVisit.empty()) {
                    toVisit.pop();
                }
                break;
        }	
            
        Room* child = nullptr; //Room(RoomType::normal, -1, 0);
        int prob = Constants::Next(3);
        Constants::Direction dir = static_cast<Constants::Direction>(prob);

        prob = Constants::Next(101);


        // TODO: ADD FOR THE FIRST ONE
        // Depending on the probability add 1, 2 or 3 Rooms
        if (prob < Constants::PROB_1_CHILD) {
            InstantiateRoom(child, actualRoom, dir);
        } else if (prob < (Constants::PROB_1_CHILD + Constants::PROB_2_CHILD)) {
            InstantiateRoom(child, actualRoom, dir);
            Constants::Direction dir2;
            do {
                prob = Constants::Next(3);
                dir2 = static_cast<Constants::Direction>(prob);
            } while (dir == dir2);
            InstantiateRoom(child, actualRoom, dir2);
        } else {
            InstantiateRoom(child, actualRoom, Constants::Direction::right);
            InstantiateRoom(child, actualRoom, Constants::Direction::down);
            InstantiateRoom(child, actualRoom, Constants::Direction::left);
        }
    }
    
    nKeys = 0;
	nLocks = 0;
    // count avg children
    CalcAvgChildren();
}

void Dungeon::InstantiateRoom(Room* child, Room* actualRoom, Constants::Direction dir) {
	
	if (actualRoom->ValidateChild(dir, roomGrid)) {
        
		Room* newRoom = new Room(RoomType::normal, -1, -1);
		child = newRoom;
		actualRoom->InsertChild(dir, child, roomGrid);
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


// Since it is recursive, we need to be carefull with the loop
void Dungeon::RefreshGridSymmetric(Room* newRoom, bool isFirst, bool willMutateSymmetric) {
    bool hasInserted;
    
    if (newRoom != nullptr) {
        roomGrid.SetRoom(newRoom->X,newRoom->Y,newRoom);
        roomList.push_back(newRoom);
        bool needRemove = false;

        if(willMutateSymmetric && !isFirst){
            // When changing the sons from symmetry,
            // it needs to update 
            //      the parent "sons" locations, direction and rotation
            Room* aux = newRoom->rightChild;
            if(newRoom->leftChild != nullptr){
                newRoom->rightChild = newRoom->leftChild;
                newRoom->rightChild->parentDirection = Constants::Direction::right;
                newRoom->rightChild->rotation = (newRoom->rotation + 90) % 360;
            }
            else{
                newRoom->rightChild = nullptr;
            }
            if(aux != nullptr){
                newRoom->leftChild = aux;
                newRoom->leftChild->parentDirection = Constants::Direction::left;
                newRoom->leftChild->rotation = (newRoom->rotation + 270) % 360;
            }
            else{
                newRoom->leftChild = nullptr;
            }
        }

        Room* auxLeft = newRoom->leftChild;
        Room* auxBottom = newRoom->bottomChild;
        Room* auxRight = newRoom->rightChild;

        newRoom->leftChild = nullptr;
        newRoom->bottomChild = nullptr;
        newRoom->rightChild = nullptr;

        ValidateAndInsert(newRoom, auxLeft, Constants::Direction::left, willMutateSymmetric);
        ValidateAndInsert(newRoom, auxRight, Constants::Direction::right, willMutateSymmetric);
        ValidateAndInsert(newRoom, auxBottom, Constants::Direction::down, willMutateSymmetric);
    }
}

bool Dungeon::ValidateAndInsert(Room* newRoom, Room* auxRoom, Constants::Direction currentDirection, bool willMutateSymmetric) {
    if (auxRoom != nullptr && auxRoom->Parent != nullptr && auxRoom->Parent->Equal(newRoom)) {
        bool canBeInserted = newRoom->ValidateChild(currentDirection, roomGrid);
        if (canBeInserted) {
            newRoom->InsertChild(currentDirection, auxRoom, roomGrid);
            RefreshGridSymmetric(auxRoom, false, willMutateSymmetric);
        } else {
            RemoveRoomsRecursive(auxRoom);
            return true;
        }
    }
    return false;
}

void Dungeon::RemoveRoomsRecursive(Room* actualRoom){

    // Will remove their sons and everything related to it
    if(actualRoom->leftChild != nullptr){
        RemoveRoomsRecursive(actualRoom->leftChild);
    }
    if(actualRoom->bottomChild != nullptr){
        RemoveRoomsRecursive(actualRoom->bottomChild);
    }
    if(actualRoom->rightChild != nullptr){
        RemoveRoomsRecursive(actualRoom->rightChild);
    }

    delete actualRoom;

}

// It will allways add the quantity  (key and barrier)
void Dungeon::AddLockAndKey(int quantity) {

    Room* actualRoom;
    Room* child;
    actualRoom = roomList[0];
    std::vector<Room*> toVisit;
    toVisit.push_back(actualRoom);
    bool hasKey = false;
    int lockId = -1;
    bool first = true;
    //
    if(nKeys >= Constants::nK){
        return;
    }
    // 
    while (!toVisit.empty() && quantity > 0) {
        // Will visit the dungeon at "RANDOM", the possible nodes !
        std::shuffle(toVisit.begin(),toVisit.end(),Constants::randGen);
        actualRoom = toVisit.front();
        toVisit.erase(toVisit.begin()); // Remove it !
        
        if (actualRoom->type == RoomType::normal && !first) {
            int chance = Constants::Next(101);
            if (chance <= Constants::PROB_KEY_ROOM + Constants::PROB_LOCKER_ROOM) {
                if (!hasKey) {
                    (*actualRoom).type = RoomType::key;
                    (*actualRoom).roomId = Constants::getNextId();
                    (*actualRoom).keyToOpen = actualRoom->roomId;
                    lockId = actualRoom->roomId;
                    hasKey = true;
                    roomGrid.SetRoom(actualRoom->X, actualRoom->Y, actualRoom);
                    nKeys++;
                } else {
                    (*actualRoom).type = RoomType::locked;
                    (*actualRoom).keyToOpen = lockId;
                    hasKey = false; // Get to put another one
                    roomGrid.SetRoom(actualRoom->X, actualRoom->Y, actualRoom);
                    quantity -= 1;
                    nLocks++;
                }
            }
        }
        first = false;
        child = actualRoom->leftChild;
        if (child != nullptr && actualRoom->Equal(child->Parent)) {
            toVisit.push_back(child);
        }
        child = actualRoom->bottomChild;
        if (child != nullptr && actualRoom->Equal(child->Parent)) {
            toVisit.push_back(child);
        }
        child = actualRoom->rightChild;
        if (child != nullptr && actualRoom->Equal(child->Parent)) {
            toVisit.push_back(child);
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
                hasLock = false;
                nLocks--;
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
    roomGrid.ClearGrid();

    // Perform BFS to reconstruct roomList
    while (!toVisit.empty()) {
        Room* actualRoom = toVisit.front();
        // std::cout << "Visiting ..." << actualRoom->roomId <<std::endl;
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

// MUTATION FORM
void Dungeon::AddRooms(){

    // Select a random room in the dungeon
    int cutPosition = Constants::Next(1, roomList.size());
    Room* actualRoom = roomList[cutPosition];
    bool roomWasAdded = false;
    while (actualRoom != nullptr){
        // check if it can add a node in one of the "Child" nodes
        std::vector<int> options;
        if( actualRoom->leftChild == nullptr){
            options.push_back(0);
        }
        if( actualRoom->bottomChild == nullptr){
            options.push_back(1);
        }
        if( actualRoom->rightChild == nullptr){
            options.push_back(2);
        }
        if(options.size() <= Constants::lCoef){
            while(options.size() > 0){
                int selectedID = Constants::Next(0,options.size());
                int selectedElement = options[selectedID];
                Constants::Direction dir = selectedElement == 0 ? Constants::Direction::left : selectedElement == 1 ? Constants::Direction::down : Constants::Direction::right;
                options.erase(options.begin() + selectedID);

                // Check if there is aviable space
                if(actualRoom->ValidateChild(dir, roomGrid)){
                    // std::cout << "ADDED A ROOM !!! " << std::endl;
                    Room* newRoom;
                    InstantiateRoom(newRoom, actualRoom, dir);
                    roomWasAdded = true;
                    break;// Add only 1 ROOM ...
                }
            }
        }
        if(roomWasAdded){
            break;
        }
        else{
            options.clear();
            if( actualRoom->leftChild != nullptr){
                options.push_back(0);
            }
            if( actualRoom->bottomChild != nullptr){
                options.push_back(1);
            }
            if( actualRoom->rightChild != nullptr){
                options.push_back(2);
            }
            if(options.size() > 0){
                int selectedID = Constants::Next(0,options.size());
                int selectedElement = options[selectedID];
                actualRoom = selectedElement == 0 ? actualRoom->leftChild  : selectedElement == 1 ? actualRoom->bottomChild : actualRoom->rightChild;
                options.clear();
            }
            else{
                actualRoom = nullptr;
            }
        }
    }
}

void Dungeon::RemoveRooms(){
    
    // Leave at least 2 nodes !!!
    if(roomList.size() <= 2){
        return;
    }
    // Select a random room in the dungeon
    int cutPosition = Constants::Next(1, roomList.size());
    Room* actualRoom = roomList[cutPosition];

    // Search the leaf, then remove !!! (all random)
    while(actualRoom != nullptr){
        bool isLeaf = true;
        std::vector<int> options;
        if( actualRoom->leftChild != nullptr){
            options.push_back(0);
        }
        if( actualRoom->bottomChild != nullptr){
            options.push_back(1);
        }
        if( actualRoom->rightChild != nullptr){
            options.push_back(2);
        }
        if(options.size() == 0){
            if(actualRoom->Parent->leftChild != nullptr && actualRoom->Parent->leftChild->X == actualRoom->X && actualRoom->Parent->leftChild->Y == actualRoom->Y){
                actualRoom->Parent->leftChild = nullptr;
            }
            if(actualRoom->Parent->bottomChild != nullptr && actualRoom->Parent->bottomChild->X == actualRoom->X && actualRoom->Parent->bottomChild->Y == actualRoom->Y){
                actualRoom->Parent->bottomChild = nullptr;
            }
            if(actualRoom->Parent->rightChild != nullptr && actualRoom->Parent->rightChild->X == actualRoom->X && actualRoom->Parent->rightChild->Y == actualRoom->Y){
                actualRoom->Parent->rightChild = nullptr;
            }
            // Remove the node from grid
            roomGrid.SetRoom(actualRoom->X,actualRoom->Y, nullptr);
            // Remove node from roomList
            roomList.erase(std::remove_if(roomList.begin(), roomList.end(), [actualRoom](Room* room) {
                return room->X == actualRoom->X && room->Y == actualRoom->Y;
            }), roomList.end());
            
            delete actualRoom;
            // std::cout << "REMOVED ROOM !!!" << std::endl;
            break;
        }
        else{
            int selectedID = Constants::Next(0,options.size());
            int selectedElement = options[selectedID];
            actualRoom = selectedElement == 0 ? actualRoom->leftChild  : selectedElement == 1 ? actualRoom->bottomChild : actualRoom->rightChild;
        }
    }
    
    // We will Get a room and we will remove it yes or yes    
}

void Dungeon::DisplayDungeon(){
    std::cout << fitness << std::endl;

    for (Room* r : roomList) {
        std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]" << ((int)r->type) << ": " << r->keyToOpen << std::endl;
    }
}

void Dungeon::CalculateSymmetry(){
    float notInHorizontalSymmetry = 0;
    int totalSize = int(roomList.size());
    for (int i = 1; i < totalSize; i++)
    {
        // If there is in the grid the position contrary to my own, then 
        if(roomGrid.GetRoom(roomList[i]->X * -1,roomList[i]->Y) != nullptr){
            notInHorizontalSymmetry += 1;
        }
    }
    // First room will not be in symmetry
    sCoef = notInHorizontalSymmetry/(totalSize-1);

}

    