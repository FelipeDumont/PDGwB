

#include "AStar.h"


// Size of the grid
int sizeX;
int sizeY;

Location::Location(){}

Location::Location(int X, int Y)
{
    this->X = X;
    this->Y = Y;
}
Location::Location(int X, int Y, Location* parent)
{
    this->X = X;
    this->Y = Y;
    this->Parent = parent;
}

Location::Location(int X,int Y, int F, int G, int H,Location* parent){
    this->X = X;
    this->Y = Y;
    this->F = F;
    this->G = G;
    this->H = H;
    this->Parent = parent;
}

int AStar::FindRoute(Dungeon dun, int matrixOffset) {

    // The path through the dungeon
    std::vector<Location> path;

    // Current, starting and target locations
    Location current;
    Location* start;
    Location* target = nullptr;

    // List of locations of the locks that were still not opened
    std::vector<Location> locksLocation;

    // List of locations of all the locks since the start of the algorithm
    std::vector<Location> allLocksLocation;

    // Counter for all the locks that were opened during the A* execution
    int neededLocks = 0;

    // The actual room being visited and its parent, to search for locks
    Room* actualRoom;
    Room* parent;

    // Type of the room
    RoomType type;

    // X and Y coordinates and their respective conversion to only-positive values
    int x, y, iPositive, jPositive;

    // List of all the locked rooms
    std::vector<int> lockedRooms;

    // List of all the keys
    std::vector<int> keys;

    // Min and max boundaries of the grid based on the original grid
    int minX, minY, maxX, maxY;
    minX = matrixOffset;
    minY = matrixOffset;
    maxX = -matrixOffset;
    maxY = -matrixOffset;

    

     // Check all the rooms and add them to the keys and locks lists if they are one of them
    for (Room* room : dun.roomList) {
        if (room->type == RoomType::key) {
            keys.push_back(room->keyToOpen);
            // std::cout << "["<< room->X << ", " << room->Y << "] " << "adding Key [ " << room->keyToOpen << "]" <<std::endl;
        }
        if (room->type == RoomType::locked) {
            lockedRooms.push_back(room->keyToOpen);
            // std::cout << "["<< room->X << ", " << room->Y << "] " << "adding Lock [ " << room->keyToOpen << "]" <<std::endl;
        }

        // Check the boundaries of the farthest rooms in the grid
        if (room->X < minX)
            minX = room->X;
        if (room->Y < minY)
            minY = room->Y;
        if (room->X > maxX)
            maxX = room->X;
        if (room->Y > maxY)
            maxY = room->Y;
    }

    // The starting location is room (0,0)
    start = new Location{ -2 * minX, -2 * minY };
    
    // std::cout << "Mins:" << minX  << ","<< minY << std::endl;
    // std::cout << "Maxs:" << maxX  << ","<< maxY << std::endl;
    // std::cout << "Start: " << start->X << " " << start->Y << std::endl;

    // List of visited rooms that are not closed yet
    std::vector<Location> openList;

    // List of closed rooms. They were visited and all neighbors explored.
    std::vector<Location> closedList;

    // g score
    int g = 0;

    // Size of the new grid
    sizeX = maxX - minX + 1;
    sizeY = maxY - minY + 1;
	// std::cout << sizeX << " " << sizeY << std::endl;

    // Instantiate the grid, check 101 ...
    std::vector<std::vector<int>> map(2 * sizeX, std::vector<int>(2 * sizeY, 101));
    // std::cout << 2 * sizeX << " " << 2 * sizeY << "\n\n";

    // std::cout << "Create new grid" << "\n\n";
    // Fill the new grid
    for (int i = minX; i < maxX + 1; ++i) {
        for (int j = minY; j < maxY + 1; ++j) {
            
            // Converts the original coordinates (may be negative) to positive
            iPositive = i - minX;
            jPositive = j - minY;
            // std::cout<< "Check the room [" << i << " " << j << "] = "<< (dun.roomGrid.GetRoom(i,j) == nullptr) << std::endl;
            
            actualRoom = dun.roomGrid.GetRoom(i,j);

            if(actualRoom != nullptr){
                // If the position has a room, check its type and fill the grid accordingly
                type = actualRoom->type;

                // 0 is a NORMAL ROOM
                if (type == RoomType::normal) {
                    map[iPositive * 2][jPositive * 2] = 0;
                    // std::cout << "NR ["<<  iPositive * 2 << ", " << jPositive * 2 << "] = 0" << std::endl;
                }
                // The sequential, positive index of the key is its representation
                else if (type == RoomType::key) {
                    map[iPositive * 2][jPositive * 2] = findIndex(keys,actualRoom->keyToOpen) + 1;
                    // std::cout << "KR ["<<  iPositive * 2 << ", " << jPositive * 2 << "] = " << map[iPositive * 2][jPositive * 2] << std::endl;
                }
                // If the room is locked, the room is a normal room, only the corridor is locked. But if the lock is the last one in the sequential order, then the room is the objective
                else if (type == RoomType::locked) {
                    if (findIndex(lockedRooms, actualRoom->keyToOpen) == lockedRooms.size() - 1) {
                        map[iPositive * 2][jPositive * 2] = 102;
                        target = new Location{ iPositive * 2, jPositive * 2 };
                        // std::cout << "LR ["<<  iPositive * 2 << ", " << jPositive * 2 << "] = " << map[iPositive * 2][jPositive * 2]<< std::endl;
                    } else
                        map[iPositive * 2][jPositive * 2] = 0;
                        // std::cout << "LR ["<<  iPositive * 2 << ", " << jPositive * 2 << "] = " << map[iPositive * 2][jPositive * 2]<< std::endl;
                } else {
                    // std::cout << "Something went wrong printing the tree!\n";
                    // std::cout << "This Room type does not exist!\n\n";
                }

                parent = actualRoom->Parent;
                // If the actual room is a locked room and has a parent, then the connection between them is locked and is represented by the negative value of the index of the key that opens the lock
                if (parent != nullptr) {
                

                    x = parent->X - actualRoom->X + 2 * iPositive;
                    y = parent->Y - actualRoom->Y + 2 * jPositive;
                    
                    if (type == RoomType::locked) {
                        Location* loc = new Location{ x, y,0,0,0, new Location{ 2 * (parent->X - actualRoom->X) + 2 * iPositive, 2 * (parent->Y - actualRoom->Y) + 2 * jPositive } };
                        locksLocation.push_back(*loc);
                        map[x][y] = -(findIndex(keys, actualRoom->keyToOpen) + 1);
                        // std::cout << "CR ["<<  x << ", " << y << "] = " << map[x][y] << "index of" << findIndex(keys, actualRoom->keyToOpen) << std::endl;
                    }
                    // If the connection is open, 100 represents a normal corridor
                    else{
                        map[x][y] = 100;
                        // std::cout << "CR ["<<  x << ", " << y << "] = " << map[x][y] << std::endl;
                    }
                    
                }
                
            } 
            
        }
    }
	
	// std::cout <<  "S" << start->X << " "  << start->Y << std::endl;
	// std::cout <<  "T" << target->X << " "  << target->Y << std::endl;

	
    // Add all the locks location to the list that will hold their values through the execution of the algorithm
    for (Location locked : locksLocation) {
        allLocksLocation.push_back(locked);
        // std::cout <<"Locked room "  <<  locked.X << " "  << locked.Y << std::endl;
    }

    // Start by adding the original position to the open list
    openList.push_back(*start);

	// std::cout << "Start the real A* with the new Map" << std::endl;
    // While there are rooms to visit in the path
    while (openList.size() > 0) {
        // Get the square with the lowest F score
        int lowest = findLowestF(openList);

        current = openList[lowest];
        // std::cout << "[ "<< current.X << "," << current.Y << "]" << std::endl;
    
        // If the current is a key, change the locked door status in the map
        if (map[current.X][current.Y] > 0 && map[current.X][current.Y] < 100) {
            // If there is still a lock to be open (there may be more keys than locks in the level, so the verification is necessary) find its location and check if the key to open it is the one found
            if (locksLocation.size() > 0) {
                for (Location room : locksLocation) {
                    // If the key we found is the one that opens the room we are checking now, change the lock to an open corridor and update the algorithm's knowledge
                    if (map[room.X][room.Y] == -map[current.X][current.Y]) {
                        map[room.X][room.Y] = 100;
                        // Remove the lock from the unopened locks location list
                        locksLocation.erase(std::remove(locksLocation.begin(), locksLocation.end(), room), locksLocation.end());

                        // Check if the parent room of the locked room was already closed by the algorithm (if it was in the closed list)
                        for (Location closedRoom : closedList) {
                            // If it was already closed, reopen it. Remove from the closed list and add to the open list
                            if (closedRoom.X == room.Parent->X && closedRoom.Y == room.Parent->Y) {
                                closedList.erase(std::remove(closedList.begin(), closedList.end(), closedRoom), closedList.end());
                                openList.push_back(closedRoom);

                                // If the closed room was a locked one, also remove one of the needed locks, as it is now reopened and will be revisited
                                for (Location locked : allLocksLocation) {
                                    if (locked.X == closedRoom.X && locked.Y == closedRoom.Y) {
                                        neededLocks--;
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }

        // Add the current square to the closed list
        closedList.push_back(current);

        // Check if the actual room is a locked one. If it is, add 1 to the number of locks needed to reach the goal
        for (Location locked : allLocksLocation) {
            if (locked.X == current.X && locked.Y == current.Y) {
                neededLocks++;
                break;
            }
        }

        // Remove it from the open list
        openList.erase(std::remove(openList.begin(), openList.end(), current), openList.end());

        // If we added the destination to the closed list, we've found a path (END)
        if (std::find_if(closedList.begin(), closedList.end(), [&](const Location& l) {
            return l.X == target->X && l.Y == target->Y;
        }) != closedList.end()) {
            break;
        }

        // Get adjacent squares to the current one
        std::vector<Location> adjacentSquares = GetWalkableAdjacentSquares(current.X, current.Y, map);
        g++;

        // Visit each adjacent node
        for (Location& adjacentSquare : adjacentSquares) {
            // If this adjacent square is already in the closed list, ignore it
            if (std::find_if(closedList.begin(), closedList.end(), [&](const Location& l) {
                return l.X == adjacentSquare.X && l.Y == adjacentSquare.Y;
            }) != closedList.end()) {
                continue;
            }

            // If it's not in the open list...
            if (std::find_if(openList.begin(), openList.end(), [&](const Location& l) {
                return l.X == adjacentSquare.X && l.Y == adjacentSquare.Y;
            }) == openList.end()) {
                // Compute its score, set the parent
                adjacentSquare.G = g;
                adjacentSquare.H = ComputeHScore(adjacentSquare.X, adjacentSquare.Y, target->X, target->Y);
                adjacentSquare.F = adjacentSquare.G + adjacentSquare.H;
                adjacentSquare.Parent = &current;
                // std::cout << "NEw : "<< adjacentSquare.X << "," << adjacentSquare.Y  << " = " << adjacentSquare.F << std::endl;

                // Add it to the open list
                path.push_back(adjacentSquare);
                openList.insert(openList.begin(), adjacentSquare);
            } else {
                // Test if using the current G score makes the adjacent square's F score
                // lower, if yes, update the parent because it means it's a better path
                if (g + adjacentSquare.H < adjacentSquare.F) {
                    adjacentSquare.G = g;
                    adjacentSquare.F = adjacentSquare.G + adjacentSquare.H;
                    adjacentSquare.Parent = &current;
                    // std::cout << "NEw : "<< adjacentSquare.X << "," << adjacentSquare.Y  << " = " << adjacentSquare.F << std::endl;
                }
            }
        }
    }

	// std::cout << "Locks " << neededLocks << " :C :C :C \n\n";

    return neededLocks;
}

// Check what adjacent rooms exist and can be visited and return the valid ones
std::vector<Location> GetWalkableAdjacentSquares(int x, int y, std::vector<std::vector<int>> map) {
    std::vector<Location> proposedLocations;

    if (y > 0)
        if(map[x][y-1] >= 0 && map[x][y-1] != 101)
            proposedLocations.push_back({x, y - 1});
    if (y < (2 * sizeY) - 1)
        if(map[x][y+1] >= 0 && map[x][y+1] != 101)
            proposedLocations.push_back({x, y + 1});
    if (x > 0)
        if(map[x-1][y] >= 0 && map[x-1][y] != 101)
            proposedLocations.push_back({x - 1, y});
    if (x < (2 * sizeX) - 1)
        if(map[x+1][y] >= 0 && map[x+1][y] != 101)
            proposedLocations.push_back({x + 1, y});

    return proposedLocations;
}

// Compute the heuristic score, in this case, a Manhattan Distance
int ComputeHScore(int x, int y, int targetX, int targetY) {
	return std::abs(targetX - x) + std::abs(targetY - y);
}

int findIndex(std::vector<int>& vec, int target){
	for(int i = 0; i < vec.size(); ++i){
		if(vec[i] == target){
			return i;
		}
	}
	return -1;
}

int findLowestF(std::vector<Location> vec){
	int idLowest = 0;
	int lowest = vec[0].F;
	for(int i = 0; i < vec.size(); ++i){
		if(vec[i].F < lowest){
			lowest = vec[i].F;
			idLowest = i;
		}
	}
	return idLowest;
}


bool compareLocationsF(Location a, Location b){
	return a.F > b.F;
}


