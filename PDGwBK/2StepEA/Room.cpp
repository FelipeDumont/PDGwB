#include "Room.h"
#include <iostream>

Room::Room() : X(0), Y(0) {}

Room::Room(int nX, int nY, int keyToOpen, int id, RoomType nT, Room* nParent) :
    X(nX), Y(nY), keyToOpen(keyToOpen), type(nT), Parent(nParent){
    
    if (id == -1)
		roomId = Constants::getNextId();
	else
		roomId = id;
}

Room::Room(RoomType roomType = RoomType::normal, int keyToOpen = -1, int id = -1) :
    X(0), Y(0), type(roomType) {
	if (id == -1)
		roomId = Constants::getNextId();
	else
		roomId = id;
	if (type == RoomType::key)
		this->keyToOpen = roomId;
	else if (type == RoomType::locked)
		this->keyToOpen = keyToOpen;
}

Room::~Room(){
    CleanRoom();
}

Room* Room::Copy() {
	Room* newRoom = new Room(this->type, this->keyToOpen, this->roomId);
	newRoom->bottomChild = bottomChild;
	newRoom->leftChild = leftChild;
	newRoom->rightChild = rightChild;
    newRoom->upChild = upChild;
	newRoom->depth = depth;
	newRoom->Parent = Parent;
	newRoom->parentDirection = parentDirection;
	newRoom->rotation = rotation;
	newRoom->X = X;
	newRoom->Y = Y;
    
	return newRoom;
}

void Room::CleanRoom(){
    // Clean the Room 
    Parent = nullptr;
    leftChild = nullptr;
    rightChild = nullptr;
    bottomChild = nullptr;
}

