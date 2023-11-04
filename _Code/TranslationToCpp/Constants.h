#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <typeinfo>
#include <random>
#include <algorithm>


class Constants;
class Dimensions;
class Coordinates;
class Boundaries;

// Constants used in the program
class Constants {
public:
	// Maximum number of children allowed for a node
	// static constexpr int MAX_CHILDREN = 3;

	// Probability of a node having children
	static constexpr float PROB_HAS_CHILD = 100.0f;

	// Probability of having 1, 2, and 3 children respectively
	static constexpr float PROB_1_CHILD = 100.0f / 3;
	static constexpr float PROB_2_CHILD = 100.0f / 3;
	static constexpr float PROB_3_CHILD = 100.0f / 3;

	// Probability of a normal room, a key room, and a locker room
	static constexpr float PROB_NORMAL_ROOM = 70.0f;
	static constexpr float PROB_KEY_ROOM = 15.0f;
	static constexpr float PROB_LOCKER_ROOM = 15.0f;

	// Crossover and mutation rates for the algorithm
	static constexpr float CROSSOVER_RATE = 90.0f;
	static constexpr float MUTATION_RATE = 5.0f;
	static constexpr float MUTATION0_RATE = 50.0f;
	static constexpr float MUTATION1_RATE = 50.0f;

	// Offset for matrix calculations
	static constexpr int MATRIXOFFSET = 150;
	static int ID;

	// Algorithm Parameters
	static int MAX_DEPTH; // Maximum depth for the algorithm
	static int POP_SIZE; // Population size for the algorithm
	static int GENERATIONS; // Number of generations for the algorithm

	// Fitness parameters
	static int nV, nK, nL; // Parameters related to fitness calculation nV(Rooms) nK(Keys) nL(Locks)
	static float lCoef; // Parameter related to fitness calculation (linear Coeficient avgChildNodes)

	static int seed;
	static std::mt19937 randGen;
	static std::uniform_int_distribution<int> prob100;
	static std::uniform_int_distribution<int> prob101;
	static std::uniform_int_distribution<int> prob3;// 0,1,2
	static std::uniform_int_distribution<int> prob1To3;// 0,1,2
    
	static std::vector<int> randomSequence;
	static bool isForced;
	static std::vector<int> forcedRandomSequence;
	// Enumeration for directions
	enum Direction {
		right = 0,
		down = 1,
		left = 2
	};
	
	static int getNextId();
	static int SelectRandom(int value);
	static int Next100();
	static int Next101();
	static int Next3();
	static int Next1To3();
};

// Dimensions class to handle width and height of objects
class Dimensions {
private:
    int width, height;

public:
	// Constructor to initialize dimensions
	Dimensions(int width, int height) : width(width), height(height) {}
	Dimensions(){}
	// Getter for width
	int getWidth() const { return width; }

	// Setter for width
	void setWidth(int value) { width = value; }

	// Getter for height
	int getHeight() const { return height; }

	// Setter for height
	void setHeight(int value) { height = value; }
};

// Coordinates class to handle x and y coordinates of objects
class Coordinates {
private:
	int x, y;

public:
	// Constructor to initialize coordinates
	Coordinates(int x, int y) : x(x), y(y) {}
	Coordinates(){}

	// Getter for x coordinate
	int getX() const { return x; }

	// Setter for x coordinate
	void setX(int value) { x = value; }

	// Getter for y coordinate
	int getY() const { return y; }

	// Setter for y coordinate
	void setY(int value) { y = value; }

	// Check if two Coordinates objects are equal
	bool equals(const Coordinates& obj) const {
		if (this == &obj)
			return true;
		if (typeid(*this) != typeid(obj))
			return false;
		const Coordinates& coordinates = obj;
		return ((coordinates.x == x) && (coordinates.y == y));
	}
};

// Boundaries class to handle the boundaries of objects
class Boundaries {
protected:
	Coordinates maxBoundaries;
	Coordinates minBoundaries;

public:
	// Constructor to initialize boundaries
	Boundaries(const Coordinates& minBoundaries, const Coordinates& maxBoundaries)
	: minBoundaries(minBoundaries), maxBoundaries(maxBoundaries) {}

	// Getter for minimum boundaries
	const Coordinates& getMinBoundaries() const { return minBoundaries; }

	// Setter for minimum boundaries
	void setMinBoundaries(const Coordinates& value) { minBoundaries = value; }

	// Getter for maximum boundaries
	const Coordinates& getMaxBoundaries() const { return maxBoundaries; }

	// Setter for maximum boundaries
	void setMaxBoundaries(const Coordinates& value) { maxBoundaries = value; }
	
	Boundaries(){}
};


#endif

