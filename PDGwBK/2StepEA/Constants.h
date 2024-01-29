#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <random>
#include <algorithm>
#include <vector>

class Constants {
public:

	/***************************************************/                     
	/***************  Testign Parameters ***************/
	/***************************************************/ 
	static bool testingMode;

	/***************************************************/                     
	/****************  Default SETTINGS ****************/
	/***************************************************/  
	// Probability of a node having children
	static constexpr float PROB_HAS_CHILD = 100.0f;

	// Probability of having 1, 2, and 3 children respectively
	static constexpr float PROB_1_CHILD = 100.0f / 3;
	static constexpr float PROB_2_CHILD = 100.0f / 3;

	/***************************************************/                     
	/***************  PARAMETER SETTINGS ***************/
	/***************************************************/                     

	// Probability of a normal room, a key room, and a locker room
	static constexpr float PROB_NORMAL_ROOM = 70.0f;
	static constexpr float PROB_KEY_ROOM = 15.0f;
	static constexpr float PROB_LOCKER_ROOM = 15.0f;

	// Crossover and mutation rates for the algorithm
	static float CROSSOVER_RATE_FORM;
	static float MUTATION0_RATE_FORM; 
	static float SYMMETRIC_RATE_FORM; 
	static float MUTATION1_RATE_FORM; 

	// static float CROSSOVER_RATE_BK; 
	static float MUTATION0_RATE_BK; 
	static float MUTATION1_RATE_BK; 

	// Algorithm Parameters
	static int MAX_DEPTH; // Maximum depth for the algorithm
	static int POP_SIZE; // Population size for the algorithm
	static int GENERATIONSFORM; // Number of generations for the algorithm
	static int GENERATIONSBK; // Number of generations for the algorithm


	// Offset for matrix calculations
	static constexpr int MATRIXOFFSET = 150; // Max number of rooms (150 in one direction !)
	static int ID;

	/***************************************************/                     
	/**************  Algorithm Parameters **************/
	/***************************************************/  
	// Fitness parameters
	static int nV, nK, nL; // Parameters related to fitness calculation nV(Rooms) nK(Keys) nL(Locks)
	static float lCoef; // Parameter related to fitness calculation (linear Coeficient avgChildNodes)

	// Seed use, and the FORCED mode to test the original Algorithm !
	static int seed;
	static std::mt19937 randGen;

	// Main configurations !!!
	static bool useDFS;
	static bool useSymmetry;
	static bool horizontalSymmetry;
	static bool verticalSymmetry;

	static float FitnessSymmetry;
	static float NeededLocks;

	// Fitness wieght
	static float lCoefW;
	static float sCoefW;

	// Analisis
	static bool analisis;

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

