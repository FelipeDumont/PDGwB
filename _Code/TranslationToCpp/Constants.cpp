#include "Constants.h"
#include<iostream>

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

bool Constants::isForced = false;
std::vector<int> Constants::forcedRandomSequence;
int Constants::currentValue = 0;

int Constants::getNextId(){
	return ID++;
}

int Constants::Next(int maxValue) {
    std::uniform_int_distribution<int> distribution(0, maxValue - 1);
	int result = distribution(randGen);
	if(isForced){
		result = forcedRandomSequence[currentValue];
		currentValue += 1;
	}
    return result;
}

int Constants::Next(int minValue, int maxValue) {
    std::uniform_int_distribution<int> distribution(minValue, maxValue - 1);
	int result = distribution(randGen);
	if(isForced){
		result = forcedRandomSequence[currentValue];
		currentValue += 1;
	}
    return result;
}

/*
int Constants::SelectRandom(int value){
	randomSequence.push_back(value);
	if(isForced){
		value =forcedRandomSequence[currentValue];
		currentValue += 1;
	}
	// std::cout << randomSequence.size() << ": " << value << "\n";
	return value;
}
*/

	
