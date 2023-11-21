#include "DFS.h"
#include <algorithm>
#include <iostream>

int DFS::FindRoute(Dungeon* dun) {
    neededLocks = 0;
    nVisitedRooms = 0;
    minX = Constants::MATRIXOFFSET;
    minY = Constants::MATRIXOFFSET;
    maxX = -Constants::MATRIXOFFSET;
    maxY = -Constants::MATRIXOFFSET;
    openList.clear();
    ClosedList.clear();
    allLocksLocation.clear();
    path.clear();
    keys.clear();
    lockedRooms.clear();
    InitiatePathFinding(dun);

    openList.push_back(new Location(start->X, start->Y));
    path.push_back(new Location(start->X, start->Y));

    while (!openList.empty()) {
        /*
        for(Location* loc :openList){
            std::cout << "open [" << loc->X << ", " << loc->Y << "] " << std::endl;
        }
        */
        Location* current = openList.front();
        ValidateKeyRoom(current);
        std::cout << "DFS current [" << current->X << ", " << current->Y << "] [" << target->X << ", " << target->Y << "]" << std::endl;

        ClosedList.push_back(current);

        if (((map[current->X][current->Y] >= 0) && (map[current->X][current->Y] < 100)) || (map[current->X][current->Y] == 102)) {
            nVisitedRooms++;
        }

        for (auto locked : allLocksLocation) {
            if (locked->X == current->X && locked->Y == current->Y) {
                neededLocks++;
                break;
            }
        }

        openList.erase(openList.begin());

        bool foundTarget = false;
        for (int i = 0; i < ((int)ClosedList.size()); i++)
        {
            if(ClosedList[i]->X == target->X && ClosedList[i]->Y == target->Y){
                foundTarget = true;
            }
        }
        

        if (!ClosedList.empty() && foundTarget) {
            std::cout << "END SEARCH HERE ..." << std::endl;
            break;
        }

        std::vector<Location*> adjacentSquares = GetWalkableAdjacentSquares(current->X, current->Y, sizeX, sizeY, map);

        for(Location* loc : adjacentSquares){
            std::cout << "Adjacants [" <<loc->X << ", " << loc->Y << "]" << std::endl;
        }

        std::cout << "Randoms " << adjacentSquares.size() << ": " ;
        for (int i = adjacentSquares.size() - 1; i > 0; i--) {
            int j = Constants::Next(0, i + 1);
            Location* temp = adjacentSquares[i];
            adjacentSquares[i] = adjacentSquares[j];
            adjacentSquares[j] = temp;

            std::cout << j << " ";
        }

        std::cout << "Finished Randoms, now clean" << std::endl;
        for (auto it = adjacentSquares.begin(); it != adjacentSquares.end(); ) {
            if (current->Parent == *it) {
                it = adjacentSquares.erase(it);
            } else {
                ++it;
            }

            // Check if the iterator is at the end before incrementing
            if (it == adjacentSquares.end()) {
                break;
            }
        }



        std::cout << "adjacent size " << adjacentSquares.size() << std::endl;
        for (Location* adjacentSquare : adjacentSquares) {
            if (std::any_of(ClosedList.begin(), ClosedList.end(),
                            [&adjacentSquare](const Location* l) {
                                return l->X == adjacentSquare->X && l->Y == adjacentSquare->Y;
                            })) {
                continue;
            }

            auto openListIt = std::find_if(openList.begin(), openList.end(),
                                        [&adjacentSquare](const Location* l) {
                                            return l->X == adjacentSquare->X && l->Y == adjacentSquare->Y;
                                        });

            if (openListIt == openList.end()) {
                std::cout << "adding " << adjacentSquare->X << ", " << adjacentSquare->Y << std::endl;
                adjacentSquare->Parent = current;
                openList.insert(openList.begin(), new Location(*adjacentSquare));
                path.push_back(new Location(*adjacentSquare));
            } else {
                (*openListIt)->Parent = current;
            }
        }

    }

    
    delete target;
    delete start;

    // Clean up dynamically allocated memory in the ClosedList and openList
    for (Location* loc : ClosedList) {
        delete loc;
    }
    for (Location* loc : openList) {
        delete loc;
    }

    return nVisitedRooms;
}

