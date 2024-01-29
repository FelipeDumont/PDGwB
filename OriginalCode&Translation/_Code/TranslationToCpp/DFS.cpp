#include "DFS.h"
#include <algorithm>
#include <iostream>


int DFS::FindRouteDFS(Dungeon* dun) {
    // Puts the starting node in the list of open nodes and in the path
    minX = Constants::MATRIXOFFSET;
    minY = Constants::MATRIXOFFSET;
    maxX = -Constants::MATRIXOFFSET;
    maxY = -Constants::MATRIXOFFSET;
    //
    openList.clear();
    ClosedList.clear();
    path.clear();
    keys.clear();
    allLocksLocation.clear();
    lockedRooms.clear();
    InitiatePathFinding(dun);

    openList.push_back(start);
    path.push_back(start);
    neededLocks = 0;
    nVisitedRooms = 0;

    // std::cout << "A" << std::endl;
    // Visit all open nodes until none is left
    
    while (!openList.empty()) {

        
        std::string so = "";
        for (Location* loc : openList) {
            so += "[" + std::to_string(loc->X) + ", " + std::to_string(loc->Y) + "] \n";
        }

        // std::cout << so;

        // get the first
        Location* current = openList.front();

        // Handles key rooms and their locks, if it is one
        ValidateKeyRoom(current);

        // add the current square to the closed list
        ClosedList.push_back(current);
        if ((map[current->X][current->Y] >= 0 && map[current->X][current->Y] < 100) || map[current->X][current->Y] == 102) {
            // std::cout << "DFS current [" << current->X << ", " << current->Y << "] to [" << target->X << ", " << target->Y << "]" << std::endl;
            nVisitedRooms++;
        }
        
        // Check if the actual room is a locked one. If it is, add 1 to the number of locks needed to reach the goal
        for (const Location locked : allLocksLocation) {
            if (locked.X == current->X && locked.Y == current->Y) {
                neededLocks++;
                break;
            }
        }

        // remove it from the open list
        openList.erase(openList.begin());

        // if we added the destination to the closed list, we've found a path
        if (!ClosedList.empty()) {
            if (std::find_if(ClosedList.begin(), ClosedList.end(), [this](Location* l) {
                return l->X == target->X && l->Y == target->Y;
            }) != ClosedList.end()) {
                break;
            }
        }

        
        // Check all adjacent squares from the current node
        std::vector<Location*> adjacentSquares = GetWalkableAdjacentSquares(current->X, current->Y, sizeX, sizeY, map);

        // Shuffle using Fisher-Yates shuffle
        std::string randoms = "";
        for (int i = adjacentSquares.size() - 1; i > 0; i--) {
            int j = Constants::Next(0, i+1);
            Location* temp = adjacentSquares[i];
            adjacentSquares[i] = adjacentSquares[j];
            adjacentSquares[j] = temp;

            randoms += std::to_string(j)  + " " + std::to_string(Constants::currentValue) + "\n";
        }

        //std::cout << "Randoms: " << (adjacentSquares.size() - 1) << ":" << randoms;
        
        for (auto it = adjacentSquares.begin(); it != adjacentSquares.end();) {
            if (current->Parent == *it) {
                it = adjacentSquares.erase(it);
                delete *it;
            } else {
                ++it;
            }
        }

        
        // std::cout << "Adjacent size " << adjacentSquares.size() << std::endl;
        for (Location* adjacentSquare : adjacentSquares) {
            
            // if this adjacent square is already in the closed list, ignore it
            if (std::find_if(ClosedList.begin(), ClosedList.end(), [adjacentSquare](Location* l) {
                return l->X == adjacentSquare->X && l->Y == adjacentSquare->Y;
            }) != ClosedList.end()) {
                delete adjacentSquare;
                continue;
            }

            // if it's not in the open list...
            if (std::find_if(openList.begin(), openList.end(), [adjacentSquare](Location* l) {
                return l->X == adjacentSquare->X && l->Y == adjacentSquare->Y;
            }) == openList.end()) {
                adjacentSquare->Parent = current;

                // and add it to the open list and add to your path
                openList.insert(openList.begin(), adjacentSquare);
                path.push_back(adjacentSquare);
            } else {
                // adjacentSquare->Parent = current;
                delete adjacentSquare;
            }
            
        }
        
    }
        
    path.clear(); // Distributed in the Open or closed list
    locksLocation.clear();

    // Delete dynamically allocated memory in locksLocation
    for (Location locked : allLocksLocation) {
        if(locked.Parent != nullptr){
            delete locked.Parent;
        }   
    }

    for (Location* loc : openList){
        loc->Parent = nullptr;
        delete loc;
    }
    

    for (Location* loc : ClosedList){
        loc->Parent = nullptr;
        delete loc;
    }

    openList.clear();
    ClosedList.clear();

    delete target;
    return nVisitedRooms;
}