bool Room::ValidateChild(Constants::Direction dir, RoomGrid roomGrid) {
    int X, Y;
    X = this->X;
    Y = this->Y;

    Room* roomInGrid = nullptr;
    // Calculate X and Y based on the parent's rotation
    switch (dir) {
        case Constants::Direction::right:
            if (this->rotation == 0) {
                X = this->X + 1;
            } 
            else if (this->rotation == 90) {
                Y = this->Y - 1;
            }
            else if (this->rotation == 180) {
                X = this->X - 1;
            }
            else if (this->rotation == 270) {
                Y = this->Y + 1;
            }
            // Check if the room is in the grid. If not, return true; otherwise, return false
            roomInGrid = roomGrid.GetRoom(X,Y);
            if (roomInGrid == nullptr) {
                // std::cout << "ValidatedPOsition " << X << ", " << Y << std::endl;
                return true;
            } else {
                return false;
            }

        case Constants::Direction::down:
            if (this->rotation == 0) {
                Y = this->Y - 1;
            } 
            else if (this->rotation == 90) {
                X = this->X - 1;
            }
            else if (this->rotation == 180) {
                Y = this->Y + 1;
            }
            else if (this->rotation == 270) {
                X = this->X + 1;
            }
            // Check if the room is in the grid. If not, return true; otherwise, return false
            roomInGrid = roomGrid.GetRoom(X,Y);
            if (roomInGrid == nullptr) {
                // std::cout << "ValidatedPOsition " << X << ", " << Y << std::endl;
                return true;
            } else {
                return false;
            }

        case Constants::Direction::left:
            if (this->rotation == 0) {
                X = this->X - 1;
            } 
            else if (this->rotation == 90) {
                Y = this->Y + 1;
            }
            else if (this->rotation == 180) {
                X = this->X + 1;
            }
            else if (this->rotation == 270) {
                Y = this->Y - 1;
            }
            // Check if the room is in the grid. If not, return true; otherwise, return false
            roomInGrid = roomGrid.GetRoom(X,Y); // Assuming GetRoom method is available in RoomGrid class
            if (roomInGrid == nullptr) {
                // std::cout << "ValidatedPOsition " << X << ", " << Y << std::endl;
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
    
    child->X = this->X;
    child->Y = this->Y;

    switch (dir) {
        case Constants::Direction::right:
            // Calculate the X and Y based on the parent's rotation
            
            if (this->rotation == 0) {
                child->X = this->X + 1;
            } 
            else if (this->rotation == 90) {
                child->Y = this->Y - 1;
            }
            else if (this->rotation == 180) {
                child->X = this->X - 1;
            }
            else if (this->rotation == 270) {
                child->Y = this->Y + 1;
            }
            roomInGrid = roomGrid.GetRoom(child->X,child->Y);
            if (roomInGrid == nullptr) {
                child->rotation = (this->rotation + 90) % 360;
                this->rightChild = child;
                this->rightChild->SetParent(this);
            }
            else{
                std::cout << "room IN Grid" << std::endl;
            }   
            break;
        case Constants::Direction::down:
            // Calculate the X and Y based on the parent's rotation
            if (this->rotation == 0) {
                child->Y = this->Y - 1;
            } 
            else if (this->rotation == 90) {
                child->X = this->X - 1;
            }
            else if (this->rotation == 180) {
                child->Y = this->Y + 1;
            }
            else if (this->rotation == 270) {
                child->X = this->X + 1;
            }
            roomInGrid = roomGrid.GetRoom(child->X,child->Y);
            if (roomInGrid == nullptr) {
                child->rotation = (this->rotation) % 360;
                this->bottomChild = child;
                this->bottomChild->SetParent(this);
            }
            else{
                std::cout << "room IN Grid" << std::endl;
            }
            break;
        case Constants::Direction::left:
            // Calculate the X and Y based on the parent's rotation
            if (this->rotation == 0) {
                child->X = this->X - 1;
            } 
            else if (this->rotation == 90) {
                child->Y = this->Y + 1;
            }
            else if (this->rotation == 180) {
                child->X = this->X + 1;
            }
            else if (this->rotation == 270) {
                child->Y = this->Y - 1;
            }
            roomInGrid = roomGrid.GetRoom(child->X,child->Y);
            if (roomInGrid == nullptr) {
                child->rotation = (this->rotation + 270) % 360;
                this->leftChild = child;
                this->leftChild->SetParent(this);
            }
            else{
                std::cout << "room IN Grid" << std::endl;
            }   
            break;
        default:
            //std::cout << "Something went wrong Creating a Child!\n";
            //std::cout << "Direction not supported:\n\tOnly Right, Down, and Left are allowed.\n\n";
            break;
    }
    // std::cout << child->roomId << " Final Position ["<< child->X << "," << child->Y << "]" << std::endl;
}





void Room::SetParent(Room* parent) {
	this->Parent = parent;
	if (parent != nullptr) {
		this->depth = ++parent->depth;
	}
}

bool Room::IsLeafNode() {
    if (bottomChild == nullptr && leftChild == nullptr && rightChild == nullptr) {
        return true;
    }
    return false;
}

bool Room::Equal(Room* other){
    return other-> X == this-> X && other->Y == this->Y && other->depth == depth && other->rotation == rotation;
}

RoomGrid::RoomGrid(){
    // grid.resize(Constants::MATRIXOFFSET*2, std::vector<Room*>(Constants::MATRIXOFFSET*2,nullptr));
    grid.resize((Constants::MATRIXOFFSET*2)*(Constants::MATRIXOFFSET*2),nullptr);
}


Room* RoomGrid::GetRoom(int x, int y){

    // return grid[x+ Constants::MATRIXOFFSET][y+ Constants::MATRIXOFFSET];
    return grid[(x+ Constants::MATRIXOFFSET) + ((y+ Constants::MATRIXOFFSET) *Constants::MATRIXOFFSET)];
}

void RoomGrid::SetRoom(int x, int y, Room* value){
    
    // grid[x+ Constants::MATRIXOFFSET][y+ Constants::MATRIXOFFSET] = value;
    grid[(x+ Constants::MATRIXOFFSET) + ((y+ Constants::MATRIXOFFSET) *Constants::MATRIXOFFSET)] = value;

}

void RoomGrid::ClearGrid() {

    grid.clear();
    // Resize to be used, but with nullptr
    grid.resize((Constants::MATRIXOFFSET*2)*(Constants::MATRIXOFFSET*2),nullptr);
}
