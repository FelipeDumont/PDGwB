#include "Constants.h"
#include<iostream>

int Constants::ID = 0;

/***************************************************/                     
/***************  Testign Parameters ***************/
/***************************************************/
bool Constants::testingMode = false;


/***************************************************/                     
/***************  PARAMETER SETTINGS ***************/
/***************************************************/ 

// Algorithm Variables "Normal"
int Constants::GENERATIONSFORM = 100;
int Constants::GENERATIONSBK = 100;
int Constants::MAX_DEPTH = 10;
int Constants::POP_SIZE = 100;

float Constants::MUTATION0_RATE_FORM = 2.5f; // 25 / 1000 (2.5%)
float Constants::SYMMETRIC_RATE_FORM = 2.5f; // 25 / 1000 (2.5%)
float Constants::MUTATION1_RATE_FORM = 2.5f; // 25 / 1000 (2.5%)
float Constants::CROSSOVER_RATE_FORM = 90.0f; // 90 / 100  (90%)

float Constants::MUTATION0_RATE_BK = 2.5; // 25 / 1000 (2.5%)
float Constants::MUTATION1_RATE_BK = 2.5f; // 25 / 1000 (2.5%)
// float Constants::CROSSOVER_RATE_BK = 90.0f; // 90 / 100  (90%)

/***************************************************/                     
/**************  Algorithm Parameters **************/
/***************************************************/ 


// Input For generation
int Constants::nV = 1;
int Constants::nK = 1;
int Constants::nL = 1;
float Constants::lCoef = 1;
float Constants::FitnessSymmetry = 1;
float Constants::NeededLocks = 1;
int Constants::seed = 100;
std::mt19937 Constants::randGen = std::mt19937(seed);

// Fitness wieght
float Constants::lCoefW = 1;
float Constants::sCoefW = 0;

// DFS and symmetry
bool Constants::useDFS = false;
bool Constants::useSymmetry = false;

// Analisis
bool Constants::analisis = false;

int Constants::getNextId(){
	return ID++;
}

int Constants::Next(int maxValue) {
	if(maxValue == 0){
		maxValue += 1;
	}
    std::uniform_int_distribution<int> distribution(0, maxValue - 1);
	int result = distribution(randGen);
    return result;
}

int Constants::Next(int minValue, int maxValue) {
    std::uniform_int_distribution<int> distribution(minValue, maxValue - 1);
	int result = distribution(randGen);
    return result;
}


	