void DFS::ValidateKeyRoom(Location* current) {
    if (map[current->X][current->Y] > 0 && map[current->X][current->Y] < 100) {
        // If there is still a lock to be opened (there may be more keys than locks in the level, so the verification is necessary), find its location and check if the key to open it is the one found
        if (!locksLocation.empty()) {
            auto it = locksLocation.begin();
            while (it != locksLocation.end()) {
                Location room = *it;
                // If the key we found is the one that opens the room we are checking now, change the lock to an open corridor and update the algorithm's knowledge
                if (map[room.X][room.Y] == -map[current->X][current->Y]) {
                    map[room.X][room.Y] = 100;
                    // Remove the lock from the unopened locks location list
                    it = locksLocation.erase(it);
                    // Check if the parent room of the locked room was already closed by the algorithm (if it was in the closed list)
                    auto closedRoomIt = ClosedList.begin();
                    while (closedRoomIt != ClosedList.end()) {
                        Location* closedRoom = *closedRoomIt;
                        // If it was already closed, reopen it. Remove from the closed list and add to the open list
                        if (closedRoom->X == room.Parent->X && closedRoom->Y == room.Parent->Y) {
                            closedRoomIt = ClosedList.erase(closedRoomIt);
                            nVisitedRooms--;

                            openList.push_back(closedRoom);
                            // If the closed room was a locked one, also remove one of the needed locks, as it is now reopened and will be revisited
                            auto lockedIt = allLocksLocation.begin();
                            while (lockedIt != allLocksLocation.end()) {
                                Location locked = *lockedIt;
                                if (locked.X == closedRoom->X && locked.Y == closedRoom->Y) {
                                    neededLocks--;
                                    lockedIt = allLocksLocation.erase(lockedIt);
                                    break;
                                } else {
                                    ++lockedIt;
                                }
                            }
                            break;
                        } else {
                            ++closedRoomIt;
                        }
                    }
                    break;
                } else {
                    ++it;
                }
            }
        }
    }
}


