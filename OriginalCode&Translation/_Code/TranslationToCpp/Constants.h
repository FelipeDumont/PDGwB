#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <random>
#include <algorithm>
#include <vector>

class Constants {
public:

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

	// Seed use, and the FORCED mode to test the original Algorithm !
	static int seed;
	static std::mt19937 randGen;
	static bool isForced;
	static std::vector<int> forcedRandomSequence;
	static int currentValue;

	// Main configurations !!!
	static bool useDFS;

	
	// Enumeration for directions
	enum Direction {
		right = 0,
		down = 1,
		left = 2
	};

	// Inner id counter, to generate unique ids !
	static int getNextId();
	
	// Next Functions
	static int Next(int maxValue);
	static int Next(int minValue, int maxValue);
};

#endif

