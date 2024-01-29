#ifndef ASTAR_H
#define ASTAR_H

#include "Dungeon.h"
#include <iostream>
#include <vector>
#include <algorithm>

class Location;
class AStar;

// Class representing the location, heuristic, and real distances of a room, along with the room used to access the current one (parent)
class Location {
public:
	int X; // X coordinate of the location
	int Y; // Y coordinate of the location
	int F = 0; // Total estimated cost of the location
	int G = 0; // Cost from the start location to the current location
	int H = 0; // Heuristic cost from the current location to the target location
	Location* Parent = nullptr; // Pointer to the parent location

	// Define the equality operator for Location objects
	bool operator==(Location other) const {
		return (X == other.X) && (Y == other.Y);
	}

	Location();
	Location(int X, int Y);
	Location(int X,int Y,Location* parent);
	Location(int X,int Y, int F, int G, int H,Location* parent);


};

// Class implementing the A* algorithm for pathfinding
class AStar {
public:
	// The A* algorithm for finding the route in the dungeon
	int FindRouteAStar(Dungeon* dun, int matrixOffset);
	// Other AStar class methods and members


};

// Retrieves the walkable adjacent squares for a given location in the map
std::vector<Location*> GetWalkableAdjacentSquares(int x, int y, const std::vector<std::vector<int>>& map);

// Finds the location with the lowest F score in a vector of locations
int findLowestF(std::vector<Location*> vec);

// Computes the heuristic score for a location based on the target location
int ComputeHScore(int x, int y, int targetX, int targetY);

// Finds the index of a target element in a vector
int findIndex(std::vector<int>& vec, int target);

// Compares two Location objects based on their F score
bool compareLocationsF(Location a, Location b);

#endif