#include "Room.h"
#include <iostream>

Room::Room(){
	X = 0;
	Y = 0;
}

Room::Room(int nX, int nY, int keyToOpen, int id, RoomType nT, Room* nParent){
    
    if (id == -1)
		roomId = Constants::getNextId();
	else
		roomId = id;
	X = nX;
	Y = nY;
	keyToOpen = keyToOpen;
	type = nT;
	Parent = nParent;
}

Room::Room(RoomType roomType = RoomType::normal, int keyToOpen = -1, int id = -1) {
	X = 0;
	Y = 0;
	if (id == -1)
		roomId = Constants::getNextId();
	else
		roomId = id;
	type = roomType;
	if (type == RoomType::key)
		this->keyToOpen = roomId;
	else if (type == RoomType::locked)
		this->keyToOpen = keyToOpen;
}

Room::~Room(){
    // Clean the Room 
    if (Parent != nullptr) delete Parent;
    if (leftChild != nullptr) delete leftChild;
    if (rightChild != nullptr) delete rightChild;
    if (bottomChild != nullptr) delete bottomChild;
}

Room* Room::Copy() {
	Room* newRoom = new Room(this->type, this->keyToOpen, this->roomId);
	newRoom->bottomChild = bottomChild;
	newRoom->leftChild = leftChild;
	newRoom->rightChild = rightChild;
	newRoom->depth = depth;
	newRoom->Parent = Parent;
	newRoom->parentDirection = parentDirection;
	newRoom->rotation = rotation;
	newRoom->X = X;
	newRoom->Y = Y;
    
	return newRoom;
}


void Room::FixBranch(std::vector<int> specialRooms) {
    std::queue<Room*> toVisit;
    std::queue<Room*> visited;
    std::queue<int> newSpecialRooms;
    int specialId;
    Room* actualRoom;
    Room* child;
    // The actual room is the root of the branch
    actualRoom = this;
    toVisit.push(actualRoom);
    // If both lock and keys are in the branch, give them new ids also, add all the special rooms in the new special rooms list
    for (int i = 0; i < ((int)specialRooms.size()-1); i++)
    {
        for (int j = i+1; j < ((int)specialRooms.size()); j++)
        {
            // std::cout << j << std::endl;
            if(specialRooms[i] == -specialRooms[j]){
                int aux = Constants::getNextId();
                if(specialRooms[i] > 0)
                    specialRooms[i] = aux;
                else
                    specialRooms[i] = -aux;
                specialRooms[j] = -specialRooms[i];
            }
        }
        
        newSpecialRooms.push(specialRooms[i]);
    }
    // Add the last special room, which normally wouldn't be added, but only if it exists
    if (!specialRooms.empty())
        newSpecialRooms.push(specialRooms.back());

    // Enqueue all the rooms
    while (!toVisit.empty()) {
        actualRoom = toVisit.front();
        toVisit.pop();
        visited.push(actualRoom);
        child = actualRoom->leftChild;
        // std::cout << "Enqueue rooms " << actualRoom->roomId << std::endl;
        if (child != nullptr)
            if (actualRoom->Equal(child->Parent)) {
                toVisit.push(child);
            }
        child = actualRoom->bottomChild;
        if (child != nullptr)
            if (actualRoom->Equal(child->Parent)) {
                toVisit.push(child);
            }
        child = actualRoom->rightChild;
        if (child != nullptr)
            if (actualRoom->Equal(child->Parent)) {
                toVisit.push(child);
            }
    }

    // std::cout << "To place randomly " << visited.size() << " " << newSpecialRooms.size() << std::endl;
    // Try to place all the special rooms in the branch randomly. If the number of remaining rooms is the same as the number of special rooms, every room must be a special one, so we finish this while loop.
    while (visited.size() > newSpecialRooms.size()) {
        actualRoom = visited.front();
        visited.pop();

        int prob = Constants::Next(101);

        // If there is a special room left, check the random number and see if it will be placed in the actual room or not
        if (!newSpecialRooms.empty()) {
            if (prob < (Constants::PROB_NORMAL_ROOM)) {
                actualRoom->type = RoomType::normal;
                actualRoom->keyToOpen = -1;
            } else {
                
                // If the room has a key, then the key must have an id and this id is added to the list of available keys
                 
                specialId = newSpecialRooms.front();
                newSpecialRooms.pop();
                if (specialId > 0) {
                    actualRoom->type = RoomType::key;
                    actualRoom->roomId = specialId;
                    actualRoom->keyToOpen = specialId;
                } else {
                    // Creates a locked room with the id of the room that contains the key to open it
                    actualRoom->type = RoomType::locked;
                    actualRoom->keyToOpen = -specialId;
                }
            }
        } else {
            actualRoom->type = RoomType::normal;
            actualRoom->keyToOpen = -1;
        }
    }
    // If there are rooms not visited, then all the next rooms must be special ones
    while (!visited.empty()) {
        actualRoom = visited.front();
        visited.pop();
        specialId = newSpecialRooms.front();
        newSpecialRooms.pop();
        if (specialId > 0) {
            actualRoom->type = RoomType::key;
            actualRoom->roomId = specialId;
            actualRoom->keyToOpen = specialId;
        } else {
            // Creates a locked room with the id of the room that contains the key to open it
            actualRoom->type = RoomType::locked;
            actualRoom->keyToOpen = -specialId;
        }
    }
    //if (!newSpecialRooms.empty())
    //    std::cout << "STOOOOOP" << std::endl;
}

