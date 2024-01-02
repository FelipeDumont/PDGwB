
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
    copyDungeon->desiredKeys = desiredKeys;
    copyDungeon->avgChildren = avgChildren;
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
        if (actualDepth > Constants::MAX_DEPTH || this->roomList.size() >= Constants::nV) {
                while (!toVisit.empty()) {
                    toVisit.pop();
                }
                break;
        }	
            
        Room* child = nullptr; //Room(RoomType::normal, -1, 0);
        int prob = Constants::Next(3);
        Constants::Direction dir = static_cast<Constants::Direction>(prob);

        prob = Constants::Next(101);

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
                RemoveRoomsRecursive(aux); // Cannot be inserted so lets remove them
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
                RemoveRoomsRecursive(aux); // Cannot be inserted so lets remove them
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
                RemoveRoomsRecursive(aux); // Cannot be inserted so lets remove them
            }
        }
    }
}

void Dungeon::RemoveRoomsRecursive(Room* room){
    // Will remove their sons and everything related to it
    if(room->leftChild != nullptr){
        RemoveRoomsRecursive(room->leftChild);
    }
    if(room->bottomChild != nullptr){
        RemoveRoomsRecursive(room->bottomChild);
    }
    if(room->rightChild != nullptr){
        RemoveRoomsRecursive(room->rightChild);
    }

    delete room;

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
// MUTATION FORM

void Dungeon::AddRooms(){

    // Select a random room in the dungeon
    int cutPosition = Constants::Next(1, roomList.size());
    Room* actualRoom = roomList[cutPosition];
    bool roomWasAdded = false;
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
    // IF we fail to add just go on ...
    
    // std::cout << "Room was added ??? " << roomWasAdded << std::endl;
    
    
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

void Dungeon::SymmetricMutation(){
    
    // std::cout << "Symetric CHANGE "<< std::endl;
    // DisplayDungeon();
    // Need to replae left with eight child
    std::vector<Room*> toVisit;
    toVisit.push_back(roomList[0]);
    
    // Clean The grid
    while (!toVisit.empty()) {
        Room* aux = nullptr;
        Room* actualRoom = toVisit.front();
        toVisit.erase(toVisit.begin());
        // std::cout << "NODE DATA " << actualRoom->X << ", " << actualRoom->Y << "LR " << actualRoom->rotation<< " " << std::endl;
        if(actualRoom->IsLeafNode() == false){
            Room* lRoom = actualRoom->leftChild;
            Room* dRoom = actualRoom->bottomChild;
            Room* rRoom = actualRoom->rightChild;
            // remove them from the grid
            aux = rRoom;
            if(aux != nullptr){
                roomGrid.SetRoom(aux->X, aux->Y, nullptr);
                toVisit.push_back(aux);
            }
            aux = dRoom;
            if(aux != nullptr){
                
                roomGrid.SetRoom(aux->X, aux->Y, nullptr);
                toVisit.push_back(aux);
            }
            aux = lRoom;
            if(aux != nullptr){
                roomGrid.SetRoom(aux->X, aux->Y, nullptr);
                toVisit.push_back(aux);
            }
        }
    }

    // std::cout << std::endl;
    // Start again, but with the roomgrid empty !
    toVisit.push_back(roomList[0]);
    while (!toVisit.empty()) {
        Room* aux = nullptr;
        Room* actualRoom = toVisit.front();
        toVisit.erase(toVisit.begin()); // Remove it !
        
        if(actualRoom->IsLeafNode() == false){
            Room* lRoom = actualRoom->leftChild;
            Room* dRoom = actualRoom->bottomChild;
            Room* rRoom = actualRoom->rightChild;
            // Remove them from the parent
            actualRoom->leftChild = nullptr;
            actualRoom->bottomChild = nullptr;
            actualRoom->rightChild = nullptr;
            // remove them from the grid
            // std::cout << "Ready to replace " << std::endl;
            aux = rRoom;
            if(aux != nullptr){
                // std::cout << "From Right " << aux->X << ", " << aux->Y  << " rotation " << aux->rotation << std::endl;
                actualRoom->InsertChild(Constants::Direction::left, aux, roomGrid);
                aux->parentDirection = Constants::Direction::left;
                roomGrid.SetRoom(aux->X,aux->Y,aux);
                toVisit.push_back(aux);
                // std::cout << "To Left " << aux->X << ", " << aux->Y << " rotation " << aux->rotation<< std::endl;
            }
            aux = dRoom;
            if(aux != nullptr){
                // std::cout << "From Down  " << aux->X << ", " << aux->Y << " rotation " << aux->rotation<< std::endl;
                actualRoom->InsertChild(Constants::Direction::down, aux, roomGrid);
                aux->parentDirection = Constants::Direction::down;
                roomGrid.SetRoom(aux->X,aux->Y,aux);
                toVisit.push_back(aux);
                // std::cout << "To Down  " << aux->X << ", " << aux->Y << " rotation " << aux->rotation<< std::endl;
            }
            aux = lRoom;
            if(aux != nullptr){
                // std::cout << "From Left  " << aux->X << ", " << aux->Y << " rotation " << aux->rotation<< std::endl;
                actualRoom->InsertChild(Constants::Direction::right, aux, roomGrid);
                aux->parentDirection = Constants::Direction::right;
                roomGrid.SetRoom(aux->X,aux->Y,aux);
                toVisit.push_back(aux);
                // std::cout << "To Right  " << aux->X << ", " << aux->Y << " rotation " << aux->rotation<< std::endl;
            }
        }
    }
    // DisplayDungeon();
    
}

void Dungeon::DisplayDungeon(){
    std::cout << fitness << std::endl;

    for (Room* r : roomList) {
        std::cout << r->roomId << " [" << r->X << "," << r->Y << " ]" << ((int)r->type) << ": " << r->keyToOpen << std::endl;
    }
}

    