void DFS::ValidateKeyRoom(Location* current) {
    if (map[current->X][current->Y] > 0 && map[current->X][current->Y] < 100) {
        if (!locksLocation.empty()) {
            for (auto it = locksLocation.begin(); it != locksLocation.end(); ) {
                Location locit = **it;
                if (map[locit.X][locit.Y] == -map[current->X][current->Y]) {
                    map[locit.X][locit.Y] = 100;
                    auto lockedIt = std::find_if(allLocksLocation.begin(), allLocksLocation.end(),
                                                 [&locit](Location* l) { return l->X == locit.X && l->Y == locit.Y; });

                    if (lockedIt != allLocksLocation.end()) {
                        neededLocks--;
                        allLocksLocation.erase(lockedIt);
                    }

                    it = locksLocation.erase(it);

                    for (auto closedRoomIt = ClosedList.begin(); closedRoomIt != ClosedList.end(); ) {
                        Location loc = **closedRoomIt;
                        if ((*closedRoomIt)->X == locit.Parent->X && (*closedRoomIt)->Y == locit.Parent->Y) {
                            closedRoomIt = ClosedList.erase(closedRoomIt);
                            nVisitedRooms--;

                            openList.push_back(new Location(loc));

                            break;
                        } else {
                            ++closedRoomIt;
                        }
                    }
                } else {
                    ++it;
                }

                // Check if the iterator is at the end before incrementing
                if (it == locksLocation.end()) {
                    break;
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
                    map[iPositive * 2][jPositive * 2] = FindIndex(keys, actualRoom->keyToOpen) + 1;
                }
                // If the room is locked, the room is a normal room, only the corridor is locked.
                // But if the lock is the last one in the sequential order, then the room is the objective
                else if (type == RoomType::locked) {
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
                        Location* lockedLocation = new Location(x, y, new Location(2 * (parent->X - actualRoom->X) + 2 * iPositive,
                                                                                   2 * (parent->Y - actualRoom->Y) + 2 * jPositive));
                        locksLocation.push_back(lockedLocation);

                        int test = FindIndex(keys, actualRoom->keyToOpen);
                        if (test == -1) {
                            std::cout << "There's a missing key here! What????";
                            map[x][y] = 100;
                        } else {
                            map[x][y] = -(test + 1);
                        }
                    } else {
                        map[x][y] = 100;
                    }
                }
            }
        }
    }

    // Add all the locks location to the list that will hold their values through the execution of the algorithm
    for (Location* locked : locksLocation) {
        allLocksLocation.push_back(new Location(*locked));
    }
}

std::vector<Location*> DFS::GetWalkableAdjacentSquares(int x, int y, int sizeX, int sizeY, const std::vector<std::vector<int>> map) {
    std::vector<Location*> proposedLocations;


    if (y > 0 && map[x][y - 1] >= 0 && map[x][y - 1] != 101){
        std::cout << x << " " << y-1 << " NEW " << std::endl;
        proposedLocations.push_back(new Location{x, y - 1});
    }
    if (y < (2 * sizeY) - 1 && map[x][y + 1] >= 0 && map[x][y + 1] != 101)
    {
        std::cout << x << " " << y+1 << " NEW " << std::endl;
        proposedLocations.push_back(new Location{x, y + 1});
    }
    if (x > 0 && map[x - 1][y] >= 0 && map[x - 1][y] != 101)
    {
        std::cout << x-1 << " " << y << " NEW " << std::endl;
        proposedLocations.push_back(new Location{x - 1, y});
    }
    if (x < (2 * sizeX) - 1 && map[x + 1][y] >= 0 && map[x + 1][y] != 101)
    {
        std::cout << x+1 << " " << y << " NEW " << std::endl;
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