void DFS::InitiatePathFinding(Dungeon* dun) {
    map.clear();

    for (Room* room : dun->roomList) {
        if (room->type == RoomType::key)
            keys.push_back(room->keyToOpen);
        if (room->type == RoomType::locked)
            lockedRooms.push_back(room->keyToOpen);

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
    start = new Location(-2 * minX, -2 * minY);

    // Size of the new grid
    sizeX = maxX - minX + 1;
    sizeY = maxY - minY + 1;

    map = std::vector<std::vector<int>>(2 * sizeX, std::vector<int>(2 * sizeY, 101));

    // Fill the new grid
    for (int i = minX; i < maxX + 1; ++i) {
        for (int j = minY; j < maxY + 1; ++j) {
            // Converts the original coordinates (may be negative) to positive
            int iPositive = i - minX;
            int jPositive = j - minY;

            Room* actualRoom = dun->roomGrid.GetRoom(i, j);

            // If the position has a room, check its type and fill the grid accordingly
            if (actualRoom != nullptr) {
                RoomType type = actualRoom->type;

                // 0 is a NORMAL ROOM
                if (type == RoomType::normal) {
                    map[iPositive * 2][jPositive * 2] = 0;
                }
                // The sequential, positive index of the key is its representation
                else if (type == RoomType::key) {
                    // std::cout << "[ " << iPositive * 2 << ", " << jPositive * 2 << "] K"<< std::endl; 
                    map[iPositive * 2][jPositive * 2] = FindIndex(keys, actualRoom->keyToOpen) + 1;
                }
                // If the room is locked, the room is a normal room, only the corridor is locked.
                // But if the lock is the last one in the sequential order, then the room is the objective
                else if (type == RoomType::locked) {
                    // std::cout << "[ " << iPositive * 2 << ", " << jPositive * 2 << "] L"<< std::endl; 
                    if (FindIndex(lockedRooms, actualRoom->keyToOpen) == lockedRooms.size() - 1) {
                        map[iPositive * 2][jPositive * 2] = 102;
                        target = new Location(iPositive * 2, jPositive * 2);
                    } else {
                        map[iPositive * 2][jPositive * 2] = 0;
                    }
                } else {
                    std::cout << "Something went wrong printing the tree!" << std::endl;
                    std::cout << "This Room type does not exist!" << std::endl << std::endl;
                }

                Room* parent = actualRoom->Parent;

                // If the actual room is a locked room and has a parent, then the connection between them is locked
                // and is represented by the negative value of the index of the key that opens the lock
                if (parent != nullptr) {
                    int x = parent->X - actualRoom->X + 2 * iPositive;
                    int y = parent->Y - actualRoom->Y + 2 * jPositive;

                    if (type == RoomType::locked) {
                        Location lockedLocation = Location(x, y, new Location(2 * (parent->X - actualRoom->X) + 2 * iPositive,
                                                                                   2 * (parent->Y - actualRoom->Y) + 2 * jPositive));
                        
                        // std::cout << "[ " << x << ", " << y << "] Other"<< std::endl; 
                        locksLocation.push_back(lockedLocation);

                        int test = FindIndex(keys, actualRoom->keyToOpen);
                        if (test == -1) {
                            std::cout << "There's a missing key here! What????";
                            map[x][y] = 100;
                        } else {
                            map[x][y] = -(test + 1);
                        }
                    } else {
                        // std::cout << "[ " << x << ", " << y << "] Other 100"<< std::endl; 
                        map[x][y] = 100;
                    }
                }
            }
        }
    }

    // Add all the locks location to the list that will hold their values through the execution of the algorithm
    for (Location locked : locksLocation) {
        allLocksLocation.push_back(locked);
    }
    
}

std::vector<Location*> DFS::GetWalkableAdjacentSquares(int x, int y, int sizeX, int sizeY, const std::vector<std::vector<int>> map) {
    std::vector<Location*> proposedLocations;


    if (y > 0 && map[x][y - 1] >= 0 && map[x][y - 1] != 101){
        // std::cout << x << " " << y-1 << " NEW " << std::endl;
        proposedLocations.push_back(new Location{x, y - 1});
    }
    if (y < (2 * sizeY) - 1 && map[x][y + 1] >= 0 && map[x][y + 1] != 101)
    {
        //std::cout << x << " " << y+1 << " NEW " << std::endl;
        proposedLocations.push_back(new Location{x, y + 1});
    }
    if (x > 0 && map[x - 1][y] >= 0 && map[x - 1][y] != 101)
    {
        //std::cout << x-1 << " " << y << " NEW " << std::endl;
        proposedLocations.push_back(new Location{x - 1, y});
    }
    if (x < (2 * sizeX) - 1 && map[x + 1][y] >= 0 && map[x + 1][y] != 101)
    {
        //std::cout << x+1 << " " << y << " NEW " << std::endl;
        proposedLocations.push_back(new Location{x + 1, y});
    }

    return proposedLocations;
}


int DFS::FindIndex(const std::vector<int> vec, int valueToFind) {
    auto it = std::find(vec.begin(), vec.end(), valueToFind);

    // Check if the value was found
    if (it != vec.end()) {
        // Calculate and return the index using std::distance
        return std::distance(vec.begin(), it);
    } else {
        // Return a special value (e.g., -1) to indicate that the value was not found
        return -1;
    }
}
