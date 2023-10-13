#include "Constants.h"

int Constants::ID = 0;
int Constants::MAX_DEPTH = 10;
int Constants::POP_SIZE = 100;
int Constants::GENERATIONS = 150;
int Constants::nV = 1;
int Constants::nK = 1;
int Constants::nL = 1;
float Constants::lCoef = 1;

int Constants::seed = 100;
std::mt19937 Constants::randGen = std::mt19937(seed);
std::uniform_int_distribution<int> Constants::prob = std::uniform_int_distribution<int>(0,101);


int Constants::getNextId(){
	return ID++;
}

int Constants::Next101(){
	return prob(Constants::randGen);
}
	