bool Room::ValidateChild(Constants::Direction dir, RoomGrid roomGrid) {
    int X, Y;
    Room* roomInGrid = nullptr;
    // std::cout << "dir ???" << dir << std::endl;
    switch (dir) {
        case Constants::Direction::right:
            // Calculate X and Y based on the parent's rotation
            if (((this->rotation / 90) % 2) != 0) {
                X = this->X;
                if (this->rotation == 90)
                    Y = this->Y + 1;
                else
                    Y = this->Y - 1;
            } else {
                if (this->rotation == 0)
                    X = this->X + 1;
                else
                    X = this->X - 1;
                Y = this->Y;
            }
            // std::cout <<  X << ", " << Y << "\n";
            // Check if the room is in the grid. If not, return true; otherwise, return false
            roomInGrid = roomGrid.GetRoom(X,Y); // Assuming GetRoom method is available in RoomGrid class
            if (roomInGrid == nullptr) {
                return true;
            } else {
                return false;
            }

        case Constants::Direction::down:
            // Calculate X and Y based on the parent's rotation
            if (((this->rotation / 90) % 2) != 0) {
                Y = this->Y;
                if (this->rotation == 90)
                    X = this->X + 1;
                else
                    X = this->X - 1;
            } else {
                if (this->rotation == 0)
                    Y = this->Y - 1;
                else
                    Y = this->Y + 1;
                X = this->X;
            }
            // std::cout <<  X << ", " << Y << "\n";
            // Check if the room is in the grid. If not, return true; otherwise, return false
            roomInGrid = roomGrid.GetRoom(X,Y);
            if (roomInGrid == nullptr) {
                return true;
            } else {
                return false;
            }

        case Constants::Direction::left:
            // Calculate X and Y based on the parent's rotation
            if (((this->rotation / 90) % 2) != 0) {
                X = this->X;
                if (this->rotation == 90)
                    Y = this->Y - 1;
                else
                    Y = this->Y + 1;
            } else {
                if (this->rotation == 0)
                    X = this->X - 1;
                else
                    X = this->X + 1;
                Y = this->Y;
            }
            // std::cout <<  X << ", " << Y << "\n";
            // Check if the room is in the grid. If not, return true; otherwise, return false
            roomInGrid = roomGrid.GetRoom(X,Y); // Assuming GetRoom method is available in RoomGrid class
            if (roomInGrid == nullptr) {
                return true;
            } else {
                return false;
            }

        default:
            // std::cout <<  "Default" << "\n";
            
            std::cout << "Something went wrong while validating a child!" << dir << "\n";
            std::cout << "Direction not supported:\n\tOnly right, down, and left are allowed.\n\n";
            return false;
        
    }
    
   return false;
}

void Room::InsertChild(Constants::Direction dir, Room* child, RoomGrid roomGrid) {
    Room* roomInGrid;
    int shortcutChance;
    
    switch (dir) {
        case Constants::Direction::right:
            // Calculate the X and Y based on the parent's rotation
            if (((this->rotation / 90) % 2) != 0) {
                child->X = this->X;
                if (this->rotation == 90) {
                    child->Y = this->Y + 1;
                } else {
                    child->Y = this->Y - 1;
                }
            } else {
                if (this->rotation == 0) {
                    child->X = this->X + 1;
                } else {
                    child->X = this->X - 1;
                }
                child->Y = this->Y;
            }
            roomInGrid = roomGrid.GetRoom(child->X,child->Y);
            if (roomInGrid == nullptr) {
                child->rotation = (this->rotation + 90) % 360;
                this->rightChild = child;
                this->rightChild->SetParent(this);
            }
            break;
        case Constants::Direction::down:
            // Calculate the X and Y based on the parent's rotation
            if (((this->rotation / 90) % 2) != 0) {
                child->Y = this->Y;
                if (this->rotation == 90) {
                    child->X = this->X + 1;
                } else {
                    child->X = this->X - 1;
                }
            } else {
                if (this->rotation == 0) {
                    child->Y = this->Y - 1;
                } else {
                    child->Y = this->Y + 1;
                }
                child->X = this->X;
            }
            roomInGrid = roomGrid.GetRoom(child->X,child->Y);
            if (roomInGrid == nullptr) {
                child->rotation = (this->rotation + 90) % 360;
                this->bottomChild = child;
                this->bottomChild->SetParent(this);
            }
            break;
        case Constants::Direction::left:
            // Calculate the X and Y based on the parent's rotation
            if (((this->rotation / 90) % 2) != 0) {
                child->X = this->X;
                if (this->rotation == 90) {
                    child->Y = this->Y - 1;
                } else {
                    child->Y = this->Y + 1;
                }
            } else {
                if (this->rotation == 0) {
                    child->X = this->X - 1;
                } else {
                    child->X = this->X + 1;
                }
                child->Y = this->Y;
            }
            roomInGrid = roomGrid.GetRoom(child->X,child->Y);
            if (roomInGrid == nullptr) {
                child->rotation = (this->rotation + 90) % 360;
                this->leftChild = child;
                this->leftChild->SetParent(this);
            }
            break;
        default:
            //std::cout << "Something went wrong Creating a Child!\n";
            //std::cout << "Direction not supported:\n\tOnly Right, Down, and Left are allowed.\n\n";
            break;
    }
}    
void Room::SetParent(Room* parent) {
	this->Parent = parent;
	if (parent != nullptr) {
		this->depth = ++parent->depth;
	}
}

void Room::FindChildren(std::vector<Room*>& roomList) {
    if (rightChild != nullptr) {
        if (rightChild->Parent != nullptr && rightChild->Parent->Equal(this)) {
            roomList.push_back(rightChild);
            rightChild->FindChildren(roomList);
        }
    }
    if (bottomChild != nullptr) {
        if (bottomChild->Parent != nullptr && bottomChild->Parent->Equal(this)) {
            roomList.push_back(bottomChild);
            bottomChild->FindChildren(roomList);
        }
    }
    if (leftChild != nullptr) {
        if (leftChild->Parent != nullptr && leftChild->Parent->Equal(this)) {
            roomList.push_back(leftChild);
            leftChild->FindChildren(roomList);
        }
    }
}

bool Room::IsLeafNode() {
    if (bottomChild == nullptr && leftChild == nullptr && rightChild == nullptr) {
        return true;
    }
    return false;
}

bool Room::Equal(Room* other){
    return other-> X == this-> X && other->Y == this->Y && other->roomId == roomId && other->depth == depth && other->rotation == rotation;
}

RoomGrid::RoomGrid(){
    grid.resize(Constants::MATRIXOFFSET*2, std::vector<Room*>(Constants::MATRIXOFFSET*2,nullptr));
}


Room* RoomGrid::GetRoom(int x, int y){

    return grid[x+ Constants::MATRIXOFFSET][y+ Constants::MATRIXOFFSET];

}

void RoomGrid::SetRoom(int x, int y, Room* value){
    
    grid[x+ Constants::MATRIXOFFSET][y+ Constants::MATRIXOFFSET] = value;

}

